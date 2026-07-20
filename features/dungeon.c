#include "dungeon.h"
#include "stronghold.h"
#include "mineshaft.h"
#include "../rng.h"

#include <stdlib.h>
#include <string.h>

void createDungeonRoomList(DungeonRoomList *list, int initialCapacity) {
    list->rooms = (DungeonRoom*)malloc(initialCapacity * sizeof(DungeonRoom));
    list->size = 0;
    list->capacity = initialCapacity;
}

void appendDungeonRoom(DungeonRoomList *list, DungeonRoom room) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->rooms = (DungeonRoom*)realloc(list->rooms, list->capacity * sizeof(DungeonRoom));
    }
    list->rooms[list->size++] = room;
}

void freeDungeonRoomList(DungeonRoomList *list) {
    free(list->rooms);
    list->rooms = NULL;
    list->size = list->capacity = 0;
}

static int placeMonsterRoom(uint64_t *rnd, int x, int y, int z, DungeonBlockFxn getBlock, DungeonSetFxn setBlock, void *ctx, DungeonRoom *room) {
    int j = nextInt(rnd, 2) + 2;
    int k = -j - 1, l = j + 1;
    int o = nextInt(rnd, 2) + 2;
    int p = -o - 1, q = o + 1;

    int openings = 0;
    for (int s = k; s <= l; s++) {
        for (int t = -1; t <= 4; t++) {
            for (int u = p; u <= q; u++) {
                int bx = x + s, by = y + t, bz = z + u;
                int solid = by >= 0 && by <= 255 && getBlock(ctx, bx, by, bz, 0);
                if ((t == -1 || t == 4) && !solid) return 0;
                if ((s == k || s == l || u == p || u == q) && t == 0 && getBlock(ctx, bx, by, bz, 1) && getBlock(ctx, bx, by + 1, bz, 1)) openings++;
            }
        }
    }
    if (openings < 1 || openings > 5) return 0;

    for (int s = k; s <= l; s++) {
        for (int t = 3; t >= -1; t--) {
            for (int u = p; u <= q; u++) {
                int bx = x + s, by = y + t, bz = z + u;
                if (s != k && t != -1 && u != p && s != l && t != 4 && u != q) {
                    setBlock(ctx, bx, by, bz, 1);
                } else if (by >= 0 && !(by - 1 >= 0 && getBlock(ctx, bx, by - 1, bz, 0))) {
                    setBlock(ctx, bx, by, bz, 1);
                } else if (getBlock(ctx, bx, by, bz, 0)) {
                    if (t == -1)
                        nextInt(rnd, 4); // mossy vs plain cobblestone
                    setBlock(ctx, bx, by, bz, 2);
                }
            }
        }
    }

    room->spawnerPos = (Pos3) {x, y, z};
    room->halfX = j;
    room->halfZ = o;
    room->chestCount = 0;
    for (int a = 0; a < 2; a++) {
        for (int b = 0; b < 3; b++) {
            int cxp = x + nextInt(rnd, j * 2 + 1) - j;
            int czp = z + nextInt(rnd, o * 2 + 1) - o;
            if (!getBlock(ctx, cxp, y, czp, 1)) continue;
            int solidSides = 0;
            solidSides += getBlock(ctx, cxp, y, czp - 1, 0);
            solidSides += getBlock(ctx, cxp, y, czp + 1, 0);
            solidSides += getBlock(ctx, cxp - 1, y, czp, 0);
            solidSides += getBlock(ctx, cxp + 1, y, czp, 0);
            if (solidSides == 1) {
                setBlock(ctx, cxp, y, czp, 3);
                room->chestPoses[room->chestCount] = (Pos3) {cxp, y, czp};
                room->chestLootSeeds[room->chestCount] = nextLong(rnd);
                room->chestCount++;
                break;
            }
        }
    }

    setBlock(ctx, x, y, z, 3);
    nextInt(rnd, 4); // spawner mob type
    return 1;
}

int simMonsterRooms(int mc, uint64_t worldSeed, int chunkX, int chunkZ, int featureIndex, DungeonBlockFxn getBlock, DungeonSetFxn setBlock, void *ctx, DungeonRoomList *roomsOut) {
    uint64_t rnd;
    uint64_t popSeed = getPopulationSeed(mc, worldSeed, chunkX << 4, chunkZ << 4);
    setSeed(&rnd, popSeed + featureIndex + 10000 * 3); // underground structures
    int placed = 0;
    for (int attempt = 0; attempt < 8; attempt++) {
        int x = (chunkX << 4) + nextInt(&rnd, 16);
        int z = (chunkZ << 4) + nextInt(&rnd, 16);
        int y = nextInt(&rnd, 256);
        DungeonRoom room;
        if (placeMonsterRoom(&rnd, x, y, z, getBlock, setBlock, ctx, &room)) {
            if (roomsOut)
                appendDungeonRoom(roomsOut, room);
            placed++;
        }
    }
    return placed;
}

void dungeonFillMask(uint8_t *mask, const Pos3List *list, int cx, int cz) {
    memset(mask, 0, 8192);
    for (int i = 0; i < list->size; i++) {
        Pos3 a = list->pos3s[i];
        int lx = a.x - cx, lz = a.z - cz;
        if (lx < 0 || lx > 15 || lz < 0 || lz > 15 || a.y < 0 || a.y > 255)
            continue;
        int idx = (a.y << 8) | (lz << 4) | lx;
        mask[idx >> 3] |= 1 << (idx & 7);
    }
}

static void carveCached(Generator *g, SurfaceNoise *sn, DungeonCarverCache *cc, int cx, int cz) {
    if (!cc->valid) {
        createPos3List(&cc->air, 16);
        createPos3List(&cc->water, 16);
        applyAllCarvers(g, sn, cx >> 4, cz >> 4, &cc->air, &cc->water);
        cc->valid = 1;
    }
}

static inline void setDetail(uint8_t *d, int x, int y, int z, int cx, int cz, int v) {
    int lx = x - cx, lz = z - cz;
    if (lx < 0 || lx > 15 || lz < 0 || lz > 15 || y < 0 || y > 255) return;
    int idx = (y << 8) | (lz << 4) | lx;
    int sh = (idx & 1) << 2;
    d[idx >> 1] = (d[idx >> 1] & ~(0xF << sh)) | (v << sh);
}

static inline int getDetail(const uint8_t *d, int x, int y, int z, int cx, int cz) {
    int lx = x - cx, lz = z - cz;
    if (lx < 0 || lx > 15 || lz < 0 || lz > 15 || y < 0 || y > 255) return 0;
    int idx = (y << 8) | (lz << 4) | lx;
    return (d[idx >> 1] >> ((idx & 1) << 2)) & 0xF;
}

static inline int maskGet(const uint8_t *mask, int cx, int cz, int x, int y, int z) {
    int lx = x - cx, lz = z - cz;
    if (lx < 0 || lx > 15 || lz < 0 || lz > 15 || y < 0 || y > 255) return 0;
    int idx = (y << 8) | (lz << 4) | lx;
    return (mask[idx >> 3] >> (idx & 7)) & 1;
}

static int anyPieceContains(const DungeonWorld *c, int x, int y, int z, const Piece *skip) {
    for (int i = 0; i < c->pieceCount; i++) {
        const Piece *p = &c->pieces[i];
        if (p == skip) continue;
        if (x >= p->bb0.x && x <= p->bb1.x && y >= p->bb0.y && y <= p->bb1.y &&
            z >= p->bb0.z && z <= p->bb1.z)
            return 1;
    }
    return 0;
}

static int pieceAt(const DungeonWorld *c, int x, int y, int z) {
    int best = -1;
    for (int i = 0; i < c->pieceCount; i++) {
        const Piece *p = &c->pieces[i];
        if (x < p->bb0.x || x > p->bb1.x || y < p->bb0.y || y > p->bb1.y ||
            z < p->bb0.z || z > p->bb1.z)
            continue;
        if (x > p->bb0.x && x < p->bb1.x && y > p->bb0.y && y < p->bb1.y &&
            z > p->bb0.z && z < p->bb1.z)
            return 1;
        if (y >= p->bb0.y + 1 && y <= p->bb0.y + 3) {
            int cxm = (p->bb0.x + p->bb1.x) >> 1;
            int czm = (p->bb0.z + p->bb1.z) >> 1;
            if ((x == p->bb0.x || x == p->bb1.x) && z >= czm - 1 && z <= czm + 1) {
                if (anyPieceContains(c, x == p->bb0.x ? x - 1 : x + 1, y, z, p))
                    return 1;
            } else if ((z == p->bb0.z || z == p->bb1.z) && x >= cxm - 1 && x <= cxm + 1) {
                if (anyPieceContains(c, x, y, z == p->bb0.z ? z - 1 : z + 1, p))
                    return 1;
            }
        }
        best = 0;
    }
    return best;
}

static int worldIdx(const DungeonWorld *c, int bx, int bz) {
    int cxi = (bx >> 4) - c->lcx0, czi = (bz >> 4) - c->lcz0;
    if (cxi < 0 || cxi >= c->ncx || czi < 0 || czi >= c->ncz) return -1;
    return c->cellToIdx[cxi * c->ncz + czi];
}

static void ensureCarver(DungeonWorld *c, int idx) {
    if (c->carverAirMask[idx]) return;
    carveCached(c->g, c->sn, &c->cc[idx], c->chunkXs[idx], c->chunkZs[idx]);
    c->carverAirMask[idx] = (uint8_t*)malloc(8192);
    c->carverWaterMask[idx] = (uint8_t*)malloc(8192);
    dungeonFillMask(c->carverAirMask[idx], &c->cc[idx].air, c->chunkXs[idx], c->chunkZs[idx]);
    dungeonFillMask(c->carverWaterMask[idx], &c->cc[idx].water, c->chunkXs[idx], c->chunkZs[idx]);
}

static void ensureMineshaft(DungeonWorld *c, int idx) {
    if (c->mineshaftAirMask[idx]) return;
    c->mineshaftAirMask[idx] = (uint8_t*)calloc(1, 8192);
    int cx = c->chunkXs[idx], cz = c->chunkZs[idx];
    for (int i = 0; i < c->mineshaftAir->size; i++) {
        Pos3 a = c->mineshaftAir->pos3s[i];
        int lx = a.x - cx, lz = a.z - cz;
        if (lx < 0 || lx > 15 || lz < 0 || lz > 15 || a.y < 0 || a.y > 255) continue;
        int b = (a.y << 8) | (lz << 4) | lx;
        c->mineshaftAirMask[idx][b >> 3] |= 1 << (b & 7);
    }
}

static double terrainDensity(DungeonWorld *c, int x, int y, int z) {
    if (y >= 152) return -1.0;
    double dens[2][2][20];
    int px = x >> 2, pz = z >> 2;
    for (int dx = 0; dx <= 1; dx++)
        for (int dz = 0; dz <= 1; dz++)
            surfaceCornerDens(c->g, c->sn, px + dx, pz + dz, dens[dx][dz]);
    int py = y >> 3;
    double fx = (x & 3) / 4.0, fy = (y & 7) / 8.0, fz = (z & 3) / 4.0;
    double l00 = lerp(fy, dens[0][0][py], dens[0][0][py+1]);
    double l10 = lerp(fy, dens[1][0][py], dens[1][0][py+1]);
    double l01 = lerp(fy, dens[0][1][py], dens[0][1][py+1]);
    double l11 = lerp(fy, dens[1][1][py], dens[1][1][py+1]);
    return lerp(fz, lerp(fx, l00, l10), lerp(fx, l01, l11));
}

static int worldBlock(void *vctx, int x, int y, int z, int wantAir) {
    DungeonWorld *c = (DungeonWorld*)vctx;
    if (y < 0 || y > 255) return wantAir;
    int idx = worldIdx(c, x, z);
    int shellFace = 0;
    if (idx >= 0) {
        int seen = idx <= c->ci;
        int cx = c->chunkXs[idx], cz = c->chunkZs[idx];
        if (seen && c->dungeonDetails[idx]) {
            int v = getDetail(c->dungeonDetails[idx], x, y, z, cx, cz);
            if (v == 1) return wantAir;
            if (v) return !wantAir;
        }

        if (idx < c->ci) {
            int pa = pieceAt(c, x, y, z);
            if (pa == 1) return wantAir;
            if (pa == 0) shellFace = 1;
        }
        if (seen) {
            ensureMineshaft(c, idx);
            if (maskGet(c->mineshaftAirMask[idx], cx, cz, x, y, z)) return wantAir;
        }
        if (seen && c->lakeDetails[idx]) {
            int v = getDetail(c->lakeDetails[idx], x, y, z, cx, cz);
            if (v == 1) return wantAir;
            if (v == 4 || v == 5) return shellFace ? !wantAir : 0;
        }
        ensureCarver(c, idx);
        if (maskGet(c->carverWaterMask[idx], cx, cz, x, y, z))
            return shellFace ? !wantAir : (wantAir ? 0 : y == 10); // water floor y10 is magma
        if (maskGet(c->carverAirMask[idx], cx, cz, x, y, z)) {
            if (y >= 11) return wantAir;      // carved air stays air even on a face
            return shellFace ? !wantAir : 0;  // below y11 carved air is lava
        }
    }
    if (shellFace) return !wantAir;
    double d = terrainDensity(c, x, y, z);
    if (d > 0) return !wantAir;
    if (y <= 62) return 0; // terrain fills with water below sea level
    return wantAir;
}

static void worldSet(void *vctx, int x, int y, int z, int kind) {
    DungeonWorld *c = (DungeonWorld*)vctx;
    if (y < 0 || y > 255) return;
    int idx = worldIdx(c, x, z);
    if (idx < 0) return;
    int cx = c->chunkXs[idx], cz = c->chunkZs[idx];
    if (!c->dungeonDetails[idx]) c->dungeonDetails[idx] = (uint8_t*)calloc(1, 32768);
    // chests/spawners stay when a later dungeon carves its interior over them
    if (kind == 1 && getDetail(c->dungeonDetails[idx], x, y, z, cx, cz) == 3) return;
    setDetail(c->dungeonDetails[idx], x, y, z, cx, cz, kind);
    if (c->dungeonAirBySrc)
        appendPos3List(kind == 1 ? &c->dungeonAirBySrc[c->ci] : &c->dungeonSolidBySrc[c->ci], (Pos3){x, y, z});
}

int dungeonSimChunk(DungeonWorld *w, int ci, Pos3List *lakeAirAll, Pos3List *lakeWaterAll, DungeonRoomList *roomsOut) {
    int nchunks = w->ncx * w->ncz;
    int cx = w->chunkXs[ci], cz = w->chunkZs[ci];
    carveCached(w->g, w->sn, &w->cc[ci], cx, cz);
    Pos3List *ca[3][3], *cw[3][3]; const uint8_t *det[3][3]; int cellIdx[3][3];

    for (int dz = 0; dz < 3; ++dz) {
        for (int dx = 0; dx < 3; ++dx) {
            int sx = cx + (dx - 1) * 16, sz = cz + (dz - 1) * 16;
            int idx = dx == 1 && dz == 1 ? ci : worldIdx(w, sx, sz);
            cellIdx[dz][dx] = idx;
            if (idx >= 0) {
                carveCached(w->g, w->sn, &w->cc[idx], sx, sz);
                ca[dz][dx] = &w->cc[idx].air;
                cw[dz][dx] = &w->cc[idx].water;
                det[dz][dx] = idx < ci ? w->lakeDetails[idx] : NULL;
            } else { ca[dz][dx] = NULL; cw[dz][dx] = NULL; det[dz][dx] = NULL; }
        }
    }

    static const int srcCell[4][2] = {{0,0},{0,1},{1,0},{1,1}}; // NW, W, N, self as [dx][dz]
    int order[4];
    for (int c = 0; c < 4; ++c) { int idx = cellIdx[srcCell[c][1]][srcCell[c][0]]; order[c] = (idx >= 0 && idx <= ci) ? idx : -1; }
    Pos3List la, lw, ll; createPos3List(&la, 4); createPos3List(&lw, 4); createPos3List(&ll, 4);
    applyAllLakes(w->g, w->sn, w->mc, w->seed, cx >> 4, cz >> 4, order, ca, cw, det, &la, &lw, &ll);
    w->lakeDetails[ci] = (uint8_t*)calloc(1, 32768);
    for (int i = 0; i < la.size; ++i) { 
        setDetail(w->lakeDetails[ci], la.pos3s[i].x, la.pos3s[i].y, la.pos3s[i].z, cx, cz, 1); 
        if (lakeAirAll) appendPos3List(lakeAirAll, la.pos3s[i]); 
    }
    for (int i = 0; i < lw.size; ++i) { 
        setDetail(w->lakeDetails[ci], lw.pos3s[i].x, lw.pos3s[i].y, lw.pos3s[i].z, cx, cz, 4); 
        if (lakeWaterAll) appendPos3List(lakeWaterAll, lw.pos3s[i]); 
    }
    for (int i = 0; i < ll.size; ++i) setDetail(w->lakeDetails[ci], ll.pos3s[i].x, ll.pos3s[i].y, ll.pos3s[i].z, cx, cz, 5);
    freePos3List(&la); freePos3List(&lw); freePos3List(&ll);

    w->ci = ci;
    int dungeonBiome = getBiomeAt(w->g, 4, ((cx >> 4) << 2) + 2, 2, ((cz >> 4) << 2) + 2);
    int fIdx = (dungeonBiome == desert || dungeonBiome == swamp) ? 3 : 2; // fossils shift the slot
    return simMonsterRooms(w->mc, w->seed, cx >> 4, cz >> 4, fIdx, worldBlock, worldSet, w, roomsOut);
}

int getDungeonsArea(Generator *g, SurfaceNoise *sn, int mc, uint64_t seed, int cx0, int cz0, int cx1, int cz1, int centerCX, int centerCZ, DungeonRoomList *roomsOut) {
    if (!g || !sn || mc < MC_1_14 || mc > MC_1_16_5 || cx0 > cx1 || cz0 > cz1) // carver/lake logic is different in 1.17+ and 1.13-
        return -1;

    enum { R = 2 }; // margin so every core chunk's own 3x3 (and getDungeons's validated 5x5) neighborhood is covered
    int lcx0 = cx0 - R, lcz0 = cz0 - R;
    int ncx = (cx1 - cx0 + 1) + 2 * R, ncz = (cz1 - cz0 + 1) + 2 * R;
    int nchunks = ncx * ncz;

    int *chunkXs = (int*)malloc(nchunks * sizeof(int));
    int *chunkZs = (int*)malloc(nchunks * sizeof(int));
    int *cellToIdx = (int*)malloc(nchunks * sizeof(int));
    int *isCore = (int*)malloc(nchunks * sizeof(int));

    int k = 0;
    for (int cx = lcx0; cx < lcx0 + ncx; ++cx)
        for (int cz = lcz0; cz < lcz0 + ncz; ++cz) {
            chunkXs[k] = cx << 4; chunkZs[k] = cz << 4; ++k;
        }

    // stable sort by squared distance from the center

    for (int i = 1; i < nchunks; ++i) {
        int xi = chunkXs[i], zi = chunkZs[i];
        long long dx = (xi >> 4) - centerCX, dz = (zi >> 4) - centerCZ;
        long long di = dx * dx + dz * dz;
        int j = i - 1;
        while (j >= 0) {
            long long ex = (chunkXs[j] >> 4) - centerCX, ez = (chunkZs[j] >> 4) - centerCZ;
            if (ex * ex + ez * ez <= di) break;
            chunkXs[j + 1] = chunkXs[j]; chunkZs[j + 1] = chunkZs[j];
            --j;
        }
        chunkXs[j + 1] = xi; chunkZs[j + 1] = zi;
    }
    int maxCoreRank = -1;
    for (int i = 0; i < nchunks; ++i) {
        int cxi = (chunkXs[i] >> 4) - lcx0, czi = (chunkZs[i] >> 4) - lcz0;
        cellToIdx[cxi * ncz + czi] = i;
        int cx = chunkXs[i] >> 4, cz = chunkZs[i] >> 4;
        isCore[i] = cx >= cx0 && cx <= cx1 && cz >= cz0 && cz <= cz1;
        if (isCore[i] && i > maxCoreRank) maxCoreRank = i;
    }

    int wx0 = lcx0 << 4, wz0 = lcz0 << 4;
    int wx1 = ((lcx0 + ncx - 1) << 4) + 15, wz1 = ((lcz0 + ncz - 1) << 4) + 15;

    Pos3List mineshaftAir;
    createPos3List(&mineshaftAir, 64);
    {
        Piece *msPieces = (Piece*)malloc(2048 * sizeof(Piece));
        Piece *msLoot = (Piece*)malloc(2048 * sizeof(Piece));
        for (int rx = lcx0 - 8; rx <= lcx0 + ncx - 1 + 8; ++rx) {
            for (int rz = lcz0 - 8; rz <= lcz0 + ncz - 1 + 8; ++rz) {
                Pos mp;
                if (!getStructurePos(Mineshaft, mc, seed, rx, rz, &mp)) continue;
                if (!isViableStructurePos(Mineshaft, g, mp.x, mp.z, 0)) continue;
                int mn = getMineshaftPieces(g, msPieces, 2048, mc, seed, rx, rz);
                int hit = 0;
                for (int i = 0; i < mn; ++i) {
                    Piece *q = &msPieces[i];
                    if (q->bb1.x >= wx0 && q->bb0.x <= wx1 &&
                        q->bb1.z >= wz0 && q->bb0.z <= wz1) { hit = 1; break; }
                }
                if (!hit) continue;
                StructureSaltConfig msconf;
                getStructureSaltConfig(Mineshaft, mc, getBiomeAt(g, 4, mp.x, 64, mp.z), &msconf);
                getMineshaftLoot(g, sn, msLoot, 2048, msconf, mc, seed, rx, rz, &mineshaftAir);
            }
        }
        free(msPieces);
        free(msLoot);
    }

    Piece *shPieces = NULL;
    int shCount = 0;
    {
        // safe upper bound on how far stronghold rings must be walked: the
        // window corner farthest from the world origin, plus slack below
        double tOriginDist = 0;
        for (int a = 0; a < 2; ++a) for (int b = 0; b < 2; ++b) {
            double wx = a ? wx1 : wx0, wz = b ? wz1 : wz0;
            double d = sqrt(wx * wx + wz * wz);
            if (d > tOriginDist) tOriginDist = d;
        }
        Piece *buf = (Piece*)malloc(400 * sizeof(Piece));
        StrongholdIter sh;
        initFirstStronghold(&sh, mc, seed);
        while (nextStronghold(&sh, g) > 0) {
            int nx = sh.pos.x < wx0 ? wx0 : (sh.pos.x > wx1 ? wx1 : sh.pos.x); // nearest point in the window
            int nz = sh.pos.z < wz0 ? wz0 : (sh.pos.z > wz1 ? wz1 : sh.pos.z);
            long long dx = sh.pos.x - nx, dz = sh.pos.z - nz;
            if (dx * dx + dz * dz <= 300LL * 300LL) {
                int n = getStrongholdPieces(buf, 400, mc, seed, sh.pos.x >> 4, sh.pos.z >> 4);
                for (int i = 0; i < n; ++i) {
                    Piece *q = &buf[i];
                    if (q->bb1.x >= wx0 - 1 && q->bb0.x <= wx1 + 1 &&
                        q->bb1.z >= wz0 - 1 && q->bb0.z <= wz1 + 1) { // door check goes one block past a piece face
                        shPieces = (Piece*)realloc(shPieces, (shCount + 1) * sizeof(Piece));
                        shPieces[shCount++] = *q;
                    }
                }
            }
            // TODO add further rings
            if (sh.dist * 16 > tOriginDist + 2500) break;
        }
        free(buf);
    }

    DungeonCarverCache *cc = (DungeonCarverCache*)calloc(nchunks, sizeof(DungeonCarverCache));
    uint8_t **lakeDetails = (uint8_t**)calloc(nchunks, sizeof(uint8_t*));

    DungeonWorld w;
    w.g = g; w.sn = sn;
    w.mc = mc; w.seed = seed;
    w.lcx0 = lcx0; w.lcz0 = lcz0;
    w.ncx = ncx; w.ncz = ncz;
    w.cc = cc; w.chunkXs = chunkXs; w.chunkZs = chunkZs;
    w.cellToIdx = cellToIdx;
    w.lakeDetails = lakeDetails;
    w.dungeonDetails = (uint8_t**)calloc(nchunks, sizeof(uint8_t*));
    w.carverAirMask = (uint8_t**)calloc(nchunks, sizeof(uint8_t*));
    w.carverWaterMask = (uint8_t**)calloc(nchunks, sizeof(uint8_t*));
    w.mineshaftAirMask = (uint8_t**)calloc(nchunks, sizeof(uint8_t*));
    w.mineshaftAir = &mineshaftAir;
    w.dungeonAirBySrc = NULL; w.dungeonSolidBySrc = NULL;
    w.pieces = shPieces; w.pieceCount = shCount;

    int placed = 0;
    for (int ci = 0; ci <= maxCoreRank; ++ci) {
        if (isCore[ci])
            placed += dungeonSimChunk(&w, ci, NULL, NULL, roomsOut);
        else
            dungeonSimChunk(&w, ci, NULL, NULL, NULL);
    }

    for (int q = 0; q < nchunks; ++q) {
        if (lakeDetails[q]) free(lakeDetails[q]);
        if (cc[q].valid) { freePos3List(&cc[q].air); freePos3List(&cc[q].water); }
        if (w.dungeonDetails[q]) free(w.dungeonDetails[q]);
        if (w.carverAirMask[q]) free(w.carverAirMask[q]);
        if (w.carverWaterMask[q]) free(w.carverWaterMask[q]);
        if (w.mineshaftAirMask[q]) free(w.mineshaftAirMask[q]);
    }
    free(w.dungeonDetails);
    free(w.carverAirMask);
    free(w.carverWaterMask);
    free(w.mineshaftAirMask);
    free(cc);
    free(lakeDetails);
    free(shPieces);
    free(chunkXs);
    free(chunkZs);
    free(cellToIdx);
    free(isCore);
    freePos3List(&mineshaftAir);
    return placed;
}

int getDungeons(Generator *g, SurfaceNoise *sn, int mc, uint64_t seed, int chunkX, int chunkZ, int centerCX, int centerCZ, DungeonRoomList *roomsOut) {
    return getDungeonsArea(g, sn, mc, seed, chunkX, chunkZ, chunkX, chunkZ, centerCX, centerCZ, roomsOut);
}

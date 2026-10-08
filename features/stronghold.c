#include "stronghold.h"

#include <string.h>

#include "piece.h"
#include "mineshaft.h"
#include "dungeon.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

STRUCT(StrongholdPieceEnv) {
    int mc;
    Piece *list;
    int *n;
    uint64_t *rng;
    int *portal;
    int imposedPiece;
    int typlast;
    int nmax;
    int ntyp[SH_PIECE_COUNT];
    uint16_t deltyp; // 16 > SH_PIECE_COUNT
    int totalWeight;
    int generationStopped;
};

static const struct {
    const Pos3 offset, size;
    const int weight, maxPlaceCount, minDepth;
    const char *name;
} stronghold_info[] = {
    {{-1, -1, 0}, { 5,  5,  7}, 40,  0,  0, "SHS"  }, // STRAIGHT
    {{-1, -1, 0}, { 9,  5, 11},  5,  5,  0, "SHPH" }, // PRISON_HALL
    {{-1, -1, 0}, { 5,  5,  5}, 20,  0,  0, "SHLT" }, // LEFT_TURN
    {{-1, -1, 0}, { 5,  5,  5}, 20,  0,  0, "SHRT" }, // RIGHT_TURN
    {{-4, -1, 0}, {11,  7, 11}, 10,  6,  0, "SHRC" }, // ROOM_CROSSING
    {{-1, -7, 0}, { 5, 11,  8},  5,  5,  0, "SHSSD"}, // STRAIGHT_STAIRS_DOWN
    {{-1, -7, 0}, { 5, 11,  5},  5,  5,  0, "SHSD" }, // STAIRS_DOWN
    {{-4, -3, 0}, {10,  9, 11},  5,  4,  0, "SH5C" }, // FIVE_CROSSING
    {{-1, -1, 0}, { 5,  5,  7},  5,  4,  0, "SHCC" }, // CHEST_CORRIDOR
    {{-4, -1, 0}, {14, 11, 15}, 10,  2,  5, "SHLi" }, // LIBRARY
    {{-4, -1, 0}, {11,  8, 16}, 20,  1,  6, "SHPR" }, // PORTAL_ROOM
    {{-1, -1, 0}, { 5,  5,  4}, -1, -1, -1, "SHFC" }, // FILLER_CORRIDOR
};

static inline int isValid(int piecePlaceCount, int pieceType) {
    int maxPlaceCount = stronghold_info[pieceType].maxPlaceCount;
    return maxPlaceCount == 0 || piecePlaceCount < maxPlaceCount;
}

static inline int canPlace(int piecePlaceCount, int pieceType, int depth) {
    return isValid(piecePlaceCount, pieceType) && depth >= stronghold_info[pieceType].minDepth;
}

static inline void updateGenerationStatus(StrongholdPieceEnv *env) {
    for (int pieceType = 0; pieceType < 11; pieceType++) {
        if ((env->deltyp >> pieceType) & 1) continue;
        int piecePlaceCount = env->ntyp[pieceType];
        int maxPlaceCount = stronghold_info[pieceType].maxPlaceCount;
        int pieceWeight = stronghold_info[pieceType].weight;
        if (maxPlaceCount > 0 && piecePlaceCount < pieceWeight) {
            return;
        }
    }
    env->generationStopped = 1;
}

static inline Piece* hasCollision(StrongholdPieceEnv *env, Pos3 b0, Pos3 b1) {
    int i, n = *env->n;
    for (i = 0; i < n; i++) {
        Piece *q = env->list + i;
        if (hasIntersection(q->bb0, q->bb1, b0, b1)) {
            return q;
        }
    }
    return NULL;
}

static int addStrongholdPiece(StrongholdPieceEnv *env, int typ, int x, int y, int z, int depth, int facing) {
    if (env->mc < MC_1_14 && typ == SH_RIGHT_TURN) {
        typ = SH_LEFT_TURN;
    }

    Pos3 pos = {x, y, z};
    Pos3 b0, b1;
    Pos3 offset; Pos3 size;

    switch (typ) {
    case SH_STRAIGHT:
    case SH_PRISON_HALL:
    case SH_LEFT_TURN:
    case SH_RIGHT_TURN:
    case SH_ROOM_CROSSING:
    case SH_STRAIGHT_STAIRS_DOWN:
    case SH_STAIRS_DOWN:
    case SH_FIVE_CROSSING:
    case SH_CHEST_CORRIDOR:
    case SH_PORTAL_ROOM:
        offset = stronghold_info[typ].offset; size = stronghold_info[typ].size; break;
    case SH_LIBRARY:
        offset = (Pos3) stronghold_info[typ].offset; size = (Pos3) stronghold_info[typ].size;
        orientBox(pos, offset, size, facing, &b0, &b1);
        if (b0.y > 10 && !hasCollision(env, b0, b1)) {
            goto L_box_end;
        }
        size.y = 6;
        break;
    case SH_FILLER_CORRIDOR:
        offset = (Pos3) stronghold_info[typ].offset; size = (Pos3) stronghold_info[typ].size;
        orientBox(pos, offset, size, facing, &b0, &b1);
        Piece *p = hasCollision(env, b0, b1);
        if (!p) {
            return 0;
        }
        if (p->bb0.y != b0.y) {
            return 0;
        }
        int minI = env->mc < MC_1_17 ? 0 : 1;
        for (int i = 2; i >= minI; --i) {
            size.z = i;
            orientBox(pos, offset, size, facing, &b0, &b1);
            if (hasIntersection(p->bb0, p->bb1, b0, b1)) {
                continue;
            }
            size.z = i + 1;
            orientBox(pos, offset, size, facing, &b0, &b1);
            if (b0.y > 1) {
                goto L_box_end;
            }
        }
        return 0;
    default: UNREACHABLE();
    }

    orientBox(pos, offset, size, facing, &b0, &b1);
    if (b0.y > 10 && hasCollision(env, b0, b1)) {
        return 0;
    }

L_box_end:;

    ;Piece *p = env->list + *env->n;
    p->name = stronghold_info[typ].name;
    p->pos = pos;
    p->bb0 = b0;
    p->bb1 = b1;
    p->rot = facing;
    p->depth = depth;
    p->type = typ;
    p->next = NULL;

    int additionalData = 0;
    switch (typ) {
    case SH_STRAIGHT:
        nextInt(env->rng, 5);
        additionalData |= (nextInt(env->rng, 2) == 0) << 0;
        additionalData |= (nextInt(env->rng, 2) == 0) << 1;
        break;
    case SH_PRISON_HALL:
    case SH_LEFT_TURN:
    case SH_RIGHT_TURN:
    case SH_STRAIGHT_STAIRS_DOWN:
    case SH_STAIRS_DOWN:
    case SH_CHEST_CORRIDOR:
    case SH_LIBRARY:
        nextInt(env->rng, 5);
        break;
    case SH_ROOM_CROSSING:
        nextInt(env->rng, 5);
        additionalData |= (nextInt(env->rng, 5) == 2) << 0;
        break;
    case SH_FIVE_CROSSING:
        nextInt(env->rng, 5);
        additionalData |= (next(env->rng, 1)) << 0;
        additionalData |= (next(env->rng, 1)) << 1;
        additionalData |= (next(env->rng, 1)) << 2;
        additionalData |= (nextInt(env->rng, 3) > 0) << 3;
        break;
    case SH_PORTAL_ROOM:
        *env->portal = 1;
        break;
    case SH_FILLER_CORRIDOR: break;
    default: UNREACHABLE();
    }

    p->additionalData = additionalData;

    // accept the piece and append it to the processing front
    //int queue = 0;
    (*env->n)++;
    Piece *q = env->list;
    while (q->next) {
        q = q->next; //queue++;
    }
    q->next = p;

    if (*env->n >= env->nmax){
        env->generationStopped = 1;
    }
    return 1;
}

static void extendStronghold(StrongholdPieceEnv *env, Piece *piece, int x, int y, int z, int facing) {
    if (piece->depth > 50) {
        return;
    }

    if (IABS(x - env->list->bb0.x) > 112 || IABS(z - env->list->bb0.z) > 112) {
        return;
    }

    if (env->generationStopped) {
        return;
    }

    int depth = piece->depth + 1;

    if (env->imposedPiece != -1) {
        int imposedPiece = env->imposedPiece;
        env->imposedPiece = -1;
        if (addStrongholdPiece(env, imposedPiece, x, y, z, depth, facing)) {
            return;
        }
    }

    for (int attempt = 0; attempt < 5; attempt++) {
        int selectedWeight = nextInt(env->rng, env->totalWeight);
        for (int pieceType = 0; pieceType < 11; pieceType++) {
            if ((env->deltyp >> pieceType) & 1) continue;

            int *piecePlaceCount = &env->ntyp[pieceType];
            int pieceWeight = stronghold_info[pieceType].weight;

            if ((selectedWeight -= pieceWeight) >= 0) continue;

            if (!canPlace(*piecePlaceCount, pieceType, depth) || pieceType == env->typlast) break;
            if (!addStrongholdPiece(env, pieceType, x, y, z, depth, facing)) continue;
            (*piecePlaceCount)++;
            env->typlast = pieceType;
            if (!isValid(*piecePlaceCount, pieceType)) {
                env->totalWeight -= pieceWeight;
                env->deltyp |= 1 << pieceType;
                updateGenerationStatus(env);
            }
            return;
        }
    }

    addStrongholdPiece(env, SH_FILLER_CORRIDOR, x, y, z, depth, facing);
}

static void generateSmallDoorChildForward(StrongholdPieceEnv *env, Piece *piece, int offx, int offy) {
    // WEST and EAST are swapped on old versions
    switch (piece->rot) {
    case 0: // facing 2
        return extendStronghold(env, piece, piece->bb0.x + offx, piece->bb0.y + offy, piece->bb0.z - 1, 0);
    case 2: // facing 0
        return extendStronghold(env, piece, piece->bb0.x + offx, piece->bb0.y + offy, piece->bb1.z + 1, 2);
    case 3: // facing 1
        return extendStronghold(env, piece, piece->bb0.x - 1, piece->bb0.y + offy, piece->bb0.z + offx, 3);
    case 1: // facing 3
        return extendStronghold(env, piece, piece->bb1.x + 1, piece->bb0.y + offy, piece->bb0.z + offx, 1);
    default: UNREACHABLE();
    }
}

static void generateSmallDoorChildLeft(StrongholdPieceEnv *env, Piece *piece, int offy, int offz) {
    switch (piece->rot) {
    case 0:
    case 2:
        return extendStronghold(env, piece, piece->bb0.x - 1, piece->bb0.y + offy, piece->bb0.z + offz, 3);
    case 3:
    case 1:
        return extendStronghold(env, piece, piece->bb0.x + offz, piece->bb0.y + offy, piece->bb0.z - 1, 0);
    default: UNREACHABLE();
    }
}

static void generateSmallDoorChildRight(StrongholdPieceEnv *env, Piece *piece, int offy, int offz) {
    switch (piece->rot) {
    case 0:
    case 2:
        return extendStronghold(env, piece, piece->bb1.x + 1, piece->bb0.y + offy, piece->bb0.z + offz, 1);
    case 3:
    case 1:
        return extendStronghold(env, piece, piece->bb0.x + offz, piece->bb0.y + offy, piece->bb1.z + 1, 2);
    default: UNREACHABLE();
    }
}

static void extendStrongholdPiece(StrongholdPieceEnv *env, Piece *piece) {
    switch (piece->type) {
    case SH_STRAIGHT:
        generateSmallDoorChildForward(env, piece, 1, 1);
        if ((piece->additionalData & (1 << 0)) != 0) {
            generateSmallDoorChildLeft(env, piece, 1, 2);
        }
        if ((piece->additionalData & (1 << 1)) != 0) {
            generateSmallDoorChildRight(env, piece, 1, 2);
        }
        break;
    case SH_PRISON_HALL:
        generateSmallDoorChildForward(env, piece, 1, 1);
        break;
    case SH_LEFT_TURN: {
        int rot = piece->rot;
        if (rot == 0 || rot == 1) {
            generateSmallDoorChildLeft(env, piece, 1, 1);
        } else {
            generateSmallDoorChildRight(env, piece, 1, 1);
        }
    } break;
    case SH_RIGHT_TURN: {
        int rot = piece->rot;
        if (rot == 0 || rot == 1) {
            generateSmallDoorChildRight(env, piece, 1, 1);
        } else {
            generateSmallDoorChildLeft(env, piece, 1, 1);
        }
    } break;
    case SH_ROOM_CROSSING:
        generateSmallDoorChildForward(env, piece, 4, 1);
        generateSmallDoorChildLeft(env, piece, 1, 4);
        generateSmallDoorChildRight(env, piece, 1, 4);
        break;
    case SH_STRAIGHT_STAIRS_DOWN:
        generateSmallDoorChildForward(env, piece, 1, 1);
        break;
    case SH_STAIRS_DOWN:
        if (piece->additionalData != 0) {
            env->imposedPiece = SH_FIVE_CROSSING;
        }
        generateSmallDoorChildForward(env, piece, 1, 1);
        break;
    case SH_FIVE_CROSSING: {
        int n = 3;
        int n2 = 5;
        int rot = piece->rot;
        if (rot == 3 || rot == 0) {
            n = 8 - n;
            n2 = 8 - n2;
        }
        generateSmallDoorChildForward(env, piece, 5, 1);
        if ((piece->additionalData & (1 << 0)) != 0) {
            generateSmallDoorChildLeft(env, piece, n, 1);
        }
        if ((piece->additionalData & (1 << 1)) != 0) {
            generateSmallDoorChildLeft(env, piece, n2, 7);
        }
        if ((piece->additionalData & (1 << 2)) != 0) {
            generateSmallDoorChildRight(env, piece, n, 1);
        }
        if ((piece->additionalData & (1 << 3)) != 0) {
            generateSmallDoorChildRight(env, piece, n2, 7);
        }
    } break;
    case SH_CHEST_CORRIDOR:
        generateSmallDoorChildForward(env, piece, 1, 1);
        break;
    case SH_LIBRARY:
    case SH_PORTAL_ROOM:
    case SH_FILLER_CORRIDOR:
        break;
    default: UNREACHABLE();
    }
}

int getStrongholdPieces(Piece *list, int n, int mc, uint64_t seed, int chunkX, int chunkZ) {
    static const int OLD_ROTATIONS[] = {2, 3, 0, 1};

    int x = (chunkX << 4) + 2;
    int z = (chunkZ << 4) + 2;

    uint64_t rng;
    if (mc <= MC_1_12_2) {
        rng = chunkGenerateRnd(seed, chunkX, chunkZ);
        next(&rng, 32);
    }

    uint64_t attempt = 0;

    int count, portal = 0;
    StrongholdPieceEnv env;
    do {
        count = 1;

        // reset pieces
        memset(&env, 0, sizeof(env));
        env.mc = mc;
        env.list = list;
        env.n = &count;
        env.rng = &rng;
        env.portal = &portal;
        env.imposedPiece = -1;
        env.ntyp[0] = 1;
        env.typlast = -1;
        env.nmax = n;
        env.totalWeight = 145;

        if (mc > MC_1_12_2) {
            rng = chunkGenerateRnd(seed + attempt++, chunkX, chunkZ);
        }

        Piece *p = list;
        p->type = SH_STAIRS_DOWN;
        p->name = stronghold_info[SH_STAIRS_DOWN].name;
        p->bb0 = p->bb1 = p->pos = (Pos3) {x, 64, z};
        int sizeX = stronghold_info[SH_STAIRS_DOWN].size.x;
        int sizeY = stronghold_info[SH_STAIRS_DOWN].size.y;
        int sizeZ = stronghold_info[SH_STAIRS_DOWN].size.z;
        int rotation = nextInt(&rng, 4);
        if (mc < MC_1_8) rotation = OLD_ROTATIONS[rotation];
        p->rot = rotation;
        switch (p->rot) {
        case 0: case 2:
            p->bb1.x += sizeX - 1; p->bb1.y += sizeY - 1; p->bb1.z += sizeZ - 1; break;
        case 1: case 3:
            p->bb1.x += sizeZ - 1; p->bb1.y += sizeY - 1; p->bb1.z += sizeX - 1; break;
        default: UNREACHABLE();
        }
        p->additionalData = 1;

        p->depth = 0;
        p->next = NULL;

        extendStrongholdPiece(&env, p);
        while (list->next && !env.generationStopped) {
            Piece *q = list;
            int len = 0;
            while (q->next) {
                q = q->next;
                len++;
            }
            int i = nextInt(&rng, len);
            for (p = list, q = list->next; i-->0; p = q, q = q->next);
            p->next = q->next;
            q->next = NULL;
            extendStrongholdPiece(&env, q);
        }

        if (mc > MC_1_12_2 && *env.portal) {
            int sz = 0; Piece *dq = list; while (dq->next) { dq = dq->next; sz++; }
            for (; sz >= 1; sz--) nextInt(&rng, sz);
        }
        if ((mc <= MC_1_12_2 && !env.portal) || (mc > MC_1_12_2 && *env.portal)) {
            int minY = p->bb0.y;
            int maxY = p->bb1.y;
            for (int i = 0; i < count; i++) {
                Piece q = list[i];
                int bMin = q.bb0.y;
                int bMax = q.bb1.y;
                minY = MIN(minY, bMin);
                maxY = MAX(maxY, bMax);
            }
            int height = maxY - minY + 1;

            int seaLevel = 63;
            int offset = 10;
            int minWorldY = mc < MC_1_18 ? 0 : -64;

            int maxAllowedY = seaLevel - offset;
            int k = height + minWorldY + 1;
            if (k < maxAllowedY) {
                k += nextInt(&rng, maxAllowedY - k);
            }

            int dy = k - maxY;
            for (int i = 0; i < count; i++) {
                Piece *q = &list[i];
                q->bb0.y += dy;
                q->bb1.y += dy;
                q->pos.y += dy;
            }
        }
    } while (!*env.portal);

    return *env.n;
}

static const Pos eye_positions[] = {
    {4, 8},
    {5, 8},
    {6, 8},
    {4, 12},
    {5, 12},
    {6, 12},
    {3, 9},
    {3, 10},
    {3, 11},
    {7, 9},
    {7, 10},
    {7, 11},
};

int getStrongholdLoot(Generator *g, SurfaceNoise *sn, Piece *list, int n, StructureSaltConfig ssconf, int mc, uint64_t seed, int chunkX, int chunkZ, DungeonRoomList *dungeonsOut) {
    if (g && (mc < MC_1_14 || mc > MC_1_16_5)) // carver/lake/mineshaft/dungeon logic is different in 1.17+ and 1.13-
        return -1;

    int count = getStrongholdPieces(list, n, mc, seed, chunkX, chunkZ);

    const int legacy = mc <= MC_1_17;
    int minX = list->bb0.x;
    int minZ = list->bb0.z;
    int maxX = list->bb1.x;
    int maxZ = list->bb1.z;
    for (int i = 0; i < count; ++i) {
        Piece *p = &list[i];
        minX = MIN(minX, p->bb0.x);
        minZ = MIN(minZ, p->bb0.z);
        maxX = MAX(maxX, p->bb1.x);
        maxZ = MAX(maxZ, p->bb1.z);
    }
    int cMinX = minX & ~15;
    int cMinZ = minZ & ~15;
    int cMaxX = maxX & ~15;
    int cMaxZ = maxZ & ~15;

    Pos3List mineshaftAir;
    createPos3List(&mineshaftAir, 64);
    if (g) {
        int sMinY = list[0].bb0.y, sMaxY = list[0].bb1.y;
        for (int i = 0; i < count; ++i) {
            sMinY = MIN(sMinY, list[i].bb0.y);
            sMaxY = MAX(sMaxY, list[i].bb1.y);
        }
        Piece *msPieces = (Piece*)malloc(2048 * sizeof(Piece));
        Piece *msLoot = (Piece*)malloc(2048 * sizeof(Piece));
        for (int rx = (minX >> 4) - 8; rx <= (maxX >> 4) + 8; ++rx) {
            for (int rz = (minZ >> 4) - 8; rz <= (maxZ >> 4) + 8; ++rz) {
                Pos mp;
                if (!getStructurePos(Mineshaft, mc, seed, rx, rz, &mp)) continue;
                if (!isViableStructurePos(Mineshaft, g, mp.x, mp.z, 0)) continue;
                int mn = getMineshaftPieces(g, msPieces, 2048, mc, seed, rx, rz);
                int hit = 0;
                for (int i = 0; i < mn; ++i) {
                    Piece *q = &msPieces[i];
                    if (q->bb1.x >= minX && q->bb0.x <= maxX &&
                        q->bb1.z >= minZ && q->bb0.z <= maxZ &&
                        q->bb1.y >= sMinY && q->bb0.y <= sMaxY) { hit = 1; break; }
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

    Pos3List lakeAirAll, lakeWaterAll;
    createPos3List(&lakeAirAll, 32);
    createPos3List(&lakeWaterAll, 32);

    int lcx0 = (minX - 1) >> 4, lcx1 = (maxX + 1) >> 4;
    int lcz0 = (minZ - 1) >> 4, lcz1 = (maxZ + 1) >> 4;
    int nchunks = (lcx1 - lcx0 + 1) * (lcz1 - lcz0 + 1);
    Pos3List *dungeonAirBySrc = NULL, *dungeonSolidBySrc = NULL;

    if (g) {
        int *chunkXs = (int*)malloc(nchunks * sizeof(int));
        int *chunkZs = (int*)malloc(nchunks * sizeof(int));
        int *cellToIdx = (int*)malloc(nchunks * sizeof(int));
        int k = 0;

        for (int cxc = lcx0; cxc <= lcx1; ++cxc) {
            for (int czc = lcz0; czc <= lcz1; ++czc) {
                chunkXs[k] = cxc << 4; chunkZs[k] = czc << 4;
                cellToIdx[k] = k;
                ++k;
            }
        }

        DungeonCarverCache *cc = (DungeonCarverCache*)calloc(nchunks, sizeof(DungeonCarverCache));
        uint8_t **lakeDetails = (uint8_t**)calloc(nchunks, sizeof(uint8_t*));

        dungeonAirBySrc = (Pos3List*)malloc(nchunks * sizeof(Pos3List));
        dungeonSolidBySrc = (Pos3List*)malloc(nchunks * sizeof(Pos3List));
        for (int q = 0; q < nchunks; ++q) {
            createPos3List(&dungeonAirBySrc[q], 4);
            createPos3List(&dungeonSolidBySrc[q], 4);
        }
        DungeonWorld dctx;
        dctx.g = g; dctx.sn = sn;
        dctx.mc = mc; dctx.seed = seed;
        dctx.lcx0 = lcx0; dctx.lcz0 = lcz0;
        dctx.ncx = lcx1 - lcx0 + 1; dctx.ncz = lcz1 - lcz0 + 1;
        dctx.cc = cc; dctx.chunkXs = chunkXs; dctx.chunkZs = chunkZs;
        dctx.cellToIdx = cellToIdx;
        dctx.lakeDetails = lakeDetails;
        dctx.dungeonDetails = (uint8_t**)calloc(nchunks, sizeof(uint8_t*));
        dctx.carverAirMask = (uint8_t**)calloc(nchunks, sizeof(uint8_t*));
        dctx.carverWaterMask = (uint8_t**)calloc(nchunks, sizeof(uint8_t*));
        dctx.mineshaftAirMask = (uint8_t**)calloc(nchunks, sizeof(uint8_t*));
        dctx.mineshaftAir = &mineshaftAir;
        dctx.dungeonAirBySrc = dungeonAirBySrc; dctx.dungeonSolidBySrc = dungeonSolidBySrc;
        dctx.pieces = list; dctx.pieceCount = count;

        for (int ci = 0; ci < nchunks; ++ci)
            dungeonSimChunk(&dctx, ci, &lakeAirAll, &lakeWaterAll, dungeonsOut);

        for (int q = 0; q < nchunks; ++q) {
            if (lakeDetails[q]) free(lakeDetails[q]);
            if (cc[q].valid) { freePos3List(&cc[q].air); freePos3List(&cc[q].water); }
            if (dctx.dungeonDetails[q]) free(dctx.dungeonDetails[q]);
            if (dctx.carverAirMask[q]) free(dctx.carverAirMask[q]);
            if (dctx.carverWaterMask[q]) free(dctx.carverWaterMask[q]);
            if (dctx.mineshaftAirMask[q]) free(dctx.mineshaftAirMask[q]);
        }
        free(dctx.dungeonDetails);
        free(dctx.carverAirMask);
        free(dctx.carverWaterMask);
        free(dctx.mineshaftAirMask);

        free(cc);
        free(lakeDetails);
        free(chunkXs);
        free(chunkZs);
        free(cellToIdx);
    }

    // slow code ahead
    for (int cx = cMinX; cx <= cMaxX; cx += 16) {
        for (int cz = cMinZ; cz <= cMaxZ; cz += 16) {

            uint8_t airMask[8192], waterMask[8192], dungeonAirMask[8192], dungeonSolidMask[8192];
            if (g) {
                Pos3List airList, waterList;
                createPos3List(&airList, 16);
                createPos3List(&waterList, 16);
                applyAllCarvers(g, sn, cx >> 4, cz >> 4, &airList, &waterList);
                for (int i = 0; i < mineshaftAir.size; ++i)
                    appendPos3List(&airList, mineshaftAir.pos3s[i]);
                for (int i = 0; i < lakeAirAll.size; ++i)
                    appendPos3List(&airList, lakeAirAll.pos3s[i]);
                for (int i = 0; i < lakeWaterAll.size; ++i)
                    appendPos3List(&waterList, lakeWaterAll.pos3s[i]);
                dungeonFillMask(airMask, &airList, cx, cz);
                dungeonFillMask(waterMask, &waterList, cx, cz);
                freePos3List(&airList);
                freePos3List(&waterList);

                // dungeon writes from chunks decorated after this one are not
                // visible to this chunk's STRONGHOLDS step
                int gidx = ((cx >> 4) - lcx0) * (lcz1 - lcz0 + 1) + ((cz >> 4) - lcz0);
                Pos3List dungeonAirList, dungeonSolidList;
                createPos3List(&dungeonAirList, 16);
                createPos3List(&dungeonSolidList, 16);
                for (int s = 0; s <= gidx && s < nchunks; ++s) {
                    for (int i = 0; i < dungeonAirBySrc[s].size; ++i)
                        appendPos3List(&dungeonAirList, dungeonAirBySrc[s].pos3s[i]);
                    for (int i = 0; i < dungeonSolidBySrc[s].size; ++i)
                        appendPos3List(&dungeonSolidList, dungeonSolidBySrc[s].pos3s[i]);
                }
                dungeonFillMask(dungeonAirMask, &dungeonAirList, cx, cz);
                dungeonFillMask(dungeonSolidMask, &dungeonSolidList, cx, cz);
                freePos3List(&dungeonAirList);
                freePos3List(&dungeonSolidList);
            } else {
                memset(airMask, 0, sizeof(airMask));
                memset(waterMask, 0, sizeof(waterMask));
                memset(dungeonAirMask, 0, sizeof(dungeonAirMask));
                memset(dungeonSolidMask, 0, sizeof(dungeonSolidMask));
            }

            CREATE_RANDOM_SOURCE(rnd, legacy);
            uint64_t populationSeed = getPopulationSeed(mc, seed, cx, cz);
            rnd.setSeed(rnd.state, populationSeed + ssconf.generationStep * 10000 + ssconf.decoratorIndex);
            for (int i = 0; i < count; ++i) {
                Piece *p = &list[i];
                if (!(p->bb1.x >= cx && p->bb0.x <= cx + 15 &&
                      p->bb1.z >= cz && p->bb0.z <= cz + 15)) {
                    continue;
                }
                switch (p->type) {
                case SH_STRAIGHT:
                    generateBox(p, cx, cz, 0, 0, 0, 4, 4, 6, 1, rnd, airMask, waterMask, dungeonAirMask, dungeonSolidMask);
                    rnd.nextFloat(rnd.state);
                    rnd.nextFloat(rnd.state);
                    rnd.nextFloat(rnd.state);
                    rnd.nextFloat(rnd.state);
                    p->chestCount = 0;
                    break;
                case SH_PRISON_HALL:
                    generateBox(p, cx, cz, 0, 0, 0, 8, 4, 10, 1, rnd, airMask, waterMask, dungeonAirMask, dungeonSolidMask);
                    rnd.skipN(rnd.state, 12);
                    // generateBox(p, cx, cz, 4, 1, 1, 4, 3, 1, 0, rnd);
                    // generateBox(p, cx, cz, 4, 1, 3, 4, 3, 3, 0, rnd);
                    // generateBox(p, cx, cz, 4, 1, 7, 4, 3, 7, 0, rnd);
                    // generateBox(p, cx, cz, 4, 1, 9, 4, 3, 9, 0, rnd);
                    p->chestCount = 0;
                    break;
                case SH_LEFT_TURN:
                case SH_RIGHT_TURN:
                    generateBox(p, cx, cz, 0, 0, 0, 4, 4, 4, 1, rnd, airMask, waterMask, dungeonAirMask, dungeonSolidMask);
                    p->chestCount = 0;
                    break;
                case SH_ROOM_CROSSING: {
                    generateBox(p, cx, cz, 0, 0, 0, 10, 6, 10, 1, rnd, airMask, waterMask, dungeonAirMask, dungeonSolidMask);
                    if (!p->additionalData) {
                        p->chestCount = 0;
                        break;
                    }

                    int chestPosX = 3, chestPosZ = 8;
                    rotPos(p->bb0, p->bb1, &chestPosX, &chestPosZ, p->rot);
                    if (chestPosX >= cx && chestPosX < cx + 16 && chestPosZ >= cz && chestPosZ < cz + 16) {
                        p->chestCount = 1;
                        p->chestPoses[0] = (Pos) {chestPosX, chestPosZ};
                        p->lootTables[0] = "stronghold_crossing";
                        p->lootSeeds[0] = rnd.nextLong(rnd.state);
                    }
                    break;
                }
                case SH_STRAIGHT_STAIRS_DOWN:
                    generateBox(p, cx, cz, 0, 0, 0, 4, 10, 7, 1, rnd, airMask, waterMask, dungeonAirMask, dungeonSolidMask);
                    p->chestCount = 0;
                    break;
                case SH_STAIRS_DOWN:
                    generateBox(p, cx, cz, 0, 0, 0, 4, 10, 4, 1, rnd, airMask, waterMask, dungeonAirMask, dungeonSolidMask);
                    p->chestCount = 0;
                    break;
                case SH_FIVE_CROSSING:
                    generateBox(p, cx, cz, 0, 0, 0, 9, 8, 10, 1, rnd, airMask, waterMask, dungeonAirMask, dungeonSolidMask);
                    rnd.skipN(rnd.state, 109);
                    // generateBox(p, cx, cz, 1, 2, 1, 8, 2, 6, 0, rnd);
                    // generateBox(p, cx, cz, 4, 1, 5, 4, 4, 9, 0, rnd);
                    // generateBox(p, cx, cz, 8, 1, 5, 8, 4, 9, 0, rnd);
                    // generateBox(p, cx, cz, 1, 4, 7, 3, 4, 9, 0, rnd);
                    // generateBox(p, cx, cz, 1, 3, 5, 3, 3, 6, 0, rnd);
                    // generateBox(p, cx, cz, 5, 1, 7, 7, 1, 8, 0, rnd);
                    p->chestCount = 0;
                    break;
                case SH_CHEST_CORRIDOR: {
                    generateBox(p, cx, cz, 0, 0, 0, 4, 4, 6, 1, rnd, airMask, waterMask, dungeonAirMask, dungeonSolidMask);
                    int chestPosX = 3, chestPosZ = 3;
                    rotPos(p->bb0, p->bb1, &chestPosX, &chestPosZ, p->rot);
                    if (chestPosX >= cx && chestPosX < cx + 16 && chestPosZ >= cz && chestPosZ < cz + 16) {
                        p->chestCount = 1;
                        p->chestPoses[0] = (Pos) {chestPosX, chestPosZ};
                        p->lootTables[0] = "stronghold_corridor";
                        p->lootSeeds[0] = rnd.nextLong(rnd.state);
                    }
                    break;
                }
                case SH_LIBRARY: {
                    int isTall = p->bb1.y - p->bb0.y + 1 > 6;
                    int currentHeight;
                    if (isTall) {
                        currentHeight = 11;
                        p->chestCount = 2;
                    } else {
                        currentHeight = 6;
                        p->chestCount = 1;
                    }

                    generateBox(p, cx, cz, 0, 0, 0, 13, currentHeight - 1, 14, 1, rnd, airMask, waterMask, dungeonAirMask, dungeonSolidMask);
                    generateMaybeBox(2, 1, 1, 11, 4, 13, rnd);
                    int chestPosX = 3, chestPosZ = 5;
                    rotPos(p->bb0, p->bb1, &chestPosX, &chestPosZ, p->rot);
                    if (chestPosX >= cx && chestPosX < cx + 16 && chestPosZ >= cz && chestPosZ < cz + 16) {
                        p->chestPoses[0] = (Pos) {chestPosX, chestPosZ};
                        p->lootTables[0] = "stronghold_library";
                        p->lootSeeds[0] = rnd.nextLong(rnd.state);
                    }
                    if (isTall) {
                        chestPosX = 12, chestPosZ = 1;
                        rotPos(p->bb0, p->bb1, &chestPosX, &chestPosZ, p->rot);
                        if (chestPosX >= cx && chestPosX < cx + 16 && chestPosZ >= cz && chestPosZ < cz + 16) {
                            p->chestPoses[1] = (Pos) {chestPosX, chestPosZ};
                            p->lootTables[1] = "stronghold_library";
                            p->lootSeeds[1] = rnd.nextLong(rnd.state);
                        }
                    }
                    break;
                }
                case SH_PORTAL_ROOM:
                    // the famous 760 skips
                    rnd.skipN(rnd.state, 760);
                    // generateBox(p, cx, cz, 0, 0, 0, 10, 7, 15, 0, rnd);
                    // generateBox(p, cx, cz, 1, 6, 1, 1, 6, 14, 0, rnd);
                    // generateBox(p, cx, cz, 9, 6, 1, 9, 6, 14, 0, rnd);
                    // generateBox(p, cx, cz, 2, 6, 1, 8, 6, 2, 0, rnd);
                    // generateBox(p, cx, cz, 2, 6, 14, 8, 6, 14, 0, rnd);
                    // generateBox(p, cx, cz, 1, 1, 1, 2, 1, 4, 0, rnd);
                    // generateBox(p, cx, cz, 8, 1, 1, 9, 1, 4, 0, rnd);
                    // generateBox(p, cx, cz, 3, 1, 8, 7, 1, 12, 0, rnd);
                    // generateBox(p, cx, cz, 4, 1, 5, 6, 1, 7, 0, rnd);
                    // generateBox(p, cx, cz, 4, 2, 6, 6, 2, 7, 0, rnd);
                    // generateBox(p, cx, cz, 4, 3, 7, 6, 3, 7, 0, rnd);

                    for (int j = 0; j < 12; j++) {
                        if (rnd.nextFloat(rnd.state) > 0.9F) {
                            Pos relPos = eye_positions[j];
                            int eyePosX = relPos.x, eyePosZ = relPos.z;
                            rotPos(p->bb0, p->bb1, &eyePosX, &eyePosZ, p->rot);
                            if (eyePosX >= cx && eyePosX < cx + 16 && eyePosZ >= cz && eyePosZ < cz + 16) {
                                p->additionalData |= (1 << j);
                            }
                        }
                    }
                    p->chestCount = 0;
                    break;
                case SH_FILLER_CORRIDOR:
                    p->chestCount = 0;
                    break;
                default: UNREACHABLE();
                }
            }
        }
    }
    if (dungeonAirBySrc) {
        for (int q = 0; q < nchunks; ++q) {
            freePos3List(&dungeonAirBySrc[q]);
            freePos3List(&dungeonSolidBySrc[q]);
        }
        free(dungeonAirBySrc);
        free(dungeonSolidBySrc);
    }
    freePos3List(&mineshaftAir);
    freePos3List(&lakeAirAll);
    freePos3List(&lakeWaterAll);
    return count;
}

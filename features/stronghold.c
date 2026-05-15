#include "stronghold.h"

#include <string.h>

#include "../finders.h"

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

L_box_end:

    Piece *p = env->list + *env->n;
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

        // necessary for <=1.12.2 to simulate random calls
        // optional for >1.12.2, could add flag for accurate heights
        if (mc <= MC_1_12_2 && !env.portal) {
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

static inline void rotPos(Pos3 bb0, Pos3 bb1, int *x, int *z, int rot) {
    int posX, posZ;
    switch (rot) {
    case 0: posX = bb0.x + *x, posZ = bb1.z - *z; break;
    case 1: posX = bb0.x + *z, posZ = bb0.z + *x; break;
    case 2: posX = bb0.x + *x, posZ = bb0.z + *z; break;
    case 3: posX = bb1.x - *z, posZ = bb0.z + *x; break;
    default: UNREACHABLE();
    }
    *x = posX, *z = posZ;
}

static void generateBox(Piece *p, int cx, int cz, int x0, int y0, int z0, int x1, int y1, int z1, int skipAir, RandomSource rnd) {
    if (!skipAir) {
        int w = x1 - x0 + 1;
        int d = z1 - z0 + 1;
        int h = y1 - y0 + 1;
        int skips = w * d * h;
        if (!(w == 1 || d == 1 || h == 1)) {
            skips -= (w - 2) * (d - 2) * (h - 2);
        }
        rnd.skipN(rnd.state, skips);
        return;
    }

    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            for (int z = z0; z <= z1; z++) {
                int tx = x, tz = z;
                rotPos(p->bb0, p->bb1, &tx, &tz, p->rot);
                if (tx >= cx && tx < cx + 16 && tz >= cz && tz < cz + 16) {
                    if (y == y0 || y == y1 || x == x0 || x == x1 || z == z0 || z == z1) {
                        rnd.nextFloat(rnd.state);
                    }
                }
            }
        }
    }
}

ATTR(always_inline)
static inline void generateMaybeBox(int x0, int y0, int z0, int x1, int y1, int z1, RandomSource rnd) {
    rnd.skipN(rnd.state, (y1-y0+1) * (x1-x0+1) * (z1-z0+1));
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

int getStrongholdLoot(Piece *list, int n, StructureSaltConfig ssconf, int mc, uint64_t seed, int chunkX, int chunkZ) {
    int count = getStrongholdPieces(list, n, mc, seed, chunkX, chunkZ);
    if (!count) {
        return 0;
    }
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

    // slow code ahead
    for (int cx = cMinX; cx <= cMaxX; cx += 16) {
        for (int cz = cMinZ; cz <= cMaxZ; cz += 16) {
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
                    generateBox(p, cx, cz, 0, 0, 0, 4, 4, 6, 1, rnd);
                    rnd.nextFloat(rnd.state);
                    rnd.nextFloat(rnd.state);
                    rnd.nextFloat(rnd.state);
                    rnd.nextFloat(rnd.state);
                    p->chestCount = 0;
                    break;
                case SH_PRISON_HALL:
                    generateBox(p, cx, cz, 0, 0, 0, 8, 4, 10, 1, rnd);
                    rnd.skipN(rnd.state, 12);
                    // generateBox(p, cx, cz, 4, 1, 1, 4, 3, 1, 0, rnd);
                    // generateBox(p, cx, cz, 4, 1, 3, 4, 3, 3, 0, rnd);
                    // generateBox(p, cx, cz, 4, 1, 7, 4, 3, 7, 0, rnd);
                    // generateBox(p, cx, cz, 4, 1, 9, 4, 3, 9, 0, rnd);
                    p->chestCount = 0;
                    break;
                case SH_LEFT_TURN:
                case SH_RIGHT_TURN:
                    generateBox(p, cx, cz, 0, 0, 0, 4, 4, 4, 1, rnd);
                    p->chestCount = 0;
                    break;
                case SH_ROOM_CROSSING: {
                    generateBox(p, cx, cz, 0, 0, 0, 10, 6, 10, 1, rnd);
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
                    generateBox(p, cx, cz, 0, 0, 0, 4, 10, 7, 1, rnd);
                    p->chestCount = 0;
                    break;
                case SH_STAIRS_DOWN:
                    generateBox(p, cx, cz, 0, 0, 0, 4, 10, 4, 1, rnd);
                    p->chestCount = 0;
                    break;
                case SH_FIVE_CROSSING:
                    generateBox(p, cx, cz, 0, 0, 0, 9, 8, 10, 1, rnd);
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
                    generateBox(p, cx, cz, 0, 0, 0, 4, 4, 6, 1, rnd);
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

                    generateBox(p, cx, cz, 0, 0, 0, 13, currentHeight - 1, 14, 1, rnd);
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
    return count;
}

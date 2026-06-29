#include "mineshaft.h"

#include <string.h>

#include "piece.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

STRUCT(MineshaftPieceEnv) {
    int mc;
    Piece *list;
    int *n;
    uint64_t *rng;
    int nmax;
};

static inline Piece* hasCollision(MineshaftPieceEnv *env, Pos3 b0, Pos3 b1) {
    int i, n = *env->n;
    for (i = 0; i < n; i++) {
        Piece *q = env->list + i;
        if (hasIntersection(q->bb0, q->bb1, b0, b1)) {
            return q;
        }
    }
    return NULL;
}

static void extendMineshaftPiece(MineshaftPieceEnv *env, Piece *piece);

static void extendMineshaft(MineshaftPieceEnv *env, int x, int y, int z, int facing, int depth) {
    if (depth > 8) {
        return;
    }

    if (IABS(x - env->list->bb0.x) > 80 || IABS(z - env->list->bb0.z) > 80) {
        return;
    }

    depth += 1;

    int randomSelection = nextInt(env->rng, 100);
    if (randomSelection >= 80) {
        int y1 = 2 + 4 * (nextInt(env->rng, 4) == 0);
        Pos3 bb0, bb1;
        switch (facing) {
        case 0: bb0 = (Pos3) {x + -1, y, z + -4}; bb1 = (Pos3) {x + 3, y + y1, z + 0}; break;
        case 1: bb0 = (Pos3) {x + 0, y, z + -1}; bb1 = (Pos3) {x + 4, y + y1, z + 3}; break;
        case 2: bb0 = (Pos3) {x + -1, y, z + 0}; bb1 = (Pos3) {x + 3, y + y1, z + 4}; break;
        case 3: bb0 = (Pos3) {x + -4, y, z + -1}; bb1 = (Pos3) {x + 0, y + y1, z + 3}; break;
        default: UNREACHABLE();
        }
        if (!hasCollision(env, bb0, bb1)) {
            Piece *p = env->list + (*env->n)++;
            p->name = "MSCrossing";
            p->pos = bb0;
            p->bb0 = bb0;
            p->bb1 = bb1;
            p->rot = facing;
            p->depth = depth;
            p->type = MS_CROSSING;
            p->next = NULL;
            extendMineshaftPiece(env, p);
            return;
        }
    } else if (randomSelection >= 70) {
        Pos3 bb0, bb1;
        switch (facing) {
        case 0: bb0 = (Pos3) {x + 0, y + -5, z + -8}; bb1 = (Pos3) {x + 2, y + 2, z + 0}; break;
        case 1: bb0 = (Pos3) {x + 0, y + -5, z + 0}; bb1 = (Pos3) {x + 8, y + 2, z + 2}; break;
        case 2: bb0 = (Pos3) {x + 0, y + -5, z + 0}; bb1 = (Pos3) {x + 2, y + 2, z + 8}; break;
        case 3: bb0 = (Pos3) {x + -8, y + -5, z + 0}; bb1 = (Pos3) {x + 0, y + 2, z + 2}; break;
        default: UNREACHABLE();
        }
        if (!hasCollision(env, bb0, bb1)) {
            Piece *p = env->list + (*env->n)++;
            p->name = "MSStairs";
            p->pos = bb0;
            p->bb0 = bb0;
            p->bb1 = bb1;
            p->rot = facing;
            p->depth = depth;
            p->type = MS_STAIRS;
            p->next = NULL;
            extendMineshaftPiece(env, p);
            return;
        }
    } else {
        for (int corridorLength = nextInt(env->rng, 3) + 2; corridorLength > 0; corridorLength--) {
            int blockLength = corridorLength * 5;

            Pos3 bb0, bb1;
            switch (facing) {
            case 0: bb0 = (Pos3) {x + 0, y, z + -(blockLength - 1)}; bb1 = (Pos3) {x + 2, y + 2, z + 0}; break;
            case 1: bb0 = (Pos3) {x + 0, y, z + 0}; bb1 = (Pos3) {x + blockLength - 1, y + 2, z + 2}; break;
            case 2: bb0 = (Pos3) {x + 0, y, z + 0}; bb1 = (Pos3) {x + 2, y + 2, z + blockLength - 1}; break;
            case 3: bb0 = (Pos3) {x + -(blockLength - 1), y, z + 0}; bb1 = (Pos3) {x + 0, y + 2, z + 2}; break;
            default: UNREACHABLE();
            }
            if (!hasCollision(env, bb0, bb1)) {
                Piece *p = env->list + (*env->n)++;
                p->name = "MSCorridor";
                p->pos = bb0;
                p->bb0 = bb0;
                p->bb1 = bb1;
                p->rot = facing;
                p->depth = depth;
                p->type = MS_CORRIDOR;
                p->next = NULL;

                int hasRails = nextInt(env->rng, 3) == 0;
                p->additionalData |= hasRails << 0;
                p->additionalData |= (!hasRails && nextInt(env->rng, 23) == 0) << 1; // spiderCorridor

                extendMineshaftPiece(env, p);
                return;
            }
        } // TODO check corridorLength > 0 check in older versions
    }
}

static void extendMineshaftPiece(MineshaftPieceEnv *env, Piece *piece) {
    if (*env->n >= env->nmax) {
        return;
    }
    switch (piece->type) {
    case MS_CORRIDOR: {
        int endSelection = nextInt(env->rng, 4);
        int rot = piece->rot;
        switch (rot) {
        case 0:
            if (endSelection <= 1) {
                extendMineshaft(env, piece->bb0.x, piece->bb0.y - 1 + nextInt(env->rng, 3), piece->bb0.z - 1, 0, piece->depth);
            } else if (endSelection == 2) {
                extendMineshaft(env, piece->bb0.x - 1, piece->bb0.y - 1 + nextInt(env->rng, 3), piece->bb0.z, 3, piece->depth);
            } else {
                extendMineshaft(env, piece->bb1.x + 1, piece->bb0.y - 1 + nextInt(env->rng, 3), piece->bb0.z, 1, piece->depth);
            }
            break;
        case 1:
            if (endSelection <= 1) {
                extendMineshaft(env, piece->bb1.x + 1, piece->bb0.y - 1 + nextInt(env->rng, 3), piece->bb0.z, 1, piece->depth);
            } else if (endSelection == 2) {
                extendMineshaft(env, piece->bb1.x - 3, piece->bb0.y - 1 + nextInt(env->rng, 3), piece->bb0.z - 1, 0, piece->depth);
            } else {
                extendMineshaft(env, piece->bb1.x - 3, piece->bb0.y - 1 + nextInt(env->rng, 3), piece->bb1.z + 1, 2, piece->depth);
            }
            break;
        case 2:
            if (endSelection <= 1) {
                extendMineshaft(env, piece->bb0.x, piece->bb0.y - 1 + nextInt(env->rng, 3), piece->bb1.z + 1, 2, piece->depth);
            } else if (endSelection == 2) {
                extendMineshaft(env, piece->bb0.x - 1, piece->bb0.y - 1 + nextInt(env->rng, 3), piece->bb1.z - 3, 3, piece->depth);
            } else {
                extendMineshaft(env, piece->bb1.x + 1, piece->bb0.y - 1 + nextInt(env->rng, 3), piece->bb1.z - 3, 1, piece->depth);
            }
            break;
        case 3:
            if (endSelection <= 1) {
                extendMineshaft(env, piece->bb0.x - 1, piece->bb0.y - 1 + nextInt(env->rng, 3), piece->bb0.z, 3, piece->depth);
            } else if (endSelection == 2) {
                extendMineshaft(env, piece->bb0.x, piece->bb0.y - 1 + nextInt(env->rng, 3), piece->bb0.z - 1, 0, piece->depth);
            } else {
                extendMineshaft(env, piece->bb0.x, piece->bb0.y - 1 + nextInt(env->rng, 3), piece->bb1.z + 1, 2, piece->depth);
            }
            break;
        default: UNREACHABLE();
        }

        if (piece->depth >= 8) {
            break;
        }
        if (rot != 0 && rot != 2) {
            for (int x = piece->bb0.x + 3; x + 3 <= piece->bb1.x; x += 5) {
                int selection = nextInt(env->rng, 5);
                if (selection == 0) {
                    extendMineshaft(env, x, piece->bb0.y, piece->bb0.z - 1, 0, piece->depth + 1);
                } else if (selection == 1) {
                    extendMineshaft(env, x, piece->bb0.y, piece->bb1.z + 1, 2, piece->depth + 1);
                }
            }
        } else {
            for (int z = piece->bb0.z + 3; z + 3 <= piece->bb1.z; z += 5) {
                int selection = nextInt(env->rng, 5);
                if (selection == 0) {
                    extendMineshaft(env, piece->bb0.x - 1, piece->bb0.y, z, 3, piece->depth + 1);
                } else if (selection == 1) {
                    extendMineshaft(env, piece->bb1.x + 1, piece->bb0.y, z, 1, piece->depth + 1);
                }
            }
        }
        break;
    }
    case MS_CROSSING: {
        switch (piece->rot) {
        case 0:
            extendMineshaft(env, piece->bb0.x + 1, piece->bb0.y, piece->bb0.z - 1, 0, piece->depth);
            extendMineshaft(env, piece->bb0.x - 1, piece->bb0.y, piece->bb0.z + 1, 3, piece->depth);
            extendMineshaft(env, piece->bb1.x + 1, piece->bb0.y, piece->bb0.z + 1, 1, piece->depth);
            break;
        case 1:
            extendMineshaft(env, piece->bb0.x + 1, piece->bb0.y, piece->bb0.z - 1, 0, piece->depth);
            extendMineshaft(env, piece->bb0.x + 1, piece->bb0.y, piece->bb1.z + 1, 2, piece->depth);
            extendMineshaft(env, piece->bb1.x + 1, piece->bb0.y, piece->bb0.z + 1, 1, piece->depth);
            break;
        case 2:
            extendMineshaft(env, piece->bb0.x + 1, piece->bb0.y, piece->bb1.z + 1, 2, piece->depth);
            extendMineshaft(env, piece->bb0.x - 1, piece->bb0.y, piece->bb0.z + 1, 3, piece->depth);
            extendMineshaft(env, piece->bb1.x + 1, piece->bb0.y, piece->bb0.z + 1, 1, piece->depth);
            break;
        case 3:
            extendMineshaft(env, piece->bb0.x + 1, piece->bb0.y, piece->bb0.z - 1, 0, piece->depth);
            extendMineshaft(env, piece->bb0.x + 1, piece->bb0.y, piece->bb1.z + 1, 2, piece->depth);
            extendMineshaft(env, piece->bb0.x - 1, piece->bb0.y, piece->bb0.z + 1, 3, piece->depth);
            break;
        default: UNREACHABLE();
        }

        int isTwoFloored = piece->bb1.y - piece->bb0.y + 1 > 3;
        if (!isTwoFloored) {
            break;
        }
        if (next(env->rng, 1)) {
            extendMineshaft(env, piece->bb0.x + 1, piece->bb0.y + 3 + 1, piece->bb0.z - 1, 0, piece->depth);
        }
        if (next(env->rng, 1)) {
            extendMineshaft(env, piece->bb0.x - 1, piece->bb0.y + 3 + 1, piece->bb0.z + 1, 3, piece->depth);
        }
        if (next(env->rng, 1)) {
            extendMineshaft(env, piece->bb1.x + 1, piece->bb0.y + 3 + 1, piece->bb0.z + 1, 1, piece->depth);
        }
        if (next(env->rng, 1)) {
            extendMineshaft(env, piece->bb0.x + 1, piece->bb0.y + 3 + 1, piece->bb1.z + 1, 2, piece->depth);
        }
        break;
    }
    case MS_ROOM: {
        int heightSpace = (piece->bb1.y - piece->bb0.y + 1) - 3 - 1;
        if (heightSpace <= 0) {
            heightSpace = 1;
        }

        int xSpan = piece->bb1.x - piece->bb0.x + 1;
        for (int pos = 0; pos < xSpan; pos += 4) {
            pos += nextInt(env->rng, xSpan);
            if (pos + 3 > xSpan) {
                break;
            }
            extendMineshaft(env, piece->bb0.x + pos, piece->bb0.y + nextInt(env->rng, heightSpace) + 1, piece->bb0.z - 1, 0, piece->depth);
        }

        for (int pos = 0; pos < xSpan; pos += 4) {
            pos += nextInt(env->rng, xSpan);
            if (pos + 3 > xSpan) {
                break;
            }
            extendMineshaft(env, piece->bb0.x + pos, piece->bb0.y + nextInt(env->rng, heightSpace) + 1, piece->bb1.z + 1, 2, piece->depth);
        }

        int zSpan = piece->bb1.z - piece->bb0.z + 1;
        for (int pos = 0; pos < zSpan; pos += 4) {
            pos += nextInt(env->rng, zSpan);
            if (pos + 3 > zSpan) {
                break;
            }
            extendMineshaft(env, piece->bb0.x - 1, piece->bb0.y + nextInt(env->rng, heightSpace) + 1, piece->bb0.z + pos, 3, piece->depth);
        }

        for (int pos = 0; pos < zSpan; pos += 4) {
            pos += nextInt(env->rng, zSpan);
            if (pos + 3 > zSpan) {
                break;
            }
            extendMineshaft(env, piece->bb1.x + 1, piece->bb0.y + nextInt(env->rng, heightSpace) + 1, piece->bb0.z + pos, 1, piece->depth);
        }
        break;
    }
    case MS_STAIRS: {
        switch (piece->rot) {
        case 0:
            extendMineshaft(env, piece->bb0.x, piece->bb0.y, piece->bb0.z - 1, 0, piece->depth);
            break;
        case 1:
            extendMineshaft(env, piece->bb1.x + 1, piece->bb0.y, piece->bb0.z, 1, piece->depth);
            break;
        case 2:
            extendMineshaft(env, piece->bb0.x, piece->bb0.y, piece->bb1.z + 1, 2, piece->depth);
            break;
        case 3:
            extendMineshaft(env, piece->bb0.x - 1, piece->bb0.y, piece->bb0.z, 3, piece->depth);
            break;
        default: UNREACHABLE();
        }
        break;
    }
    default: UNREACHABLE();
    }
}

int getMineshaftPieces(Piece *list, int n, int mc, uint64_t seed, int chunkX, int chunkZ) {
    int x = (chunkX << 4) + 2;
    int z = (chunkZ << 4) + 2;

    uint64_t rng = chunkGenerateRnd(seed, chunkX, chunkZ);
    if (mc > MC_1_18_2) {
        nextDouble(&rng);
    }

    int count = 1;

    MineshaftPieceEnv env;
    memset(&env, 0, sizeof(env));
    env.mc = mc;
    env.list = list;
    env.n = &count;
    env.rng = &rng;
    env.nmax = n;

    Piece *p = list;
    p->type = MS_ROOM;
    p->name = "MSRoom";

    p->bb0 = p->bb1 = p->pos = (Pos3) {x, 50, z};
    p->bb1.x += 7 + nextInt(&rng, 6);
    p->bb1.y += 4 + nextInt(&rng, 6);
    p->bb1.z += 7 + nextInt(&rng, 6);

    p->depth = 0;
    p->next = NULL;

    extendMineshaftPiece(&env, p);
    return count;
}

static int isSupportingBox(Piece *p, int cx, int cz, int x0, int x1, int z0) {
    for (int x = x0; x <= x1; x++) {
        int tx = x, tz = z0;
        rotPos(p->bb0, p->bb1, &tx, &tz, p->rot);
        if (tx < cx || tx >= cx + 16 || tz < cz || tz >= cz + 16) {
            return 0;
        }
    }

    return 1;
}

static void placeSupport(Piece *p, int cx, int cz, int x0, int z, int x1, RandomSource rnd) {
    if (isSupportingBox(p, cx, cz, x0, x1, z)) {
        if (rnd.nextInt(rnd.state, 4) != 0) {
            maybeGenerateBlock(rnd);
            maybeGenerateBlock(rnd);
        }
    }
}

static void maybePlaceCobWeb(Piece *p, int cx, int cz, RandomSource rnd, int x, int z) {
    rotPos(p->bb0, p->bb1, &x, &z, p->rot);
    if (x >= cx && x < cx + 16 && z >= cz && z < cz + 16) {
        rnd.nextFloat(rnd.state);
    }
}

int getMineshaftLoot(Piece *list, int n, StructureSaltConfig ssconf, int mc, uint64_t seed, int chunkX, int chunkZ) {
    int count = getMineshaftPieces(list, n, mc, seed, chunkX, chunkZ);

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

        p->chestCount = 0;
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
                case MS_CORRIDOR: {
                    // isInInvalidLocation ignored
                    int numSections; // TODO cache?
                    if (p->rot == 0 || p->rot == 2) {
                        numSections = (p->bb1.z - p->bb0.z + 1) / 5;
                    } else {
                        numSections = (p->bb1.x - p->bb0.x + 1) / 5;
                    }
                    int length = numSections * 5 - 1;
                    generateMaybeBox(0, 2, 0, 2, 2, length, rnd);
                    if ((p->additionalData >> 1) & 1) {
                        generateMaybeBox(0, 0, 0, 2, 1, length, rnd);
                    }

                    for (int section = 0; section < numSections; section++) {
                        int z = 2 + section * 5;
                        placeSupport(p, cx, cz, 0, z, 2, rnd);
                        maybePlaceCobWeb(p, cx, cz, rnd, 0, z - 1);
                        maybePlaceCobWeb(p, cx, cz, rnd, 2, z - 1);
                        maybePlaceCobWeb(p, cx, cz, rnd, 0, z + 1);
                        maybePlaceCobWeb(p, cx, cz, rnd, 2, z + 1);
                        maybePlaceCobWeb(p, cx, cz, rnd, 0, z - 2);
                        maybePlaceCobWeb(p, cx, cz, rnd, 2, z - 2);
                        maybePlaceCobWeb(p, cx, cz, rnd, 0, z + 2);
                        maybePlaceCobWeb(p, cx, cz, rnd, 2, z + 2);

                        if (rnd.nextInt(rnd.state, 100) == 0) {
                            int chestPosX = 2, chestPosZ = z - 1;
                            rotPos(p->bb0, p->bb1, &chestPosX, &chestPosZ, p->rot);
                            if (chestPosX >= cx && chestPosX < cx + 16 && chestPosZ >= cz && chestPosZ < cz + 16) {
                                rnd.nextBoolean(rnd.state);
                                p->chestPoses[p->chestCount] = (Pos) {chestPosX, chestPosZ};
                                p->lootTables[p->chestCount] = "abandoned_mineshaft";
                                p->lootSeeds[p->chestCount] = rnd.nextLong(rnd.state);
                                p->chestCount++;
                            }
                        }

                        if (rnd.nextInt(rnd.state, 100) == 0) {
                            int chestPosX = 0, chestPosZ = z + 1;
                            rotPos(p->bb0, p->bb1, &chestPosX, &chestPosZ, p->rot);
                            if (chestPosX >= cx && chestPosX < cx + 16 && chestPosZ >= cz && chestPosZ < cz + 16) {
                                rnd.nextBoolean(rnd.state);
                                p->chestPoses[p->chestCount] = (Pos) {chestPosX, chestPosZ};
                                p->lootTables[p->chestCount] = "abandoned_mineshaft";
                                p->lootSeeds[p->chestCount] = rnd.nextLong(rnd.state);
                                p->chestCount++;
                            }
                        }

                        // this.spiderCorridor && !this.hasPlacedSpider
                        // spiderCorridor is 0b_X_, hasPlacedSpider is 0bX__
                        if (((p->additionalData >> 1) & 0b11) == 0b01) {
                            int newX = 1;
                            int newZ = z - 1 + rnd.nextInt(rnd.state, 3);
                            rotPos(p->bb0, p->bb1, &newX, &newZ, p->rot);
                            if (newX >= cx && newX < cx + 16 && newZ >= cz && newZ < cz + 16) {
                                p->additionalData |= 1 << 2;
                                // spawner.setEntityId(EntityType.CAVE_SPIDER, random);
                            }
                        }
                    }

                    // this.hasRails
                    if ((p->additionalData >> 0) & 1) {
                        for (int zx = 0; zx <= length; zx++) {
                            int tx = 1, tz = zx;
                            rotPos(p->bb0, p->bb1, &tx, &tz, p->rot);
                            if (tx >= cx && tx < cx + 16 && tz >= cz && tz < cz + 16) {
                                maybeGenerateBlock(rnd);
                            }
                        }
                    }
                    break;
                }
                case MS_CROSSING:
                case MS_ROOM:
                case MS_STAIRS:
                    break;
                default: UNREACHABLE();
                }
            }
        }
    }

    return count;
}

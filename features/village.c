#include "village.h"

#include "../loot/items.h"

#include <stdlib.h>

STRUCT(VillagePieceList) {
    int count;
    Piece *items;
};

STRUCT(VillagePieceWeight) {
    int pieceType, villagePieceWeight, villagePiecesSpawned, villagePiecesLimit;
};

STRUCT(VillagePieceEnv) {
    Piece base;
    Generator *generator;
    uint64_t rng;
    VillagePieceList pendingRoads, pendingHouses;
    VillagePieceList *allPieces;
    VillagePieceWeight pieceWeights[HOUSE_NUM];
    int pieceWeightCount, structVillagePieceWeightIndex;
};

STRUCT(VillageLootEntry) {
    int type, min, max, weight;
};

static const struct {
    const Pos3 size;
    const int weight;
    const char *name;
} village_info[] = {
    {{ 6, 15,  6},  0, "ViStart"}, // WELL
    {{ 3,  3,  0},  0, "ViSR"   }, // PATH
    {{ 5,  6,  5},  4, "ViSH"   }, // HOUSE_4_GARDEN
    {{ 5, 12,  9}, 20, "ViST"   }, // CHURCH
    {{ 9,  9,  6}, 20, "ViBH"   }, // HOUSE_1
    {{ 4,  6,  5},  3, "ViSmH"  }, // WOOD_HUT
    {{ 9,  7, 11}, 15, "ViPH"   }, // HALL
    {{13,  4,  9},  3, "ViDF"   }, // FIELD_1
    {{ 7,  4,  9},  3, "ViF"    }, // FIELD_2
    {{10,  6,  7}, 15, "ViS"    }, // HOUSE_2 (blacksmith)
    {{ 9,  7, 12},  8, "ViTRH"  }, // HOUSE_3
    {{ 3,  4,  2},  0, "ViL"    }, // TORCH
};

static inline void addComponent(VillagePieceList *list, Piece piece) {
    if (list->count >= VILLAGE_PIECES_MAX) return;
    list->items[list->count++] = piece;
}

static inline Piece removeComponentAt(VillagePieceList *list, int index) {
    Piece removed = list->items[index];
    for (int i = index; i < list->count - 1; i++) {
        list->items[i] = list->items[i + 1];
    }
    list->count--;
    return removed;
}

static inline int intersectsAny(const VillagePieceList *list, Pos3 b0, Pos3 b1) {
    for (int i = 0; i < list->count; i++) {
        if (hasIntersection(b0, b1, list->items[i].bb0, list->items[i].bb1)) {
            return 1;
        }
    }
    return 0;
}

static inline int getPieceLength(const Piece *p) {
    int sx = p->bb1.x - p->bb0.x + 1;
    int sz = p->bb1.z - p->bb0.z + 1;
    return sx > sz ? sx : sz;
}

static inline void addWeight(VillagePieceWeight *weights, int *count, int piece, int weight, int limit) {
    if (limit > 0) {
        weights[*count] = (VillagePieceWeight){piece, weight, 0, limit};
        (*count)++;
    }
}

static void getVillageWeights(uint64_t *rng, VillagePieceWeight *weights, int *weightCount,
    int mc, uint64_t seed, int chunkX, int chunkZ) {
    int limits[HOUSE_NUM];
    *rng = getHouseList(limits, mc, seed, chunkX, chunkZ);
    int count = 0;
    for (int i = 0; i < HOUSE_NUM; i++) {
        int type = VILLAGE_HOUSE_4_GARDEN + i;
        addWeight(weights, &count, type, village_info[type].weight, limits[i]);
    }
    *weightCount = count;
}

static void getVillageBox(int x, int z, int type, int length, int facing, Pos3 *b0, Pos3 *b1) {
    Pos3 size = village_info[type].size;
    if (type == VILLAGE_PATH) size.z = length;
    orientBox((Pos3){x, 64, z}, (Pos3){0, 0, 0}, size, facing, b0, b1);
}

static int generateAndAddHousePiece(VillagePieceEnv *start, int x, int z, int dir, Piece *outPiece);
static int generateAndAddRoadPiece(VillagePieceEnv *start, int x, int z, int dir);

static int createPiece(VillagePieceEnv *start, VillagePieceWeight *weight, int x, int z, int dir, Piece *outPiece) {
    int type = weight->pieceType;
    *outPiece = (Piece){0};
    outPiece->type = type;
    outPiece->name = village_info[type].name;
    outPiece->rot = dir;
    outPiece->pos = (Pos3){x, 64, z};
    orientBox(outPiece->pos, (Pos3){0, 0, 0}, village_info[type].size, dir, &outPiece->bb0, &outPiece->bb1);
    if (intersectsAny(start->allPieces, outPiece->bb0, outPiece->bb1)) {
        return 0;
    }

    switch (weight->pieceType) {
    case VILLAGE_HOUSE_4_GARDEN: skipNextN(&start->rng, 1); break;
    case VILLAGE_WOOD_HUT: skipNextN(&start->rng, 2); break;
    case VILLAGE_FIELD_1:
    case VILLAGE_FIELD_2:
        // wheat-only farms do not choose crops before 1.4
        if (start->generator->mc < MC_1_4) break;
        skipNextN(&start->rng, type == VILLAGE_FIELD_1 ? 4 : 2);
        break;
    default: break;
    }

    return 1;
}

static int pickWeightedHouse(VillagePieceEnv *start, int x, int z, int dir, Piece *outPiece) {
    int totalWeight = 0;
    int validPiecesCount = 0;

    for (int i = 0; i < start->pieceWeightCount; i++) {
        VillagePieceWeight *weight = &start->pieceWeights[i];
        if (weight->villagePiecesLimit > 0 && weight->villagePiecesSpawned < weight->villagePiecesLimit) {
            totalWeight += weight->villagePieceWeight;
            validPiecesCount++;
        }
    }
    if (validPiecesCount == 0 || totalWeight <= 0) {
        return 0;
    }

    for (int j = 0; j < 5; j++) {
        int choice = nextInt(&start->rng, totalWeight);

        for (int i = 0; i < start->pieceWeightCount; i++) {
            VillagePieceWeight *weight = &start->pieceWeights[i];

            if (weight->villagePiecesLimit > 0 && weight->villagePiecesSpawned < weight->villagePiecesLimit) {
                choice -= weight->villagePieceWeight;
                if (choice < 0) {
                    int isDuplicate = (i == start->structVillagePieceWeightIndex && validPiecesCount > 1);
                    if (isDuplicate) {
                        break;
                    }
                    if (createPiece(start, weight, x, z, dir, outPiece)) {
                        weight->villagePiecesSpawned++;
                        start->structVillagePieceWeightIndex = i;
                        return 1;
                    }
                }
            }
        }
    }
    Pos3 b0, b1;
    getVillageBox(x, z, VILLAGE_TORCH, 0, dir, &b0, &b1);
    if (!intersectsAny(start->allPieces, b0, b1)) {
        *outPiece = (Piece){0};
        outPiece->name = village_info[VILLAGE_TORCH].name;
        outPiece->pos = (Pos3){x, 64, z};
        outPiece->type = VILLAGE_TORCH;
        outPiece->rot = dir;
        outPiece->bb0 = b0;
        outPiece->bb1 = b1;
        return 1;
    }
    return 0;
}

static int checkBiomeViability(VillagePieceEnv *start, Pos3 b0, Pos3 b1) {
    // from 1.10, only the village origin must be in a village biome
    if (start->generator->mc >= MC_1_10) return 1;
    int centerX = (b0.x + b1.x) / 2;
    int centerZ = (b0.z + b1.z) / 2;
    int k = b1.x - b0.x;
    int l = b1.z - b0.z;
    int radius = ((k > l) ? k : l) / 2 + 4;

    int minX4 = (centerX - radius) >> 2;
    int maxX4 = (centerX + radius) >> 2;
    int minZ4 = (centerZ - radius) >> 2;
    int maxZ4 = (centerZ + radius) >> 2;

    int sx = maxX4 - minX4 + 1;
    int sz = maxZ4 - minZ4 + 1;

    Range r = {4, minX4, minZ4, sx, sz, 16, 1};
    int *ids = allocCache(start->generator, r);
    if (!ids) return 0;

    int viable = 1;
    if (genBiomes(start->generator, ids, r) == 0) {
        for (int i = 0; i < sx * sz; i++) {
            if (!isViableFeatureBiome(start->generator->mc, Village, ids[i])) {
                viable = 0;
                break;
            }
        }
    } else {
        viable = 0;
    }

    free(ids);
    return viable;
}

static int generateAndAddHousePiece(VillagePieceEnv *start, int x, int z, int dir, Piece *outPiece) {
    if (abs(x - start->base.bb0.x) > 112 || abs(z - start->base.bb0.z) > 112) return 0;

    Piece piece;
    if (pickWeightedHouse(start, x, z, dir, &piece)) {
        if (checkBiomeViability(start, piece.bb0, piece.bb1)) {
            addComponent(start->allPieces, piece);
            addComponent(&start->pendingHouses, piece);

            if (outPiece) *outPiece = piece;
            return 1;
        }
    }
    return 0;
}

static int generateAndAddRoadPiece(VillagePieceEnv *start, int x, int z, int dir) {
    if (abs(x - start->base.bb0.x) > 112 || abs(z - start->base.bb0.z) > 112) return 0;

    Pos3 b0, b1;
    int found = 0;
    int length = nextIntBetween(&start->rng, 3, 5) * 7;

    for (int i = length; i >= 7; i -= 7) {
        getVillageBox(x, z, VILLAGE_PATH, i, dir, &b0, &b1);
        if (!intersectsAny(start->allPieces, b0, b1)) {
            found = 1;
            break;
        }
    }

    if (!found) return 0;

    if (!checkBiomeViability(start, b0, b1)) {
        return 0;
    }

    Piece path = {0};
    path.type = VILLAGE_PATH;
    path.name = village_info[VILLAGE_PATH].name;
    path.pos = (Pos3){x, 64, z};
    path.bb0 = b0;
    path.bb1 = b1;
    path.rot = dir;

    addComponent(start->allPieces, path);
    addComponent(&start->pendingRoads, path);
    return 1;
}

static int getNextComponentNN(VillagePieceEnv *start, const Piece *comp, int offset, Piece *outPiece) {
    switch (comp->rot) {
    case 0:
    case 2: return generateAndAddHousePiece(start, comp->bb0.x - 1, comp->bb0.z + offset, 3, outPiece);
    case 3:
    case 1: return generateAndAddHousePiece(start, comp->bb0.x + offset, comp->bb0.z - 1, 0, outPiece);
    default: UNREACHABLE();
    }
}

static int getNextComponentPP(VillagePieceEnv *start, const Piece *comp, int offset, Piece *outPiece) {
    switch (comp->rot) {
    case 0:
    case 2: return generateAndAddHousePiece(start, comp->bb1.x + 1, comp->bb0.z + offset, 1, outPiece);
    case 3:
    case 1: return generateAndAddHousePiece(start, comp->bb0.x + offset, comp->bb1.z + 1, 2, outPiece);
    default: UNREACHABLE();
    }
}

static void buildPath(const Piece *comp, VillagePieceEnv *start) {
    int flag = 0;

    for (int i = nextInt(&start->rng, 5); i < getPieceLength(comp) - 8; i += 2 + nextInt(&start->rng, 5)) {
        Piece piece;
        if (getNextComponentNN(start, comp, i, &piece)) {
            int pLen = getPieceLength(&piece);
            i += pLen;
            flag = 1;
        }
    }
    for (int j = nextInt(&start->rng, 5); j < getPieceLength(comp) - 8; j += 2 + nextInt(&start->rng, 5)) {
        Piece piece;
        if (getNextComponentPP(start, comp, j, &piece)) {
            int pLen = getPieceLength(&piece);
            j += pLen;
            flag = 1;
        }
    }

    if (flag && nextInt(&start->rng, 3) > 0) {
        switch (comp->rot) {
        case 0: generateAndAddRoadPiece(start, comp->bb0.x - 1, comp->bb0.z, 3); break;
        case 2: generateAndAddRoadPiece(start, comp->bb0.x - 1, comp->bb1.z - 2, 3); break;
        case 3: generateAndAddRoadPiece(start, comp->bb0.x, comp->bb0.z - 1, 0); break;
        case 1: generateAndAddRoadPiece(start, comp->bb1.x - 2, comp->bb0.z - 1, 0); break;
        default: UNREACHABLE();
        }
    }
    if (flag && nextInt(&start->rng, 3) > 0) {
        switch (comp->rot) {
        case 0: generateAndAddRoadPiece(start, comp->bb1.x + 1, comp->bb0.z, 1); break;
        case 2: generateAndAddRoadPiece(start, comp->bb1.x + 1, comp->bb1.z - 2, 1); break;
        case 3: generateAndAddRoadPiece(start, comp->bb0.x, comp->bb1.z + 1, 2); break;
        case 1: generateAndAddRoadPiece(start, comp->bb1.x - 2, comp->bb1.z + 1, 2); break;
        default: UNREACHABLE();
        }
    }
}

static void buildComponent(const Piece *comp, VillagePieceEnv *startPiece) {
    switch (comp->type) {
    case VILLAGE_WELL:
        generateAndAddRoadPiece(startPiece, comp->bb0.x - 1, comp->bb0.z + 1, 3);
        generateAndAddRoadPiece(startPiece, comp->bb1.x + 1, comp->bb0.z + 1, 1);
        generateAndAddRoadPiece(startPiece, comp->bb0.x + 1, comp->bb0.z - 1, 0);
        generateAndAddRoadPiece(startPiece, comp->bb0.x + 1, comp->bb1.z + 1, 2);
        break;
    case VILLAGE_PATH:
        buildPath(comp, startPiece);
        break;
    default: break;
    }
}

static void buildComponents(VillagePieceEnv *startPiece) {
    while (startPiece->pendingRoads.count > 0 || startPiece->pendingHouses.count > 0) {
        if (startPiece->pendingRoads.count == 0) {
            int i = nextInt(&startPiece->rng, startPiece->pendingHouses.count);
            Piece currentPiece = removeComponentAt(&startPiece->pendingHouses, i);
            buildComponent(&currentPiece, startPiece);
        } else {
            int i = nextInt(&startPiece->rng, startPiece->pendingRoads.count);
            Piece currentPiece = removeComponentAt(&startPiece->pendingRoads, i);
            buildComponent(&currentPiece, startPiece);
        }
    }
}

int getVillagePieces(Piece *list, Generator *g, int chunkX, int chunkZ) {
    static const int OLD_ROTATIONS[] = {2, 3, 0, 1};

    int mc = g->mc;
    uint64_t seed = g->seed;
    if (mc < MC_1_0 || mc >= MC_1_14) return -1;

    VillagePieceEnv env = {0};
    VillagePieceList pieces = {0, list};
    env.pendingRoads.items = malloc(VILLAGE_PIECES_MAX * sizeof(Piece));
    env.pendingHouses.items = malloc(VILLAGE_PIECES_MAX * sizeof(Piece));
    if (!env.pendingRoads.items || !env.pendingHouses.items) {
        free(env.pendingRoads.items);
        free(env.pendingHouses.items);
        return 0;
    }
    env.allPieces = &pieces;
    env.generator = g;
    env.base.type = VILLAGE_WELL;
    env.base.name = village_info[VILLAGE_WELL].name;
    env.base.bb0 = env.base.pos = (Pos3){(chunkX << 4) + 2, 64, (chunkZ << 4) + 2};
    orientBox(env.base.pos, (Pos3){0, 0, 0}, village_info[VILLAGE_WELL].size, 2, &env.base.bb0, &env.base.bb1);

    getVillageWeights(&env.rng, env.pieceWeights, &env.pieceWeightCount, mc, seed, chunkX, chunkZ);
    env.structVillagePieceWeightIndex = -1;
    int rotation = nextInt(&env.rng, 4);
    if (mc < MC_1_8) rotation = OLD_ROTATIONS[rotation];
    env.base.rot = rotation;
    // abandoned villages were added in 1.10
    if (mc >= MC_1_10) nextInt(&env.rng, 50);
    addComponent(&pieces, env.base);
    buildComponent(&env.base, &env);
    buildComponents(&env);

    free(env.pendingRoads.items);
    free(env.pendingHouses.items);
    return pieces.count;
}

// Loot simulation
static const VillageLootEntry blacksmith_loot[] = {
    {ITEM_DIAMOND, 1, 3, 3},
    {ITEM_IRON_INGOT, 1, 5, 10},
    {ITEM_GOLD_INGOT, 1, 3, 5},
    {ITEM_BREAD, 1, 3, 15},
    {ITEM_APPLE, 1, 3, 15},
    {ITEM_IRON_PICKAXE, 1, 1, 5},
    {ITEM_IRON_SWORD, 1, 1, 5},
    {ITEM_IRON_CHESTPLATE, 1, 1, 5},
    {ITEM_IRON_HELMET, 1, 1, 5},
    {ITEM_IRON_LEGGINGS, 1, 1, 5},
    {ITEM_IRON_BOOTS, 1, 1, 5},
    {ITEM_OBSIDIAN, 3, 7, 5},
    {ITEM_OAK_SAPLING, 3, 7, 5},
    {ITEM_SADDLE, 1, 1, 3},
    {ITEM_IRON_HORSE_ARMOR, 1, 1, 1},
    {ITEM_GOLDEN_HORSE_ARMOR, 1, 1, 1},
    {ITEM_DIAMOND_HORSE_ARMOR, 1, 1, 1}
};

static void generateBlacksmithChest(uint64_t *rng, int rolls, VillageItemStack *inventory) {
    for (int i = 0; i < rolls; i++) {
        int weightRoll = nextInt(rng, 94);
        const VillageLootEntry *selected = NULL;

        for (int j = 0; j < 17; j++) {
            weightRoll -= blacksmith_loot[j].weight;
            if (weightRoll < 0) {
                selected = &blacksmith_loot[j];
                break;
            }
        }

        if (selected == NULL) continue;

        const int amount = nextIntBetween(rng, selected->min, selected->max);
        const int slot = nextInt(rng, 27);
        inventory[slot].type = selected->type;
        inventory[slot].count = amount;
    }
}

void getVillageBlacksmithInventory(const Piece *list, int count, uint64_t seed, VillageItemStack *inventory) {
    int chestX = 0, chestZ = 0;
    int foundBlacksmith = 0;

    for (int i = 0; i < count; i++) {
        const Piece *comp = &list[i];
        if (comp->type == VILLAGE_HOUSE_2) {
            switch (comp->rot) {
            case 0: chestX = comp->bb0.x + 5; chestZ = comp->bb1.z - 5; break;
            case 2: chestX = comp->bb0.x + 5; chestZ = comp->bb0.z + 5; break;
            case 3: chestX = comp->bb1.x - 5; chestZ = comp->bb0.z + 5; break;
            case 1: chestX = comp->bb0.x + 5; chestZ = comp->bb0.z + 5; break;
            default: break;
            }
            foundBlacksmith = 1;
            break;
        }
    }

    if (!foundBlacksmith) {
        for (int i = 0; i < 27; i++) {
            inventory[i].type = ITEM_AIR;
            inventory[i].count = 0;
        }
        return;
    }

    int chestChunkX = (chestX - 8) >> 4;
    int chestChunkZ = (chestZ - 8) >> 4;
    uint64_t populationSeed = getPopulationSeed(MC_1_8_9, seed, chestChunkX, chestChunkZ);
    uint64_t popRng;
    setSeed(&popRng, populationSeed);

    for (int j = 0; j < count; j++) {
        const Piece *c = &list[j];
        if (c->type == VILLAGE_HOUSE_2) break;
        if (c->type == VILLAGE_FIELD_1 || c->type == VILLAGE_FIELD_2) {
            int minX = c->bb0.x;
            int maxX = c->bb1.x;
            int minZ = c->bb0.z;
            int maxZ = c->bb1.z;
            if (maxX < (chestChunkX << 4) + 8 || minX > (chestChunkX << 4) + 23 ||
                maxZ < (chestChunkZ << 4) + 8 || minZ > (chestChunkZ << 4) + 23) {
                continue;
            }

            skipNextN(&popRng, c->type == VILLAGE_FIELD_1 ? 56 : 28);
        }
    }

    for (int i = 0; i < 27; i++) {
        inventory[i].type = ITEM_AIR;
        inventory[i].count = 0;
    }

    int rolls = 3 + nextInt(&popRng, 6);
    generateBlacksmithChest(&popRng, rolls, inventory);
}

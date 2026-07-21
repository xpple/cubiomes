#include "abandoned_camp.h"

int getAbandonedCampPieces(Piece *list, StructureSaltConfig ssconf, StructureVariant *sv, int mc, uint64_t seed, int chunkX, int chunkZ) {
    int minBlockX = chunkX << 4;
    int minBlockZ = chunkZ << 4;
    Piece* tent = &list[0];
    // there are way too many abandoned camp variations, the caller can construct the full string themselves
    // using snprintf and mixing caller and callee freeing in getStructurePieces would be a bad idea
    tent->name = "abandoned_camp/tent/%s/tent_%s_%d";

    // you'd hope this wasn't handwritten
    switch (sv->biome) {
    case bamboo_jungle:
        switch (sv->start) {
        case 1:
        case 3:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 2:
            tent->chestCount = 3;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_barrel";
            tent->lootTables[2] = "abandoned_camp_barrel";
            break;
        case 4:
        case 10:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_barrel";
            break;
        case 5:
        case 8:
        case 9:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 6:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            tent->lootTables[1] = "abandoned_camp_common_chest";
            break;
        case 7:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            tent->lootTables[1] = "abandoned_camp_barrel";
            break;
        default: UNREACHABLE();
        }
        break;
    case birch_forest:
        switch (sv->start) {
        case 1:
        case 2:
        case 4:
        case 5:
        case 9:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 3:
        case 6:
        case 7:
        case 8:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 10:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_barrel";
            break;
        default: UNREACHABLE();
        }
        break;
    case cherry_grove:
        switch (sv->start) {
        case 1:
        case 2:
        case 4:
        case 5:
        case 7:
        case 8:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 3:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 6:
            tent->chestCount = 3;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            tent->lootTables[1] = "abandoned_camp_common_chest";
            tent->lootTables[2] = "abandoned_camp_common_chest";
            break;
        case 9:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            tent->lootTables[1] = "abandoned_camp_common_chest";
            break;
        case 10:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_barrel";
            break;
        default: UNREACHABLE();
        }
        break;
    case dappled_forest:
        switch (sv->start) {
        case 1:
        case 3:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 2:
            tent->chestCount = 3;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_barrel";
            tent->lootTables[2] = "abandoned_camp_barrel";
            break;
        case 4:
        case 10:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_barrel";
            break;
        case 5:
        case 7:
        case 8:
        case 9:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 6:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_common_chest";
            break;
        default: UNREACHABLE();
        }
        break;
    case flower_forest:
        switch (sv->start) {
        case 1:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 8:
        case 9:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 7:
        case 10:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_barrel";
            break;
        default: UNREACHABLE();
        }
        break;
    case forest:
        switch (sv->start) {
        case 1:
        case 2:
        case 3:
        case 10:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 4:
            tent->chestCount = 3;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_barrel";
            tent->lootTables[2] = "abandoned_camp_barrel";
            break;
        case 5:
        case 6:
        case 8:
        case 9:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 7:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_barrel";
            break;
        default: UNREACHABLE();
        }
        break;
    case meadow:
        switch (sv->start) {
        case 1:
        case 3:
        case 7:
        case 10:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 2:
            tent->chestCount = 4;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_barrel";
            tent->lootTables[2] = "abandoned_camp_barrel";
            tent->lootTables[3] = "abandoned_camp_barrel";
            break;
        case 4:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_barrel";
            break;
        case 5:
        case 6:
        case 8:
        case 9:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_barrel";
            break;
        default: UNREACHABLE();
        }
        break;
    case old_growth_birch_forest:
        switch (sv->start) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_barrel";
            break;
        default: UNREACHABLE();
        }
        break;
    case old_growth_pine_taiga:
        switch (sv->start) {
        case 1:
        case 3:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 2:
        case 4:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_barrel";
            break;
        case 5:
        case 6:
        case 8:
        case 9:
        case 10:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 7:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            tent->lootTables[1] = "abandoned_camp_barrel";
            break;
        default: UNREACHABLE();
        }
        break;
    case old_growth_spruce_taiga:
        switch (sv->start) {
        case 1:
        case 2:
        case 3:
        case 9:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 4:
        case 10:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_barrel";
            break;
        case 5:
        case 6:
        case 8:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 7:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            tent->lootTables[1] = "abandoned_camp_barrel";
            break;
        default: UNREACHABLE();
        }
        break;
    case pale_garden:
        switch (sv->start) {
        case 1:
        case 5:
        case 6:
        case 8:
        case 9:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 2:
        case 3:
        case 7:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 4:
        case 10:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_barrel";
            break;
        default: UNREACHABLE();
        }
        break;
    case savanna:
        switch (sv->start) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_barrel";
            break;
        default: UNREACHABLE();
        }
        break;
    case snowy_taiga:
        switch (sv->start) {
        case 1:
        case 4:
        case 10:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 2:
            tent->chestCount = 3;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_barrel";
            tent->lootTables[2] = "abandoned_camp_barrel";
            break;
        case 3:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_barrel";
            break;
        default: UNREACHABLE();
        }
        break;
    case sparse_jungle:
        switch (sv->start) {
        case 1:
        case 3:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 2:
        case 4:
        case 10:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_barrel";
            break;
        case 5:
        case 8:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 6:
            tent->chestCount = 3;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            tent->lootTables[1] = "abandoned_camp_common_chest";
            tent->lootTables[2] = "abandoned_camp_common_chest";
            break;
        case 7:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            tent->lootTables[1] = "abandoned_camp_barrel";
            break;
        case 9:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            tent->lootTables[1] = "abandoned_camp_common_chest";
            break;
        default: UNREACHABLE();
        }
        break;
    case swamp:
        switch (sv->start) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 6:
        case 8:
        case 9:
        case 10:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 7:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            tent->lootTables[1] = "abandoned_camp_barrel";
            break;
        default: UNREACHABLE();
        }
        break;
    case taiga:
        switch (sv->start) {
        case 1:
        case 3:
        case 5:
        case 6:
        case 7:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 2:
        case 4:
        case 8:
        case 9:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 10:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_barrel";
            break;
        default: UNREACHABLE();
        }
        break;
    case windswept_forest:
        switch (sv->start) {
        case 1:
        case 4:
        case 6:
        case 7:
        case 9:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 2:
        case 10:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_barrel";
            break;
        case 3:
        case 5:
        case 8:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            break;
        default: UNREACHABLE();
        }
        break;
    case wooded_badlands:
        switch (sv->start) {
        case 1:
            tent->chestCount = 4;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_barrel";
            tent->lootTables[2] = "abandoned_camp_barrel";
            tent->lootTables[3] = "abandoned_camp_barrel";
            break;
        case 2:
        case 3:
        case 5:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 4:
        case 10:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_barrel";
            tent->lootTables[1] = "abandoned_camp_barrel";
            break;
        case 6:
        case 7:
        case 8:
            tent->chestCount = 1;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 9:
            tent->chestCount = 2;
            tent->lootTables[0] = "abandoned_camp_common_chest";
            tent->lootTables[1] = "abandoned_camp_common_chest";
            break;
        default: UNREACHABLE();
        }
        break;
    default: UNREACHABLE();
    }

    // assume the chests all fall in the same chunk
    int middlePosX, middlePosZ;
    switch (sv->rotation) {
    case 0: middlePosX = minBlockX + 8; middlePosZ = minBlockZ + 8; break; // 0
    case 1: middlePosX = minBlockX - 8; middlePosZ = minBlockZ + 8; break; // 90
    case 2: middlePosX = minBlockX - 8; middlePosZ = minBlockZ - 8; break; // 180
    case 3: middlePosX = minBlockX + 8; middlePosZ = minBlockZ - 8; break; // 270
    default: UNREACHABLE();
    }

    CREATE_RANDOM_SOURCE(rnd, 0);
    uint64_t populationSeed = getPopulationSeed(mc, seed, middlePosX & ~15, middlePosZ & ~15);
    rnd.setSeed(rnd.state, populationSeed + ssconf.generationStep * 10000 + ssconf.decoratorIndex);
    for (int i = 0; i < tent->chestCount; ++i) {
        tent->lootSeeds[i] = rnd.nextLong(rnd.state);
        tent->chestPoses[i] = (Pos) {middlePosX, middlePosZ};
    }

    Piece* camp = &list[1];
    camp->name = "abandoned_camp/camp/%s/campsite_%s_%d";
    camp->chestCount = 0; // TODO

    return 2;
}

#include "abandoned_camp.h"

int getAbandonedCampPieces(Piece *list, StructureSaltConfig ssconf, StructureVariant *sv, int mc, uint64_t seed, int chunkX, int chunkZ) {
    int minBlockX = chunkX << 4;
    int minBlockZ = chunkZ << 4;
    Piece* tent = &list[0];
    tent->type = sv->start;
    // TODO maybe write the name in full?
    // would make file a _lot_ longer
    switch (sv->start) {
    case 1: tent->name = "abandoned_camp/tent/%s/tent_%s_1"; break;
    case 2: tent->name = "abandoned_camp/tent/%s/tent_%s_2"; break;
    case 3: tent->name = "abandoned_camp/tent/%s/tent_%s_3"; break;
    case 4: tent->name = "abandoned_camp/tent/%s/tent_%s_4"; break;
    case 5: tent->name = "abandoned_camp/tent/%s/tent_%s_5"; break;
    case 6: tent->name = "abandoned_camp/tent/%s/tent_%s_6"; break;
    case 7: tent->name = "abandoned_camp/tent/%s/tent_%s_7"; break;
    case 8: tent->name = "abandoned_camp/tent/%s/tent_%s_8"; break;
    case 9: tent->name = "abandoned_camp/tent/%s/tent_%s_9"; break;
    case 10: tent->name = "abandoned_camp/tent/%s/tent_%s_10"; break;
    default: UNREACHABLE();
    }

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

    // assume the tent chests all fall in the same chunk
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
    uint64_t rng = chunkGenerateRnd(seed, chunkX, chunkZ);
    rng = (rng * 205749139540585ULL + 277363943098ULL) & MASK48; // skip nextInt(4) and nextInt(10)
    int campCount = 3 * 15 + 3;
    int positions[campCount];
    for (int i = 0; i < campCount; i++) {
        positions[i] = i;
    }
    for (int i = campCount; i > 1; --i) {
        const int swapTo = nextInt(&rng, i);
        int tmp = positions[swapTo];
        positions[swapTo] = positions[i - 1];
        positions[i - 1] = tmp;
    }
    int campIdx = 1 + positions[0];
    camp->type = campIdx;
    if (campIdx <= 15) {
        switch (campIdx) {
        case 1:
            camp->name = "abandoned_camp/camp/default/campsite_default_chest_1";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 2:
            camp->name = "abandoned_camp/camp/default/campsite_default_chest_2";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 3:
            camp->name = "abandoned_camp/camp/default/campsite_default_chest_3";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 4:
            camp->name = "abandoned_camp/camp/default/campsite_default_chest_4";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 5:
            camp->name = "abandoned_camp/camp/default/campsite_default_chest_5";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 6:
            camp->name = "abandoned_camp/camp/default/campsite_default_chest_6";
            camp->chestCount = 2;
            camp->lootTables[0] = "abandoned_camp_secret_chest";
            camp->lootTables[1] = "abandoned_camp_common_chest";
            break;
        case 7:
            camp->name = "abandoned_camp/camp/default/campsite_default_chest_7";
            camp->chestCount = 2;
            camp->lootTables[0] = "abandoned_camp_secret_chest";
            camp->lootTables[1] = "abandoned_camp_common_chest";
            break;
        case 8:
            camp->name = "abandoned_camp/camp/default/campsite_default_chest_8";
            camp->chestCount = 2;
            camp->lootTables[0] = "abandoned_camp_secret_chest";
            camp->lootTables[1] = "abandoned_camp_common_chest";
            break;
        case 9:
            camp->name = "abandoned_camp/camp/default/campsite_default_chest_9";
            camp->chestCount = 2;
            camp->lootTables[0] = "abandoned_camp_secret_chest";
            camp->lootTables[1] = "abandoned_camp_common_chest";
            break;
        case 10:
            camp->name = "abandoned_camp/camp/default/campsite_default_chest_10";
            camp->chestCount = 2;
            camp->lootTables[0] = "abandoned_camp_secret_chest";
            camp->lootTables[1] = "abandoned_camp_common_chest";
            break;
        case 11:
            camp->name = "abandoned_camp/camp/default/campsite_default_chest_11";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 12:
            camp->name = "abandoned_camp/camp/default/campsite_default_chest_12";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 13:
            camp->name = "abandoned_camp/camp/default/campsite_default_chest_13";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 14:
            camp->name = "abandoned_camp/camp/default/campsite_default_chest_14";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 15:
            camp->name = "abandoned_camp/camp/default/campsite_default_chest_15";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_common_chest";
            break;
        default: UNREACHABLE();
        }
    } else if (campIdx <= 30) {
        switch (campIdx - 15) {
        case 1:
            camp->name = "abandoned_camp/camp/default/campsite_default_barrel_1";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 2:
            camp->name = "abandoned_camp/camp/default/campsite_default_barrel_2";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 3:
            camp->name = "abandoned_camp/camp/default/campsite_default_barrel_3";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 4:
            camp->name = "abandoned_camp/camp/default/campsite_default_barrel_4";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 5:
            camp->name = "abandoned_camp/camp/default/campsite_default_barrel_5";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 6:
            camp->name = "abandoned_camp/camp/default/campsite_default_barrel_6";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 7:
            camp->name = "abandoned_camp/camp/default/campsite_default_barrel_7";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 8:
            camp->name = "abandoned_camp/camp/default/campsite_default_barrel_8";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 9:
            camp->name = "abandoned_camp/camp/default/campsite_default_barrel_9";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 10:
            camp->name = "abandoned_camp/camp/default/campsite_default_barrel_10";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 11:
            camp->name = "abandoned_camp/camp/default/campsite_default_barrel_11";
            camp->chestCount = 2;
            camp->lootTables[0] = "abandoned_camp_secret_chest";
            camp->lootTables[1] = "abandoned_camp_barrel";
            break;
        case 12:
            camp->name = "abandoned_camp/camp/default/campsite_default_barrel_12";
            camp->chestCount = 2;
            camp->lootTables[0] = "abandoned_camp_secret_chest";
            camp->lootTables[1] = "abandoned_camp_barrel";
            break;
        case 13:
            camp->name = "abandoned_camp/camp/default/campsite_default_barrel_13";
            camp->chestCount = 2;
            camp->lootTables[0] = "abandoned_camp_secret_chest";
            camp->lootTables[1] = "abandoned_camp_barrel";
            break;
        case 14:
            camp->name = "abandoned_camp/camp/default/campsite_default_barrel_14";
            camp->chestCount = 2;
            camp->lootTables[0] = "abandoned_camp_secret_chest";
            camp->lootTables[1] = "abandoned_camp_barrel";
            break;
        case 15:
            camp->name = "abandoned_camp/camp/default/campsite_default_barrel_15";
            camp->chestCount = 2;
            camp->lootTables[0] = "abandoned_camp_secret_chest";
            camp->lootTables[1] = "abandoned_camp_barrel";
            break;
        default: UNREACHABLE();
        }
        camp->chestCount = 1;
    } else if (campIdx <= 45) {
        switch (campIdx - 30) {
        case 1:
            camp->name = "abandoned_camp/camp/default/campsite_default_special_1";
            camp->chestCount = 2;
            camp->lootTables[0] = "abandoned_camp_secret_chest";
            camp->lootTables[1] = "abandoned_camp_common_chest";
            break;
        case 2:
            camp->name = "abandoned_camp/camp/default/campsite_default_special_2";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 3:
            camp->name = "abandoned_camp/camp/default/campsite_default_special_3";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 4:
            camp->name = "abandoned_camp/camp/default/campsite_default_special_4";
            camp->chestCount = 2;
            camp->lootTables[0] = "abandoned_camp_secret_chest";
            camp->lootTables[1] = "abandoned_camp_barrel";
            break;
        case 5:
            camp->name = "abandoned_camp/camp/default/campsite_default_special_5";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 6:
            camp->name = "abandoned_camp/camp/default/campsite_default_special_6";
            camp->chestCount = 2;
            camp->lootTables[0] = "abandoned_camp_secret_chest";
            camp->lootTables[1] = "abandoned_camp_barrel";
            break;
        case 7:
            camp->name = "abandoned_camp/camp/default/campsite_default_special_7";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 8:
            camp->name = "abandoned_camp/camp/default/campsite_default_special_8";
            camp->chestCount = 2;
            camp->lootTables[0] = "abandoned_camp_secret_chest";
            camp->lootTables[1] = "abandoned_camp_barrel";
            break;
        case 9:
            camp->name = "abandoned_camp/camp/default/campsite_default_special_9";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 10:
            camp->name = "abandoned_camp/camp/default/campsite_default_special_10";
            camp->chestCount = 2;
            camp->lootTables[0] = "abandoned_camp_common_chest";
            camp->lootTables[1] = "abandoned_camp_common_chest";
            break;
        case 11:
            camp->name = "abandoned_camp/camp/default/campsite_default_special_11";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_common_chest";
            break;
        case 12:
            camp->name = "abandoned_camp/camp/default/campsite_default_special_12";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 13:
            camp->name = "abandoned_camp/camp/default/campsite_default_special_13";
            camp->chestCount = 2;
            camp->lootTables[0] = "abandoned_camp_secret_chest";
            camp->lootTables[1] = "abandoned_camp_common_chest";
            break;
        case 14:
            camp->name = "abandoned_camp/camp/default/campsite_default_special_14";
            camp->chestCount = 1;
            camp->lootTables[0] = "abandoned_camp_barrel";
            break;
        case 15:
            camp->name = "abandoned_camp/camp/default/campsite_default_special_15";
            camp->chestCount = 2;
            camp->lootTables[0] = "abandoned_camp_barrel";
            camp->lootTables[1] = "abandoned_camp_common_chest";
            break;
        default: UNREACHABLE();
        }
    } else {
        int variant = campIdx - 45;
        switch (sv->biome) {
        case bamboo_jungle:
            switch (variant) {
            case 1:
                camp->name = "abandoned_camp/camp/bamboo_jungle/campsite_bamboo_jungle_1";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            case 2:
                camp->name = "abandoned_camp/camp/bamboo_jungle/campsite_bamboo_jungle_2";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_barrel";
                break;
            case 3:
                camp->name = "abandoned_camp/camp/bamboo_jungle/campsite_bamboo_jungle_3";
                camp->chestCount = 2;
                camp->lootTables[0] = "abandoned_camp_secret_chest";
                camp->lootTables[1] = "abandoned_camp_common_chest";
                break;
            default: UNREACHABLE();
            }
            break;
        case birch_forest:
            switch (variant) {
            case 1:
                camp->name = "abandoned_camp/camp/birch_forest/campsite_birch_forest_1";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            case 2:
                camp->name = "abandoned_camp/camp/birch_forest/campsite_birch_forest_2";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            case 3:
                camp->name = "abandoned_camp/camp/birch_forest/campsite_birch_forest_3";
                camp->chestCount = 3;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                camp->lootTables[1] = "abandoned_camp_barrel";
                camp->lootTables[2] = "abandoned_camp_barrel";
                break;
            default: UNREACHABLE();
            }
            break;
        case cherry_grove:
            switch (variant) {
            case 1:
                camp->name = "abandoned_camp/camp/cherry_grove/campsite_cherry_grove_1";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            case 2:
                camp->name = "abandoned_camp/camp/cherry_grove/campsite_cherry_grove_2";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_barrel";
                break;
            case 3:
                camp->name = "abandoned_camp/camp/cherry_grove/campsite_cherry_grove_3";
                camp->chestCount = 2;
                camp->lootTables[0] = "abandoned_camp_secret_chest";
                camp->lootTables[1] = "abandoned_camp_common_chest";
                break;
            default: UNREACHABLE();
            }
            break;
        case dappled_forest:
            switch (variant) {
            case 1:
                camp->name = "abandoned_camp/camp/dappled_forest/campsite_dappled_forest_1";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            case 2:
                camp->name = "abandoned_camp/camp/dappled_forest/campsite_dappled_forest_2";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_barrel";
                break;
            case 3:
                camp->name = "abandoned_camp/camp/dappled_forest/campsite_dappled_forest_3";
                camp->chestCount = 2;
                camp->lootTables[0] = "abandoned_camp_secret_chest";
                camp->lootTables[1] = "abandoned_camp_barrel";
                break;
            default: UNREACHABLE();
            }
            break;
        case flower_forest:
            switch (variant) {
            case 1:
                camp->name = "abandoned_camp/camp/flower_forest/campsite_flower_forest_1";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            case 2:
                camp->name = "abandoned_camp/camp/flower_forest/campsite_flower_forest_2";
                camp->chestCount = 2;
                camp->lootTables[0] = "abandoned_camp_secret_chest";
                camp->lootTables[1] = "abandoned_camp_common_chest";
                break;
            case 3:
                camp->name = "abandoned_camp/camp/flower_forest/campsite_flower_forest_3";
                camp->chestCount = 2;
                camp->lootTables[0] = "abandoned_camp_barrel";
                camp->lootTables[1] = "abandoned_camp_common_chest";
                break;
            default: UNREACHABLE();
            }
            break;
        case forest:
            switch (variant) {
            case 1:
                camp->name = "abandoned_camp/camp/forest/campsite_forest_1";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_barrel";
                break;
            case 2:
                camp->name = "abandoned_camp/camp/forest/campsite_forest_2";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            case 3:
                camp->name = "abandoned_camp/camp/forest/campsite_forest_3";
                camp->chestCount = 2;
                camp->lootTables[0] = "abandoned_camp_secret_chest";
                camp->lootTables[1] = "abandoned_camp_common_chest";
                break;
            default: UNREACHABLE();
            }
            break;
        case meadow:
            switch (variant) {
            case 1:
                camp->name = "abandoned_camp/camp/meadow/campsite_meadow_1";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            case 2:
                camp->name = "abandoned_camp/camp/meadow/campsite_meadow_2";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            case 3:
                camp->name = "abandoned_camp/camp/meadow/campsite_meadow_3";
                camp->chestCount = 2;
                camp->lootTables[0] = "abandoned_camp_secret_chest";
                camp->lootTables[1] = "abandoned_camp_barrel";
                break;
            default: UNREACHABLE();
            }
            break;
        case old_growth_birch_forest:
            switch (variant) {
            case 1:
                camp->name = "abandoned_camp/camp/old_growth_birch_forest/campsite_old_growth_birch_forest_1";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            case 2:
                camp->name = "abandoned_camp/camp/old_growth_birch_forest/campsite_old_growth_birch_forest_2";
                camp->chestCount = 3;
                camp->lootTables[0] = "abandoned_camp_secret_chest";
                camp->lootTables[1] = "abandoned_camp_barrel";
                camp->lootTables[2] = "abandoned_camp_barrel";
                break;
            case 3:
                camp->name = "abandoned_camp/camp/old_growth_birch_forest/campsite_old_growth_birch_forest_3";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            default: UNREACHABLE();
            }
            break;
        case old_growth_pine_taiga:
            switch (variant) {
            case 1:
                camp->name = "abandoned_camp/camp/old_growth_pine_taiga/campsite_old_growth_pine_taiga_1";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_barrel";
                break;
            case 2:
                camp->name = "abandoned_camp/camp/old_growth_pine_taiga/campsite_old_growth_pine_taiga_2";
                camp->chestCount = 2;
                camp->lootTables[0] = "abandoned_camp_secret_chest";
                camp->lootTables[1] = "abandoned_camp_common_chest";
                break;
            case 3:
                camp->name = "abandoned_camp/camp/old_growth_pine_taiga/campsite_old_growth_pine_taiga_3";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            default: UNREACHABLE();
            }
            break;
        case old_growth_spruce_taiga:
            switch (variant) {
            case 1:
                camp->name = "abandoned_camp/camp/old_growth_spruce_taiga/campsite_old_growth_spruce_taiga_1";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_barrel";
                break;
            case 2:
                camp->name = "abandoned_camp/camp/old_growth_spruce_taiga/campsite_old_growth_spruce_taiga_2";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_barrel";
                break;
            case 3:
                camp->name = "abandoned_camp/camp/old_growth_spruce_taiga/campsite_old_growth_spruce_taiga_3";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            default: UNREACHABLE();
            }
            break;
        case pale_garden:
            switch (variant) {
            case 1:
                camp->name = "abandoned_camp/camp/pale_garden/campsite_pale_garden_1";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_barrel";
                break;
            case 2:
                camp->name = "abandoned_camp/camp/pale_garden/campsite_pale_garden_2";
                camp->chestCount = 2;
                camp->lootTables[0] = "abandoned_camp_secret_chest";
                camp->lootTables[1] = "abandoned_camp_common_chest";
                break;
            case 3:
                camp->name = "abandoned_camp/camp/pale_garden/campsite_pale_garden_3";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            default: UNREACHABLE();
            }
            break;
        case savanna:
            switch (variant) {
            case 1:
                camp->name = "abandoned_camp/camp/savanna/campsite_savanna_1";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_barrel";
                break;
            case 2:
                camp->name = "abandoned_camp/camp/savanna/campsite_savanna_2";
                camp->chestCount = 2;
                camp->lootTables[0] = "abandoned_camp_secret_chest";
                camp->lootTables[1] = "abandoned_camp_barrel";
                break;
            case 3:
                camp->name = "abandoned_camp/camp/savanna/campsite_savanna_3";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            default: UNREACHABLE();
            }
            break;
        case snowy_taiga:
            switch (variant) {
            case 1:
                camp->name = "abandoned_camp/camp/snowy_taiga/campsite_snowy_taiga_1";
                camp->chestCount = 2;
                camp->lootTables[0] = "abandoned_camp_secret_chest";
                camp->lootTables[1] = "abandoned_camp_common_chest";
                break;
            case 2:
                camp->name = "abandoned_camp/camp/snowy_taiga/campsite_snowy_taiga_2";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            case 3:
                camp->name = "abandoned_camp/camp/snowy_taiga/campsite_snowy_taiga_3";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_barrel";
                break;
            default: UNREACHABLE();
            }
            break;
        case sparse_jungle:
            switch (variant) {
            case 1:
                camp->name = "abandoned_camp/camp/sparse_jungle/campsite_sparse_jungle_1";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            case 2:
                camp->name = "abandoned_camp/camp/sparse_jungle/campsite_sparse_jungle_2";
                camp->chestCount = 3;
                camp->lootTables[0] = "abandoned_camp_secret_chest";
                camp->lootTables[1] = "abandoned_camp_barrel";
                camp->lootTables[2] = "abandoned_camp_barrel";
                break;
            case 3:
                camp->name = "abandoned_camp/camp/sparse_jungle/campsite_sparse_jungle_3";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            default: UNREACHABLE();
            }
            break;
        case swamp:
            switch (variant) {
            case 1:
                camp->name = "abandoned_camp/camp/swamp/campsite_swamp_1";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            case 2:
                camp->name = "abandoned_camp/camp/swamp/campsite_swamp_2";
                camp->chestCount = 2;
                camp->lootTables[0] = "abandoned_camp_secret_chest";
                camp->lootTables[1] = "abandoned_camp_barrel";
                break;
            case 3:
                camp->name = "abandoned_camp/camp/swamp/campsite_swamp_3";
                camp->chestCount = 2;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                camp->lootTables[1] = "abandoned_camp_barrel";
                break;
            default: UNREACHABLE();
            }
            break;
        case taiga:
            switch (variant) {
            case 1:
                camp->name = "abandoned_camp/camp/taiga/campsite_taiga_1";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            case 2:
                camp->name = "abandoned_camp/camp/taiga/campsite_taiga_2";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_barrel";
                break;
            case 3:
                camp->name = "abandoned_camp/camp/taiga/campsite_taiga_3";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_barrel";
                break;
            default: UNREACHABLE();
            }
            break;
        case windswept_forest:
            switch (variant) {
            case 1:
                camp->name = "abandoned_camp/camp/windswept_forest/campsite_windswept_forest_1";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_barrel";
                break;
            case 2:
                camp->name = "abandoned_camp/camp/windswept_forest/campsite_windswept_forest_2";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            case 3:
                camp->name = "abandoned_camp/camp/windswept_forest/campsite_windswept_forest_3";
                camp->chestCount = 2;
                camp->lootTables[0] = "abandoned_camp_secret_chest";
                camp->lootTables[1] = "abandoned_camp_barrel";
                break;
            default: UNREACHABLE();
            }
            break;
        case wooded_badlands:
            switch (variant) {
            case 1:
                camp->name = "abandoned_camp/camp/wooded_badlands/campsite_wooded_badlands_1";
                camp->chestCount = 2;
                camp->lootTables[0] = "abandoned_camp_barrel";
                camp->lootTables[1] = "abandoned_camp_barrel";
                break;
            case 2:
                camp->name = "abandoned_camp/camp/wooded_badlands/campsite_wooded_badlands_2";
                camp->chestCount = 1;
                camp->lootTables[0] = "abandoned_camp_common_chest";
                break;
            case 3:
                camp->name = "abandoned_camp/camp/wooded_badlands/campsite_wooded_badlands_3";
                camp->chestCount = 2;
                camp->lootTables[0] = "abandoned_camp_secret_chest";
                camp->lootTables[1] = "abandoned_camp_barrel";
                break;
            default: UNREACHABLE();
            }
            break;
        default: UNREACHABLE();
        }
    }

    // assume the camp chests all fall in the same chunk, but not in the same chunk the tent generated in
    // instead, assume the camp generates with offset (-16, 0)
    int campPosX, campPosZ;
    switch (sv->rotation) {
    case 0: campPosX = middlePosX + -16; campPosZ = middlePosZ + 0; break;
    case 1: campPosX = middlePosX - 0; campPosZ = middlePosZ + -16; break;
    case 2: campPosX = middlePosX - -16; campPosZ = middlePosZ - 0; break;
    case 3: campPosX = middlePosX + 0; campPosZ = middlePosZ - -16; break;
    default: UNREACHABLE();
    }

    populationSeed = getPopulationSeed(mc, seed, campPosX & ~15, campPosZ & ~15);
    rnd.setSeed(rnd.state, populationSeed + ssconf.generationStep * 10000 + ssconf.decoratorIndex);
    for (int i = 0; i < camp->chestCount; ++i) {
        camp->lootSeeds[i] = rnd.nextLong(rnd.state);
        camp->chestPoses[i] = (Pos) {campPosX, campPosZ};
    }

    return 2;
}

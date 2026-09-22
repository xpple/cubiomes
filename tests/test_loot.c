#include "testing.h"
#include "../finders.h"
#include "../loot/items.h"
#include "../loot/loot_tables.h"
#include "../loot/mc_loot.h"

static int test_outpost_loot_1_21_11() {
    int ret = 0;

    uint64_t seed = -7594343816690189614L;
    int structure = Outpost;
    int mc = MC_1_21_9;

    StructureSaltConfig saltConfig;
    // in 1.18+ the salts do not depend on the biome
    ASSERT_NONZERO(ret, getStructureSaltConfig(structure, mc, -1, &saltConfig));

    int x = 1344;
    int z = -9872;

    StructureVariant sv;
    // the biome does not matter for the outpost variant
    ASSERT_NONZERO(ret, getVariant(&sv, structure, mc, seed, x, z, -1));

    Piece piece;
    int numPieces = getStructurePieces(&piece, 1, structure, saltConfig, &sv, mc, seed, x, z);
    ASSERT_EQ(ret, numPieces, 1);

    const char *lootTable = piece.lootTables[0];

    ASSERT_STR_EQ(ret, lootTable, "chests/pillager_outpost");
    uint64_t lootSeed = piece.lootSeeds[0];
    LootTableContext *ltc;

    ASSERT_NONZERO(ret, init_loot_table_name(&ltc, lootTable, mc));
    set_loot_seed(ltc, lootSeed);
    generate_loot(ltc);

    int hasFortune1Book = 0;

    int lootCount = ltc->generated_item_count;
    for (int lootIdx = 0; lootIdx < lootCount; lootIdx++) {
        ItemStack itemStack = ltc->generated_items[lootIdx];
        int itemId = get_global_item_id(ltc, itemStack.item);
        if (itemId != ITEM_BOOK) {
            continue;
        }
        EnchantInstance *enchantments = itemStack.enchantments;
        int enchantmentCount = itemStack.enchantment_count;
        for (int enchantmentIdx = 0; enchantmentIdx < enchantmentCount; enchantmentIdx++) {
            EnchantInstance enchantInstance = enchantments[enchantmentIdx];
            int itemEnchantment = enchantInstance.enchantment;
            if (itemEnchantment != FORTUNE) {
                continue;
            }
            int level = enchantInstance.level;
            if (level != 1) {
                continue;
            }
            hasFortune1Book = 1;
        }
    }

    ASSERT_TRUE(ret, hasFortune1Book);

    return ret;
}

static int test_jungle_pyramid_loot_26_2() {
    int ret = 0;

    uint64_t seed = -4911581849163216631ULL;
    int structure = Jungle_Pyramid;
    int mc = MC_26_2;

    int x = 256;
    int z = -2016;

    StructureSaltConfig saltConfig;
    // in 1.18+ the salts do not depend on the biome
    ASSERT_NONZERO(ret, getStructureSaltConfig(structure, mc, -1, &saltConfig));

    Piece piece;
    // variant is not used for jungle pyramids
    int numPieces = getStructurePieces(&piece, 1, structure, saltConfig, NULL, mc, seed, x, z);
    ASSERT_EQ(ret, numPieces, 1);
    ASSERT_EQ(ret, piece.chestCount, 4);

    const char *lootTable1 = piece.lootTables[0];
    uint64_t lootSeed1 = piece.lootSeeds[0];
    ASSERT_STR_EQ(ret, lootTable1, "chests/jungle_temple_dispenser");
    ASSERT_EQ(ret, lootSeed1, 230780238426740339ULL);

    const char *lootTable2 = piece.lootTables[1];
    uint64_t lootSeed2 = piece.lootSeeds[1];
    ASSERT_STR_EQ(ret, lootTable2, "chests/jungle_temple_dispenser");
    ASSERT_EQ(ret, lootSeed2, 318519966113226178ULL);

    const char *lootTable3 = piece.lootTables[2];
    uint64_t lootSeed3 = piece.lootSeeds[2];
    ASSERT_STR_EQ(ret, lootTable3, "chests/jungle_temple");
    ASSERT_EQ(ret, lootSeed3, -894605277458083196ULL);

    const char *lootTable4 = piece.lootTables[3];
    uint64_t lootSeed4 = piece.lootSeeds[3];
    ASSERT_STR_EQ(ret, lootTable4, "chests/jungle_temple");
    ASSERT_EQ(ret, lootSeed4, 3293249825004981781ULL);

    return ret;
}

static int test_abandoned_camp_loot_26_3() {
    int ret = 0;

    uint64_t seed = -3829811542736183482ULL;
    int structure = Abandoned_Camp;
    int mc = MC_26_3;

    Generator g;
    setupGenerator(&g, mc, 0);
    applySeed(&g, DIM_OVERWORLD, seed);

    int x = 224;
    int z = -384;

    int biome = getBiomeAt(&g, 4, x >> 2, 320 >> 2, z >> 2);
    ASSERT_EQ(ret, biome, savanna);

    StructureSaltConfig saltConfig;
    ASSERT_NONZERO(ret, getStructureSaltConfig(structure, mc, biome, &saltConfig));

    StructureVariant sv;
    ASSERT_NONZERO(ret, getVariant(&sv, structure, mc, seed, x, z, biome));

    Piece pieces[2];
    int numPieces = getStructurePieces(pieces, 2, structure, saltConfig, &sv, mc, seed, x, z);
    ASSERT_EQ(ret, numPieces, 2);

    Piece tentPiece = pieces[0];
    ASSERT_EQ(ret, tentPiece.type, 9); // tent_savanna_9
    ASSERT_STR_EQ(ret, tentPiece.lootTables[0], "barrels/abandoned_camp_barrel");
    ASSERT_EQ(ret, tentPiece.lootSeeds[0], -5681689646423109866ULL);

    Piece campPiece = pieces[1];
    ASSERT_EQ(ret, campPiece.type - 30, 14); // campsite_default_special_14
    ASSERT_STR_EQ(ret, campPiece.lootTables[0], "barrels/abandoned_camp_barrel");
    ASSERT_EQ(ret, campPiece.lootSeeds[0], 4514331715204795226ULL);

    return ret;
}

int main() {
    int ret = 0;

    ret += test_outpost_loot_1_21_11();
    ret += test_jungle_pyramid_loot_26_2();
    ret += test_abandoned_camp_loot_26_3();

    return ret;
}

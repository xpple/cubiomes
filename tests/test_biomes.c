#include "testing.h"
#include "../generator.h"

static int test_biomes_1_16_5() {
    int ret = 0;

    Generator g;
    setupGenerator(&g, MC_1_16_5, 0);

    applySeed(&g, DIM_OVERWORLD, 1437905338718953247ULL);
    ASSERT_EQ(ret, getBiomeAt(&g, 4, 3611 - 8, 0, -141), wooded_badlands_plateau);
    ASSERT_EQ(ret, getBiomeAt(&g, 4, -54 >> 2, 0, -23 >> 2), cold_ocean);
    ASSERT_EQ(ret, getBiomeAt(&g, 4, 68 >> 2, 0, 47 >> 2), mushroom_fields);
    ASSERT_EQ(ret, getBiomeAt(&g, 4, 186 >> 2, 0, 249 >> 2), frozen_ocean);
    ASSERT_EQ(ret, getBiomeAt(&g, 4, 3256313 >> 2, 0, -3265404 >> 2), forest);

    applySeed(&g, DIM_NETHER, 1551515151585454ULL);
    ASSERT_EQ(ret, getBiomeAt(&g, 4, 181 >> 2, 0, 209 >> 2), crimson_forest);
    ASSERT_EQ(ret, getBiomeAt(&g, 4, 404 >> 2, 0, 416 >> 2), soul_sand_valley);
    ASSERT_EQ(ret, getBiomeAt(&g, 4, 308 >> 2, 0, 32 >> 2), basalt_deltas);

    applySeed(&g, DIM_END, 1551515151585454ULL);
    ASSERT_EQ(ret, getBiomeAt(&g, 1, 10000, 0, 10000), end_barrens);

    return ret;
}

static int test_biomes_1_18_2() {
    int ret = 0;

    Generator g;
    setupGenerator(&g, MC_1_18_2, 0);
    applySeed(&g, DIM_NETHER, 12345);
    int biome = getBiomeAt(&g, 1, 0, 0, 0);
    ASSERT_EQ(ret, biome, nether_wastes);

    return ret;
}

static int test_biomes_26_3() {
    int ret = 0;

    Generator g;
    setupGenerator(&g, MC_26_3, 0);
    applySeed(&g, DIM_OVERWORLD, -3829811542736183482ULL);
    int biome = getBiomeAt(&g, 1, 68148, 77, 80990);
    ASSERT_EQ(ret, biome, dappled_forest);

    return ret;
}

int main() {
    int ret = 0;

    ret += test_biomes_1_16_5();
    ret += test_biomes_1_18_2();
    ret += test_biomes_26_3();

    return ret;
}

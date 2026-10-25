#include "testing.h"
#include "../finders.h"

static int test_structures_1_16_5() {
    int ret = 0;

    int mc = MC_1_16_5;

    StructureConfig sconf;
    int posX, posZ;
    Pos out;

    ASSERT_NONZERO(ret, getStructureConfig(Ruined_Portal_N, mc, &sconf));
    ASSERT_EQ(ret, sconf.regionSize, 25);
    ASSERT_EQ(ret, sconf.chunkRange, 15);
    ASSERT_EQ(ret, sconf.salt, 34222645);

    posX = 528, posZ = 160;
    getStructurePos(Ruined_Portal_N, mc, 1551515151585454ULL, floordiv(posX >> 4, sconf.regionSize), floordiv(posZ >> 4, sconf.regionSize), &out);
    ASSERT_EQ(ret, posX, out.x);
    ASSERT_EQ(ret, posZ, out.z);

    ASSERT_NONZERO(ret, getStructureConfig(Fortress, mc, &sconf));
    ASSERT_EQ(ret, sconf.regionSize, 27);
    ASSERT_EQ(ret, sconf.chunkRange, 27 - 4);
    ASSERT_EQ(ret, sconf.salt, 30084232);

    posX = 544, posZ = 32;
    getStructurePos(Fortress, mc, 1551515151585454ULL, floordiv(posX >> 4, sconf.regionSize), floordiv(posZ >> 4, sconf.regionSize), &out);
    ASSERT_EQ(ret, posX, out.x);
    ASSERT_EQ(ret, posZ, out.z);

    return ret;
}

static int test_end_city_height_versions() {
    int ret = 0;
    uint64_t seed = 6983438112510666596ULL;
    int versions[] = {MC_1_9, MC_1_10, MC_1_11, MC_1_12, MC_1_13, MC_1_14, MC_1_18, MC_1_19, MC_26_3};
    int numVersions = sizeof(versions) / sizeof(*versions);
    SurfaceNoise sn;
    initSurfaceNoise(&sn, DIM_END, seed);

    for (int i = 0; i < numVersions; i++) {
        Generator g;
        setupGenerator(&g, versions[i], 0);
        applySeed(&g, DIM_END, seed);
        Pos pos;
        ASSERT_NONZERO(ret, getStructurePos(End_City, g.mc, seed, -40, -17, &pos));
        ASSERT_EQ(ret, pos.x, -12768);
        ASSERT_EQ(ret, pos.z, -5424);
        ASSERT_NONZERO(ret, isViableStructurePos(End_City, &g, pos.x, pos.z, 0));
        int expectedHeight = g.mc <= MC_1_12 ? 60 : 0;
        ASSERT_EQ(ret, isViableEndCityTerrain(&g, &sn, pos.x, pos.z), expectedHeight);
        ASSERT_EQ(ret, isViableEndCityTerrain(&g, &sn, -12760, -5416), expectedHeight);
    }
    return ret;
}

int main() {
    int ret = 0;

    ret += test_structures_1_16_5();
    ret += test_end_city_height_versions();

    return ret;
}

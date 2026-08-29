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

int main() {
    int ret = 0;

    ret += test_structures_1_16_5();

    return ret;
}

#include "testing.h"
#include "../carver.h"

static int test_carvers_1_21_11() {
    int ret = 0;

    uint64_t seed = 12345;
    int version = MC_1_21_11;
    CanyonCarverConfig canyonCarverConfig;
    ASSERT_NONZERO(ret, getCanyonCarverConfig(CANYON_CARVER, version, &canyonCarverConfig));

    int cx = 2720 >> 4;
    int cz = 96 >> 4;
    uint64_t rnd; // unused
    ASSERT_NONZERO(ret, checkCanyonStart(seed, cx, cz, canyonCarverConfig, &rnd));

    return ret;
}

int main() {
    int ret = 0;

    ret += test_carvers_1_21_11();

    return ret;
}

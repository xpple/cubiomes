// find a seed with a certain structure at the origin chunk
#include "finders.h"
#include <stdio.h>

int main()
{
    int structType = Outpost;
    int mc = MC_1_18;

    Generator g;
    setupGenerator(&g, mc, 0);

    uint64_t lower48;
    for (lower48 = 0; ; lower48++)
    {
        // The structure position depends only on the region coordinates and
        // the lower 48-bits of the world seed.
        Pos p;
        if (!getStructurePos(structType, mc, lower48, 0, 0, &p))
            continue;

        // Look for a seed with the structure at the origin chunk.
        if (p.x >= 16 || p.z >= 16)
            continue;

        // Look for a full 64-bit seed with viable biomes.
        uint64_t upper16;
        for (upper16 = 0; upper16 < 0x10000; upper16++)
        {
            uint64_t seed = lower48 | (upper16 << 48);
            applySeed(&g, DIM_OVERWORLD, seed);
            if (isViableStructurePos(structType, &g, p.x, p.z, 0))
            {
                printf("Seed %" PRId64 " has a Pillager Outpost at (%d, %d).\n",
                    (int64_t) seed, p.x, p.z);
                return 0;
            }
        }
    }
}
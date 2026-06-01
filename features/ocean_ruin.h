#ifndef OCEAN_RUIN_H_
#define OCEAN_RUIN_H_

#include "../finders.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Generate ocean ruin pieces that contain loot chests. The returned pieces
 * include chest positions, loot tables and loot seeds.
 *
 * @param list the output list of pieces
 * @param n the maximum number of pieces to generate
 * @param ssconf the salt config, use getStructureSaltConfig to obtain it
 * @param sv the structure variant, used to distinguish warm/cold ruins
 * @param mc the Minecraft version
 * @param seed the world seed
 * @param posX the block X-coordinate as yielded by getStructurePos
 * @param posZ the block Z-coordinate as yielded by getStructurePos
 * @return the number of pieces, or -1 if unsupported or insufficient output
 */
int getOceanRuinLoot(Piece *list, int n, StructureSaltConfig ssconf,
        StructureVariant *sv, int mc, uint64_t seed, int posX, int posZ);

#ifdef __cplusplus
}
#endif

#endif // OCEAN_RUIN_H_

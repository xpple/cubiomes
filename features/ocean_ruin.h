#ifndef OCEAN_RUIN_H_
#define OCEAN_RUIN_H_

#include "../finders.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    OR_WARM_1 = 0,
    OR_WARM_2,
    OR_WARM_3,
    OR_WARM_4,
    OR_WARM_5,
    OR_WARM_6,
    OR_WARM_7,
    OR_WARM_8,
    OR_BRICK_1,
    OR_CRACKED_1,
    OR_MOSSY_1,
    OR_BRICK_2,
    OR_CRACKED_2,
    OR_MOSSY_2,
    OR_BRICK_3,
    OR_CRACKED_3,
    OR_MOSSY_3,
    OR_BRICK_4,
    OR_CRACKED_4,
    OR_MOSSY_4,
    OR_BRICK_5,
    OR_CRACKED_5,
    OR_MOSSY_5,
    OR_BRICK_6,
    OR_CRACKED_6,
    OR_MOSSY_6,
    OR_BRICK_7,
    OR_CRACKED_7,
    OR_MOSSY_7,
    OR_BRICK_8,
    OR_CRACKED_8,
    OR_MOSSY_8,
    OR_BIG_WARM_4,
    OR_BIG_WARM_5,
    OR_BIG_WARM_6,
    OR_BIG_WARM_7,
    OR_BIG_BRICK_1,
    OR_BIG_CRACKED_1,
    OR_BIG_MOSSY_1,
    OR_BIG_BRICK_2,
    OR_BIG_CRACKED_2,
    OR_BIG_MOSSY_2,
    OR_BIG_BRICK_3,
    OR_BIG_CRACKED_3,
    OR_BIG_MOSSY_3,
    OR_BIG_BRICK_8,
    OR_BIG_CRACKED_8,
    OR_BIG_MOSSY_8,
    OR_PIECE_COUNT,
};

/**
 * Generate ocean ruin pieces. The returned pieces include chest positions and
 * loot tables for pieces that contain loot chests, but not loot seeds.
 *
 * @param list the output list of pieces
 * @param n the maximum number of pieces to generate
 * @param sv the structure variant, used to distinguish warm/cold ruins
 * @param mc the Minecraft version
 * @param seed the world seed
 * @param posX the block X-coordinate as yielded by getStructurePos
 * @param posZ the block Z-coordinate as yielded by getStructurePos
 * @return the number of pieces, or -1 if unsupported or insufficient output
 */
int getOceanRuinPieces(Piece *list, int n, StructureVariant *sv,
        int mc, uint64_t seed, int posX, int posZ);

/**
 * Generate ocean ruin pieces. The returned pieces include chest positions,
 * loot tables and loot seeds for pieces that contain loot chests.
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

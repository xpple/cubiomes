#ifndef MINESHAFT_H_
#define MINESHAFT_H_

#include "../finders.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Generates the mineshaft pieces for a given mineshaft.
 * @param g the biome generator
 * @param list the empty list of mineshaft pieces
 * @param n the maximum number of pieces (typically 400 is fine)
 * @param mc the minecraft version
 * @param seed the world seed
 * @param chunkX the chunk X-coordinate
 * @param chunkZ the chunk Z-coordinate
 * @return the completed list of mineshaft pieces
 */
int getMineshaftPieces(Generator *g, Piece *list, int n, int mc, uint64_t seed, int chunkX, int chunkZ);

enum {
    MS_CORRIDOR,
    MS_CROSSING,
    MS_ROOM,
    MS_STAIRS,
    MS_PIECE_COUNT,
};

/**
 * Generates the mineshaft pieces and loot for a given mineshaft.
 * 
 * Warning: Very slow
 * @param g the biome generator
 * @param sn surface noise
 * @param list the empty list of mineshaft pieces
 * @param n the maximum number of pieces (typically 400 is fine)
 * @param mc the minecraft version
 * @param seed the world seed
 * @param chunkX the chunk X-coordinate
 * @param chunkZ the chunk Z-coordinate
 * @param airOut Pass NULL. Used internally for strongholds (strongholds depend on certain mineshaft generation)
 * @return the completed list of mineshaft pieces with loot
 */
int getMineshaftLoot(Generator *g, SurfaceNoise *sn, Piece *list, int n, StructureSaltConfig ssconf, int mc, uint64_t seed, int chunkX, int chunkZ, Pos3List *airOut);

#ifdef __cplusplus
}
#endif

#endif //MINESHAFT_H_
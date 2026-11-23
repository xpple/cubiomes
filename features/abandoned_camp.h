#ifndef ABANDONED_CAMP_H_
#define ABANDONED_CAMP_H_

#include "../finders.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Generate the two pieces of an Abandoned Camp at the given chunk position, including
/// loot tables and seeds. The piece names use %s as placeholder for the biome. The user
/// can insert those themselves. Chest positions are approximate.
int getAbandonedCampPieces(Piece *list, StructureSaltConfig ssconf, StructureVariant *sv, int mc, uint64_t seed, int chunkX, int chunkZ);

#ifdef __cplusplus
}
#endif

#endif //ABANDONED_CAMP_H_

#ifndef ABANDONED_CAMP_H_
#define ABANDONED_CAMP_H_

#include "../finders.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Generate the two pieces of an Abandoned Camp at the given chunk position, including
/// loot tables and seeds. The tent piece names use %s as placeholder for the biome. The
/// user can insert those themselves. Chest positions are approximate.
///
/// \param[out] list two piece output list
/// \param ssconf salt configuration
/// \param[in] sv structure variant
/// \param mc Minecraft version
/// \param seed world seed
/// \param chunkX,chunkZ structure origin in chunk coordinates
/// \return two, the number of pieces written
int getAbandonedCampPieces(Piece *list, StructureSaltConfig ssconf, StructureVariant *sv, int mc, uint64_t seed, int chunkX, int chunkZ);

#ifdef __cplusplus
}
#endif

#endif //ABANDONED_CAMP_H_

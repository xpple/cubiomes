#ifndef ABANDONED_CAMP_H_
#define ABANDONED_CAMP_H_

#include "../finders.h"

#ifdef __cplusplus
extern "C" {
#endif

int getAbandonedCampPieces(Piece *list, StructureSaltConfig ssconf, StructureVariant *sv, int mc, uint64_t seed, int chunkX, int chunkZ);

#ifdef __cplusplus
}
#endif

#endif //ABANDONED_CAMP_H_

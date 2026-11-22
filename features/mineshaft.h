#ifndef MINESHAFT_H_
#define MINESHAFT_H_

#include "../finders.h"

#ifdef __cplusplus
extern "C" {
#endif

int getMineshaftPieces(Piece *list, int n, int mc, uint64_t seed, int chunkX, int chunkZ);

enum {
    MS_CORRIDOR,
    MS_CROSSING,
    MS_ROOM,
    MS_STAIRS,
    MS_PIECE_COUNT,
};

int getMineshaftLoot(Piece *list, int n, StructureSaltConfig ssconf, int mc, uint64_t seed, int chunkX, int chunkZ);

#ifdef __cplusplus
}
#endif

#endif //MINESHAFT_H_

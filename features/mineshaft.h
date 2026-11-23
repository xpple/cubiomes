#ifndef MINESHAFT_H_
#define MINESHAFT_H_

#include "../finders.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Generate the structure pieces of a Mineshaft at the given chunk position.
/// The output buffer must hold `n` pieces. Returns the number of pieces generated.
/// The starting room is at Y=50, without the final height adjustment.
int getMineshaftPieces(Piece *list, int n, int mc, uint64_t seed, int chunkX, int chunkZ);

enum {
    MS_CORRIDOR,
    MS_CROSSING,
    MS_ROOM,
    MS_STAIRS,
    MS_PIECE_COUNT,
};

/// Generates Mineshaft pieces as in `getMineshaftPieces`, then fills their
/// chest positions, loot tables and loot seeds. Returns the number of pieces
/// generated.
int getMineshaftLoot(Piece *list, int n, StructureSaltConfig ssconf, int mc, uint64_t seed, int chunkX, int chunkZ);

#ifdef __cplusplus
}
#endif

#endif //MINESHAFT_H_

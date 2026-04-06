#ifndef STRONGHOLD_H_
#define STRONGHOLD_H_

#include "../finders.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Generate the structure pieces of a Stronghold. The maximum number of pieces
 * that are generated is limited to 'n'. A buffer length of around 400 should
 * be sufficient in practice, but a fortress can in theory contain many more
 * than that. The number of generated pieces is given by the return value.
 */
int getStrongholdPieces(Piece *list, int n, int mc, uint64_t seed, int chunkX, int chunkZ);

enum {
    SH_STRAIGHT = 0,
    SH_PRISON_HALL,
    SH_LEFT_TURN,
    SH_RIGHT_TURN,
    SH_ROOM_CROSSING,
    SH_STRAIGHT_STAIRS_DOWN,
    SH_STAIRS_DOWN,
    SH_FIVE_CROSSING,
    SH_CHEST_CORRIDOR,
    SH_LIBRARY,
    SH_PORTAL_ROOM,
    SH_FILLER_CORRIDOR,
    SH_PIECE_COUNT,
};

int getStrongholdLoot(Piece *list, int n, StructureSaltConfig ssconf, int mc, uint64_t seed, int chunkX, int chunkZ);

#ifdef __cplusplus
}
#endif

#endif //STRONGHOLD_H_

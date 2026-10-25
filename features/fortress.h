#ifndef FORTRESS_H_
#define FORTRESS_H_

#include "../finders.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Generate the structure pieces of a Nether Fortress. The maximum number of
 * pieces that are generated is limited to 'n'. A buffer length of around 400
 * should be sufficient in practice, but a fortress can in theory contain many
 * more than that. The number of generated pieces is given by the return value.
 */
int getFortressPieces(Piece *list, int n, int mc, uint64_t seed, int chunkX, int chunkZ);
enum
{   // Fortress piece types
    FORTRESS_START,
    BRIDGE_STRAIGHT,
    BRIDGE_CROSSING,
    BRIDGE_FORTIFIED_CROSSING,
    BRIDGE_STAIRS,
    BRIDGE_SPAWNER,
    BRIDGE_CORRIDOR_ENTRANCE,
    CORRIDOR_STRAIGHT,
    CORRIDOR_CROSSING,
    CORRIDOR_TURN_RIGHT,
    CORRIDOR_TURN_LEFT,
    CORRIDOR_STAIRS,
    CORRIDOR_T_CROSSING,
    CORRIDOR_NETHER_WART,
    FORTRESS_END,
    FORTRESS_PIECE_COUNT,
};

#ifdef __cplusplus
}
#endif

#endif //FORTRESS_H_

#ifndef END_CITY_H_
#define END_CITY_H_

#include "../finders.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Generate the structure pieces of an End City. This pieces buffer should be
 * large enough to hold END_CITY_PIECES_MAX elements.
 * @pieces          : output buffer
 * @seed            : world seed
 * @chunkX, chunkZ  : 16x16 chunk position
 *
 * Returns the number of structure pieces generated.
 */
int getEndCityPieces(Piece *pieces, uint64_t seed, int chunkX, int chunkZ);
enum
{   // End City piece types
    BASE_FLOOR,
    BASE_ROOF,
    BRIDGE_END,
    BRIDGE_GENTLE_STAIRS,
    BRIDGE_PIECE,
    BRIDGE_STEEP_STAIRS,
    FAT_TOWER_BASE,
    FAT_TOWER_MIDDLE,
    FAT_TOWER_TOP,
    SECOND_FLOOR_1,
    SECOND_FLOOR_2,
    SECOND_ROOF,
    END_SHIP,
    THIRD_FLOOR_1,
    THIRD_FLOOR_2,
    THIRD_ROOF,
    TOWER_BASE,
    TOWER_FLOOR, // unused
    TOWER_PIECE,
    TOWER_TOP,
    END_CITY_PIECE_COUNT,
    END_CITY_PIECES_MAX = 421
};

#ifdef __cplusplus
}
#endif

#endif //END_CITY_H_

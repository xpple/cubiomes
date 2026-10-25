#ifndef VILLAGE_H_
#define VILLAGE_H_

#include "../finders.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    VILLAGE_WELL,
    VILLAGE_PATH,
    VILLAGE_HOUSE_4_GARDEN,
    VILLAGE_CHURCH,
    VILLAGE_HOUSE_1,
    VILLAGE_WOOD_HUT,
    VILLAGE_HALL,
    VILLAGE_FIELD_1,
    VILLAGE_FIELD_2,
    VILLAGE_HOUSE_2,
    VILLAGE_HOUSE_3,
    VILLAGE_TORCH,
    VILLAGE_PIECE_COUNT,
};

// estimate
enum { VILLAGE_PIECES_MAX = 64 };

/**
 * Generate pre-1.14 village pieces. The output array must hold at least
 * VILLAGE_PIECES_MAX entries. Further appends are ignored once this limit
 * is reached. Returns the generated number of pieces, zero upon failure,
 * or -1 for unsupported versions.
 *
 * For versions before 1.10, the generator must be initialized. For versions
 * after, it suffices to only populate the fields mc and seed. For all
 * pieces, the Y coordinate is set to 64.
 */
int getVillagePieces(Piece *list, Generator *g, int chunkX, int chunkZ);

STRUCT(VillageItemStack) {
    int type, count;
};

/**
 * Generate the loot for the blacksmith chest. Assumes version 1.8.9.
 */
void getVillageBlacksmithInventory(const Piece *list, int count, uint64_t seed, VillageItemStack *inventory);

#ifdef __cplusplus
}
#endif

#endif // VILLAGE_H_

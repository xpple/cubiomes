#ifndef DUNGEON_H_
#define DUNGEON_H_

#include "../finders.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*DungeonBlockFxn)(void *ctx, int x, int y, int z, int wantAir);

typedef void (*DungeonSetFxn)(void *ctx, int x, int y, int z, int kind);

STRUCT(DungeonRoom) {
    Pos3 spawnerPos;
    int halfX, halfZ; // room half widths (2 or 3)
    int chestCount;
    Pos3 chestPoses[2];
    uint64_t chestLootSeeds[2];
};

STRUCT(DungeonRoomList) {
    DungeonRoom *rooms;
    int size;
    int capacity;
};

void createDungeonRoomList(DungeonRoomList *list, int initialCapacity);
void appendDungeonRoom(DungeonRoomList *list, DungeonRoom room);
void freeDungeonRoomList(DungeonRoomList *list);

int simMonsterRooms(int mc, uint64_t worldSeed, int chunkX, int chunkZ, int featureIndex, DungeonBlockFxn getBlock, DungeonSetFxn setBlock, void *ctx, DungeonRoomList *roomsOut);

STRUCT(DungeonCarverCache) { Pos3List air, water; int valid; };

STRUCT(DungeonWorld) {
    Generator *g; SurfaceNoise *sn;
    int mc; uint64_t seed;
    int lcx0, lcz0, ncx, ncz, ci;
    DungeonCarverCache *cc;
    int *chunkXs, *chunkZs;
    const int *cellToIdx; // conversion from xz chunk to decoration order
    uint8_t **lakeDetails; // 1 is air, 2 is water, 3 is lava
    uint8_t **dungeonDetails; // 1 is cave air, 2 is cobble, 3 is chest
    uint8_t **carverAirMask, **carverWaterMask; // masks for carvers
    uint8_t **mineshaftAirMask; // mask for mineshaft
    const Pos3List *mineshaftAir;
    Pos3List *dungeonAirBySrc, *dungeonSolidBySrc; // optional (pass NULL)
    const Piece *pieces; int pieceCount; // stronghold pieces, optional
};

/**
 * Fills the mask for dungeon data
 */

void dungeonFillMask(uint8_t *mask, const Pos3List *list, int cx, int cz);

/**
 * Decorates chunk ci as far as monster rooms are concerned: carvers,
 * lakes (written to w->lakeDetails[ci] and optionally appended to
 * lakeAirAll/lakeWaterAll), then the 8 monster room attempts
 * Rooms placed in this chunk are appended to roomsOut (optional)
 * Call once per chunk in decoration order
 */

int dungeonSimChunk(DungeonWorld *w, int ci, Pos3List *lakeAirAll, Pos3List *lakeWaterAll, DungeonRoomList *roomsOut);

/**
 * Finds the monster rooms (and their chest loot seeds) of a single chunk.
 * The chunks around it are assumed to have decorated in radial order
 * Nearby strongholds and mineshafts are detected and simulated automatically
 *
 * Warning: **very** slow (~50 ms/chunk) (carvers + lakes for a 5x5 chunk window, plus full
 * mineshaft/stronghold loot sim for any mineshaft/stronghold nearby)
 * Shuold only use this function for checking <100 chunks (10 x 10 chunk area)
 * For more chunks use getDungeonsArea instead
 *
 * @param g the biome generator
 * @param sn surface noise
 * @param mc the minecraft version (MC_1_14 to MC_1_16_5)
 * @param seed the world seed
 * @param chunkX the chunk X-coordinate
 * @param chunkZ the chunk Z-coordinate
 * @param centerCX chunk X of the generation center (generally pass the same as chunkX)
 * @param centerCZ chunk Z of the generation center (generally pass the same as chunkX)
 * @param roomsOut rooms placed in the chunk, with chest positions and loot seeds
 * @return the number of rooms placed in the chunk, or -1 for error
 */
int getDungeons(Generator *g, SurfaceNoise *sn, int mc, uint64_t seed, int chunkX, int chunkZ, int centerCX, int centerCZ, DungeonRoomList *roomsOut);

/**
 * Same as getDungeons, but finds the monster rooms/chest loot seeds of every chunk
 * in a large area much faster via sharing carver/lake/mineshaft/stronghold state
 * across the whole area instead of recomputing it per chunk the way a loop
 * of getDungeons calls would
 *
 * All rooms in the area are appended to one roomsOut list (rooms carry
 * absolute world positions, so grouping by chunk afterwards is >> 4 if needed)
 *
 * Warning: still slow, and memory use grows with the area (a lake/dungeon
 * detail buffer is kept per chunk, including a margin around the chunk)
 * Fine for <1 million chunks (1000 x 1000 chunk area)
 *
 * @param g the biome generator
 * @param sn surface noise
 * @param mc the minecraft version (MC_1_14 to MC_1_16_5)
 * @param seed the world seed
 * @param chunkX1 1st chunk X-coordinate
 * @param chunkZ1 1st chunk Z-coordinate
 * @param chunkX2 2nd chunk X-coordinate
 * @param chunkZ2 2nd chunk Z-coordinate
 * @param centerCX center chunk X-coordinate (dungeons depend on generation order. Pass 0 if searching around 0,0)
 * @param centerCZ center chunk X-coordinate (dungeons depend on generation order. Pass 0 if searching around 0,0)
 * @param roomsOut rooms placed in the chunk, with chest positions and loot seeds
 * @return the number of rooms placed in the chunk, or -1 for error
 */
int getDungeonsArea(Generator *g, SurfaceNoise *sn, int mc, uint64_t seed, int chunkX1, int chunkZ1, int chunkX2, int chunkZ2, int centerCX, int centerCZ, DungeonRoomList *roomsOut);

#ifdef __cplusplus
}
#endif

#endif // DUNGEON_H_

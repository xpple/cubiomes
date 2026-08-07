#ifndef CARVER_H_
#define CARVER_H_

#include "finders.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Cache of which blocks in a chunk are naturally water, used by the carvers to
 * tell a carved air block from a carved water one
 */
STRUCT(NaturalWaterCache) {
    const Generator *g;
    const SurfaceNoise *sn;
    int chunkX, chunkZ;
    uint64_t colWater[256];
    uint8_t colValid[256];
};

/* The 2x2 of density corners around a block, for the water checks
 */
void naturalWaterColumnDensity(const Generator *g, const SurfaceNoise *sn, int x, int z,
                               double dens[2][2][SURFACE_DENS_CELLS]);

/**
 * Find all carved positions in the given chunk.
 * Positions are separated into air and water (poses and waterPoses respectively)
 * @param g the generator (must be initialized)
 * @param chunkX the chunk X-coordinate
 * @param chunkZ the chunk Z-coordinate
 * @param poses Pos3List of all carved air blocks
 * @param waterPoses Pos3List of all carved water blocks
 */
void applyAllCarvers(Generator *g, const SurfaceNoise *sn, int chunkX, int chunkZ, Pos3List* poses, Pos3List* waterPoses);

enum CanyonCarvers {
    CANYON_CARVER,
    UNDERWATER_CANYON_CARVER,
    CANYON_CARVER_NUM,
};

STRUCT(CanyonCarverConfig)
{
    int dim;
    float probability;
    int carverIndex;
    int range;
    int (*y)(uint64_t*, int, int, int); int minY, maxY, innerY;
    float yScale;
    float (*verticalRotation)(uint64_t*, float, float); float minVerRot, maxVerRot;
    float (*distanceFactor)(uint64_t*, float, float); float minDistance, maxDistance;
    float (*thickness)(uint64_t*, float, float, float); float minThickness, maxThickness, plateauThickness;
    int widthSmoothness;
    float (*horizontalRadiusFactor)(uint64_t*, float, float); float minHorRadius, maxHorRadius;
    float verticalRadiusDefaultFactor;
    float verticalRadiusCenterFactor;
};

enum CaveCarvers {
    CAVE_CARVER,
    CAVE_EXTRA_UNDERGROUND_CARVER,
    OCEAN_CAVE_CARVER,
    UNDERWATER_CAVE_CARVER,
    NETHER_CAVE_CARVER,
    CAVE_CARVER_NUM,
};

STRUCT(CaveCarverConfig)
{
    int dim;
    float probability;
    int carverIndex;
    int range;
    int caveBound;
    float (*thickness)(uint64_t*);
    double tunnelYScale;
    int (*y)(uint64_t*, int, int, int); int minY, maxY, innerY;
    float (*yScale)(uint64_t*, float, float); float minYScale, maxYScale;
    float (*horizontalRadiusMultiplier)(uint64_t*, float, float); float minHorRadius, maxHorRadius;
    float (*verticalRadiusMultiplier)(uint64_t*, float, float); float minVerRadius, maxVerRadius;
    float (*floorLevel)(uint64_t*, float, float); float minFloorLevel, maxFloorLevel;
};

/**
 * Get the canyon carver configuration for the canyon type.
 * @param canyonCarverType the canyon carver type
 * @param mc the Minecraft version
 * @param cconf the config
 * @return zero if failed
 */
int getCanyonCarverConfig(int canyonCarverType, int mc, CanyonCarverConfig* cconf);

/**
 * Check whether the canyon carver type exists in this biome. The biome is only used for
 * UNDERWATER_CANYON_CARVER, which was removed in 1.18. So for >=1.18 this function is no
 * longer needed.
 * @param canyonCarverType the canyon carver type
 * @param biome the biome in biome scale at (chunkX << 2, chunkZ << 2)
 * @return 1 if the canyon carver type exists in this biome
 */
int isViableCanyonBiome(int canyonCarverType, int biome);

/**
 * Get the cave carver configuration for the cave type. The biome is only used for CAVE_CARVER for
 * versions <1.18. For >=1.18 the biome can always be -1.
 * @param caveCarverType the cave carver type
 * @param mc the Minecraft version
 * @param biome the biome in biome scale at (chunkX << 2, chunkZ << 2)
 * @param cconf the config
 * @return zero if failed
 */
int getCaveCarverConfig(int caveCarverType, int mc, int biome, CaveCarverConfig* cconf);

/**
 * Check whether the cave carver type exists in this biome. The biome is used for OCEAN_CAVE_CARVER
 * and UNDERWATER_CAVE_CARVER, both only for versions <1.18. So for >=1.18 this function is no longer needed.
 * @param caveCarverType the cave carver type
 * @param biome the biome in biome scale at (chunkX << 2, chunkZ << 2)
 * @return 1 if the cave carver type exists in this biome
 */
int isViableCaveBiome(int caveCarverType, int biome);

/**
 * Check whether the canyon type generates at the given chunk.
 * @param seed the world seed (structure seed suffices)
 * @param chunkX the chunk X-coordinate
 * @param chunkZ the chunk Z-coordinate
 * @param ccc the canyon carver config
 * @param rnd an uninitialised random instance (used for carveCanyon)
 * @return 1 if a canyon starts here
 */
int checkCanyonStart(uint64_t seed, int chunkX, int chunkZ, CanyonCarverConfig ccc, uint64_t* rnd);

/**
 * Check whether the cave type generates at the given chunk.
 * @param seed the world seed (structure seed suffices)
 * @param chunkX the chunk X-coordinate
 * @param chunkZ the chunk Z-coordinate
 * @param ccc the cave carver config
 * @param rnd an uninitialised random instance (used for carveCave)
 * @return 1 if a cave starts here
 */
int checkCaveStart(uint64_t seed, int chunkX, int chunkZ, CaveCarverConfig ccc, uint64_t* rnd);

/**
 * Carve out a canyon at the given chunk. An initial size of 1024 should be sufficient for the positions.
 * Check the docs of isViableCanyonBiome to determine whether the biomes array must be populated. If so,
 * note that the scheme is biomes[z][x]. The biomes array can be populated by calling genBiomes with
 * allocCache(g, r), where Range r = {16, cx - 8, cz - 8, 17, 17, 0, 0};.
 * @param seed the world seed (structure seed suffices)
 * @param mc the Minecraft version
 * @param chunkX the chunk X-coordinate
 * @param chunkZ the chunk Z-coordinate
 * @param ccc the canyon carver config
 * @param canyonCarverType the canyon carver type
 * @param biomes a grid of biomes[z][x] in biome scale around the central chunk with offset -8 to 8
 * @param poses list to which positions will be written
 */
void carveCanyon(uint64_t seed, int mc, int chunkX, int chunkZ, CanyonCarverConfig ccc, int canyonCarverType, int biomes[17][17], Pos3List* poses);

/**
 * Carve out a cave at the given chunk. An initial size of 1024 should be sufficient for the positions.
 * Check the docs of isViableCaveBiome to determine whether the biomes array must be populated. If so,
 * note that the scheme is biomes[z][x]. The biomes array can be populated by calling genBiomes with
 * allocCache(g, r), where Range r = {16, cx - 8, cz - 8, 17, 17, 0, 0};.
 * @param seed the world seed (structure seed suffices)
 * @param mc the Minecraft version
 * @param chunkX the chunk X-coordinate
 * @param chunkZ the chunk Z-coordinate
 * @param ccc the cave carver config
 * @param caveCarverType the cave carver type
 * @param biomes a grid of biomes[z][x] in biome scale around the central chunk with offset -8 to 8
 * @param poses list to which positions will be written
 */
void carveCave(uint64_t seed, int mc, int chunkX, int chunkZ, CaveCarverConfig ccc, int caveCarverType, int biomes[17][17], Pos3List* poses);

#ifdef __cplusplus
}
#endif

#endif //CARVER_H_

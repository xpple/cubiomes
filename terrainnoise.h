#ifndef TERRAINNOISE_H_
#define TERRAINNOISE_H_

#include "generator.h"

enum
{
    OTP_PILLAR,
    OTP_PILLAR_RARENESS,
    OTP_PILLAR_THICKNESS,
    OTP_SPAGHETTI_2D,
    OTP_SPAGHETTI_2D_ELEVATION,
    OTP_SPAGHETTI_2D_MODULATOR,
    OTP_SPAGHETTI_2D_THICKNESS,
    OTP_SPAGHETTI_3D_1,
    OTP_SPAGHETTI_3D_2,
    OTP_SPAGHETTI_3D_RARITY,
    OTP_SPAGHETTI_3D_THICKNESS,
    OTP_SPAGHETTI_ROUGHNESS,
    OTP_SPAGHETTI_ROUGHNESS_MODULATOR,
    OTP_CAVE_ENTRANCE,
    OTP_CAVE_LAYER,
    OTP_CAVE_CHEESE,
    OTP_NOODLE,
    OTP_NOODLE_THICKNESS,
    OTP_NOODLE_RIDGE_A,
    OTP_NOODLE_RIDGE_B,
    OTP_JAGGED,
    OTP_MAX,
};
STRUCT(TerrainNoise)
{
    Generator g;

    // shared
    BlendedNoise base3dNoise;

    // overworld
    SplineStack ss;
    Spline* factorSpline;
    Spline* jaggednessSpline;
    PerlinNoise oct[2*45];
    DoublePerlinNoise noises[OTP_MAX];
};

#ifdef __cplusplus
extern "C"
{
#endif

int setupTerrainNoise(TerrainNoise *params, int mc, int flags);

/**
 * Initialise terrain noise parameters with the world seed.
 *
 * @param params the terrain noise parameters
 * @param ws the world seed
 * @param mc the Minecraft version
 * @param largeBiomes whether large biomes should be used for the biome noise
 * @return 0 on failure
 */
int initTerrainNoise(TerrainNoise *params, uint64_t ws, int dim);

/**
 * Sample `overworld/caves/spaghetti_roughness_function`.
 *
 * @param params the terrain noise parameters
 * @param x the world X-coordinate
 * @param y the world Y-coordinate
 * @param z the world Z-coordinate
 * @return the sampled value
 */
double sampleSpaghettiRoughness(TerrainNoise *params, int x, int y, int z);

/**
 * Sample `overworld/caves/spaghetti_2d_thickness_modulator`.
 *
 * @param params the terrain noise parameters
 * @param x the world X-coordinate
 * @param y the world Y-coordinate
 * @param z the world Z-coordinate
 * @return the sampled value
 */
double sampleSpaghetti2dThicknessModulator(TerrainNoise *params, int x, int y, int z);

/**
 * Sample `overworld/caves/spaghetti_2d`.
 *
 * @param params the terrain noise parameters
 * @param x the world X-coordinate
 * @param y the world Y-coordinate
 * @param z the world Z-coordinate
 * @return the sampled value
 */
double sampleSpaghetti2d(TerrainNoise *params, int x, int y, int z);

/**
 * Sample `spaghetti_3d`.
 *
 * @param params the terrain noise parameters
 * @param x the world X-coordinate
 * @param y the world Y-coordinate
 * @param z the world Z-coordinate
 * @return the sampled value
 */
double sampleSpaghetti3d(TerrainNoise *params, int x, int y, int z);

/**
 * Sample `cave_entrance`.
 *
 * @param params the terrain noise parameters
 * @param x the world X-coordinate
 * @param y the world Y-coordinate
 * @param z the world Z-coordinate
 * @return the sampled value
 */
double sampleCaveEntrance(TerrainNoise *params, int x, int y, int z);

/**
 * Sample `overworld/caves/entrances`.
 *
 * @param params the terrain noise parameters
 * @param x the world X-coordinate
 * @param y the world Y-coordinate
 * @param z the world Z-coordinate
 * @param spaghettiRoughness the value of `overworld/caves/spaghetti_roughness_function`
 * at the same location
 * @return the sampled value
 */
double sampleEntrances(TerrainNoise *params, int x, int y, int z, double spaghettiRoughness);

/**
 * Sample `cave_layer`.
 *
 * @param params the terrain noise parameters
 * @param x the world X-coordinate
 * @param y the world Y-coordinate
 * @param z the world Z-coordinate
 * @return the sampled value
 */
double sampleCaveLayer(TerrainNoise *params, int x, int y, int z);

/**
 * Sample `overworld/sloped_cheese`. Use sampleNoiseParameters to sample the splines.
 *
 * @param params the terrain noise parameters
 * @param x the world X-coordinate
 * @param y the world Y-coordinate
 * @param z the world Z-coordinate
 * @param depth the value of the depth spline
 * @param factor the value of the factor spline
 * @param jagged the value of the jagged spline
 * @return the sampled value
 */
double sampleSlopedCheese(TerrainNoise *params, int x, int y, int z, double depth, double factor, double jagged);

/**
 * Sample `cave_cheese`.
 *
 * @param params the terrain noise parameters
 * @param x the world X-coordinate
 * @param y the world Y-coordinate
 * @param z the world Z-coordinate
 * @param slopedCheese the value of `overworld/sloped_cheese` at the same location
 * @return the sampled value
 */
double sampleCaveCheese(TerrainNoise *params, int x, int y, int z, double slopedCheese);

/**
 * Sample `overworld/caves/pillars`.
 *
 * @param params the terrain noise parameters
 * @param x the world X-coordinate
 * @param y the world Y-coordinate
 * @param z the world Z-coordinate
 * @return the sampled value
 */
double samplePillars(TerrainNoise *params, int x, int y, int z);

/**
 * Sample `overworld/caves/noodle`.
 *
 * @param params the terrain noise parameters
 * @param x the world X-coordinate
 * @param y the world Y-coordinate
 * @param z the world Z-coordinate
 * @return the sampled value
 */
double sampleNoodle(TerrainNoise *params, int x, int y, int z);

/**
 * Sample `underground`.
 *
 * @param params the terrain noise parameters
 * @param x the world X-coordinate
 * @param y the world Y-coordinate
 * @param z the world Z-coordinate
 * @param spaghettiRoughness, entrances, slopedCheese the values of `overworld/caves/spaghetti_roughness_function`,
 * `overworld/caves/entrances` and `overworld/sloped_cheese` at the same location
 * @return the sampled value
 */
double sampleUnderground(TerrainNoise *params, int x, int y, int z, double spaghettiRoughness, double entrances, double slopedCheese);

/**
 * Sample `final_density`.
 *
 * @param params the terrain noise parameters
 * @param x the world X-coordinate
 * @param y the world Y-coordinate
 * @param z the world Z-coordinate
 * @param spaghettiRoughness, entrances, slopedCheese the values of `overworld/caves/spaghetti_roughness_function`,
 * `overworld/caves/entrances` and `overworld/sloped_cheese` at the same location
 * @return the sampled value
 */
double sampleFinalDensity(TerrainNoise *params, int x, int y, int z, double spaghettiRoughness, double entrances, double slopedCheese);

/**
 * Sample `preliminary_surface_level`.
 *
 * @param params the terrain noise parameters
 * @param x the world X-coordinate
 * @param z the world Z-coordinate
 * @return the preliminary surface Y-level
 */
int samplePreliminarySurfaceLevel(TerrainNoise *params, int x, int z);

/**
 * Sample the terrain noise column at the given cell coordinates. Minecraft's terrain
 * generates at 1:4 XZ scale, so cell = block >> 2. The Y buffer is at 1:8 scale, which
 * is why it has size 384 / 8 = 48, plus one to interpolate the last vertical cell.
 *
 * @param params the terrain noise parameters
 * @param cellX the cell X-coordinate
 * @param cellZ the cell Z-coordinate
 * @param buffer the Y buffer
 */
void sampleNoiseColumn(TerrainNoise *params, int cellX, int cellZ, double buffer[48 + 1]);

/**
 * Generate a terrain column at the given block coordinates. The result will be written
 * to blocks. The dsxz buffers can be obtained through sampleNoiseColumn. If the
 * flag is set, the generation will stop as soon as a solid block is found, and will
 * return the Y-coordinate of the above air block.
 *
 * @param x the block X-coordinate
 * @param z the block Z-coordinate
 * @param blocks the target blocks
 * @param ds00 the (0, 0) noise column
 * @param ds01 the (0, 1) noise column
 * @param ds10 the (1, 0) noise column
 * @param ds11 the (1, 1) noise column
 * @param flag the boolean flag for the stop condition
 * @return the last air block if the flag was set, -64 otherwise
 */
int generateColumn(int x, int z, int blocks[384], const double ds00[48 + 1], const double ds01[48 + 1], const double ds10[48 + 1], const double ds11[48 + 1], int flag);

/**
 * Generate a region of terrain using memoisation to prevent recalculating noise columns.
 * One can use int (*blocks)[384] = malloc(blockW * blockH * sizeof(*blocks)); to allocate the
 * array. Similarly, the blocks can then be accessed using blocks[relX * blocksH + relZ][y].
 * Here blockH = chunkH << 4, relX ranges over [0, chunkW << 4), relZ ranges over [0, chunkH << 4)
 * and y ranges over [0, 384). Similarly (if flag is true), ys are written to like relX * blockH +
 * relZ.
 *
 * @param params the terrain noise parameters
 * @param chunkX the chunk X-coordinate
 * @param chunkZ the chunk Z-coordinate
 * @param chunkW the chunk width
 * @param chunkH the chunk height
 * @param blocks the target blocks
 * @param ys the target Y coordinates
 * @param flag the boolean flag for the stop condition
 */
void generateRegion(TerrainNoise *params, int chunkX, int chunkZ, int chunkW, int chunkH, int (*blocks)[384], int* ys, int flag);

#ifdef __cplusplus
}
#endif


#endif //TERRAINNOISE_H_

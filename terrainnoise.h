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

    union {
        // overworld >=1.18
        struct {
            SplineStack ss;
            Spline* factorSpline;
            Spline* jaggednessSpline;
            PerlinNoise oct[2*45];
            DoublePerlinNoise noises[OTP_MAX];
        };
        // end, <1.18 overworld
        struct {
            SurfaceNoise sn;
        };
    };
};

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Set up terrain noise for a given version and the specified biome generator flags.
 *
 * @param params the terrain noise parameters
 * @param mc the Minecraft version
 * @param flags the biome generator flags
 */
void setupTerrainNoise(TerrainNoise *params, int mc, int flags);

/**
 * Initialise terrain noise parameters with the world seed.
 *
 * @param params the terrain noise parameters
 * @param ws the world seed
 * @param dim the dimension
 */
void initTerrainNoise(TerrainNoise *params, uint64_t ws, int dim);

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
 * Sample `final_density`.
 *
 * @param base3dNoise the nether terrain noise
 * @param x the world X-coordinate
 * @param y the world Y-coordinate
 * @param z the world Z-coordinate
 * @return the sampled value
 */
double sampleNetherFinalDensity(BlendedNoise* base3dNoise, int x, int y, int z);

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
 * Sample the terrain noise column at the given cell coordinates.
 *
 * For the overworld and nether, the terrain generates at 1:4 XZ scale, so cell = block >> 2.
 * The vertical scale is 1:8.
 *
 * For the end, the terrain generates at 1:8 XZ scale (so cell = block >> 3). The vertical
 * scale is 1:4.
 *
 * The colYMin and colYMax parameters determine the minimum and maximum cells to sample. The
 * column array must therefore be of the form column[colYMax - colYMin + 1]. For example, to
 * generate a full column for the 1.18+ overworld, the array size must be 49. To generate a
 * full column for the end, the array size must be 33.
 *
 * @param params the terrain noise parameters
 * @param cellX the cell X-coordinate
 * @param cellZ the cell Z-coordinate
 * @param colYMin the minimum Y column
 * @param colYMax the maximum Y column
 * @param column the Y column
 */
void sampleNoiseColumn(TerrainNoise *params, int cellX, int cellZ, int colYMin, int colYMax, double column[]);

/**
 * Sample the nether terrain noise column at the given cell coordinates. See
 * sampleNoiseColumn for more info.
 *
 * @param base3dNoise the terrain noise
 * @param cellX the cell X-coordinate
 * @param cellZ the cell Z-coordinate
 * @param colYMin the minimum Y column
 * @param colYMax the maximum Y column
 * @param column the Y column
 */
void sampleNetherNoiseColumn(BlendedNoise* base3dNoise, int cellX, int cellZ, int colYMin, int colYMax, double column[]);

/**
 * Generate a terrain column at the given block coordinates. If not NULL, the result will
 * be written to blocks. The dsxz buffers can be obtained through sampleNoiseColumn. See
 * also that function for documentation about the size of the dsxz parameters, and
 * colYMin and colYMax.
 *
 * The cell height is 8 for the overworld and nether, and 4 for the end. The blocks array
 * must therefore be of the form int blocks[(colYMax - colYMin) * cellHeight].
 *
 * The parameters percentX and percentZ can be computed as (block % cellWidth) / (double)
 * cellWidth, where block is the block coordinate, and cellWidth is the width of a noise
 * cell. For the overworld and nether, that is 4. For the end, that is 8.
 *
 * The interpFunc parameter determines the interpolation function used to combine the
 * cell columns. For >=1.18 that is lerp3, for <1.18 that is lerp3old.
 *
 * If the flag parameter is set, the generation will stop as soon as a solid block is found,
 * and will return the Y-coordinate of the above air block. Note that if blocks is NULL and
 * the flag is not set, the function is essentially useless.
 *
 * @param blocks the target blocks, can be NULL
 * @param ds00 the (0, 0) noise column
 * @param ds01 the (0, 1) noise column
 * @param ds10 the (1, 0) noise column
 * @param ds11 the (1, 1) noise column
 * @param colYMin the minimum Y column
 * @param colYMax the maximum Y column
 * @param cellHeight the vertical (Y) cell size
 * @param percentX the relative X as percentage of the width
 * @param percentZ the relative Z as percentage of the width
 * @param interpFunc the noise cell interpolation function
 * @param worldMinY the world minimum Y coordinate (e.g. -64 for >=1.18 overworld)
 * @param flag the boolean flag for the stop condition
 * @return the last air block if the flag was set, the world minimum y coordinate otherwise
 */
int generateColumn(int blocks[], const double *ds00, const double *ds01, const double *ds10, const double *ds11, int colYMin, int colYMax, int cellHeight, double percentX, double percentZ, interpFunc interpFunc, int worldMinY, int flag);

/**
 * Generate a region of terrain using memoisation to prevent recalculating noise columns. One
 * can use int (*blocks)[h] = malloc(blockW * blockH * sizeof(*blocks)); to allocate the array,
 * where h == (colYMax - colYMin) * cellHeight. See sampleNoiseColumn for documentation about
 * the colYMin and colYMax parameters. The blocks array can be accessed using
 * blocks[relX * blockH + relZ][y]. Here blockH = chunkH << 4, relX ranges over [0, chunkW << 4),
 * relZ ranges over [0, chunkH << 4) and y ranges over [0, h).
 *
 * cellWidth is the width of a noise cell. For the overworld and nether, that is 4. For the end,
 * that is 8. The cell height is 8 for the overworld and nether, and 4 for the end.
 *
 * If the flag is true, ys are written to like relX * blockH + relZ.
 *
 * Both the blocks array and the ys array can be NULL. Setting both to NULL renders the function
 * useless.
 *
 * @param params the terrain noise parameters
 * @param chunkX the chunk X-coordinate
 * @param chunkZ the chunk Z-coordinate
 * @param chunkW the chunk width
 * @param chunkH the chunk height
 * @param blocks the target blocks, can be NULL
 * @param colYMin the minimum Y column
 * @param colYMax the maximum Y column
 * @param ys the target Y coordinates
 * @param flag the boolean flag for the stop condition
 */
void generateRegion(TerrainNoise *params, int chunkX, int chunkZ, int chunkW, int chunkH, int (*blocks)[], int colYMin, int colYMax, int* ys, int flag);

#ifdef __cplusplus
}
#endif


#endif //TERRAINNOISE_H_

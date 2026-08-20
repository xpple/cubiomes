#ifndef ORE_H_
#define ORE_H_

#include "../finders.h"

#ifdef __cplusplus
extern "C" {
#endif

enum Ores {
    AndesiteOre,
    BlackstoneOre,
    BuriedDiamondOre,
    BuriedLapisOre,
    ClayOre,
    CoalOre,
    CopperOre,
    DeepslateOre,
    DeltasGoldOre,
    DeltasQuartzOre,
    DiamondOre,
    DioriteOre,
    DirtOre,
    EmeraldOre,
    ExtraGoldOre,
    GoldOre,
    GraniteOre,
    GravelOre,
    InfestedOre,
    IronOre,
    LapisOre,
    LargeCopperOre,
    LargeDebrisOre,
    LargeDiamondOre,
    LowerAndesiteOre,
    LowerCoalOre,
    LowerDioriteOre,
    LowerGoldOre,
    LowerGraniteOre,
    LowerRedstoneOre,
    MagmaOre,
    MediumDiamondOre,
    MiddleIronOre,
    NetherGoldOre,
    NetherGravelOre,
    NetherQuartzOre,
    RedstoneOre,
    SmallDebrisOre,
    SmallIronOre,
    SoulSandOre,
    TuffOre,
    UpperAndesiteOre,
    UpperCoalOre,
    UpperDioriteOre,
    UpperGraniteOre,
    UpperIronOre,
    ORE_NUM,
};


// use getOreConfig() for the version specific ore configuration
STRUCT(OreConfig)
{
    int32_t         index;
    int32_t         step;
    int32_t         size;
    int32_t         repeatCount;
    int           (*heightProvider)(RandomSource *rnd, int, int, int);
    int32_t         h1; // the parameters for the height provider
    int32_t         h2; // since the provider takes 2 or 3 arguments,
    int32_t         h3; // the third parameter is sometimes unused
    uint32_t        oreType;
    uint32_t        oreBlock;
    int8_t          dim;
    uint8_t         numReplaceBlocks;
    const uint32_t* replaceBlocks;
    float           discardChanceOnAirExposure;
};

//==============================================================================
// Ore height providers
//==============================================================================

// <=1.16.5
static inline int providerRange(RandomSource *rnd, const int bottomOffset, const int topOffset, const int maximumY) {
    return absNextInt(rnd, maximumY - topOffset) + bottomOffset;
}

// <=1.16.5
static inline int providerDepthAverage(RandomSource *rnd, const int baseline, const int spread, const int h3) {
    int a = absNextInt(rnd, spread);
    int b = absNextInt(rnd, spread);
    return a + b - spread + baseline;
}

// <=1.16.5
static inline int providerEmeraldOre(RandomSource *rnd, int h1, int h2, int h3) {
    return absNextInt(rnd, 28) + 4;
}

// <=1.16.5
static inline int providerMagmaOre(RandomSource *rnd, int h1, int h2, int h3) {
    return 32 - 5 + absNextInt(rnd, 10);
}

// >=1.17
static inline int providerUniformRange(RandomSource *rnd, const int minOffset, const int maxOffset, const int h3) {
    if (minOffset > maxOffset) {
        return minOffset;
    }
    return absNextIntBetween(rnd, minOffset, maxOffset);
}

// >=1.17
static inline int providerTriangleRange(RandomSource *rnd, const int minOffset, const int maxOffset, const int h3) {
    if (minOffset > maxOffset) {
        return minOffset;
    }
    const int range = maxOffset - minOffset;
    if (range <= 0) {
        return absNextIntBetween(rnd, minOffset, maxOffset);
    }
    const int midPoint = range / 2;
    const int midPoint2 = range - midPoint;
    int a = absNextIntBetween(rnd, 0, midPoint2);
    int b = absNextIntBetween(rnd, 0, midPoint);
    return minOffset + a + b;
}


/**
 * Get the ore config for a given ore type.
 *
 * @param oreType the ore type as listed in Ores.
 * @param mc the Minecraft version as listed in MCVersion
 * @param biomeID the biome ID as listed in BiomeID, unused for >=1.18
 * @param oconf the target config
 * @return 0 on failure
 */
int getOreConfig(int oreType, int mc, int biomeID, OreConfig *oconf);

/**
 * Get the biome used for ore generation for a given chunk. After this, call
 * `isViableOreBiome` to check whether the ore can generate in the chunk.
 * @param g the generator
 * @param chunkX the chunk X-coordinate
 * @param chunkZ the chunk Z-coordinate
 * @param y the Y-coordinate (in block coordinates) to check for, unused for <1.18
 * @return the biome ID
 */
int getBiomeForOreGen(const Generator *g, int chunkX, int chunkZ, int y);

/**
 * Check whether the given ore type generates in this biome.
 * @param mc the Minecraft version
 * @param oreType the ore type
 * @param biomeID the biome ID
 * @return 0 if the ore does not generate in this biome
 */
int isViableOreBiome(int mc, int oreType, int biomeID);

/**
 * Generate the ores of the given type in the chunk.
 * @param g the generator
 * @param sn the surface noise
 * @param config the ore config
 * @param chunkX the chunk X-coordinate
 * @param chunkZ the chunk Y-coordinate
 * @return a sized array of ore positions
 */
Pos3List generateOres(const Generator *g, const SurfaceNoise *sn, OreConfig config, int chunkX, int chunkZ);

Pos3 generateBaseOrePosition(int mc, OreConfig config, int chunkX, int chunkZ, RandomSource *rnd);

void generateOrePositions(const Generator *g, const SurfaceNoise *sn, OreConfig config, Pos3 pos, RandomSource *rnd, Pos3List* pos3s);

void generateVeinPart(int mc, OreConfig config, RandomSource *rnd, double offsetXPos, double offsetXNeg, double offsetZPos, double offsetZNeg, double offsetYPos, double offsetYNeg, int startX, int startY, int startZ, int oreSize, int radius, Pos3List* pos3s);

#ifdef __cplusplus
}
#endif

#endif //ORE_H_

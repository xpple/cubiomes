#include "carver.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

// https://c-faq.com/misc/bitsets.html
#ifdef BITMASK
    #undef BITMASK
#endif
#define BITMASK(b) (1 << ((b) % CHAR_BIT))
#ifdef BITSLOT
    #undef BITSLOT
#endif
#define BITSLOT(b) ((b) / CHAR_BIT)
#ifdef BITSET
    #undef BITSET
#endif
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#ifdef BITCLEAR
    #undef BITCLEAR
#endif
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#ifdef BITTEST
    #undef BITTEST
#endif
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#ifdef BITNSLOTS
    #undef BITNSLOTS
#endif
#define BITNSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)

#ifdef PI
    #undef PI
#endif
#define PI 3.14159265358979323846

// WARNING: This is dangerous because a/b can be evaluated multiple times.
// E.g. MIN(nextInt(&rnd, 5), 5) will call nextInt(&rnd, 5) twice instead of once.
#ifdef MIN
    #undef MIN
#endif
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#ifdef MAX
    #undef MAX
#endif
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int getCanyonCarverConfig(int canyonCarverType, int mc, CanyonCarverConfig* cconf) {
    static const CanyonCarverConfig
    c_canyon_carver_113 = {DIM_OVERWORLD, 0.02F, 1, 4, providerBiasedToBottom, 20, 67, 8, 3.0F, providerUniformFloatBetween, -0.125F, 0.125F, NULL, -1.0F, -1.0F, providerTrapezoidFloatBetween, 0.0F, 6.0F, 2.0F, 3, providerUniformFloatBetween, 0.75F, 1.0F, 1.0F, 0.0F},
    c_canyon_carver_117 = {DIM_OVERWORLD, 0.02F, 1, 4, providerBiasedToBottom, 20, 67, 8, 3.0F, providerUniformFloatBetween, -0.125F, 0.125F, providerUniformFloatBetween, 0.75F, 1.0F, providerTrapezoidFloatBetween, 0.0F, 6.0F, 2.0F, 3, providerUniformFloatBetween, 0.75F, 1.0F, 1.0F, 0.0F},
    c_canyon_carver_118 = {DIM_OVERWORLD, 0.01F, 2, 4, providerUniformIntBetween, 10, 67, -1, 3.0F, providerUniformFloatBetween, -0.125F, 0.125F, providerUniformFloatBetween, 0.75F, 1.0F, providerTrapezoidFloatBetween, 0.0F, 6.0F, 2.0F, 3, providerUniformFloatBetween, 0.75F, 1.0F, 1.0F, 0.0F},

    c_underwater_canyon_carver_113 = {DIM_OVERWORLD, 0.02F, 0, 4, providerBiasedToBottom, 20, 67, 8, 3.0F, providerUniformFloatBetween, -0.125F, 0.125F, providerUniformFloatBetween, 0.75F, 1.0F, providerTrapezoidFloatBetween, 0.0F, 6.0F, 2.0F, 3, providerUniformFloatBetween, 0.75F, 1.0F, 1.0F, 0.0F}
    ;

    switch (canyonCarverType) {
    case CANYON_CARVER:
        if (mc <= MC_1_16_5) *cconf = c_canyon_carver_113;
        else if (mc <= MC_1_17_1) *cconf = c_canyon_carver_117;
        else *cconf = c_canyon_carver_118;
        return mc > MC_1_12_2;
    case UNDERWATER_CANYON_CARVER:
        *cconf = c_underwater_canyon_carver_113;
        return mc > MC_1_12 && mc <= MC_1_17_1;
    default:
        fprintf(stderr, "ERR initCanyonCarverConfig: unsupported canyon carver type %d\n", canyonCarverType);
        memset(cconf, 0, sizeof(CanyonCarverConfig));
        return 0;
    }
}

int isViableCanyonBiome(int canyonCarverType, int biome) {
    switch (canyonCarverType) {
    case CANYON_CARVER:
        return 1;
    case UNDERWATER_CANYON_CARVER:
        return isOceanic(biome);
    default:
        fprintf(stderr, "ERR isViableCanyonBiome: unsupported canyon carver type %d\n", canyonCarverType);
        return 0;
    }
}

static inline float getCaveThickness(uint64_t* rnd) {
    float a = nextFloat(rnd);
    float b = nextFloat(rnd);
    float f = a * 2.0F + b;
    if (nextInt(rnd, 10) == 0) {
        float c = nextFloat(rnd);
        float d = nextFloat(rnd);
        f *= c * d * 3.0F + 1.0F;
    }
    return f;
}

static inline float getNetherCaveThickness(uint64_t* rnd) {
    float a = nextFloat(rnd);
    float b = nextFloat(rnd);
    return (a * 2.0F + b) * 2.0F;
}

int getCaveCarverConfig(int caveCarverType, int mc, int biome, CaveCarverConfig* cconf) {
    static const CaveCarverConfig
    c_cave_113 = {DIM_OVERWORLD, 1.0F / 7, 0, 4, 15, getCaveThickness, 1.0, providerBiasedToBottom, 0, 127, 8, providerConstantFloat, 0.5F, -1, providerConstantFloat, 1.0F, -1, providerConstantFloat, 1.0F, -1, providerConstantFloat, -0.7F, -1},
    c_cave_deep_ocean_113 = {DIM_OVERWORLD, 1.0F / 15, 0, 4, 15, getCaveThickness, 1.0, providerBiasedToBottom, 0, 127, 8, providerConstantFloat, 0.5F, -1, providerConstantFloat, 1.0F, -1, providerConstantFloat, 1.0F, -1, providerConstantFloat, -0.7F, -1},
    c_cave_118 = {DIM_OVERWORLD, 0.15F, 0, 4, 15, getCaveThickness, 1.0, providerUniformIntBetween, -64 + 8, 180, -1, providerUniformFloatBetween, 0.1F, 0.9F, providerUniformFloatBetween, 0.7F, 1.4F, providerUniformFloatBetween, 0.8F, 1.3F, providerUniformFloatBetween, -1.0F, -0.4F},

    c_cave_extra_underground_118 = {DIM_OVERWORLD, 0.07F, 1, 4, 15, getCaveThickness, 1.0, providerUniformIntBetween, -64 + 8, 47, -1, providerUniformFloatBetween, 0.1F, 0.9F, providerUniformFloatBetween, 0.7F, 1.4F, providerUniformFloatBetween, 0.8F, 1.3F, providerUniformFloatBetween, -1.0F, -0.4F},

    c_ocean_cave_1164 = {DIM_OVERWORLD, 1.0F / 15, 1, 4, 15, getCaveThickness, 1.0, providerBiasedToBottom, 0, 127, 8, providerConstantFloat, 0.5F, -1, providerConstantFloat, 1.0F, -1, providerConstantFloat, 1.0F, -1, providerConstantFloat, -0.7F, -1},

    c_underwater_cave_113 = {DIM_OVERWORLD, 1.0F / 15, 1, 4, 15, getCaveThickness, 1.0, providerBiasedToBottom, 0, 127, 8, providerConstantFloat, 0.5F, -1, providerConstantFloat, 1.0F, -1, providerConstantFloat, 1.0F, -1, providerConstantFloat, -0.7F, -1},

    c_nether_cave_113 = {DIM_NETHER, 0.2F, 0, 4, 10, getNetherCaveThickness, 5.0, providerUniformIntBetween, 0, 127 - 1, -1, providerConstantFloat, 0.5F, -1, providerConstantFloat, 1.0F, -1, providerConstantFloat, 1.0F, -1, providerConstantFloat, -0.7F, -1}
    ;

    switch (caveCarverType) {
    case CAVE_CARVER:
        if (mc <= MC_1_17_1) {
            if (isDeepOcean(biome) || biome == frozen_ocean) *cconf = c_cave_deep_ocean_113;
            else *cconf = c_cave_113;
        }
        else *cconf = c_cave_118;
        return mc > MC_1_12_2;
    case CAVE_EXTRA_UNDERGROUND_CARVER:
        *cconf = c_cave_extra_underground_118;
        return mc > MC_1_17_1;
    case OCEAN_CAVE_CARVER:
        *cconf = c_ocean_cave_1164;
        return mc > MC_1_16_1 && mc <= MC_1_17_1;
    case UNDERWATER_CAVE_CARVER:
        *cconf = c_underwater_cave_113;
        return mc > MC_1_12_2 && mc <= MC_1_17_1;
    case NETHER_CAVE_CARVER:
        *cconf = c_nether_cave_113;
        return mc > MC_1_12_2;
    default:
        fprintf(stderr, "ERR initCaveCarverConfig: unsupported cave carver type %d\n", caveCarverType);
        memset(cconf, 0, sizeof(CaveCarverConfig));
        return 0;
    }
}

int isViableCaveBiome(int caveCarverType, int biome) {
    switch (caveCarverType) {
    case CAVE_CARVER:
    case CAVE_EXTRA_UNDERGROUND_CARVER:
    case NETHER_CAVE_CARVER:
        return 1;
    case OCEAN_CAVE_CARVER:
        return isOceanic(biome);
    case UNDERWATER_CAVE_CARVER:
        return isDeepOcean(biome) || biome == frozen_ocean;
    default:
        fprintf(stderr, "ERR isViableCaveBiome: unsupported cave carver type %d\n", caveCarverType);
        return 0;
    }
}

static inline int getCarveMaskIndex(int x, int y, int z, int worldMinY) {
    return (x & 15) | (z & 15) << 4 | (y - worldMinY) << 8;
}

static inline void setCarveMask(char carvingMask[], int x, int y, int z, int worldMinY) {
    int maskIndex = getCarveMaskIndex(x, y, z, worldMinY);
    BITSET(carvingMask, maskIndex);
}

static inline int getCarveMask(const char carvingMask[], int x, int y, int z, int worldMinY) {
    int maskIndex = getCarveMaskIndex(x, y, z, worldMinY);
    return BITTEST(carvingMask, maskIndex);
}

static int canReach(int chunkX, int chunkZ, double x, double z, int branchIndex, int branchCount, float width) {
    double d = (chunkX << 4) + 8;
    double e = (chunkZ << 4) + 8;
    double f = x - d;
    double g = z - e;
    double h = branchCount - branchIndex;
    double i = width + 2.0F + 16.0F;
    return f * f + g * g - h * h <= i * i;
}

int checkCanyonStart(uint64_t seed, int chunkX, int chunkZ, CanyonCarverConfig ccc, uint64_t* rnd)
{
    *rnd = chunkGenerateRnd(seed + ccc.carverIndex, chunkX, chunkZ);
    return nextFloat(rnd) <= ccc.probability;
}

int checkCaveStart(uint64_t seed, int chunkX, int chunkZ, CaveCarverConfig ccc, uint64_t* rnd)
{
    *rnd = chunkGenerateRnd(seed + ccc.carverIndex, chunkX, chunkZ);
    return nextFloat(rnd) <= ccc.probability;
}

static inline int shouldSkipCanyonCarve(double relativeX, double relativeY, double relativeZ, int y, int worldMinY, /* float* */ void* widthFactors) {
    int i = y - worldMinY;
    return (relativeX * relativeX + relativeZ * relativeZ) * ((float*) widthFactors)[i - 1] + relativeY * relativeY / 6.0 >= 1.0;
}

static inline int shouldSkipCaveCarve(double relativeX, double relativeY, double relativeZ, int y, int worldMinY, /* double* */ void* minRelativeY) {
    return relativeY <= *(double*) minRelativeY ? 1 : relativeX * relativeX + relativeY * relativeY + relativeZ * relativeZ >= 1.0;
}

static void carveEllipsoid(int chunkX, int chunkZ, double x, double y, double z, double horizontalRadius, double verticalRadius, int worldMinY, int worldHeight, char carvingMask[], int (*shouldSkip)(double, double, double, int, int, void*), void* arg, Pos3List* poses);

static void carveCanyonInner(CanyonCarverConfig ccc, int mc, uint64_t *rnd, int sourceChunkX, int sourceChunkZ, int offsetChunkX, int offsetChunkZ, char carvingMask[], Pos3List* poses);

void carveCanyon(uint64_t seed, int mc, int chunkX, int chunkZ, CanyonCarverConfig ccc, int canyonCarverType, int biomes[17][17], Pos3List* poses) {
    const int worldHeight = mc > MC_1_17_1 ? 384 : 256;
    int slots = BITNSLOTS(256 * worldHeight);
    char carvingMask[slots];
    memset(carvingMask, 0, slots);

    for (int relChunkX = -8; relChunkX <= 8; ++relChunkX) {
        for (int relChunkZ = -8; relChunkZ <= 8; ++relChunkZ) {
            int offsetChunkX = chunkX + relChunkX;
            int offsetChunkZ = chunkZ + relChunkZ;
            int biome = biomes[relChunkZ + 8][relChunkX + 8];
            if (!isViableCanyonBiome(canyonCarverType, biome)) {
                continue;
            }
            uint64_t rnd;
            if (!checkCanyonStart(seed, offsetChunkX, offsetChunkZ, ccc, &rnd)) {
                continue;
            }

            carveCanyonInner(ccc, mc, &rnd, chunkX, chunkZ, offsetChunkX, offsetChunkZ, carvingMask, poses);
        }
    }
}

static void initWidthFactors(uint64_t* rnd, int worldHeight, float widthFactors[], CanyonCarverConfig ccc) {
    float f = 1.0F;

    // j == 0
    {
        float a = nextFloat(rnd);
        float b = nextFloat(rnd);
        f = 1.0F + a * b;
        widthFactors[0] = f * f;
    }
    for (int j = 1; j < worldHeight; j++) {
        if (nextInt(rnd, ccc.widthSmoothness) == 0) {
            float a = nextFloat(rnd);
            float b = nextFloat(rnd);
            f = 1.0F + a * b;
        }

        widthFactors[j] = f * f;
    }
}

static double updateVerticalRadius(CanyonCarverConfig ccc, uint64_t* rnd, double verticalRadius, float branchCount, float currentBranch) {
    float f = 1.0F - fabs(0.5F - currentBranch / branchCount) * 2.0F;
    float g = ccc.verticalRadiusDefaultFactor + ccc.verticalRadiusCenterFactor * f;
    return g * verticalRadius * nextFloatBetween(rnd, 0.75F, 1.0F);
}

static void carveCanyonInner(CanyonCarverConfig ccc, int mc, uint64_t *rnd, int sourceChunkX, int sourceChunkZ, int offsetChunkX, int offsetChunkZ, char carvingMask[], Pos3List* poses) {
    int range = (ccc.range * 2 - 1) * 16;
    double x = (offsetChunkX << 4) + nextInt(rnd, 16);
    double y = ccc.y(rnd, ccc.minY, ccc.maxY, ccc.innerY);
    double z = (offsetChunkZ << 4) + nextInt(rnd, 16);
    float yaw = nextFloat(rnd) * (float) (PI * 2);
    float pitch = ccc.verticalRotation(rnd, ccc.minVerRot, ccc.maxVerRot);
    double horizontalVerticalRatio = ccc.yScale;
    float thickness = ccc.thickness(rnd, ccc.minThickness, ccc.maxThickness, ccc.plateauThickness);
    int branchCount;
    // one could reuse `distanceFactor` for this and interpret the float bits as int bits, decided against it
    if (mc <= MC_1_16_5) {
        branchCount = range - nextInt(rnd, range / 4);
    } else {
        branchCount = (int)(range * ccc.distanceFactor(rnd, ccc.minDistance, ccc.maxDistance));
    }
    uint64_t seed = nextLong(rnd);
    setSeed(rnd, seed);
    int worldMinY;
    int worldHeight;
    if (mc > MC_1_17_1) {
        worldMinY = -64;
        worldHeight = 384;
    } else {
        worldMinY = 0;
        worldHeight = 256;
    }
    float widthFactors[worldHeight];
    initWidthFactors(rnd, worldHeight, widthFactors, ccc);
    float f = 0.0F;
    float g = 0.0F;

    for (int branchIndex = 0; branchIndex < branchCount; branchIndex++) {
        double horizontalRadius = 1.5 + sin(branchIndex * (float) PI / branchCount) * thickness;
        double verticalRadius = horizontalRadius * horizontalVerticalRatio;
        horizontalRadius *= ccc.horizontalRadiusFactor(rnd, ccc.minHorRadius, ccc.maxHorRadius);
        verticalRadius = updateVerticalRadius(ccc, rnd, verticalRadius, branchCount, branchIndex);
        float h = cos(pitch);
        float j = sin(pitch);
        x += cos(yaw) * h;
        y += j;
        z += sin(yaw) * h;
        pitch *= 0.7F;
        pitch += g * 0.05F;
        yaw += f * 0.05F;
        g *= 0.8F;
        f *= 0.5F;
        float f1 = nextFloat(rnd); float f2 = nextFloat(rnd); float f3 = nextFloat(rnd);
        g += (f1 - f2) * f3 * 2.0F;
        float f4 = nextFloat(rnd); float f5 = nextFloat(rnd); float f6 = nextFloat(rnd);
        f += (f4 - f5) * f6 * 4.0F;
        if (nextInt(rnd, 4) == 0) {
            continue;
        }
        if (!canReach(sourceChunkX, sourceChunkZ, x, z, branchIndex, branchCount, thickness)) {
            return;
        }

        carveEllipsoid(sourceChunkX, sourceChunkZ, x, y, z, horizontalRadius, verticalRadius, worldMinY, worldHeight, carvingMask, shouldSkipCanyonCarve, widthFactors, poses);
    }
}

static void carveCaveInner(CaveCarverConfig ccc, uint64_t* rnd, int sourceChunkX, int sourceChunkZ, int chunkX, int chunkZ, int mc, char carvingMask[], Pos3List* poses);

void carveCave(uint64_t seed, int mc, int chunkX, int chunkZ, CaveCarverConfig ccc, int caveCarverType, int biomes[17][17], Pos3List* poses) {
    int worldHeight;
    if (mc > MC_1_17_1) {
        worldHeight = ccc.dim == DIM_OVERWORLD ? 384 : 128;
    } else {
        worldHeight = ccc.dim == DIM_OVERWORLD ? 256 : 128;
    }
    int slots = BITNSLOTS(256 * worldHeight);
    char carvingMask[slots];
    memset(carvingMask, 0, slots);

    for (int relChunkX = -8; relChunkX <= 8; ++relChunkX) {
        for (int relChunkZ = -8; relChunkZ <= 8; ++relChunkZ) {
            int offsetChunkX = chunkX + relChunkX;
            int offsetChunkZ = chunkZ + relChunkZ;
            int biome = biomes[relChunkZ + 8][relChunkX + 8];
            if (!isViableCaveBiome(caveCarverType, biome)) {
                continue;
            }
            uint64_t rnd;
            if (!checkCaveStart(seed, offsetChunkX, offsetChunkZ, ccc, &rnd)) {
                continue;
            }
            carveCaveInner(ccc, &rnd, chunkX, chunkZ, offsetChunkX, offsetChunkZ, mc, carvingMask, poses);
        }
    }
}

static void createRoom(int sourceChunkX, int sourceChunkZ, double x, double y, double z, float radius, double horizontalVerticalRatio, int worldMinY, int worldHeight, char carvingMask[], double floorLevel, Pos3List* poses);

static void createTunnel(CaveCarverConfig ccc, int sourceChunkX, int sourceChunkZ, uint64_t seed, double x, double y, double z, double horizontalRadiusMultiplier, double verticalRadiusMultiplier, float thickness, float yaw, float pitch, int branchIndex, int branchCount, double horizontalVerticalRatio, int worldMinY, int worldHeight, char carvingMask[], double floorLevel, Pos3List* poses);

static void carveCaveInner(CaveCarverConfig ccc, uint64_t* rnd, int sourceChunkX, int sourceChunkZ, int chunkX, int chunkZ, int mc, char carvingMask[], Pos3List* poses) {
    int worldMinY;
    int worldHeight;
    if (mc > MC_1_17_1) {
        worldMinY = ccc.dim == DIM_OVERWORLD ? -64 : 0;
        worldHeight = ccc.dim == DIM_OVERWORLD ? 384 : 128;
    } else {
        worldMinY = 0;
        worldHeight = ccc.dim == DIM_OVERWORLD ? 256 : 128;
    }

    int range = (ccc.range * 2 - 1) << 4;

    int r1 = nextInt(rnd, ccc.caveBound);
    int r2 = nextInt(rnd, r1 + 1);
    int caveCount = nextInt(rnd, r2 + 1);

    for (int k = 0; k < caveCount; k++) {
        double x = (chunkX << 4) + nextInt(rnd, 16);
        double y = ccc.y(rnd, ccc.minY, ccc.maxY, ccc.innerY);
        double z = (chunkZ << 4) + nextInt(rnd, 16);
        double horizontalRadius = ccc.horizontalRadiusMultiplier(rnd, ccc.minHorRadius, ccc.maxHorRadius);
        double verticalRadius = ccc.verticalRadiusMultiplier(rnd, ccc.minVerRadius, ccc.maxVerRadius);
        double floorLevel = ccc.floorLevel(rnd, ccc.minFloorLevel, ccc.maxFloorLevel);
        int m = 1;
        if (nextInt(rnd, 4) == 0) {
            double yScale = ccc.yScale(rnd, ccc.minYScale, ccc.maxYScale);
            float radius = 1.0F + nextFloat(rnd) * 6.0F;
            if (mc <= MC_1_17_1) {
                nextLong(rnd);
            }
            createRoom(sourceChunkX, sourceChunkZ, x, y, z, radius, yScale, worldMinY, worldHeight, carvingMask, floorLevel, poses);
            m += nextInt(rnd, 4);
        }

        for (int p = 0; p < m; p++) {
            float q = nextFloat(rnd) * (float) (PI * 2);
            float o = (nextFloat(rnd) - 0.5F) / 4.0F;
            float r = ccc.thickness(rnd);
            int s = range - nextInt(rnd, range / 4);
            createTunnel(ccc, sourceChunkX, sourceChunkZ, nextLong(rnd), x, y, z, horizontalRadius, verticalRadius, r, q, o, 0, s, ccc.tunnelYScale, worldMinY, worldHeight, carvingMask, floorLevel, poses);
        }
    }
}

static void createRoom(int sourceChunkX, int sourceChunkZ, double x, double y, double z, float radius, double horizontalVerticalRatio, int worldMinY, int worldHeight, char carvingMask[], double floorLevel, Pos3List* poses) {
    double horizontalRadius = 1.5 + sin(PI / 2) * radius;
    double verticalRadius = horizontalRadius * horizontalVerticalRatio;
    carveEllipsoid(sourceChunkX, sourceChunkZ, x + 1.0, y, z, horizontalRadius, verticalRadius, worldMinY, worldHeight, carvingMask, shouldSkipCaveCarve, &floorLevel, poses);
}

static void createTunnel(CaveCarverConfig ccc, int sourceChunkX, int sourceChunkZ, uint64_t seed, double x, double y, double z, double horizontalRadiusMultiplier, double verticalRadiusMultiplier, float thickness, float yaw, float pitch, int branchIndex, int branchCount, double horizontalVerticalRatio, int worldMinY, int worldHeight, char carvingMask[], double floorLevel, Pos3List* poses) {
    uint64_t rnd;
    setSeed(&rnd, seed);
    int i = nextInt(&rnd, branchCount / 2) + branchCount / 4;
    int bl = nextInt(&rnd, 6) == 0;
    float f = 0.0F;
    float g = 0.0F;

    for (int j = branchIndex; j < branchCount; j++) {
        double d = 1.5 + sin((float) PI * j / branchCount) * thickness;
        double e = d * horizontalVerticalRatio;
        float h = cos(pitch);
        x += cos(yaw) * h;
        y += sin(pitch);
        z += sin(yaw) * h;
        pitch *= bl ? 0.92F : 0.7F;
        pitch += g * 0.1F;
        yaw += f * 0.1F;
        g *= 0.9F;
        f *= 0.75F;
        float f1 = nextFloat(&rnd); float f2 = nextFloat(&rnd); float f3 = nextFloat(&rnd);
        g += (f1 - f2) * f3 * 2.0F;
        float f4 = nextFloat(&rnd); float f5 = nextFloat(&rnd); float f6 = nextFloat(&rnd);
        f += (f4 - f5) * f6 * 4.0F;
        if (j == i && thickness > 1.0F) {
            uint64_t s1 = nextLong(&rnd); float t1 = nextFloat(&rnd);
            createTunnel(ccc, sourceChunkX, sourceChunkZ, s1, x, y, z, horizontalRadiusMultiplier, verticalRadiusMultiplier, t1 * 0.5F + 0.5F, yaw - (float) (PI / 2), pitch / 3.0F, j, branchCount, 1.0, worldMinY, worldHeight, carvingMask, floorLevel, poses);
            uint64_t s2 = nextLong(&rnd); float t2 = nextFloat(&rnd);
            createTunnel(ccc, sourceChunkX, sourceChunkZ, s2, x, y, z, horizontalRadiusMultiplier, verticalRadiusMultiplier, t2 * 0.5F + 0.5F, yaw + (float) (PI / 2), pitch / 3.0F, j, branchCount, 1.0, worldMinY, worldHeight, carvingMask, floorLevel, poses);
            return;
        }

        if (nextInt(&rnd, 4) == 0) continue;

        if (!canReach(sourceChunkX, sourceChunkZ, x, z, j, branchCount, thickness)) {
            return;
        }

        carveEllipsoid(sourceChunkX, sourceChunkZ, x, y, z, d * horizontalRadiusMultiplier, e * verticalRadiusMultiplier, worldMinY, worldHeight, carvingMask, shouldSkipCaveCarve, &floorLevel, poses);
    }
}

static void carveEllipsoid(int chunkX, int chunkZ, double x, double y, double z, double horizontalRadius, double verticalRadius, int worldMinY, int worldHeight, char carvingMask[], int (*shouldSkip)(double, double, double, int, int, void*), void* arg, Pos3List* poses) {
    const int startChunkX = chunkX << 4;
    const int startChunkZ = chunkZ << 4;
    const double midChunkX = startChunkX + 8;
    const double midChunkZ = startChunkZ + 8;
    double f = 16.0 + horizontalRadius * 2.0;
    if (fabs(x - midChunkX) > f || fabs(z - midChunkZ) > f) {
        return;
    }
    const double floorMinX = floor(x - horizontalRadius) - startChunkX - 1;
    const int minX = MAX(floorMinX, 0);
    const double floorMaxX = floor(x + horizontalRadius) - startChunkX;
    const int maxX = MIN(floorMaxX, 15);
    const double floorMinY = floor(y - verticalRadius) - 1;
    const int minY = MAX(floorMinY, worldMinY + 1);
    int n = 0;
    const double floorMaxY = floor(y + verticalRadius) + 1;
    const int maxY = MIN(floorMaxY, worldMinY + worldHeight - 1 - n);
    const double floorMinZ = floor(z - horizontalRadius) - startChunkZ - 1;
    const int minZ = MAX(floorMinZ, 0);
    const double floorMaxZ = floor(z + horizontalRadius) - startChunkZ;
    const int maxZ = MIN(floorMaxZ, 15);

    for (int relX = minX; relX <= maxX; relX++) {
        int absX = startChunkX + relX;
        double relativeX = (absX + 0.5 - x) / horizontalRadius;

        for (int relZ = minZ; relZ <= maxZ; relZ++) {
            int absZ = startChunkZ + relZ;
            double relativeZ = (absZ + 0.5 - z) / horizontalRadius;
            if (relativeX * relativeX + relativeZ * relativeZ >= 1.0) continue;

            for (int absY = maxY; absY > minY; absY--) {
                double relativeY = (absY - 0.5 - y) / verticalRadius;
                if (shouldSkip(relativeX, relativeY, relativeZ, absY, worldMinY, arg) || getCarveMask(carvingMask, relX, absY, relZ, worldMinY)) continue;
                setCarveMask(carvingMask, relX, absY, relZ, worldMinY);
                appendPos3List(poses, (Pos3) {absX, absY, absZ});
            }
        }
    }
}

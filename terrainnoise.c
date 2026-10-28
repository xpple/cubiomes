#include "terrainnoise.h"

#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>

static inline float peaksAndValleys(float weirdness) {
    return -(fabsf(fabsf(weirdness) - 0.6666667F) - 0.33333334F) * 3.0F;
}

static Spline* createErosionFactorSpline(SplineStack *ss, float value, int higherValues) {
    Spline *sp = &ss->stack[ss->len++];
    sp->typ = SP_WEIRDNESS;
    addSplineVal(sp, -0.2F, createFixSpline(ss, 6.3f), 0.0F);
    addSplineVal(sp, 0.2F, createFixSpline(ss, value), 0.0F);

    Spline* sp2 = &ss->stack[ss->len++];
    sp2->typ = SP_WEIRDNESS;
    addSplineVal(sp2, -0.05f, createFixSpline(ss, 6.3f), 0.0F);
    addSplineVal(sp2, 0.05f, createFixSpline(ss, 2.67f), 0.0F);

    Spline* sp3 = &ss->stack[ss->len++];
    sp3->typ = SP_WEIRDNESS;
    addSplineVal(sp3, -0.05f, createFixSpline(ss, 2.67f), 0.0F);
    addSplineVal(sp3, 0.05f, createFixSpline(ss, 6.3f), 0.0F);

    Spline* sp4 = &ss->stack[ss->len++];
    sp4->typ = SP_EROSION;
    addSplineVal(sp4, -0.6f, sp, 0.0F);
    addSplineVal(sp4, -0.5f, sp2, 0.0F);
    addSplineVal(sp4, -0.35f, sp, 0.0F);
    addSplineVal(sp4, -0.25f, sp, 0.0F);
    addSplineVal(sp4, -0.1f, sp3, 0.0F);
    addSplineVal(sp4, 0.03f, sp, 0.0F);

    if (higherValues) {
        Spline* sp5 = &ss->stack[ss->len++];
        sp5->typ = SP_WEIRDNESS;
        addSplineVal(sp5, 0.0f, createFixSpline(ss, value), 0.0F);
        addSplineVal(sp5, 0.1f, createFixSpline(ss, 0.625f), 0.0F);

        Spline* sp6 = &ss->stack[ss->len++];
        sp6->typ = SP_RIDGES;
        addSplineVal(sp6, -0.9f, createFixSpline(ss, value), 0.0F);
        addSplineVal(sp6, -0.69f, sp5, 0.0F);

        addSplineVal(sp4, 0.35f, createFixSpline(ss, value), 0.0F);
        addSplineVal(sp4, 0.45f, sp6, 0.0F);
        addSplineVal(sp4, 0.55f, sp6, 0.0F);
        addSplineVal(sp4, 0.62f, createFixSpline(ss, value), 0.0F);
    } else {
        Spline* sp5 = &ss->stack[ss->len++];
        sp5->typ = SP_RIDGES;
        addSplineVal(sp5, -0.7f, sp, 0.0F);
        addSplineVal(sp5, -0.15f, createFixSpline(ss, 1.37f), 0.0F);

        Spline* sp6 = &ss->stack[ss->len++];
        sp6->typ = SP_RIDGES;
        addSplineVal(sp6, 0.45f, sp, 0.0F);
        addSplineVal(sp6, 0.7f, createFixSpline(ss, 1.56f), 0.0F);

        addSplineVal(sp4, 0.05f, sp6, 0.0F);
        addSplineVal(sp4, 0.4f, sp6, 0.0F);
        addSplineVal(sp4, 0.45f, sp5, 0.0F);
        addSplineVal(sp4, 0.55f, sp5, 0.0F);
        addSplineVal(sp4, 0.58f, createFixSpline(ss, value), 0.0F);
    }

    return sp4;
}

static Spline* createFactorSpline(SplineStack *ss) {
    Spline *sp = &ss->stack[ss->len++];
    sp->typ = SP_CONTINENTALNESS;
    addSplineVal(sp, -0.19F, createFixSpline(ss, 3.95F), 0.0F);

    addSplineVal(sp, -0.15f, createErosionFactorSpline(ss, 6.25f, 1), 0.0F);
    addSplineVal(sp, -0.1f, createErosionFactorSpline(ss, 5.47f, 1), 0.0F);
    addSplineVal(sp, 0.03f, createErosionFactorSpline(ss, 5.08f, 1), 0.0F);
    addSplineVal(sp, 0.06f, createErosionFactorSpline(ss, 4.69f, 0), 0.0F);
    return sp;
}

static Spline* createWeirdnessJaggednessSpline(SplineStack *ss, float magnitude) {
    float f = 0.63F * magnitude;
    float g = 0.3F * magnitude;
    Spline *sp = &ss->stack[ss->len++];
    sp->typ = SP_WEIRDNESS;
    addSplineVal(sp, -0.01F, createFixSpline(ss, f), 0.0F);
    addSplineVal(sp, 0.01F, createFixSpline(ss, g), 0.0F);
    return sp;
}

static Spline* createRidgeJaggednessSpline(SplineStack *ss, float highWeirdnessMagnitude, float midWeirdnessMagnitude) {
    const float f = peaksAndValleys(0.4F);
    const float g = peaksAndValleys(0.56666666F);
    const float h = (f + g) / 2.0F;
    Spline *sp = &ss->stack[ss->len++];
    sp->typ = SP_RIDGES;
    addSplineVal(sp, f, createFixSpline(ss, 0.0F), 0.0F);
    if (midWeirdnessMagnitude > 0.0F) {
        addSplineVal(sp, h, createWeirdnessJaggednessSpline(ss, midWeirdnessMagnitude), 0.0F);
    } else {
        addSplineVal(sp, h, createFixSpline(ss, 0.0F), 0.0F);
    }

    if (highWeirdnessMagnitude > 0.0F) {
        addSplineVal(sp, 1.0F, createWeirdnessJaggednessSpline(ss, highWeirdnessMagnitude), 0.0F);
    } else {
        addSplineVal(sp, 1.0F, createFixSpline(ss, 0.0F), 0.0F);
    }

    return sp;
}

static Spline* createErosionJaggednessSpline(SplineStack *ss, float highErosionHighWeirdness, float lowErosionHighWeirdness, float highErosionMidWeirdness, float lowErosionMidWeirdness) {
    Spline* sp = &ss->stack[ss->len++];
    sp->typ = SP_EROSION;
    addSplineVal(sp, -1.0F, createRidgeJaggednessSpline(ss, highErosionHighWeirdness, highErosionMidWeirdness), 0.0F);

    Spline* sp2 = createRidgeJaggednessSpline(ss, lowErosionHighWeirdness, lowErosionMidWeirdness);
    addSplineVal(sp, -0.78F, sp2, 0.0F);
    addSplineVal(sp, -0.5775F, sp2, 0.0F);
    addSplineVal(sp, -0.375F, createFixSpline(ss, 0.0F), 0.0F);

    return sp;
}

static Spline* createJaggednessSpline(SplineStack *ss) {
    Spline *sp = &ss->stack[ss->len++];
    sp->typ = SP_CONTINENTALNESS;

    addSplineVal(sp, -0.11F, createFixSpline(ss, 0.0F), 0.0F);

    addSplineVal(sp, 0.03F, createErosionJaggednessSpline(ss, 1.0F, 0.5F, 0.0F, 0.0F), 0.0F);
    addSplineVal(sp, 0.65F, createErosionJaggednessSpline(ss, 1.0F, 1.0F, 1.0F, 0.0F), 0.0F);
    return sp;
}

void setupTerrainNoise(TerrainNoise *params, int mc, int flags) {
    setupGenerator(&params->g, mc, flags);
    if (mc >= MC_1_18) {
        memset(&params->ss, 0, sizeof(params->ss)); // reset spline stack so params can be re-initialized safely
        params->factorSpline = createFactorSpline(&params->ss);
        params->jaggednessSpline = createJaggednessSpline(&params->ss);
    }
}

static inline int initTerrainNoiseHelper(DoublePerlinNoise *dpn, const Xoroshiro xr, const uint64_t md5[2], PerlinNoise *oct, const int firstOctave, const double amplitudes[], const int length) {
    Xoroshiro x = {xr.lo ^ md5[0], xr.hi ^ md5[1]};
    return xDoublePerlinInit(dpn, &x, oct, amplitudes, firstOctave, length, -1);
}

void initTerrainNoise(TerrainNoise *params, uint64_t ws, int dim) {
    static const uint64_t md5_pillar[2] = {0xd4defd1400fa5347, 0xccb6785451feaa1c}; // minecraft:pillar
    static const uint64_t md5_pillar_rareness[2] = {0x8ddd6be7cd4b24d0, 0x0485e8665333197a}; // minecraft:pillar_rareness
    static const uint64_t md5_pillar_thickness[2] = {0x1a28cb2542f8308d, 0xc4bba10b7fc7168c}; // minecraft:pillar_thickness
    static const uint64_t md5_spaghetti_2d[2] = {0x34748c98fa19c477, 0x2a02051eb9e9b9cd}; // minecraft:spaghetti_2d
    static const uint64_t md5_spaghetti_2d_elevation[2] = {0x29dd5fcf38d9edcd, 0x5bb1d7a839f6d4ab}; // minecraft:spaghetti_2d_elevation
    static const uint64_t md5_spaghetti_2d_modulator[2] = {0x95e07097e4685522, 0x0232deaf95eb5fed}; // minecraft:spaghetti_2d_modulator
    static const uint64_t md5_spaghetti_2d_thickness[2] = {0x37e5cb432b85f4c8, 0xe5e16b35b407aebc}; // minecraft:spaghetti_2d_thickness
    static const uint64_t md5_spaghetti_3d_1[2] = {0xa5053f53ce3f5840, 0x834fa38a3ded87b7}; // minecraft:spaghetti_3d_1
    static const uint64_t md5_spaghetti_3d_2[2] = {0xb9a6367335a0ceef, 0xd61ccfd0fac10ac3}; // minecraft:spaghetti_3d_2
    static const uint64_t md5_spaghetti_3d_rarity[2] = {0xee4780c98d93a439, 0xbe45d334fdb76d2c}; // minecraft:spaghetti_3d_rarity
    static const uint64_t md5_spaghetti_3d_thickness[2] = {0x897e1f20ad2d2b27, 0xf4142f5a58d1d5ab}; // minecraft:spaghetti_3d_thickness
    static const uint64_t md5_spaghetti_roughness[2] = {0x7aed57fd327d6591, 0xd495a3593e4d67bd}; // minecraft:spaghetti_roughness
    static const uint64_t md5_spaghetti_roughness_modulator[2] = {0xe19387d2ceaf4eaa, 0xcacdcd34e1bc2003}; // minecraft:spaghetti_roughness_modulator
    static const uint64_t md5_cave_entrance[2] = {0xf1008a17493d9a65, 0xbd1ce09e8bc70ea0}; // minecraft:cave_entrance
    static const uint64_t md5_cave_layer[2] = {0x4dfe67be2ef51a83, 0x5a4ae8c4423e7206}; // minecraft:cave_layer
    static const uint64_t md5_cave_cheese[2] = {0xb159093bc7baaa50, 0x53abc45424417c20}; // minecraft:cave_cheese
    static const uint64_t md5_noodle[2] = {0xd23ce12b0c37e44b, 0x66b1fdfbf6a474f3}; // minecraft:noodle
    static const uint64_t md5_noodle_thickness[2] = {0x2fa49bbe949d4212, 0x5f82c95251d19891}; // minecraft:noodle_thickness
    static const uint64_t md5_noodle_ridge_a[2] = {0x86172ac1315f6026, 0x4a664470c7d7205f}; // minecraft:noodle_ridge_a
    static const uint64_t md5_noodle_ridge_b[2] = {0xeb232b4b89fdde91, 0x031ee565ead84e5c}; // minecraft:noodle_ridge_b
    static const uint64_t md5_jagged[2] = {0xf902c0a7c9daa994, 0x71ecd96a8da5e503}; // minecraft:jagged

    applySeed(&params->g, dim, ws);

    initBlendedNoise(&params->base3dNoise, ws, dim);

    if (dim != DIM_NETHER && (params->g.mc < MC_1_18 || dim == DIM_END)) {
        initSurfaceNoise(&params->sn, dim, ws);
    }

    if (params->g.mc >= MC_1_18 && dim == DIM_OVERWORLD) {
        Xoroshiro wsx;
        xSetSeed(&wsx, ws);
        const uint64_t lo = xNextLong(&wsx);
        const uint64_t hi = xNextLong(&wsx);
        const Xoroshiro xr = {lo, hi};

        int n = 0;
        static const double pillar_amp[2] = {1.0, 1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_PILLAR], xr, md5_pillar, params->oct + n, -7, pillar_amp, 2);
        static const double pillar_rareness_amp[1] = {1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_PILLAR_RARENESS], xr, md5_pillar_rareness, params->oct + n, -8, pillar_rareness_amp, 1);
        static const double pillar_thickness_amp[1] = {1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_PILLAR_THICKNESS], xr, md5_pillar_thickness, params->oct + n, -8, pillar_thickness_amp, 1);
        static const double spaghetti_2d_amp[1] = {1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_SPAGHETTI_2D], xr, md5_spaghetti_2d, params->oct + n, -7, spaghetti_2d_amp, 1);
        static const double spaghetti_2d_elevation_amp[1] = {1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_SPAGHETTI_2D_ELEVATION], xr, md5_spaghetti_2d_elevation, params->oct + n, -8, spaghetti_2d_elevation_amp, 1);
        static const double spaghetti_2d_modulator_amp[1] = {1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_SPAGHETTI_2D_MODULATOR], xr, md5_spaghetti_2d_modulator, params->oct + n, -11, spaghetti_2d_modulator_amp, 1);
        static const double spaghetti_2d_thickness_amp[1] = {1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_SPAGHETTI_2D_THICKNESS], xr, md5_spaghetti_2d_thickness, params->oct + n, -11, spaghetti_2d_thickness_amp, 1);
        static const double spaghetti_3d_1_amp[1] = {1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_SPAGHETTI_3D_1], xr, md5_spaghetti_3d_1, params->oct + n, -7, spaghetti_3d_1_amp, 1);
        static const double spaghetti_3d_2_amp[1] = {1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_SPAGHETTI_3D_2], xr, md5_spaghetti_3d_2, params->oct + n, -7, spaghetti_3d_2_amp, 1);
        static const double spaghetti_3d_rarity_amp[1] = {1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_SPAGHETTI_3D_RARITY], xr, md5_spaghetti_3d_rarity, params->oct + n, -11, spaghetti_3d_rarity_amp, 1);
        static const double spaghetti_3d_thickness_amp[1] = {1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_SPAGHETTI_3D_THICKNESS], xr, md5_spaghetti_3d_thickness, params->oct + n, -8, spaghetti_3d_thickness_amp, 1);
        static const double spaghetti_roughness_amp[1] = {1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_SPAGHETTI_ROUGHNESS], xr, md5_spaghetti_roughness, params->oct + n, -5, spaghetti_roughness_amp, 1);
        static const double spaghetti_roughness_modulator_amp[1] = {1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_SPAGHETTI_ROUGHNESS_MODULATOR], xr, md5_spaghetti_roughness_modulator, params->oct + n, -8, spaghetti_roughness_modulator_amp, 1);
        static const double cave_entrance_amp[3] = {0.4, 0.5, 1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_CAVE_ENTRANCE], xr, md5_cave_entrance, params->oct + n, -7, cave_entrance_amp, 3);
        static const double cave_layer_amp[1] = {1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_CAVE_LAYER], xr, md5_cave_layer, params->oct + n, -8, cave_layer_amp, 1);
        static const double cave_cheese_amp[9] = {0.5, 1.0, 2.0, 1.0, 2.0, 1.0, 0.0, 2.0, 0.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_CAVE_CHEESE], xr, md5_cave_cheese, params->oct + n, -8, cave_cheese_amp, 9);
        static const double noodle_amp[1] = {1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_NOODLE], xr, md5_noodle, params->oct + n, -8, noodle_amp, 1);
        static const double noodle_thickness_amp[1] = {1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_NOODLE_THICKNESS], xr, md5_noodle_thickness, params->oct + n, -8, noodle_thickness_amp, 1);
        static const double noodle_ridge_a_amp[1] = {1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_NOODLE_RIDGE_A], xr, md5_noodle_ridge_a, params->oct + n, -7, noodle_ridge_a_amp, 1);
        static const double noodle_ridge_b_amp[1] = {1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_NOODLE_RIDGE_B], xr, md5_noodle_ridge_b, params->oct + n, -7, noodle_ridge_b_amp, 1);
        static const double jagged_amp[16] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_JAGGED], xr, md5_jagged, params->oct + n, -16, jagged_amp, 16);

        // 1.18+ NoiseBasedAquifer noises: barrier, floodedness, fluid level spread, and lava band
        static const uint64_t md5_aquifer_barrier[2] = {0xe170fda6c51d093f, 0x134f3e4bc3f40b0c}; // minecraft:aquifer_barrier
        static const uint64_t md5_aquifer_floodedness[2] = {0x1b5d4f85fdc2d89e, 0x22c7e3e611bf67c6}; // minecraft:aquifer_fluid_level_floodedness
        static const uint64_t md5_aquifer_spread[2] = {0x08f136f47cd9aaa2, 0x3666182f5fa0ac6f}; // minecraft:aquifer_fluid_level_spread
        static const uint64_t md5_aquifer_lava[2] = {0x0e5121277ab51b72, 0x2724d8c56aa005ba}; // minecraft:aquifer_lava
        static const uint64_t md5_aquifer[2] = {0x7bb15cc403c6ace6, 0x0bdd56bc9d232691}; // minecraft:aquifer
        static const double aq_amp[1] = {1.0};
        n += initTerrainNoiseHelper(&params->noises[OTP_AQUIFER_BARRIER], xr, md5_aquifer_barrier, params->oct + n, -3, aq_amp, 1);
        n += initTerrainNoiseHelper(&params->noises[OTP_AQUIFER_FLUID_LEVEL_FLOODEDNESS], xr, md5_aquifer_floodedness, params->oct + n, -7, aq_amp, 1);
        n += initTerrainNoiseHelper(&params->noises[OTP_AQUIFER_FLUID_LEVEL_SPREAD], xr, md5_aquifer_spread, params->oct + n, -5, aq_amp, 1);
        n += initTerrainNoiseHelper(&params->noises[OTP_AQUIFER_LAVA], xr, md5_aquifer_lava, params->oct + n, -1, aq_amp, 1);

        // Positional random of `minecraft:aquifer`: the first two 64-bit words derived from the world seed,
        // used to jitter aquifer grid sample points within each 16x12x16 cell.
        Xoroshiro aqr = {xr.lo ^ md5_aquifer[0], xr.hi ^ md5_aquifer[1]};
        params->aquiferPositional.lo = xNextLong(&aqr);
        params->aquiferPositional.hi = xNextLong(&aqr);

        // Invalidate chunk caches
        params->aqCache.chunkKey = INT64_MIN;
        params->owCache.chunkKey = INT64_MIN;
        for (int i = 0; i < AQUIFER_PSL_CACHE_SIZE; i++) {
            params->pslKeys[i] = INT64_MIN;
        }

        if ((size_t)n > sizeof(params->oct) / sizeof(*params->oct)) {
            fprintf(stderr, "initTerrainNoise(): TerrainNoise is malformed, buffer too small\n");
            exit(1);
        }
    }
}

/// alias for clampedMap
ATTR(always_inline)
static inline double yClampedGradient(double y, int fromY, int toY, double fromValue, double toValue) {
    return clampedMap(y, fromY, toY, fromValue, toValue);
}

ATTR(always_inline)
static inline double mapFromUnitTo(double input, double fromY, double toY) {
    return ((fromY + toY) * 0.5) + (((toY - fromY) * 0.5) * input);
}

ATTR(always_inline)
static inline double remap(double value, double inMin, double inMax, double outMin, double outMax) {
    const double slope = (outMax - outMin) / (inMax - inMin);
    const double intercept = outMin - inMin * slope;
    return value * slope + intercept;
}

ATTR(always_inline)
static inline double rangeChoice(double input, double minInclusive, double maxExclusive, double whenInRange, double whenOutOfRange) {
    return input >= minInclusive && input < maxExclusive ? whenInRange : whenOutOfRange;
}

ATTR(always_inline)
static inline double yLimitedInterpolatable(double input, double whenInRange, int minY, int maxY, int whenOutOfRange) {
    // interpolate?
    return rangeChoice(input, minY, maxY + 1, whenInRange, whenOutOfRange);
}

ATTR(always_inline)
static inline double postProcess(double densityFunction) {
    double clamped = clamp(densityFunction * 0.64, -1.0, 1.0);
    return clamped / 2.0 - clamped * clamped * clamped / 24.0;
}

ATTR(always_inline)
static inline double slide(double input, int minY, int height, int topStartOffset, int topEndOffset, double topDelta, int bottomStartOffset, int bottomEndOffset, double bottomDelta, int y) {
    double a = clampedMap(y, minY + height - topStartOffset, minY + height - topEndOffset, 1.0, 0.0);
    double b = lerp(a, topDelta, input);
    double c = clampedMap(y, minY + bottomStartOffset, minY + bottomEndOffset, 0.0, 1.0);
    return lerp(c, bottomDelta, b);
}

ATTR(always_inline)
static inline double slideOverworld(double densityFunction, int y) {
    return slide(densityFunction, -64, 384, 80, 64, -0.078125, 0, 24, 0.1171875, y);
}

ATTR(always_inline)
static inline double slideNetherLike(double densityFunction, int minY, int height, int y) {
    return slide(densityFunction, minY, height, 24, 0, 0.9375, -8, 24, 2.5, y);
}

ATTR(always_inline)
static inline double getSpaghettiRarity2d(double value) {
    if (value < -0.75) {
        return 0.5;
    } else if (value < -0.5) {
        return 0.75;
    } else if (value < 0.5) {
        return 1.0;
    } else {
        return value < 0.75 ? 2.0 : 3.0;
    }
}

ATTR(always_inline)
static inline double getSpaghettiRarity3d(double value) {
    if (value < -0.5) {
        return 0.75;
    } else if (value < 0.0) {
        return 1.0;
    } else {
        return value < 0.5 ? 1.5 : 2.0;
    }
}

ATTR(always_inline)
static inline double noiseGradientDensity(double min, double max) {
    double f = max * min;
    return f > 0. ? f * 4. : f;
}

double sampleSpaghettiRoughness(TerrainNoise *params, int x, int y, int z) {
    double spaghettiRoughnessModulator = map(sampleDoublePerlin(&params->noises[OTP_SPAGHETTI_ROUGHNESS_MODULATOR], x, y, z), -1.0, 1.0, 0.0, 0.1);
    return (0.4 - fabs(sampleDoublePerlin(&params->noises[OTP_SPAGHETTI_ROUGHNESS], x, y, z))) * spaghettiRoughnessModulator;
}

double sampleSpaghetti2dThicknessModulator(TerrainNoise *params, int x, int y, int z) {
    return map(sampleDoublePerlin(&params->noises[OTP_SPAGHETTI_2D_THICKNESS], x * 2, y, z * 2), -1.0, 1.0, -0.6, -1.3);
}

double sampleSpaghetti2d(TerrainNoise *params, int x, int y, int z) {
    double spaghetti2dModulator = sampleDoublePerlin(&params->noises[OTP_SPAGHETTI_2D_MODULATOR], x * 2, y, z * 2);
    double rarity = getSpaghettiRarity2d(spaghetti2dModulator);
    double spaghetti2dThickness = map(sampleDoublePerlin(&params->noises[OTP_SPAGHETTI_2D_THICKNESS], x * 2, y, z * 2), -1.0, 1.0, 0.6, 1.3);
    double spaghetti2d = sampleDoublePerlin(&params->noises[OTP_SPAGHETTI_2D], x / rarity, y / rarity, z / rarity);
    double a = fabs(rarity * spaghetti2d) - 0.083 * spaghetti2dThickness;
    double spaghetti2dElevation = map(sampleDoublePerlin(&params->noises[OTP_SPAGHETTI_2D_ELEVATION], x, 0.0, z), -1.0, 1.0, floordiv(-64, 8), 8.0);
    double b = fabs(spaghetti2dElevation - y / 8.0) - spaghetti2dThickness;
    return clamp(fmax(b * b * b, a), -1.0, 1.0);
}

double sampleSpaghetti3d(TerrainNoise *params, int x, int y, int z) {
    double spaghetti3dRarity = sampleDoublePerlin(&params->noises[OTP_SPAGHETTI_3D_RARITY], x * 2, y, z * 2);
    double spaghetti3dThickness = mapFromUnitTo(sampleDoublePerlin(&params->noises[OTP_SPAGHETTI_3D_THICKNESS], x, y, z), -0.065, -0.088);
    double rarity = getSpaghettiRarity3d(spaghetti3dRarity);
    double spaghetti3d1 = rarity * fabs(sampleDoublePerlin(&params->noises[OTP_SPAGHETTI_3D_1], x / rarity, y / rarity, z / rarity));
    double spaghetti3d2 = rarity * fabs(sampleDoublePerlin(&params->noises[OTP_SPAGHETTI_3D_2], x / rarity, y / rarity, z / rarity));
    return clamp(fmax(spaghetti3d1, spaghetti3d2) + spaghetti3dThickness, -1.0, 1.0);
}

double sampleCaveEntrance(TerrainNoise *params, int x, int y, int z) {
    double caveEntrance = sampleDoublePerlin(&params->noises[OTP_CAVE_ENTRANCE], x * 0.75, y * 0.5, z * 0.75);
    return caveEntrance + 0.37 + yClampedGradient(y, -10, 30, 0.3, 0.0);
}

double sampleEntrances(TerrainNoise *params, int x, int y, int z, double spaghettiRoughness) {
    double spaghetti3d = sampleSpaghetti3d(params, x, y, z);
    double caveEntrance = sampleCaveEntrance(params, x, y, z);
    return fmin(caveEntrance, spaghettiRoughness + spaghetti3d);
}

double sampleCaveLayer(TerrainNoise *params, int x, int y, int z) {
    double caveLayer = sampleDoublePerlin(&params->noises[OTP_CAVE_LAYER], x, y * 8, z);
    return caveLayer * caveLayer * 4.0;
}

double sampleSlopedCheese(TerrainNoise *params, int x, int y, int z, double depth, double factor, double jagged) {
    depth = depth + clampedMap(y, -64, 320, 1.5, -1.5);
    double density = noiseGradientDensity(factor, depth + jagged);
    return density + sampleBase3dNoise(&params->base3dNoise, x, y, z);
}

double sampleCaveCheese(TerrainNoise *params, int x, int y, int z, double slopedCheese) {
    double caveCheese = sampleDoublePerlin(&params->noises[OTP_CAVE_CHEESE], x, y * 0.6666666666666666, z);
    return clamp(0.27 + caveCheese, -1.0, 1.0) + clamp(1.5 + (-0.64 * slopedCheese), 0.0, 0.5);
}

double samplePillars(TerrainNoise *params, int x, int y, int z) {
    double pillar = sampleDoublePerlin(&params->noises[OTP_PILLAR], x * 25, y * 0.3, z * 25);
    double pillarRareness = map(sampleDoublePerlin(&params->noises[OTP_PILLAR_RARENESS], x, y, z), -1.0, 1.0, 0.0, -2.0);
    double pillarThickness = map(sampleDoublePerlin(&params->noises[OTP_PILLAR_THICKNESS], x, y, z), -1.0, 1.0, 0.0, 1.1);
    return (pillar * 2.0 + pillarRareness) * pillarThickness * pillarThickness * pillarThickness;
}

double sampleNoodle(TerrainNoise *params, int x, int y, int z) {
    double noodle = yLimitedInterpolatable(y, sampleDoublePerlin(&params->noises[OTP_NOODLE], x, y, z), -60, 320, -1);
    double noodleThickness = yLimitedInterpolatable(y, map(sampleDoublePerlin(&params->noises[OTP_NOODLE_THICKNESS], x, y, z), -1.0, 1.0, -0.05, -0.1), -60, 320, 0);
    double noodleRidgeA = yLimitedInterpolatable(y, sampleDoublePerlin(&params->noises[OTP_NOODLE_RIDGE_A], x * 2.6666666666666665, y * 2.6666666666666665, z * 2.6666666666666665), -60, 320, 0);
    double noodleRidgeB = yLimitedInterpolatable(y, sampleDoublePerlin(&params->noises[OTP_NOODLE_RIDGE_B], x * 2.6666666666666665, y * 2.6666666666666665, z * 2.6666666666666665), -60, 320, 0);
    double scaledMax = 1.5 * fmax(fabs(noodleRidgeA), fabs(noodleRidgeB));
    return rangeChoice(noodle, -1000000.0, 0.0, 64.0, noodleThickness + scaledMax);
}

double sampleUnderground(TerrainNoise *params, int x, int y, int z, double spaghettiRoughness, double entrances, double slopedCheese) {
    double spaghetti2d = sampleSpaghetti2d(params, x, y, z);
    double caveLayer = sampleCaveLayer(params, x, y, z);
    double a = fmin(caveLayer + sampleCaveCheese(params, x, y, z, slopedCheese), entrances);
    double b = fmin(a, spaghetti2d + spaghettiRoughness);
    double pillars = samplePillars(params, x, y, z);
    double c = pillars >= -1000000.0 && pillars < 0.03 ? -1000000.0 : pillars;
    return fmax(b, c);
}

double sampleFinalDensity(TerrainNoise *params, int x, int y, int z, double spaghettiRoughness, double entrances, double slopedCheese) {
    double a = fmin(slopedCheese, 5.0 * entrances);
    double b = rangeChoice(slopedCheese, -1000000.0, 1.5625, a, sampleUnderground(params, x, y, z, spaghettiRoughness, entrances, slopedCheese));
    return fmin(postProcess(slideOverworld(b, y)), sampleNoodle(params, x, y, z));
}

// samplePreliminarySurfaceLevel() is implemented below, together with the aquifer.

static inline double sampleDepthNoise(const OctaveNoise *on, int x, int z, int mc) {
    double noise = sampleOctaveAmp(on, x * 200, 10.0, z * 200, 1.0, 0.0, 1);

    // pre-1.16 terrain expects depth noise at the unnormalised 16-octave scale
    // (2^16 - 1 = 65535), then scales it by 1/8000. Since `octaveInit` normalises
    // the amplitudes to sum to 1, restore the vanilla scale first
    if (mc < MC_1_16_1) {
        noise *= 65535.0;
        noise /= 8000.0;
    }

    noise = noise < 0.0 ? -noise * 0.3 : noise;

    if (mc >= MC_1_16_1) {
        noise = noise * 3.0 * 65535.0 / 8000.0 - 2.0;
        if (noise < 0.0) {
            return 17.0 * noise / 28.0 / 64.0;
        }
        return fmin(noise, 1.0) * 17.0 / 40.0 / 64.0;
    }

    noise = noise * 3.0 - 2.0;

    if (mc <= MC_1_13_2) {
        if (noise < 0.0) {
            noise = noise / 2.0;
            if (noise < -1.0) {
                noise = -1.0;
            }
            noise = noise / 1.4 / 2.0;
        } else {
            if (noise > 1.0) {
                noise = 1.0;
            }
            noise = noise / 8.0;
        }
        return noise;
    }

    if (noise < 0.0) {
        return noise / 28.0;
    }
    return fmin(noise, 1.0) / 40.0;
}

static inline void getWeightedDepthAndScale(Generator *g, OctaveNoise *on, int cellX, int cellZ, double *depth, double *scale) {
    // 10 / sqrt(i**2 + j**2 + 0.2)
    static const float BIOME_KERNEL[25] = {
        3.492151499f, 4.385290146f, 4.879500389f, 4.385290146f, 3.492151499f,
        4.385290146f, 6.741998196f, 9.128708839f, 6.741998196f, 4.385290146f,
        4.879500389f, 9.128708839f, 22.36067963f, 9.128708839f, 4.879500389f,
        4.385290146f, 6.741998196f, 9.128708839f, 6.741998196f, 4.385290146f,
        3.492151499f, 4.385290146f, 4.879500389f, 4.385290146f, 3.492151499f,
    };

    Range r = {4, cellX - 2, cellZ - 2, 5, 5, 0, 1};
    int *cache = allocCache(g, r);
    genBiomes(g, cache, r);

    double depthAtCenter, scaleAtCenter;
    int biomeAtCenter = cache[2 * r.sx + 2];
    getBiomeDepthAndScale(biomeAtCenter, &depthAtCenter, &scaleAtCenter, NULL);

    double weightedScale = 0.0;
    double weightedDepth = 0.0;
    double totalWeight = 0.0;

    for (int rz = 0; rz < 5; ++rz) {
        for (int rx = 0; rx < 5; ++rx) {
            int biomeId = cache[rz * r.sx + rx];
            double biomeDepth, biomeScale;
            getBiomeDepthAndScale(biomeId, &biomeDepth, &biomeScale, NULL);

            double weight = BIOME_KERNEL[rx + rz * 5] / (biomeDepth + 2.0);
            if (biomeDepth > depthAtCenter) {
                weight *= 0.5;
            }

            weightedScale += biomeScale * weight;
            weightedDepth += biomeDepth * weight;
            totalWeight += weight;
        }
    }
    free(cache);

    weightedScale /= totalWeight;
    weightedDepth /= totalWeight;
    weightedScale = weightedScale * 0.9 + 0.1;
    weightedDepth = (weightedDepth * 4.0 - 1.0) / 8.0;

    if (g->mc >= MC_1_16_1) {
        *depth = weightedDepth * 17.0 / 64.0;
        *scale = 96.0 / weightedScale;
    } else {
        double noise = sampleDepthNoise(on, cellX, cellZ, g->mc);
        *depth = weightedDepth + noise;
        if (g->mc <= MC_1_13_2) {
            *depth = weightedDepth + noise * 0.2;
        }
        *scale = weightedScale;
    }
}

static inline double computeNoiseFalloff(double depth, double scale, int y) {
    double fallOff = ((double)y - (8.5 + depth * 8.5 / 8.0 * 4.0)) * 12.0 * 128.0 / 256.0 / scale;
    if (fallOff < 0.0) {
        fallOff *= 4.0;
    }
    return fallOff;
}

void sampleOWNoiseColumnOld(TerrainNoise *params, int cellX, int cellZ, int colYMin, int colYMax, double column[]) {
    const int worldHeight = 256;
    const int cellHeight = 2 << 2;
    const int noiseSizeY = floordiv(worldHeight, cellHeight);

    const double densityFactor = 1.0;
    const double densityOffset = -0.46875;

    const double maxNoiseY = (double)(noiseSizeY + 1) - 4.0;
    const double minNoiseY = 0;

    const int topSlideTarget = -10;
    const int topSlideSize = 3;
    const int topSlideOffset = 0;

    const int bottomSlideTarget = -30;
    const int bottomSlideSize = 0;
    const int bottomSlideOffset = 0;

    double depth, scale;
    getWeightedDepthAndScale(&params->g, &params->sn.octdepth, cellX, cellZ, &depth, &scale);

    double randomOffset = sampleDepthNoise(&params->sn.octdepth, cellX, cellZ, params->g.mc);

    for (int y = colYMin; y <= colYMax; ++y) {
        double noise = sampleSurfaceNoise(&params->sn, cellX, y, cellZ);

        if (params->g.mc >= MC_1_16_1) {
            double fallOff = 1.0 - (double)y * 2.0 / (double)noiseSizeY + randomOffset;
            fallOff = fallOff * densityFactor + densityOffset;
            fallOff = (fallOff + depth) * scale;
            noise = fallOff > 0.0 ? noise + fallOff * 4.0 : noise + fallOff;

            if (topSlideSize > 0.0) {
                double t = ((double)(noiseSizeY - y) - topSlideOffset) / topSlideSize;
                noise = clampedLerp(t, topSlideTarget, noise);
            }

            if (bottomSlideSize > 0.0) {
                double t = ((double)y - bottomSlideOffset) / bottomSlideSize;
                noise = clampedLerp(t, bottomSlideTarget, noise);
            }
        } else {
            noise -= computeNoiseFalloff(depth, scale, y);

            double sizeY = maxNoiseY;
            double minY = minNoiseY;

            if ((double)y > sizeY) {
                double t = (y - sizeY - topSlideOffset) / topSlideSize;
                noise = clampedLerp(t, noise, topSlideTarget);
            } else if((double)y < minY) {
                double t = (minY - (double)y) / (minY - 1.0);
                noise = clampedLerp(t, noise, bottomSlideTarget);
            }
        }

        column[y - colYMin] = noise;
    }
}

static void sampleCornerColumn18(TerrainNoise *params, int cellX, int cellZ, int colYMin, int colYMax, double out[]);

void sampleNoiseColumn(TerrainNoise *params, int cellX, int cellZ, int colYMin, int colYMax, double column[]) {
    switch (params->g.dim) {
    case DIM_NETHER:
        sampleNetherNoiseColumn(&params->base3dNoise, cellX, cellZ, colYMin, colYMax, column);
        return;
    case DIM_END:
        sampleNoiseColumnEnd(column, &params->sn, &params->g.en, cellX, cellZ, colYMin, colYMax);
        return;
    default: break;
    }
    if (params->g.mc <= MC_1_17_1) {
        sampleOWNoiseColumnOld(params, cellX, cellZ, colYMin, colYMax, column);
        return;
    }

    // Pre-squeeze, clamped main density at the cell corners (see sampleCornerColumn18). Its sign
    // equals the sign of the squeezed inal_density without noodles and aquifers.
    sampleCornerColumn18(params, cellX, cellZ, colYMin, colYMax, column);
}

int generateColumn(uint8_t blocks[], const double *ds00, const double *ds01, const double *ds10, const double *ds11, int colYMin, int colYMax, int cellHeight, double percentX, double percentZ, int interpFunc, int worldMinY, int flag) {
    for (int cellY = colYMax - 1; cellY >= colYMin; --cellY) {
        const int cellIdx = cellY - colYMin;
        const double noise000 = ds00[cellIdx];
        const double noise001 = ds01[cellIdx];
        const double noise100 = ds10[cellIdx];
        const double noise101 = ds11[cellIdx];
        const double noise010 = ds00[cellIdx + 1];
        const double noise011 = ds01[cellIdx + 1];
        const double noise110 = ds10[cellIdx + 1];
        const double noise111 = ds11[cellIdx + 1];

        const int cellBlockIdx = cellIdx * cellHeight;
        for (int relY = cellHeight - 1; relY >= 0; --relY) {
            const double percentY = relY / (double) cellHeight;
            double noise;
            // using a function pointer is significantly slower due to inlining problems
            switch (interpFunc) {
            case INTERP_PRE_1_18: noise = lerp3(percentY, percentX, percentZ, noise000, noise010, noise100, noise110, noise001, noise011, noise101, noise111); break;
            case INTERP_1_18: noise = lerp3(percentX, percentY, percentZ, noise000, noise100, noise010, noise110, noise001, noise101, noise011, noise111); break;
            default: UNREACHABLE();
            }

            const uint8_t block = noise > 0.0;
            if (blocks) {
                blocks[cellBlockIdx + relY] = block;
            }

            if (flag && block) {
                const int y = worldMinY + cellY * cellHeight + relY;
                return y + 1;
            }
        }
    }
    return worldMinY;
}

void generateRegion(TerrainNoise *params, int chunkX, int chunkZ, int chunkW, int chunkH, uint8_t (*blocks)[], int colYMin, int colYMax, int* ys, int flag) {
    const int blockH = chunkH << 4;

    int cellWidth, cellHeight;
    switch (params->g.dim) {
    case DIM_OVERWORLD:
    case DIM_NETHER:
        cellWidth = 4;
        cellHeight = 8;
        break;
    case DIM_END:
        cellWidth = 8;
        cellHeight = 4;
        break;
    default: UNREACHABLE();
    }

    int height = (colYMax - colYMin) * cellHeight;
    uint8_t (*blocks2)[height] = (uint8_t (*)[height]) blocks;

    const int chunkCellsW = floordiv(chunkW << 4, cellWidth) + 1;
    const int chunkCellsH = floordiv(chunkH << 4, cellWidth) + 1;
    const int minCellX = floordiv(chunkX << 4, cellWidth);
    const int minCellZ = floordiv(chunkZ << 4, cellWidth);

    const int interpFunc = params->g.mc >= MC_1_18 ? INTERP_1_18 : INTERP_PRE_1_18;
    const int worldMinY = params->g.mc >= MC_1_18 && params->g.dim == DIM_OVERWORLD ? -64 : 0;

    const int cells = chunkCellsW * chunkCellsH;
    double (*ds)[colYMax - colYMin + 1] = malloc(cells * sizeof(*ds));

    for (int relCellX = 0; relCellX < chunkCellsW; ++relCellX) {
        const int cellX = minCellX + relCellX;
        for (int relCellZ = 0; relCellZ < chunkCellsH; ++relCellZ) {
            const int cellZ = minCellZ + relCellZ;
            const int idx = relCellX * chunkCellsH + relCellZ;
            sampleNoiseColumn(params, cellX, cellZ, colYMin, colYMax, ds[idx]);
        }
    }

    for (int relCellX = 0; relCellX < chunkCellsW - 1; ++relCellX) {
        const int relBlockX = relCellX * cellWidth;
        for (int relCellZ = 0; relCellZ < chunkCellsH - 1; ++relCellZ) {
            const int relBlockZ = relCellZ * cellWidth;

            const double *ds00 = ds[(relCellX + 0) * chunkCellsH + (relCellZ + 0)];
            const double *ds01 = ds[(relCellX + 0) * chunkCellsH + (relCellZ + 1)];
            const double *ds10 = ds[(relCellX + 1) * chunkCellsH + (relCellZ + 0)];
            const double *ds11 = ds[(relCellX + 1) * chunkCellsH + (relCellZ + 1)];

            for (int relX = 0; relX < cellWidth; ++relX) {
                const double percentX = relX / (double) cellWidth;
                for (int relZ = 0; relZ < cellWidth; ++relZ) {
                    const double percentZ = relZ / (double) cellWidth;
                    const int idx = (relBlockX + relX) * blockH + (relBlockZ + relZ);
                    const int y = generateColumn(blocks ? blocks2[idx] : NULL, ds00, ds01, ds10, ds11, colYMin, colYMax, cellHeight, percentX, percentZ, interpFunc, worldMinY, flag);
                    if (ys) {
                        ys[idx] = y;
                    }
                }
            }
        }
    }
    free(ds);
}

double sampleNetherFinalDensity(BlendedNoise* base3dNoise, int x, int y, int z) {
    double sample = sampleBase3dNoise(base3dNoise, x, y, z);
    sample = slideNetherLike(sample, 0, 128, y);
    sample = postProcess(sample);
    return sample; 
}

void sampleNetherNoiseColumn(BlendedNoise* base3dNoise, int cellX, int cellZ, int colYMin, int colYMax, double column[]) {
    const int minY = 0;
    const int cellWidth = 1 << 2;
    const int cellHeight = 2 << 2;
    int x = cellX * cellWidth, z = cellZ * cellWidth;

    for (int i = colYMin; i <= colYMax; ++i) {
        int idx = i - colYMin;
        int y = minY + i * cellHeight;
        double finalDensity = sampleNetherFinalDensity(base3dNoise, x, y, z);
        column[idx] = finalDensity;
    }
}


//==============================================================================
// Exact 1.18+ Overworld `WORLD_SURFACE_WG`: 3D cell interpolation, noodles and
// the NoiseBasedAquifer. Ported from the verified implementations in
// VillageGenerator (ExactHeightSampler / AquiferSampler) and LowYSwampHut.
//
// Order of operations (vanilla): the pre-squeeze, clamped main density and the
// three noodle sub-functions (thickness, ridge_a, ridge_b) are sampled at the
// 4x8x4 cell corners and trilinearly interpolated per block. Only then is the
// main density squeezed and combined with the noodle (whose toggle noise is
// sampled per block) via min().
//==============================================================================

#define EW_MIN_Y (-64)
#define EW_HEIGHT 384
#define EW_CELLS_Y 48

static inline double ewLerpFromProgress(const DoublePerlinNoise *n, double x, double y, double z, double a, double b) {
    double v = sampleDoublePerlin(n, x, y, z);
    double t = (v - (-1.0)) / (1.0 - (-1.0));
    return clampedLerp(t, a, b);
}

static inline double ewScaleTunnels(double v) {
    if (v < -0.5) return 0.75;
    if (v < 0.0) return 1.0;
    if (v < 0.5) return 1.5;
    return 2.0;
}

static inline double ewScaleCaves(double v) {
    if (v < -0.75) return 0.5;
    if (v < -0.5) return 0.75;
    if (v < 0.5) return 1.0;
    if (v < 0.75) return 2.0;
    return 3.0;
}

static inline float ewNormalizedWeirdness(double w) {
    double a = fabs(w);
    double b = -0.6666666666666666 + a;
    double c = fabs(b);
    double d = -0.3333333333333333 + c;
    return (float) (-3.0 * d);
}

static inline double ewSlideApply(double target, int size, int offset, double density, int y) {
    if (size <= 0) return density;
    double dd = (double) (y - offset) / (double) size;
    return clampedLerp(dd, target, density);
}

static inline double ewApplySlides(double density, int cellY) {
    int i = cellY - (-8);
    density = ewSlideApply(-0.078125, 2, 8, density, 48 - i);
    density = ewSlideApply(0.1171875, 3, 0, density, i);
    return density;
}

static double ewCaveEntrance(TerrainNoise *p, int x, int y, int z) {
    double s = sampleDoublePerlin(&p->noises[OTP_CAVE_ENTRANCE], (double) x * 0.75, (double) y * 0.5, (double) z * 0.75);
    return s + 0.37 + clampedLerp((10.0 + (double) y) / 40.0, 0.3, 0.0);
}

static double ewSpaghettiRoughness(TerrainNoise *p, int x, int y, int z) {
    double m = ewLerpFromProgress(&p->noises[OTP_SPAGHETTI_ROUGHNESS_MODULATOR], x, y, z, 0.0, 0.1);
    double r = fabs(sampleDoublePerlin(&p->noises[OTP_SPAGHETTI_ROUGHNESS], x, y, z));
    return (0.4 - r) * m;
}

static double ewSpaghetti3d(TerrainNoise *p, int x, int y, int z) {
    double rarity = sampleDoublePerlin(&p->noises[OTP_SPAGHETTI_3D_RARITY], (double) (x * 2), (double) y, (double) (z * 2));
    double s = ewScaleTunnels(rarity);
    double h = ewLerpFromProgress(&p->noises[OTP_SPAGHETTI_3D_THICKNESS], x, y, z, 0.065, 0.088);
    double l1 = sampleDoublePerlin(&p->noises[OTP_SPAGHETTI_3D_1], (double) x / s, (double) y / s, (double) z / s);
    double m1 = fabs(s * l1) - h;
    double l2 = sampleDoublePerlin(&p->noises[OTP_SPAGHETTI_3D_2], (double) x / s, (double) y / s, (double) z / s);
    double m2 = fabs(s * l2) - h;
    double maxM = fmax(m1, m2);
    return fmax(-1.0, fmin(1.0, maxM));
}

static double ewSpaghetti2d(TerrainNoise *p, int x, int y, int z) {
    double mod = sampleDoublePerlin(&p->noises[OTP_SPAGHETTI_2D_MODULATOR], (double) (x * 2), (double) y, (double) (z * 2));
    double s = ewScaleCaves(mod);
    double h = ewLerpFromProgress(&p->noises[OTP_SPAGHETTI_2D_THICKNESS], (double) (x * 2), (double) y, (double) (z * 2), 0.6, 1.3);
    double v = sampleDoublePerlin(&p->noises[OTP_SPAGHETTI_2D], (double) x / s, (double) y / s, (double) z / s);
    double j = fabs(s * v) - 0.083 * h;
    double l = ewLerpFromProgress(&p->noises[OTP_SPAGHETTI_2D_ELEVATION], (double) x, 0.0, (double) z, -8.0, 8.0);
    double m = fabs(l - (double) y / 8.0) - h;
    m = m * m * m;
    return fmax(-1.0, fmin(1.0, fmax(m, j)));
}

static double ewPillar(TerrainNoise *p, int x, int y, int z) {
    double rareness = ewLerpFromProgress(&p->noises[OTP_PILLAR_RARENESS], x, y, z, 0.0, 2.0);
    double thick = ewLerpFromProgress(&p->noises[OTP_PILLAR_THICKNESS], x, y, z, 0.0, 1.1);
    thick = thick * thick * thick;
    double f = sampleDoublePerlin(&p->noises[OTP_PILLAR], (double) x * 25.0, (double) y * 0.3, (double) z * 25.0);
    double g = thick * (f * 2.0 - rareness);
    return g > 0.03 ? g : -INFINITY;
}

static double ewCaveLayer(TerrainNoise *p, int x, int y, int z) {
    double v = sampleDoublePerlin(&p->noises[OTP_CAVE_LAYER], (double) x, (double) (y * 8), (double) z);
    return v * v * 4.0;
}

/**
 * Sample pre-squeeze main density at 4x8x4 cell corners (cellX, cellZ) across the vertical range.
 *
 * NOTE: In Vanilla 1.18+, main density is clamped to [-64, 64] before being trilinearly interpolated
 * across the cell corners. The non-linear postProcess (squeeze) is applied AFTER cell interpolation.
 * Squeezing at the cell corners before interpolation would distort the interpolation curve and shift
 * the surface zero-crossing, leading to 1-8 block height discrepancies.
 */
static void sampleCornerColumn18(TerrainNoise *params, int cellX, int cellZ, int colYMin, int colYMax, double out[]) {
    const int bx = cellX * 4;
    const int bz = cellZ * 4;
    const BiomeNoise *bn = &params->g.bn;

    double px = (double) bx * 0.25 + sampleDoublePerlin(&bn->climate[NP_SHIFT], (double) bx * 0.25, 0.0, (double) bz * 0.25) * 4.0;
    double pz = (double) bz * 0.25 + sampleDoublePerlin(&bn->climate[NP_SHIFT], (double) bz * 0.25, (double) bx * 0.25, 0.0) * 4.0;
    double c = sampleDoublePerlin(&bn->climate[NP_CONTINENTALNESS], px, 0.0, pz);
    double e = sampleDoublePerlin(&bn->climate[NP_EROSION], px, 0.0, pz);
    double w = sampleDoublePerlin(&bn->climate[NP_WEIRDNESS], px, 0.0, pz);
    float np[4] = {(float) c, (float) e, ewNormalizedWeirdness(w), (float) w};

    double offset = (double) (getSpline(bn->sp, np) - 0.50375f);
    double factor = (double) getSpline(params->factorSpline, np);
    double peaks = (double) getSpline(params->jaggednessSpline, np);

    double jagged = 0.0;
    if (peaks != 0.0) {
        double n = sampleDoublePerlin(&params->noises[OTP_JAGGED], (double) bx * 1500.0, 0.0, (double) bz * 1500.0);
        jagged = n > 0.0 ? peaks * n : (peaks / 2.0) * n;
    }

    for (int i = colYMin; i <= colYMax; i++) {
        int y = EW_MIN_Y + i * 8;
        double baseNoise = sampleBase3dNoise(&params->base3dNoise, bx, y, bz);

        double depth = (1.0 - (double) y / 128.0) + offset;
        double dens = (depth + jagged) * factor;
        dens = dens > 0.0 ? dens * 4.0 : dens;
        double g = dens + baseNoise;

        double cheese, spaghetti, pillarVal;
        if (g < -64.0) {
            cheese = g;
            spaghetti = 64.0;
            pillarVal = -64.0;
        } else {
            double diff = g - 1.5625;
            double caveEnt = ewCaveEntrance(params, bx, y, bz);
            double rough = ewSpaghettiRoughness(params, bx, y, bz);
            double sp3d = ewSpaghetti3d(params, bx, y, bz);
            double m = fmin(caveEnt, sp3d + rough);

            if (diff < 0.0) {
                cheese = g;
                spaghetti = m * 5.0;
                pillarVal = -64.0;
            } else {
                double layer = ewCaveLayer(params, bx, y, bz);
                if (layer > 64.0) {
                    cheese = 64.0;
                } else {
                    double raw = sampleDoublePerlin(&params->noises[OTP_CAVE_CHEESE], (double) bx, (double) y / 1.5, (double) bz);
                    double p = fmax(-1.0, fmin(1.0, raw + 0.27));
                    double q = diff * 1.28;
                    cheese = p + clampedLerp(q, 0.5, 0.0) + layer;
                }
                double sp2dVal = ewSpaghetti2d(params, bx, y, bz);
                spaghetti = fmin(m, sp2dVal + rough);
                pillarVal = ewPillar(params, bx, y, bz);
            }
        }

        double v = fmax(fmin(cheese, spaghetti), pillarVal);
        v = ewApplySlides(v, y / 8);
        if (v < -64.0) v = -64.0;
        else if (v > 64.0) v = 64.0;
        out[i - colYMin] = v;
    }
}

#define EW_NOODLE_THICK_BIAS (-0.07500000000000001)
#define EW_NOODLE_THICK_MUL (-0.025)
#define EW_NOODLE_RIDGE_SCALE 2.6666666666666665

static inline int ewNoodleYInRange(int y) {
    return y >= -60 && y <= 320;
}

/**
 * Sample noodle cave sub-functions at cell corners across the vertical range.
 *
 * In Vanilla `overworld/caves/noodle.json`:
 *   - `noodle_thickness` (bias -0.075, factor -0.025)
 *   - `noodle_ridge_a`   (scale 2.6666666666666665)
 *   - `noodle_ridge_b`   (scale 2.6666666666666665)
 * are all wrapped in `interpolated`, meaning they are sampled at cell corners and trilinearly
 * interpolated. The `noodle` toggle noise, in contrast, is sampled per block.
 */
static void sampleNoodleCornerColumn18(TerrainNoise *params, int cellX, int cellZ, double thick[], double ridgeA[], double ridgeB[]) {
    const int bx = cellX * 4;
    const int bz = cellZ * 4;
    for (int i = 0; i <= EW_CELLS_Y; i++) {
        int y = EW_MIN_Y + i * 8;
        if (!ewNoodleYInRange(y)) {
            thick[i] = 0.0;
            ridgeA[i] = 0.0;
            ridgeB[i] = 0.0;
        } else {
            thick[i] = EW_NOODLE_THICK_BIAS + EW_NOODLE_THICK_MUL * sampleDoublePerlin(&params->noises[OTP_NOODLE_THICKNESS], (double) bx, (double) y, (double) bz);
            ridgeA[i] = sampleDoublePerlin(&params->noises[OTP_NOODLE_RIDGE_A], (double) bx * EW_NOODLE_RIDGE_SCALE, (double) y * EW_NOODLE_RIDGE_SCALE, (double) bz * EW_NOODLE_RIDGE_SCALE);
            ridgeB[i] = sampleDoublePerlin(&params->noises[OTP_NOODLE_RIDGE_B], (double) bx * EW_NOODLE_RIDGE_SCALE, (double) y * EW_NOODLE_RIDGE_SCALE, (double) bz * EW_NOODLE_RIDGE_SCALE);
        }
    }
}

static inline int64_t ewChunkKey(int chunkX, int chunkZ) {
    return (int64_t) (((uint64_t) (int64_t) chunkX << 32) | (uint64_t) (uint32_t) chunkZ);
}

static OverworldChunkCache *ewGetChunkCache(TerrainNoise *params, int chunkX, int chunkZ) {
    OverworldChunkCache *cache = &params->owCache;
    int64_t key = ewChunkKey(chunkX, chunkZ);
    if (cache->chunkKey == key) {
        return cache;
    }
    cache->chunkKey = key;
    const int baseCellX = chunkX * 4;
    const int baseCellZ = chunkZ * 4;
    for (int gx = 0; gx < 5; gx++) {
        for (int gz = 0; gz < 5; gz++) {
            int idx = gx * 5 + gz;
            sampleCornerColumn18(params, baseCellX + gx, baseCellZ + gz, 0, EW_CELLS_Y, cache->corners[idx]);
            sampleNoodleCornerColumn18(params, baseCellX + gx, baseCellZ + gz,
                    cache->noodleCorners[0][idx], cache->noodleCorners[1][idx], cache->noodleCorners[2][idx]);
        }
    }
    return cache;
}

// Trilinear interpolation matching Vanilla `NoiseInterpolator` / `Mth.lerp3`:
// interpolates Y first, then X, then Z.
static inline double ewLerp3(double dx, double dy, double dz,
        double n000, double n100, double n010, double n110,
        double n001, double n101, double n011, double n111) {
    double xz00 = lerp(dy, n000, n010);
    double xz10 = lerp(dy, n100, n110);
    double xz01 = lerp(dy, n001, n011);
    double xz11 = lerp(dy, n101, n111);
    double z0 = lerp(dx, xz00, xz10);
    double z1 = lerp(dx, xz01, xz11);
    return lerp(dz, z0, z1);
}

static double ewInterpCorners(const double grid[25][49], int gx, int gz, double dx, double dy, double dz, int cellY) {
    const double *c00 = grid[gx * 5 + gz];
    const double *c10 = grid[(gx + 1) * 5 + gz];
    const double *c01 = grid[gx * 5 + gz + 1];
    const double *c11 = grid[(gx + 1) * 5 + gz + 1];
    return ewLerp3(dx, dy, dz,
            c00[cellY], c10[cellY],
            c00[cellY + 1], c10[cellY + 1],
            c01[cellY], c11[cellY],
            c01[cellY + 1], c11[cellY + 1]);
}

double sampleInterpolatedDensity18(TerrainNoise *params, int x, int y, int z) {
    int yOff = y - EW_MIN_Y;
    if (yOff < 0 || yOff >= EW_HEIGHT) {
        return -1.0;
    }

    // 1. Get chunk cache of 4x8x4 cell corners
    OverworldChunkCache *entry = ewGetChunkCache(params, x >> 4, z >> 4);
    int gx = (x >> 2) & 3;
    int gz = (z >> 2) & 3;
    int cellY = yOff / 8;
    double dx = (double) (x & 3) / 4.0;
    double dy = (double) (yOff - cellY * 8) / 8.0;
    double dz = (double) (z & 3) / 4.0;

    // 2. Trilinearly interpolate pre-squeeze main density across cell corners, then squeeze
    double v = ewInterpCorners(entry->corners, gx, gz, dx, dy, dz, cellY);
    double squeezy = postProcess(v);

    // 3. Evaluate noodle caves: toggle noise per-block, thickness & ridges interpolated from corners
    double noodleN = ewNoodleYInRange(y) ? sampleDoublePerlin(&params->noises[OTP_NOODLE], (double) x, (double) y, (double) z) : -1.0;
    double noodleVal;
    if (noodleN >= -1000000.0 && noodleN < 0.0) {
        noodleVal = 64.0;
    } else {
        double thick = ewInterpCorners(entry->noodleCorners[0], gx, gz, dx, dy, dz, cellY);
        double ridgeA = ewInterpCorners(entry->noodleCorners[1], gx, gz, dx, dy, dz, cellY);
        double ridgeB = ewInterpCorners(entry->noodleCorners[2], gx, gz, dx, dy, dz, cellY);
        noodleVal = thick + 1.5 * fmax(fabs(ridgeA), fabs(ridgeB));
    }
    return fmin(squeezy, noodleVal);
}

//------------------------------------------------------------------------------
// preliminary surface level & aquifer
//------------------------------------------------------------------------------

#define AQ_NO_FLUID_Y (-32512)
#define AQ_LAVA_Y (-54)
#define AQ_WATER_Y 63
#define AQ_LAVA_CUT (-54)
#define AQ_FLOWING_UPDATE_SIMULARITY (-0.76)
#define AQ_DEEP_DARK_EROSION (-0.22499999403953552)
#define AQ_DEEP_DARK_DEPTH (0.8999999761581421)

static inline double ewClampedGradient(double y, double fromY, double toY, double fromValue, double toValue) {
    double t = clamp((y - fromY) / (toY - fromY), 0.0, 1.0);
    return lerp(t, fromValue, toValue);
}

static inline double pslDensity(double y, double offset, double factor) {
    double depth = ewClampedGradient(y, -64.0, 320.0, 1.5, -1.5) + offset;
    double f = depth * factor;
    double cheese = 4.0 * (f > 0.0 ? f : f * 0.25);
    double c = clamp(-0.703125 + cheese, -64.0, 64.0);
    double a1 = ewClampedGradient(y, 240.0, 256.0, 1.0, 0.0);
    double a2 = ewClampedGradient(y, -64.0, -40.0, 0.0, 1.0);
    return -0.390625 + (0.1171875 + a2 * (-0.1171875 + (-0.078125 + a1 * (0.078125 + c))));
}

/// climate noise at the quarter-resolution (flat cache) coordinates of the block
static void pslClimate(TerrainNoise *p, int biomeX, int biomeZ, double *c, double *e, double *w) {
    const BiomeNoise *bn = &p->g.bn;
    double sx = (double) biomeX + sampleDoublePerlin(&bn->climate[NP_SHIFT], (double) biomeX, 0.0, (double) biomeZ) * 4.0;
    double sz = (double) biomeZ + sampleDoublePerlin(&bn->climate[NP_SHIFT], (double) biomeZ, (double) biomeX, 0.0) * 4.0;
    *c = sampleDoublePerlin(&bn->climate[NP_CONTINENTALNESS], sx, 0.0, sz);
    *e = sampleDoublePerlin(&bn->climate[NP_EROSION], sx, 0.0, sz);
    *w = sampleDoublePerlin(&bn->climate[NP_WEIRDNESS], sx, 0.0, sz);
}

/**
 * Sample 1.18+ preliminary surface level (Vanilla `preliminary_surface_level`).
 *
 * Evaluated at quarter-resolution coordinates (x >> 2, z >> 2) matching Vanilla's 2D flat cache.
 * Uses exact double constants (-0.5037500262260437 and 10.0 + (-10.0 + factor)) matching Vanilla's
 * OffsetDoubleFunction and FactorDoubleFunction, and scans downward in steps of 8 blocks to find
 * the highest Y where density is positive.
 */
int samplePreliminarySurfaceLevel(TerrainNoise *params, int blockX, int blockZ) {
    const int biomeX = blockX >> 2;
    const int biomeZ = blockZ >> 2;
    const int64_t key = ewChunkKey(biomeX, biomeZ);
    const uint64_t ukey = (uint64_t) key;
    const int slot = (int) ((ukey ^ (ukey >> 32)) & (AQUIFER_PSL_CACHE_SIZE - 1));
    if (params->pslKeys[slot] == key) {
        return params->pslValues[slot];
    }

    double cd, ed, wd;
    pslClimate(params, biomeX, biomeZ, &cd, &ed, &wd);
    float cf = (float) cd, ef = (float) ed, wf = (float) wd;
    float np[4] = {cf, ef, peaksAndValleys(wf), wf};

    double rawOffset = (double) getSpline(params->g.bn.sp, np);
    double rawFactor = (double) getSpline(params->factorSpline, np);
    double offset = -0.5037500262260437 + rawOffset;
    double factor = 10.0 + (-10.0 + rawFactor);

    double inv = 1.0 / factor;
    double inner = inv * 0.2734375 + offset * -1.0;
    double upper = clamp(128.0 + -128.0 * inner, -40.0, 320.0);

    int res = -64;
    int y = (int) (upper / 8.0) * 8;
    if (y > -64) {
        for (int i = y; i >= -64; i -= 8) {
            if (pslDensity((double) i, offset, factor) > 0.0) {
                res = i;
                break;
            }
        }
    }
    params->pslKeys[slot] = key;
    params->pslValues[slot] = res;
    return res;
}

static int aqIsDeepDarkRegion(TerrainNoise *p, int x, int y, int z) {
    (void) z;
    double cd, ed, wd;
    pslClimate(p, x >> 2, z >> 2, &cd, &ed, &wd);
    if (!(ed < AQ_DEEP_DARK_EROSION)) {
        return 0;
    }
    float cf = (float) cd, ef = (float) ed, wf = (float) wd;
    float np[4] = {cf, ef, peaksAndValleys(wf), wf};
    double rawOffset = (double) getSpline(p->g.bn.sp, np);
    double offset = -0.5037500262260437 + rawOffset;
    double depth = ewClampedGradient((double) y, -64.0, 320.0, 1.5, -1.5) + offset;
    return depth > AQ_DEEP_DARK_DEPTH;
}

static inline int64_t aqPackPos(int x, int y, int z) {
    return (int64_t) ((((uint64_t) (int64_t) x & 0x3FFFFFFULL) << 38)
            | (((uint64_t) (int64_t) z & 0x3FFFFFFULL) << 12)
            | ((uint64_t) (int64_t) y & 0xFFFULL));
}
static inline int aqPosX(int64_t v) { return (int) (v >> 38); }
static inline int aqPosY(int64_t v) { return (int) (((int64_t) ((uint64_t) v << 52)) >> 52); }
static inline int aqPosZ(int64_t v) { return (int) (((int64_t) ((uint64_t) v << 26)) >> 38); }

static inline int64_t aqHashCode(int x, int y, int z) {
    int32_t xm = (int32_t) ((uint32_t) x * 3129871u);
    uint64_t i = (uint64_t) (int64_t) xm ^ ((uint64_t) (int64_t) z * 116129781ULL) ^ (uint64_t) (int64_t) y;
    i = i * i * 42317861ULL + i * 11ULL;
    return ((int64_t) i) >> 16;
}

static inline int floorDivInt(int a, int b) {
    int q = a / b;
    if ((a % b != 0) && ((a < 0) != (b < 0))) q--;
    return q;
}

static inline uint8_t aqLevelBlockAt(int level, uint8_t type, int atY) {
    return atY < level ? type : AQUIFER_BLOCK_AIR;
}

static inline double aqSimilarity(int a, int b) {
    return 1.0 - (double) (b - a) / 25.0;
}

static inline double aqClampedLerp(double start, double end, double delta) {
    if (delta < 0.0) return start;
    if (delta > 1.0) return end;
    return lerp(delta, start, end);
}

static int aqIndex(const AquiferChunkCache *st, int gx, int gy, int gz) {
    int dx = gx - st->minGridX;
    int dy = gy - st->minGridY;
    int dz = gz - st->minGridZ;
    if (dx < 0 || dx >= st->gridSizeX) return -1;
    if (dy < 0 || dy >= st->gridSizeY) return -1;
    if (dz < 0 || dz >= st->gridSizeZ) return -1;
    return (dy * st->gridSizeZ + dz) * st->gridSizeX + dx;
}

static AquiferChunkCache *aqGetChunkState(TerrainNoise *p, int chunkX, int chunkZ) {
    AquiferChunkCache *st = &p->aqCache;
    int64_t key = ewChunkKey(chunkX, chunkZ);
    if (st->chunkKey == key) {
        return st;
    }
    st->chunkKey = key;

    const int minBlockX = chunkX * 16, maxBlockX = minBlockX + 15;
    const int minBlockZ = chunkZ * 16, maxBlockZ = minBlockZ + 15;
    const int minGx = (minBlockX - 5) >> 4;
    const int maxGx = ((maxBlockX - 5) >> 4) + 1;
    const int minGz = (minBlockZ - 5) >> 4;
    const int maxGz = ((maxBlockZ - 5) >> 4) + 1;
    st->minGridX = minGx;
    st->gridSizeX = maxGx - minGx + 1;
    st->minGridZ = minGz;
    st->gridSizeZ = maxGz - minGz + 1;
    const int minGy = floorDivInt(EW_MIN_Y + 1, 12) - 1;
    const int maxGy = floorDivInt(EW_MIN_Y + EW_HEIGHT + 1, 12) + 1;
    st->minGridY = minGy;
    st->gridSizeY = maxGy - minGy + 1;

    const int n = st->gridSizeX * st->gridSizeY * st->gridSizeZ;
    if (n > AQUIFER_MAX_CELLS) {
        fprintf(stderr, "aqGetChunkState(): aquifer grid too large\n");
        exit(1);
    }
    for (int i = 0; i < n; i++) {
        st->cacheValid[i] = 0;
        st->location[i] = INT64_MAX;
    }

    const int x1 = minGx * 16, z1 = minGz * 16;
    const int x2 = maxGx * 16 + 9, z2 = maxGz * 16 + 9;
    int maxPsl = INT_MIN;
    for (int z = z1; z <= z2; z += 4) {
        for (int x = x1; x <= x2; x += 4) {
            int psl = samplePreliminarySurfaceLevel(p, x, z);
            if (psl > maxPsl) maxPsl = psl;
        }
    }
    const int g = floorDivInt((maxPsl + 8) + 12, 12) + 1;
    st->skipSamplingAboveY = g * 12 + 11 - 1;
    return st;
}

static int aqRandomizedFluidSurfaceLevel(TerrainNoise *p, int x, int y, int z, int minCellTopY) {
    int fx = floorDivInt(x, 16);
    int fy = floorDivInt(y, 40);
    int fz = floorDivInt(z, 16);
    int base = fy * 40 + 20;
    double n = sampleDoublePerlin(&p->noises[OTP_AQUIFER_FLUID_LEVEL_SPREAD], (double) fx, (double) fy * 0.7142857142857143, (double) fz) * 10.0;
    int q = (int) floor(n / 3.0) * 3;
    int level = base + q;
    return level < minCellTopY ? level : minCellTopY;
}

static int aqComputeSurfaceLevel(TerrainNoise *p, int x, int y, int z, int globalLevel, int minCellTopY, int hasAbove) {
    if (aqIsDeepDarkRegion(p, x, y, z)) {
        return AQ_NO_FLUID_Y;
    }
    int d = (minCellTopY + 8) - y;
    double f = hasAbove ? aqClampedLerp(1.0, 0.0, (double) d / 64.0) : 0.0;
    double high = lerp(inverseLerp(f, 1.0, 0.0), -0.3, 0.8);
    double low = lerp(inverseLerp(f, 1.0, 0.0), -0.8, 0.4);

    double floodedness = clamp(sampleDoublePerlin(&p->noises[OTP_AQUIFER_FLUID_LEVEL_FLOODEDNESS], (double) x, (double) y * 0.67, (double) z), -1.0, 1.0);
    if (floodedness - high > 0.0) return globalLevel;
    if (floodedness - low > 0.0) return aqRandomizedFluidSurfaceLevel(p, x, y, z, minCellTopY);
    return AQ_NO_FLUID_Y;
}

static uint8_t aqComputeFluidType(TerrainNoise *p, int x, int y, int z, uint8_t globalType, int level) {
    uint8_t t = globalType;
    if (level <= -10 && level != AQ_NO_FLUID_Y && globalType != AQUIFER_BLOCK_LAVA) {
        int bx = floorDivInt(x, 64);
        int by = floorDivInt(y, 40);
        int bz = floorDivInt(z, 64);
        double n = sampleDoublePerlin(&p->noises[OTP_AQUIFER_LAVA], (double) bx, (double) by, (double) bz);
        if (fabs(n) > 0.3) t = AQUIFER_BLOCK_LAVA;
    }
    return t;
}

static void aqComputeFluid(TerrainNoise *p, AquiferChunkCache *st, int idx, int x, int y, int z) {
    static const int offsets[13][2] = {
        {0, 0},
        {-2, -1}, {-1, -1}, {0, -1}, {1, -1},
        {-3, 0}, {-2, 0}, {-1, 0}, {1, 0},
        {-2, 1}, {-1, 1}, {0, 1}, {1, 1},
    };
    const int globalLevel = y < AQ_LAVA_CUT ? AQ_LAVA_Y : AQ_WATER_Y;
    const uint8_t globalType = y < AQ_LAVA_CUT ? AQUIFER_BLOCK_LAVA : AQUIFER_BLOCK_WATER;

    int minCellTopY = INT_MAX;
    const int yPlus12 = y + 12;
    const int yMinus12 = y - 12;
    int hasAbove = 0;

    for (int i = 0; i < 13; i++) {
        int bx = x + offsets[i][0] * 16;
        int bz = z + offsets[i][1] * 16;
        int top = samplePreliminarySurfaceLevel(p, bx, bz);
        int topAdj = top + 8;
        int isCenter = i == 0;

        if (isCenter && yMinus12 > topAdj) {
            st->fluidLevel[idx] = globalLevel;
            st->fluidType[idx] = globalType;
            st->cacheValid[idx] = 1;
            return;
        }
        int above = yPlus12 > topAdj;
        if (above || isCenter) {
            int lvl = topAdj < AQ_LAVA_CUT ? AQ_LAVA_Y : AQ_WATER_Y;
            uint8_t typ = topAdj < AQ_LAVA_CUT ? AQUIFER_BLOCK_LAVA : AQUIFER_BLOCK_WATER;
            if (aqLevelBlockAt(lvl, typ, topAdj) != AQUIFER_BLOCK_AIR) {
                if (isCenter) hasAbove = 1;
                if (above) {
                    st->fluidLevel[idx] = lvl;
                    st->fluidType[idx] = typ;
                    st->cacheValid[idx] = 1;
                    return;
                }
            }
        }
        if (top < minCellTopY) minCellTopY = top;
    }

    int level = aqComputeSurfaceLevel(p, x, y, z, globalLevel, minCellTopY, hasAbove);
    uint8_t type = aqComputeFluidType(p, x, y, z, globalType, level);
    st->fluidLevel[idx] = level;
    st->fluidType[idx] = type;
    st->cacheValid[idx] = 1;
}

static inline void aqEnsureStatus(TerrainNoise *p, AquiferChunkCache *st, int idx) {
    if (st->cacheValid[idx]) return;
    int64_t pos = st->location[idx];
    aqComputeFluid(p, st, idx, aqPosX(pos), aqPosY(pos), aqPosZ(pos));
}

static double aqCalculatePressure(TerrainNoise *p, int x, int y, int z, double *barrierAcc,
        int aLevel, uint8_t aType, int bLevel, uint8_t bType) {
    uint8_t sa = aqLevelBlockAt(aLevel, aType, y);
    uint8_t sb = aqLevelBlockAt(bLevel, bType, y);

    if ((sa == AQUIFER_BLOCK_LAVA && sb == AQUIFER_BLOCK_WATER) || (sa == AQUIFER_BLOCK_WATER && sb == AQUIFER_BLOCK_LAVA)) {
        return 2.0;
    }

    int dy = abs(aLevel - bLevel);
    if (dy == 0) return 0.0;

    double mid = 0.5 * ((double) aLevel + (double) bLevel);
    double f = ((double) y + 0.5) - mid;
    double half = (double) dy / 2.0;
    double n = half - fabs(f);

    double o;
    if (f > 0.0) {
        double q = 0.0 + n;
        o = q > 0.0 ? q / 1.5 : q / 2.5;
    } else {
        double q = 3.0 + n;
        o = q > 0.0 ? q / 3.0 : q / 10.0;
    }

    double inner;
    if (o < -2.0 || o > 2.0) {
        inner = 0.0;
    } else if (isnan(*barrierAcc)) {
        double q = sampleDoublePerlin(&p->noises[OTP_AQUIFER_BARRIER], (double) x, (double) y * 0.5, (double) z);
        *barrierAcc = q;
        inner = q;
    } else {
        inner = *barrierAcc;
    }
    return 2.0 * (inner + o);
}

/**
 * 1.18+ NoiseBasedAquifer substance calculation (Vanilla `NoiseBasedAquifer.computeSubstance`).
 *
 * Determines whether carved space (density <= 0) is air, water, or lava:
 * 1. Fast checks: positive density is solid stone; Y above maximum preliminary surface level or below
 *    deep lava cutoff defaults to global fluid levels.
 * 2. Samples the nearest jittered 3D grid points (spacing 16x12x16) using aquiferPositional PRNG.
 * 3. Evaluates barrier noise pressure between neighboring fluid levels. If barrier pressure
 *    pushes (density + pressure) > 0, carving is blocked (returns AQUIFER_BLOCK_SOLID) to
 *    form a watertight/lavatight barrier preventing fluid leaks into caves.
 */
uint8_t computeAquiferSubstance(TerrainNoise *params, int x, int y, int z, double density) {
    if (density > 0.0) {
        return AQUIFER_BLOCK_SOLID;
    }

    AquiferChunkCache *st = aqGetChunkState(params, x >> 4, z >> 4);

    const int globalLevel = y < AQ_LAVA_CUT ? AQ_LAVA_Y : AQ_WATER_Y;
    const uint8_t globalType = y < AQ_LAVA_CUT ? AQUIFER_BLOCK_LAVA : AQUIFER_BLOCK_WATER;

    if (y > st->skipSamplingAboveY) {
        return aqLevelBlockAt(globalLevel, globalType, y);
    }
    if (aqLevelBlockAt(globalLevel, globalType, y) == AQUIFER_BLOCK_LAVA) {
        return AQUIFER_BLOCK_LAVA;
    }

    const int gx = floorDivInt(x - 5, 16);
    const int gy = floorDivInt(y + 1, 12);
    const int gz = floorDivInt(z - 5, 16);

    int d0 = INT_MAX, d1 = INT_MAX, d2 = INT_MAX, d3 = INT_MAX;
    int s0 = 0, s1 = 0, s2 = 0, s3 = 0;

    for (int i = 0; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            for (int k = 0; k <= 1; k++) {
                int cx = gx + i, cy = gy + j, cz = gz + k;
                int idx = aqIndex(st, cx, cy, cz);
                if (idx < 0) continue;

                int64_t packed = st->location[idx];
                if (packed == INT64_MAX) {
                    Xoroshiro rnd = {(uint64_t) aqHashCode(cx, cy, cz) ^ params->aquiferPositional.lo, params->aquiferPositional.hi};
                    int px = cx * 16 + xNextInt(&rnd, 10);
                    int py = cy * 12 + xNextInt(&rnd, 9);
                    int pz = cz * 16 + xNextInt(&rnd, 10);
                    packed = aqPackPos(px, py, pz);
                    st->location[idx] = packed;
                }

                int dx = aqPosX(packed) - x;
                int dy = aqPosY(packed) - y;
                int dz = aqPosZ(packed) - z;
                int dist = dx * dx + dy * dy + dz * dz;

                if (dist <= d0) {
                    d3 = d2; d2 = d1; d1 = d0; d0 = dist;
                    s3 = s2; s2 = s1; s1 = s0; s0 = idx;
                } else if (dist <= d1) {
                    d3 = d2; d2 = d1; d1 = dist;
                    s3 = s2; s2 = s1; s1 = idx;
                } else if (dist <= d2) {
                    d3 = d2; d2 = dist;
                    s3 = s2; s2 = idx;
                } else if (dist <= d3) {
                    d3 = dist; s3 = idx;
                }
            }
        }
    }

    aqEnsureStatus(params, st, s0);
    const int l0 = st->fluidLevel[s0];
    const uint8_t t0 = st->fluidType[s0];
    const double s01 = aqSimilarity(d0, d1);
    const uint8_t result = aqLevelBlockAt(l0, t0, y);

    if (!(s01 > 0.0)) {
        if (s01 >= AQ_FLOWING_UPDATE_SIMULARITY) {
            aqEnsureStatus(params, st, s1);
        }
        return result;
    }

    const int belowLevel = (y - 1) < AQ_LAVA_CUT ? AQ_LAVA_Y : AQ_WATER_Y;
    const uint8_t belowType = (y - 1) < AQ_LAVA_CUT ? AQUIFER_BLOCK_LAVA : AQUIFER_BLOCK_WATER;
    if (result == AQUIFER_BLOCK_WATER && aqLevelBlockAt(belowLevel, belowType, y - 1) == AQUIFER_BLOCK_LAVA) {
        return result;
    }

    double barrierAcc = NAN;

    aqEnsureStatus(params, st, s1);
    const int l1 = st->fluidLevel[s1];
    const uint8_t t1 = st->fluidType[s1];
    double p1 = s01 * aqCalculatePressure(params, x, y, z, &barrierAcc, l0, t0, l1, t1);
    if (density + p1 > 0.0) {
        return AQUIFER_BLOCK_SOLID;
    }

    aqEnsureStatus(params, st, s2);
    const int l2 = st->fluidLevel[s2];
    const uint8_t t2 = st->fluidType[s2];
    const double s02 = aqSimilarity(d0, d2);
    if (s02 > 0.0) {
        double p2 = s01 * s02 * aqCalculatePressure(params, x, y, z, &barrierAcc, l0, t0, l2, t2);
        if (density + p2 > 0.0) {
            return AQUIFER_BLOCK_SOLID;
        }
    }

    const double s12 = aqSimilarity(d1, d2);
    if (s12 > 0.0) {
        double p3 = s01 * s12 * aqCalculatePressure(params, x, y, z, &barrierAcc, l1, t1, l2, t2);
        if (density + p3 > 0.0) {
            return AQUIFER_BLOCK_SOLID;
        }
    }

    int b0 = !(l0 == l1 && t0 == t1);
    int b1 = (s12 >= AQ_FLOWING_UPDATE_SIMULARITY) && !(l1 == l2 && t1 == t2);
    int b2 = (s02 >= AQ_FLOWING_UPDATE_SIMULARITY) && !(l0 == l2 && t0 == t2);
    if (!(b0 || b1 || b2)) {
        aqEnsureStatus(params, st, s3);
    }
    return result;
}

/**
 * Exact WORLD_SURFACE_WG surface height.
 * For >=1.18, scans downward from y = 319 to bedrock at exact block coordinates (x, z),
 * evaluating 3D cell interpolation, noodle caves, and NoiseBasedAquifer fluids.
 * For <1.18, samples the 4 neighboring noise columns and performs pre-1.18 cell interpolation.
 * Returns y + 1 of the highest non-air block (or worldMinY if none).
 */
int getExactSurfaceHeight(TerrainNoise *params, int x, int z) {
    if (params->g.mc < MC_1_18) {
        if (params->g.dim != DIM_OVERWORLD) return 0;
        int cellX = x >> 2;
        int cellZ = z >> 2;
        double ds00[33], ds01[33], ds10[33], ds11[33];
        sampleOWNoiseColumnOld(params, cellX, cellZ, 0, 32, ds00);
        sampleOWNoiseColumnOld(params, cellX, cellZ + 1, 0, 32, ds01);
        sampleOWNoiseColumnOld(params, cellX + 1, cellZ, 0, 32, ds10);
        sampleOWNoiseColumnOld(params, cellX + 1, cellZ + 1, 0, 32, ds11);
        double px = (x & 3) / 4.0;
        double pz = (z & 3) / 4.0;
        return generateColumn(NULL, ds00, ds01, ds10, ds11, 0, 32, 8, px, pz, INTERP_PRE_1_18, 0, 1);
    }

    for (int i = EW_CELLS_Y - 1; i >= 0; i--) {
        for (int j = 7; j >= 0; j--) {
            int y = (-8 + i) * 8 + j;
            double density = sampleInterpolatedDensity18(params, x, y, z);
            if (computeAquiferSubstance(params, x, y, z, density) != AQUIFER_BLOCK_AIR) {
                return y + 1;
            }
        }
    }
    return EW_MIN_Y;
}

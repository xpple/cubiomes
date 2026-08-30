#include "terrainnoise.h"

#include <stdio.h>

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

    if (dim == DIM_OVERWORLD) {
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

int samplePreliminarySurfaceLevel(TerrainNoise *params, int x, int z) {
    double px = x * 0.25 + sampleDoublePerlin(&params->g.bn.climate[NP_SHIFT], x * 0.25, 0, z * 0.25) * 4.0;
    double pz = z * 0.25 + sampleDoublePerlin(&params->g.bn.climate[NP_SHIFT], z * 0.25, x * 0.25, 0) * 4.0;

    float c = sampleDoublePerlin(&params->g.bn.climate[NP_CONTINENTALNESS], px, 0, pz);
    float e = sampleDoublePerlin(&params->g.bn.climate[NP_EROSION], px, 0, pz);
    float w = sampleDoublePerlin(&params->g.bn.climate[NP_WEIRDNESS], px, 0, pz);

    float np_param[] = {
            c, e, peaksAndValleys(w), w,
    };

    double offset = getSpline(params->g.bn.sp, np_param) - 0.50375F;
    double factor = getSpline(params->factorSpline, np_param);
    double upperBound = clamp(remap(0.2734375 / factor - offset, 1.5, -1.5, -64.0, 320.0), -40.0, 320.0);

    const int lowerBound = -64;
    const int cellHeight = 2 << 2;
    const int upperCell = floor(upperBound / cellHeight) * cellHeight;
    if (upperCell <= lowerBound) {
        return lowerBound;
    }
    for (int y = upperCell; y >= lowerBound; y -= cellHeight) {
        double depth = offset + yClampedGradient(y, -64, 320, 1.5, -1.5);
        double density = slideOverworld(clamp(noiseGradientDensity(factor, depth) - 0.703125, -64.0, 64.0), y) - 0.390625;
        if (density > 0.0) {
            return y;
        }
    }
    return lowerBound;
}

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

    const int minY = -64;
    const int cellWidth = 1 << 2;
    const int cellHeight = 2 << 2;
    int x = cellX * cellWidth, z = cellZ * cellWidth;

    float np_param[4];
    sampleNoiseParameters(&params->g.bn, cellX, cellZ, np_param);

    double depth = getSpline(params->g.bn.sp, np_param) - 0.50375f;
    double factor = getSpline(params->factorSpline, np_param);
    double jagged = sampleDoublePerlin(&params->noises[OTP_JAGGED], x * 1500.0, 0, z * 1500.0);
    jagged = jagged >= 0.0 ? jagged : jagged / 2.0;
    jagged *= getSpline(params->jaggednessSpline, np_param);

    for (int i = colYMin; i <= colYMax; ++i) {
        int idx = i - colYMin;
        int y = minY + i * cellHeight;
        double spaghettiRoughness = sampleSpaghettiRoughness(params, x, y, z);
        double entrances = sampleEntrances(params, x, y, z, spaghettiRoughness);
        double slopedCheese = sampleSlopedCheese(params, x, y, z, depth, factor, jagged);
        double finalDensity = sampleFinalDensity(params, x, y, z, spaghettiRoughness, entrances, slopedCheese);
        column[idx] = finalDensity;
    }
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

#include "ore.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

// https://c-faq.com/misc/bitsets.html
#define BITMASK(b) (1 << ((b) % CHAR_BIT))
#define BITSLOT(b) ((b) / CHAR_BIT)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)

#define PI 3.14159265358979323846

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int getOreConfig(int oreType, int mc, int biomeID, OreConfig *oconf)
{
    static const uint32_t BASE_STONE_OVERWORLD_REPLACEABLES[] = {STONE, GRANITE, DIORITE, ANDESITE, DEEPSLATE, TUFF};
    static const uint32_t BASE_STONE_NETHER_REPLACEABLES[] = {NETHERRACK, BASALT, BLACKSTONE};
    static const uint32_t STONE_REPLACEABLES[] = {STONE};
    static const uint32_t NETHERRACK_REPLACEABLES[] = {NETHERRACK};

    // check the step in BiomeDefaultFeatures.java
    // check the size and discardChanceOnAirExposure in OreFeatures.java
    // check the repeatCount in OrePlacements.java

    static const OreConfig
    // overworld
    o_andesite_113 =  {4, 4, 33, 10, providerRange,        0,  0, 80, AndesiteOre, ANDESITE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_andesite_1161 = {4, 6, 33, 10, providerRange,        0,  0, 80, AndesiteOre, ANDESITE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_andesite_117 =  {4, 6, 33, 10, providerUniformRange, 0, 79, -1, AndesiteOre, ANDESITE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_buried_diamond_118 = {20, 6, 8, 4, providerTriangleRange, -64 + -80, -64 + 80, -1, BuriedDiamondOre, DIAMOND_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 1.0F},
    o_buried_diamond_120 = {21, 6, 8, 4, providerTriangleRange, -64 + -80, -64 + 80, -1, BuriedDiamondOre, DIAMOND_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 1.0F},

    o_buried_lapis_118 = {22, 6, 7, 4, providerUniformRange, -64, 64, -1, BuriedLapisOre, LAPIS_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 1.0F},
    o_buried_lapis_120 = {23, 6, 7, 4, providerUniformRange, -64, 64, -1, BuriedLapisOre, LAPIS_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 1.0F},

    o_clay_118 = {26, 6, 33, 46, providerUniformRange, 0, 256, -1, ClayOre, CLAY, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_clay_120 = {27, 6, 33, 46, providerUniformRange, 0, 256, -1, ClayOre, CLAY, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_coal_113 =  {5, 4, 17, 20, providerRange,        0,   0, 128, CoalOre, COAL_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_coal_1161 = {5, 6, 17, 20, providerRange,        0,   0, 128, CoalOre, COAL_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_coal_117 =  {7, 6, 17, 20, providerUniformRange, 0, 127,  -1, CoalOre, COAL_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_copper_117 = {13, 6, 10,  6, providerTriangleRange,   0,  96, -1, CopperOre, COPPER_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_copper_118 = {24, 6, 10, 16, providerTriangleRange, -16, 112, -1, CopperOre, COPPER_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_copper_120 = {25, 6, 10, 16, providerTriangleRange, -16, 112, -1, CopperOre, COPPER_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_deepslate_117 = {6, 6, 64, 2, providerUniformRange, 0, 16, -1, DeepslateOre, DEEPSLATE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_diamond_113 =  { 9, 4, 8, 1, providerRange, 0, 0, 16, DiamondOre, DIAMOND_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_diamond_1161 = { 9, 6, 8, 1, providerRange, 0, 0, 16, DiamondOre, DIAMOND_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    // maxOffset was 16 in 1.17, changed to 15 in 1.17.1
    o_diamond_117 =  {11, 6, 8, 1, providerUniformRange,          0,       15, -1, DiamondOre, DIAMOND_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_diamond_118 =  {18, 6, 4, 7, providerTriangleRange, -64 + -80, -64 + 80, -1, DiamondOre, DIAMOND_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.5F},

    o_diorite_113 =  {3, 4, 33, 10, providerRange,        0, 0,  80, DioriteOre, DIORITE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_diorite_1161 = {3, 6, 33, 10, providerRange,        0, 0,  80, DioriteOre, DIORITE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_diorite_117 =  {3, 6, 33, 10, providerUniformRange, 0, 79, -1, DioriteOre, DIORITE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_dirt_113 =  {0, 4, 33, 10, providerRange,        0, 0,  256, DirtOre, DIRT, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_dirt_1161 = {0, 6, 33, 10, providerRange,        0, 0,  256, DirtOre, DIRT, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_dirt_117 =  {0, 6, 33, 10, providerUniformRange, 0, 255, -1, DirtOre, DIRT, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_dirt_118 =  {0, 6, 33,  7, providerUniformRange, 0, 160, -1, DirtOre, DIRT, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_emerald_113 =  {14, 4, 1,   1, providerEmeraldOre,     -1,  -1, -1, EmeraldOre, EMERALD_ORE, DIM_OVERWORLD, 1, STONE_REPLACEABLES, 0.0F},
    o_emerald_1161 = {14, 6, 1,   1, providerEmeraldOre,     -1,  -1, -1, EmeraldOre, EMERALD_ORE, DIM_OVERWORLD, 1, STONE_REPLACEABLES, 0.0F},
    o_emerald_117 =  {17, 6, 1,   1, providerUniformRange,    4,  31, -1, EmeraldOre, EMERALD_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_emerald_118 =  {31, 6, 3, 100, providerTriangleRange, -16, 480, -1, EmeraldOre, EMERALD_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_emerald_119 =  {32, 6, 3, 100, providerTriangleRange, -16, 480, -1, EmeraldOre, EMERALD_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_emerald_120 =  {33, 6, 3, 100, providerTriangleRange, -16, 480, -1, EmeraldOre, EMERALD_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_extra_gold_113 =  {11, 4, 9, 20, providerRange,        32,  32, 80, ExtraGoldOre, GOLD_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_extra_gold_1161 = {11, 6, 9, 20, providerRange,        32,  32, 80, ExtraGoldOre, GOLD_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_extra_gold_117 =  {14, 6, 9, 20, providerUniformRange, 32,  79, -1, ExtraGoldOre, GOLD_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_extra_gold_118 =  {27, 6, 9, 50, providerUniformRange, 32, 256, -1, ExtraGoldOre, GOLD_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.5F},
    o_extra_gold_120 =  {28, 6, 9, 50, providerUniformRange, 32, 256, -1, ExtraGoldOre, GOLD_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.5F},

    o_gold_113 =  { 7, 4, 9, 2, providerRange,           0,  0, 32, GoldOre, GOLD_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_gold_1161 = { 7, 6, 9, 2, providerRange,           0,  0, 32, GoldOre, GOLD_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_gold_117 =  { 9, 6, 9, 2, providerUniformRange,    0, 31, -1, GoldOre, GOLD_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_gold_118 =  {14, 6, 9, 4, providerTriangleRange, -64, 32, -1, GoldOre, GOLD_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.5F},

    o_granite_113 =  {2, 4, 33, 10, providerRange,        0,  0, 80, GraniteOre, GRANITE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_granite_1161 = {2, 6, 33, 10, providerRange,        0,  0, 80, GraniteOre, GRANITE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_granite_117 =  {2, 6, 33, 10, providerUniformRange, 0, 79, -1, GraniteOre, GRANITE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_gravel_113 =  {1, 4, 33, 8,  providerRange,          0,   0, 256, GravelOre, GRAVEL, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_gravel_1161 = {1, 6, 33, 8,  providerRange,          0,   0, 256, GravelOre, GRAVEL, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_gravel_117 =  {1, 6, 33, 8,  providerUniformRange,   0, 255,  -1, GravelOre, GRAVEL, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_gravel_118 =  {1, 6, 33, 14, providerUniformRange, -64, 319,  -1, GravelOre, GRAVEL, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_infested_113 =  {0, 5, 9,  7, providerRange,          0,  0, 64, InfestedOre, INFESTED_STONE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_infested_1161 = {2, 7, 9,  7, providerRange,          0,  0, 64, InfestedOre, INFESTED_STONE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_infested_117 =  {4, 7, 9,  7, providerUniformRange,   0, 63, -1, InfestedOre, INFESTED_STONE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_infested_118 =  {2, 7, 9, 14, providerUniformRange, -64, 63, -1, InfestedOre, INFESTED_STONE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_infested_1192 = {4, 7, 9, 14, providerUniformRange, -64, 63, -1, InfestedOre, INFESTED_STONE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_infested_262 =  {6, 7, 9, 14, providerUniformRange, -64, 63, -1, InfestedOre, INFESTED_STONE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_iron_113 =  {6, 4, 9, 20, providerRange,        0,  0, 64, IronOre, IRON_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_iron_1161 = {6, 6, 9, 20, providerRange,        0,  0, 64, IronOre, IRON_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_iron_117 =  {8, 6, 9, 20, providerUniformRange, 0, 63, -1, IronOre, IRON_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_lapis_113 =  {10, 4, 7, 1, providerDepthAverage,   16, 16, -1, LapisOre, LAPIS_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_lapis_1161 = {10, 6, 7, 1, providerDepthAverage,   16, 16, -1, LapisOre, LAPIS_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_lapis_117 =  {12, 6, 7, 1, providerTriangleRange,   0, 30, -1, LapisOre, LAPIS_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_lapis_118 =  {21, 6, 7, 2, providerTriangleRange, -32, 32, -1, LapisOre, LAPIS_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_lapis_120 =  {22, 6, 7, 2, providerTriangleRange, -32, 32, -1, LapisOre, LAPIS_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_large_copper_118 = {23, 6, 20, 16, providerTriangleRange, -16, 112, -1, LargeCopperOre, COPPER_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_large_copper_120 = {24, 6, 20, 16, providerTriangleRange, -16, 112, -1, LargeCopperOre, COPPER_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    // uses rareOrePlacement
    o_large_diamond_118 = {19, 6, 12, 9, providerTriangleRange, -64 + -80, -64 + 80, -1, LargeDiamondOre, DIAMOND_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.7F},
    o_large_diamond_120 = {20, 6, 12, 9, providerTriangleRange, -64 + -80, -64 + 80, -1, LargeDiamondOre, DIAMOND_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.7F},

    o_lower_andesite_118 = {7, 6, 64, 2, providerUniformRange, 0, 60, -1, LowerAndesiteOre, ANDESITE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_lower_coal_118 = {10, 6, 17, 20, providerTriangleRange, 0, 192, -1, LowerCoalOre, COAL_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.5F},

    o_lower_diorite_118 = {5, 6, 64, 2, providerUniformRange, 0, 60, -1, LowerDioriteOre, DIORITE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    // uses UniformInt.of(0, 1)
    o_lower_gold_118 = {15, 6, 9, 1, providerUniformRange, -64, -48, -1, LowerGoldOre, GOLD_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.5F},

    o_lower_granite_118 = {3, 6, 64, 2, providerUniformRange, 0, 60, -1, LowerGraniteOre, GRANITE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_lower_redstone_118 = {17, 6, 8, 8, providerTriangleRange, -64 + -32, -64 + 32, -1, LowerRedstoneOre, REDSTONE_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_medium_diamond_120 = {19, 6, 8, 2, providerUniformRange, -64, -4, -1, MediumDiamondOre, DIAMOND_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.5F},

    o_middle_iron_118 = {12, 6, 9, 10, providerTriangleRange, -24, 56, -1, MiddleIronOre, IRON_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_redstone_113 =  { 8, 4, 8, 8, providerRange,          0,  0, 16, RedstoneOre, REDSTONE_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_redstone_1161 = { 8, 6, 8, 8, providerRange,          0,  0, 16, RedstoneOre, REDSTONE_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_redstone_117 =  {10, 6, 8, 8, providerUniformRange,   0, 15, -1, RedstoneOre, REDSTONE_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_redstone_118 =  {16, 6, 8, 4, providerUniformRange, -64, 15, -1, RedstoneOre, REDSTONE_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_small_iron_118 = {13, 6, 4, 10, providerUniformRange, -64, 72, -1, SmallIronOre, IRON_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_tuff_117 = {5, 6, 33, 1, providerUniformRange,   0, 16, -1, TuffOre, TUFF, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},
    o_tuff_118 = {8, 6, 64, 2, providerUniformRange, -64,  0, -1, TuffOre, TUFF, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    // uses rareOrePlacement
    o_upper_andesite_118 = {6, 6, 64, 6, providerUniformRange, 64, 128, -1, UpperAndesiteOre, ANDESITE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_upper_coal_118 = {9, 6, 17, 30, providerUniformRange, 136, 319, -1, UpperCoalOre, COAL_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    // uses rareOrePlacement
    o_upper_diorite_118 = {4, 6, 64, 6, providerUniformRange, 64, 128, -1, UpperDioriteOre, DIORITE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    // uses rareOrePlacement
    o_upper_granite_118 = {2, 6, 64, 6, providerUniformRange, 64, 128, -1, UpperGraniteOre, GRANITE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    o_upper_iron_118 = {11, 6, 9, 90, providerTriangleRange, 80, 384, -1, UpperIronOre, IRON_ORE, DIM_OVERWORLD, 6, BASE_STONE_OVERWORLD_REPLACEABLES, 0.0F},

    // nether
    o_blackstone_1161 =                {12, 7, 33, 2, providerRange,        5, 10, 37, BlackstoneOre, BLACKSTONE, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_blackstone_1161_crimson_forest = { 9, 7, 33, 2, providerRange,        5, 10, 37, BlackstoneOre, BLACKSTONE, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_blackstone_1161_warped_forest =  {10, 7, 33, 2, providerRange,        5, 10, 37, BlackstoneOre, BLACKSTONE, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_blackstone_117 =                 {12, 7, 33, 2, providerUniformRange, 5, 31, -1, BlackstoneOre, BLACKSTONE, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_blackstone_118 =                 {18, 7, 33, 2, providerUniformRange, 5, 31, -1, BlackstoneOre, BLACKSTONE, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},

    o_deltas_gold_1161 = {13, 7, 10, 20, providerRange,            10,       20, 128, DeltasGoldOre, NETHER_GOLD_ORE, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_deltas_gold_117 =  {13, 7, 10, 20, providerUniformRange, 0 + 10, 127 - 10,  -1, DeltasGoldOre, NETHER_GOLD_ORE, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},

    o_deltas_quartz_1161 = {14, 7, 14, 32, providerRange,            10,       20, 128, DeltasQuartzOre, NETHER_QUARTZ_ORE, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_deltas_quartz_117 =  {14, 7, 14, 32, providerUniformRange, 0 + 10, 127 - 10,  -1, DeltasQuartzOre, NETHER_QUARTZ_ORE, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},

    // scatter ore, no count
    o_large_debris_1161 =                {15, 7, 3, 1, providerDepthAverage, 16,  8, -1, LargeDebrisOre, ANCIENT_DEBRIS, DIM_NETHER, 3, BASE_STONE_NETHER_REPLACEABLES, 0.0F},
    o_large_debris_1161_crimson_forest = {12, 7, 3, 1, providerDepthAverage, 16,  8, -1, LargeDebrisOre, ANCIENT_DEBRIS, DIM_NETHER, 3, BASE_STONE_NETHER_REPLACEABLES, 0.0F},
    o_large_debris_1161_warped_forest =  {13, 7, 3, 1, providerDepthAverage, 16,  8, -1, LargeDebrisOre, ANCIENT_DEBRIS, DIM_NETHER, 3, BASE_STONE_NETHER_REPLACEABLES, 0.0F},
    o_large_debris_117 =                 {15, 7, 3, 1, providerTriangleRange, 8, 24, -1, LargeDebrisOre, ANCIENT_DEBRIS, DIM_NETHER, 3, BASE_STONE_NETHER_REPLACEABLES, 0.0F},
    o_large_debris_118 =                 {21, 7, 3, 1, providerTriangleRange, 8, 24, -1, LargeDebrisOre, ANCIENT_DEBRIS, DIM_NETHER, 3, BASE_STONE_NETHER_REPLACEABLES, 1.0F},

    o_magma_113 =                   { 8, 5, 33, 4, providerMagmaOre,     -1, -1, -1, MagmaOre, MAGMA_BLOCK, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_magma_1161 =                  { 9, 7, 33, 4, providerMagmaOre,     -1, -1, -1, MagmaOre, MAGMA_BLOCK, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_magma_1161_soul_sand_valley = { 8, 7, 33, 4, providerMagmaOre,     -1, -1, -1, MagmaOre, MAGMA_BLOCK, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_magma_1161_crimson_forest =   { 6, 7, 33, 4, providerMagmaOre,     -1, -1, -1, MagmaOre, MAGMA_BLOCK, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_magma_1161_warped_forest =    { 7, 7, 33, 4, providerMagmaOre,     -1, -1, -1, MagmaOre, MAGMA_BLOCK, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_magma_1161_basalt_deltas =    {11, 7, 33, 4, providerMagmaOre,     -1, -1, -1, MagmaOre, MAGMA_BLOCK, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_magma_117 =                   { 9, 7, 33, 4, providerUniformRange, 27, 36, -1, MagmaOre, MAGMA_BLOCK, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_magma_118 =                   {11, 7, 33, 4, providerUniformRange, 27, 36, -1, MagmaOre, MAGMA_BLOCK, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},

    o_nether_gold_1161 =                {13, 7, 10, 10, providerRange,            10,       20, 128, NetherGoldOre, NETHER_GOLD_ORE, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_nether_gold_1161_crimson_forest = {10, 7, 10, 10, providerRange,            10,       20, 128, NetherGoldOre, NETHER_GOLD_ORE, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_nether_gold_1161_warped_forest =  {11, 7, 10, 10, providerRange,            10,       20, 128, NetherGoldOre, NETHER_GOLD_ORE, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_nether_gold_117 =                 {13, 7, 10, 10, providerUniformRange, 0 + 10, 127 - 10,  -1, NetherGoldOre, NETHER_GOLD_ORE, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_nether_gold_118 =                 {19, 7, 10, 10, providerUniformRange, 0 + 10, 127 - 10,  -1, NetherGoldOre, NETHER_GOLD_ORE, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},

    o_nether_gravel_1161 =                {11, 7, 33, 2, providerRange,        5,  0, 37, NetherGravelOre, GRAVEL, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_nether_gravel_1161_crimson_forest = { 8, 7, 33, 2, providerRange,        5,  0, 37, NetherGravelOre, GRAVEL, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_nether_gravel_1161_warped_forest =  { 9, 7, 33, 2, providerRange,        5,  0, 37, NetherGravelOre, GRAVEL, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_nether_gravel_117 =                 {11, 7, 33, 2, providerUniformRange, 5, 41, -1, NetherGravelOre, GRAVEL, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_nether_gravel_118 =                 {17, 7, 33, 2, providerUniformRange, 5, 41, -1, NetherGravelOre, GRAVEL, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},

    o_nether_quartz_113 =                 { 7, 5, 14, 16, providerRange,            10,       20, 128, NetherQuartzOre, NETHER_QUARTZ_ORE, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_nether_quartz_1161 =                {14, 7, 14, 16, providerRange,            10,       20, 128, NetherQuartzOre, NETHER_QUARTZ_ORE, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_nether_quartz_1161_crimson_forest = {11, 7, 14, 16, providerRange,            10,       20, 128, NetherQuartzOre, NETHER_QUARTZ_ORE, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_nether_quartz_1161_warped_forest =  {12, 7, 14, 16, providerRange,            10,       20, 128, NetherQuartzOre, NETHER_QUARTZ_ORE, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_nether_quartz_117 =                 {14, 7, 14, 16, providerUniformRange, 0 + 10, 127 - 10,  -1, NetherQuartzOre, NETHER_QUARTZ_ORE, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_nether_quartz_118 =                 {20, 7, 14, 16, providerUniformRange, 0 + 10, 127 - 10,  -1, NetherQuartzOre, NETHER_QUARTZ_ORE, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},

    // scatter ore, no count
    o_small_debris_1161 =                {16, 7, 2, 1, providerRange,        8,      16, 128, SmallDebrisOre, ANCIENT_DEBRIS, DIM_NETHER, 3, BASE_STONE_NETHER_REPLACEABLES, 0.0F},
    o_small_debris_1161_crimson_forest = {13, 7, 2, 1, providerRange,        8,      16, 128, SmallDebrisOre, ANCIENT_DEBRIS, DIM_NETHER, 3, BASE_STONE_NETHER_REPLACEABLES, 0.0F},
    o_small_debris_1161_warped_forest =  {14, 7, 2, 1, providerRange,        8,      16, 128, SmallDebrisOre, ANCIENT_DEBRIS, DIM_NETHER, 3, BASE_STONE_NETHER_REPLACEABLES, 0.0F},
    o_small_debris_117 =                 {16, 7, 2, 1, providerUniformRange, 8, 127 - 8,  -1, SmallDebrisOre, ANCIENT_DEBRIS, DIM_NETHER, 3, BASE_STONE_NETHER_REPLACEABLES, 0.0F},
    o_small_debris_118 =                 {22, 7, 2, 1, providerUniformRange, 8, 127 - 8,  -1, SmallDebrisOre, ANCIENT_DEBRIS, DIM_NETHER, 3, BASE_STONE_NETHER_REPLACEABLES, 1.0F},

    o_soul_sand_1161 = {10, 7, 12, 12, providerRange,        0,  0, 32, SoulSandOre, SOUL_SAND, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_soul_sand_117 =  {10, 7, 12, 12, providerUniformRange, 0, 31, -1, SoulSandOre, SOUL_SAND, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F},
    o_soul_sand_118 =  {16, 7, 12, 12, providerUniformRange, 0, 31, -1, SoulSandOre, SOUL_SAND, DIM_NETHER, 1, NETHERRACK_REPLACEABLES, 0.0F}
    ;

    switch (oreType)
    {
    // overworld
    case AndesiteOre:
        if (mc < MC_1_16_1) *oconf = o_andesite_113;
        else if (mc < MC_1_17) *oconf = o_andesite_1161;
        else *oconf = o_andesite_117;
        return mc > MC_1_12 && mc <= MC_1_17;
    case BuriedDiamondOre:
        if (mc < MC_1_20) *oconf = o_buried_diamond_118;
        else *oconf = o_buried_diamond_120;
        return mc > MC_1_17;
    case BuriedLapisOre:
        if (mc < MC_1_20) *oconf = o_buried_lapis_118;
        else *oconf = o_buried_lapis_120;
        return mc > MC_1_17;
    case ClayOre:
        if (mc < MC_1_20) *oconf = o_clay_118;
        else *oconf = o_clay_120;
        return mc > MC_1_17;
    case CoalOre:
        if (mc < MC_1_16_1) *oconf = o_coal_113;
        else if (mc < MC_1_17) *oconf = o_coal_1161;
        else *oconf = o_coal_117;
        return mc > MC_1_12 && mc <= MC_1_17;
    case CopperOre:
        if (mc < MC_1_18) *oconf = o_copper_117;
        else if (mc < MC_1_20) *oconf = o_copper_118;
        else *oconf = o_copper_120;
        return mc > MC_1_16;
    case DeepslateOre:
        *oconf = o_deepslate_117;
        return mc > MC_1_16 && mc <= MC_1_17;
    case DiamondOre:
        if (mc < MC_1_16_1) *oconf = o_diamond_113;
        else if (mc < MC_1_17) *oconf = o_diamond_1161;
        else if (mc < MC_1_18) *oconf = o_diamond_117;
        else *oconf = o_diamond_118;
        return mc > MC_1_12;
    case DioriteOre:
        if (mc < MC_1_16_1) *oconf = o_diorite_113;
        else if (mc < MC_1_17) *oconf = o_diorite_1161;
        else *oconf = o_diorite_117;
        return mc > MC_1_12 && mc <= MC_1_17;
    case DirtOre:
        if (mc < MC_1_16_1) *oconf = o_dirt_113;
        else if (mc < MC_1_17) *oconf = o_dirt_1161;
        else if (mc < MC_1_18) *oconf = o_dirt_117;
        else *oconf = o_dirt_118;
        return mc > MC_1_12;
    case EmeraldOre:
        if (mc < MC_1_16_1) *oconf = o_emerald_113;
        else if (mc < MC_1_17) *oconf = o_emerald_1161;
        else if (mc < MC_1_18) *oconf = o_emerald_117;
        else if (mc < MC_1_19) *oconf = o_emerald_118;
        else if (mc < MC_1_20) *oconf = o_emerald_119;
        else *oconf = o_emerald_120;
        return mc > MC_1_12;
    case ExtraGoldOre:
        if (mc < MC_1_16_1) *oconf = o_extra_gold_113;
        else if (mc < MC_1_17) *oconf = o_extra_gold_1161;
        else if (mc < MC_1_18) *oconf = o_extra_gold_117;
        else if (mc < MC_1_20) *oconf = o_extra_gold_118;
        else *oconf = o_extra_gold_120;
        return mc > MC_1_12;
    case GoldOre:
        if (mc < MC_1_16_1) *oconf = o_gold_113;
        else if (mc < MC_1_17) *oconf = o_gold_1161;
        else if (mc < MC_1_18) *oconf = o_gold_117;
        else *oconf = o_gold_118;
        return mc > MC_1_12;
    case GraniteOre:
        if (mc < MC_1_16_1) *oconf = o_granite_113;
        else if (mc < MC_1_17) *oconf = o_granite_1161;
        else *oconf = o_granite_117;
        return mc > MC_1_12 && mc <= MC_1_17;
    case GravelOre:
        if (mc < MC_1_16_1) *oconf = o_gravel_113;
        else if (mc < MC_1_17) *oconf = o_gravel_1161;
        else if (mc < MC_1_18) *oconf = o_gravel_117;
        else *oconf = o_gravel_118;
        return mc > MC_1_12;
    case InfestedOre:
        if (mc < MC_1_16_1) *oconf = o_infested_113;
        else if (mc < MC_1_17) *oconf = o_infested_1161;
        else if (mc < MC_1_18) *oconf = o_infested_117;
        else if (mc < MC_1_19_2) *oconf = o_infested_118;
        else if (mc < MC_26_2) *oconf = o_infested_1192;
        else *oconf = o_infested_262;
        return mc > MC_1_12;
    case IronOre:
        if (mc < MC_1_16_1) *oconf = o_iron_113;
        else if (mc < MC_1_17) *oconf = o_iron_1161;
        else *oconf = o_iron_117;
        return mc > MC_1_12 && mc <= MC_1_17;
    case LapisOre:
        if (mc < MC_1_16_1) *oconf = o_lapis_113;
        else if (mc < MC_1_17) *oconf = o_lapis_1161;
        else if (mc < MC_1_18) *oconf = o_lapis_117;
        else if (mc < MC_1_20) *oconf = o_lapis_118;
        else *oconf = o_lapis_120;
        return mc > MC_1_12;
    case LargeCopperOre:
        if (mc < MC_1_20) *oconf = o_large_copper_118;
        else *oconf = o_large_copper_120;
        return mc > MC_1_17;
    case LargeDiamondOre:
        if (mc < MC_1_20) *oconf = o_large_diamond_118;
        else *oconf = o_large_diamond_120;
        return mc > MC_1_17;
    case LowerAndesiteOre:
        *oconf = o_lower_andesite_118;
        return mc > MC_1_17;
    case LowerCoalOre:
        *oconf = o_lower_coal_118;
        return mc > MC_1_17;
    case LowerDioriteOre:
        *oconf = o_lower_diorite_118;
        return mc > MC_1_17;
    case LowerGoldOre:
        *oconf = o_lower_gold_118;
        return mc > MC_1_17;
    case LowerGraniteOre:
        *oconf = o_lower_granite_118;
        return mc > MC_1_17;
    case LowerRedstoneOre:
        *oconf = o_lower_redstone_118;
        return mc > MC_1_17;
    case MediumDiamondOre:
        *oconf = o_medium_diamond_120;
        return mc > MC_1_19;
    case MiddleIronOre:
        *oconf = o_middle_iron_118;
        return mc > MC_1_17;
    case RedstoneOre:
        if (mc < MC_1_16_1) *oconf = o_redstone_113;
        else if (mc < MC_1_17) *oconf = o_redstone_1161;
        else if (mc < MC_1_18) *oconf = o_redstone_117;
        else *oconf = o_redstone_118;
        return mc > MC_1_12;
    case SmallIronOre:
        *oconf = o_small_iron_118;
        return mc > MC_1_17;
    case TuffOre:
        if (mc <= MC_1_17) *oconf = o_tuff_117;
        else *oconf = o_tuff_118;
        return mc > MC_1_16;
    case UpperAndesiteOre:
        *oconf = o_upper_andesite_118;
        return mc > MC_1_17;
    case UpperCoalOre:
        *oconf = o_upper_coal_118;
        return mc > MC_1_17;
    case UpperDioriteOre:
        *oconf = o_upper_diorite_118;
        return mc > MC_1_17;
    case UpperGraniteOre:
        *oconf = o_upper_granite_118;
        return mc > MC_1_17;
    case UpperIronOre:
        *oconf = o_upper_iron_118;
        return mc > MC_1_17;
    // nether
    case BlackstoneOre:
        if (mc < MC_1_17 && biomeID == crimson_forest) *oconf = o_blackstone_1161_crimson_forest;
        else if (mc < MC_1_17 && biomeID == warped_forest) *oconf = o_blackstone_1161_warped_forest;
        else if (mc < MC_1_17) *oconf = o_blackstone_1161;
        else if (mc < MC_1_18) *oconf = o_blackstone_117;
        else *oconf = o_blackstone_118;
        return mc > MC_1_15;
    case DeltasGoldOre:
        if (mc < MC_1_17) *oconf = o_deltas_gold_1161;
        else *oconf = o_deltas_gold_117;
        return mc > MC_1_15;
    case DeltasQuartzOre:
        if (mc < MC_1_17) *oconf = o_deltas_quartz_1161;
        else *oconf = o_deltas_quartz_117;
        return mc > MC_1_15;
    case LargeDebrisOre:
        if (mc < MC_1_17 && biomeID == crimson_forest) *oconf = o_large_debris_1161_crimson_forest;
        else if (mc < MC_1_17 && biomeID == warped_forest) *oconf = o_large_debris_1161_warped_forest;
        else if (mc < MC_1_17) *oconf = o_large_debris_1161;
        else if (mc < MC_1_18) *oconf = o_large_debris_117;
        else *oconf = o_large_debris_118;
        return mc > MC_1_15;
    case MagmaOre:
        if (mc < MC_1_16_1) *oconf = o_magma_113;
        else if (mc < MC_1_17 && biomeID == soul_sand_valley) *oconf = o_magma_1161_soul_sand_valley;
        else if (mc < MC_1_17 && biomeID == crimson_forest) *oconf = o_magma_1161_crimson_forest;
        else if (mc < MC_1_17 && biomeID == warped_forest) *oconf = o_magma_1161_warped_forest;
        else if (mc < MC_1_17 && biomeID == basalt_deltas) *oconf = o_magma_1161_basalt_deltas;
        else if (mc < MC_1_17) *oconf = o_magma_1161;
        else if (mc < MC_1_18) *oconf = o_magma_117;
        else *oconf = o_magma_118;
        return mc > MC_1_12;
    case NetherGoldOre:
        if (mc < MC_1_17 && biomeID == crimson_forest) *oconf = o_nether_gold_1161_crimson_forest;
        else if (mc < MC_1_17 && biomeID == warped_forest) *oconf = o_nether_gold_1161_warped_forest;
        else if (mc < MC_1_17) *oconf = o_nether_gold_1161;
        else if (mc < MC_1_18) *oconf = o_nether_gold_117;
        else *oconf = o_nether_gold_118;
        return mc > MC_1_15;
    case NetherGravelOre:
        if (mc < MC_1_17 && biomeID == crimson_forest) *oconf = o_nether_gravel_1161_crimson_forest;
        else if (mc < MC_1_17 && biomeID == warped_forest) *oconf = o_nether_gravel_1161_warped_forest;
        else if (mc < MC_1_17) *oconf = o_nether_gravel_1161;
        else if (mc < MC_1_18) *oconf = o_nether_gravel_117;
        else *oconf = o_nether_gravel_118;
        return mc > MC_1_15;
    case NetherQuartzOre:
        if (mc < MC_1_16_1) *oconf = o_nether_quartz_113;
        else if (mc < MC_1_17 && biomeID == crimson_forest) *oconf = o_nether_quartz_1161_crimson_forest;
        else if (mc < MC_1_17 && biomeID == warped_forest) *oconf = o_nether_quartz_1161_warped_forest;
        else if (mc < MC_1_17) *oconf = o_nether_quartz_1161;
        else if (mc < MC_1_18) *oconf = o_nether_quartz_117;
        else *oconf = o_nether_quartz_118;
        return mc > MC_1_12;
    case SmallDebrisOre:
        if (mc < MC_1_17 && biomeID == crimson_forest) *oconf = o_small_debris_1161_crimson_forest;
        else if (mc < MC_1_17 && biomeID == warped_forest) *oconf = o_small_debris_1161_warped_forest;
        else if (mc < MC_1_17) *oconf = o_small_debris_1161;
        else if (mc < MC_1_18) *oconf = o_small_debris_117;
        else *oconf = o_small_debris_118;
        return mc > MC_1_15;
    case SoulSandOre:
        if (mc < MC_1_17) *oconf = o_soul_sand_1161;
        else if (mc < MC_1_18) *oconf = o_soul_sand_117;
        else *oconf = o_soul_sand_118;
        return mc > MC_1_15;
    default:
        memset(oconf, 0, sizeof(OreConfig));
        return 0;
    }
}

int isViableOreBiome(int mc, int oreType, int biomeID)
{
    // check this in OverworldBiomes.java/NetherBiomes.java
    switch (oreType)
    {
    // overworld
    case AndesiteOre:
    case BuriedDiamondOre:
    case BuriedLapisOre:
    case CoalOre:
    case CopperOre:
    case DeepslateOre:
    case DiamondOre:
    case DioriteOre:
    case DirtOre:
    case GoldOre:
    case GraniteOre:
    case GravelOre:
    case IronOre:
    case LapisOre:
    case LargeDiamondOre:
    case LowerAndesiteOre:
    case LowerCoalOre:
    case LowerDioriteOre:
    case LowerGoldOre:
    case LowerGraniteOre:
    case LowerRedstoneOre:
    case MediumDiamondOre:
    case MiddleIronOre:
    case RedstoneOre:
    case SmallIronOre:
    case TuffOre:
    case UpperAndesiteOre:
    case UpperCoalOre:
    case UpperDioriteOre:
    case UpperGraniteOre:
    case UpperIronOre:
        return isOverworld(mc, biomeID);
    case EmeraldOre:
    case InfestedOre:
        switch (biomeID) {
        case mountains: // == windswept_hills
        case mountain_edge:
        case wooded_mountains: // == windswept_forest
        case gravelly_mountains: // == windswept_gravelly_hills
        case modified_gravelly_mountains:
        case meadow:
        case grove:
        case snowy_slopes:
        case jagged_peaks:
        case frozen_peaks:
        case stony_peaks:
        case cherry_grove:
            return 1;
        default: return 0;
        }
    case ExtraGoldOre:
        switch (biomeID) {
        case badlands:
        case wooded_badlands_plateau: // == wooded_badlands
        case badlands_plateau:
        case eroded_badlands:
        case modified_wooded_badlands_plateau:
        case modified_badlands_plateau:
            return 1;
        default: return 0;
        }
    case LargeCopperOre:
        return biomeID == dripstone_caves || (mc <= MC_1_19_2 && biomeID == deep_dark); // emulate MC-255133
    case ClayOre:
        return biomeID == lush_caves;
    // nether
    case LargeDebrisOre:
    case MagmaOre:
    case SmallDebrisOre:
        return getDimension(biomeID) == DIM_NETHER;
    case BlackstoneOre:
    case NetherGoldOre:
    case NetherQuartzOre:
    case NetherGravelOre:
        return (biomeID == nether_wastes || biomeID == soul_sand_valley ||
                biomeID == crimson_forest || biomeID == warped_forest);
    case DeltasGoldOre:
    case DeltasQuartzOre:
        return biomeID == basalt_deltas;
    case SoulSandOre:
        return biomeID == soul_sand_valley;
    default:
        fprintf(stderr, "isViableOreBiome: not implemented for ore type %d.\n", oreType);
        exit(1);
    }
    return 0;
}

int getBiomeForOreGen(const Generator *g, int chunkX, int chunkZ, int y)
{
    if (g->mc <= MC_1_15) {
        return getBiomeAt(g, 1, (chunkX << 4) + 8, 0, (chunkZ << 4) + 8);
    }
    if (g->mc <= MC_1_17) {
        return getBiomeAt(g, 4, (chunkX << 2) + 2, 0, (chunkZ << 2) + 2);
    }
    return getBiomeAt(g, 4, (chunkX << 2) + 2, y >> 2, (chunkZ << 2) + 2);
}

Pos3List generateOres(const Generator *g, const SurfaceNoise *sn, OreConfig config, int chunkX, int chunkZ)
{
    uint64_t populationSeed = getPopulationSeed(g->mc, g->seed, chunkX << 4, chunkZ << 4);
    CREATE_RANDOM_SOURCE(rnd, g->mc <= MC_1_17);
    // set decorator seed
    rnd.setSeed(rnd.state, populationSeed + config.index + 10000 * config.step);

    int oreType = config.oreType;
    int repeatCount;
    // rareOrePlacement check
    if (oreType == LargeDiamondOre || oreType == UpperAndesiteOre ||
        oreType == UpperDioriteOre || oreType == UpperGraniteOre) {
        repeatCount = rnd.nextFloat(rnd.state) < 1.0F / config.repeatCount;
    } else {
        repeatCount = config.repeatCount;
    }

    Pos3List pos3s;
    // x^2/4 is an approx that works for sizes <= 64, but only works for ores that use this config
    const int approxSize = repeatCount * ((config.size * config.size) >> 2);
    createPos3List(&pos3s, MAX(8, approxSize));

    for (int i = 0; i < repeatCount; i++) {
        Pos3 basePos = generateBaseOrePosition(g->mc, config, chunkX, chunkZ, rnd);
        int biome = getBiomeAt(g, 1, basePos.x, basePos.y, basePos.z);
        if (isViableOreBiome(g->mc, oreType, biome)) {
            generateOrePositions(g, sn, config, basePos, rnd, &pos3s);
        }
    }
    return pos3s;
}

Pos3 generateBaseOrePosition(int mc, OreConfig config, int chunkX, int chunkZ, RandomSource rnd)
{
    if ((mc <= MC_1_17 && config.oreType == EmeraldOre) || (mc <= MC_NEWEST && config.oreType == LowerGoldOre)) {
        return (Pos3) {chunkX << 4, 0, chunkZ << 4};
    }
    if (mc <= MC_1_14) {
        int blockX = (chunkX << 4) + rnd.nextInt(rnd.state, 16);
        int blockY = config.heightProvider(rnd, config.h1, config.h2, config.h3);
        int blockZ = (chunkZ << 4) + rnd.nextInt(rnd.state, 16);
        return (Pos3) {blockX, blockY, blockZ};
    } else {
        int blockX = (chunkX << 4) + rnd.nextInt(rnd.state, 16);
        int blockZ = (chunkZ << 4) + rnd.nextInt(rnd.state, 16);
        int blockY = config.heightProvider(rnd, config.h1, config.h2, config.h3);
        return (Pos3) {blockX, blockY, blockZ};
    }
}

void generateOrePositions(const Generator *g, const SurfaceNoise *sn, OreConfig config, Pos3 pos, RandomSource rnd, Pos3List* pos3s)
{
    int mc = g->mc;
    if ((mc <= MC_1_17 && config.oreType == EmeraldOre) || (mc <= MC_NEWEST && config.oreType == LowerGoldOre)) {
        int count;
        if (config.oreType == EmeraldOre) {
            if (mc <= MC_1_16) {
                count = 3 + rnd.nextInt(rnd.state, 6);
            } else {
                // was 6, 24 in 1.17, changed to 3, 8 in 1.17.1
                count = rnd.nextIntBetween(rnd.state, 3, 8);
            }
        } else {
            count = rnd.nextIntBetween(rnd.state, 0, 1);
        }
        for (int i = 0; i < count; i++) {
            if (mc <= MC_1_14) {
                int x = pos.x + rnd.nextInt(rnd.state, 16);
                int y = config.heightProvider(rnd, config.h1, config.h2, config.h3);
                int z = pos.z + rnd.nextInt(rnd.state, 16);
                appendPos3List(pos3s, (Pos3) {x, y, z});
            } else {
                int x = pos.x + rnd.nextInt(rnd.state, 16);
                int z = pos.z + rnd.nextInt(rnd.state, 16);
                int y = config.heightProvider(rnd, config.h1, config.h2, config.h3);
                appendPos3List(pos3s, (Pos3) {x, y, z});
            }
        }
        return;
    }

    // scatter
    if (config.oreType == LargeDebrisOre || config.oreType == SmallDebrisOre) {
        int count = rnd.nextInt(rnd.state, config.size + 1);
        for (int i = 0; i < count; ++i) {
            int size = MIN(i, 7);
            float a, b;
            a = rnd.nextFloat(rnd.state);
            b = rnd.nextFloat(rnd.state);
            int x = roundf((a - b) * (float)size);
            a = rnd.nextFloat(rnd.state);
            b = rnd.nextFloat(rnd.state);
            int y = roundf((a - b) * (float)size);
            a = rnd.nextFloat(rnd.state);
            b = rnd.nextFloat(rnd.state);
            int z = roundf((a - b) * (float)size);
            Pos3 startPos = (Pos3) {pos.x + x, pos.y + y, pos.z + z};

            // TODO: check if the block at startPos is contained in config.replaceBlocks
            // TODO: and if the block at startPos is not air-exposed
            if (1) {
                appendPos3List(pos3s, startPos);
            }
        }

        return;
    }

    // regular
    float angle = rnd.nextFloat(rnd.state) * (float)PI;
    float size = (float)config.size / 8.0F;
    int amortizedSize = ceil(((float)config.size / 16.0F * 2.0F + 1.0F) / 2.0F);
    double offsetXPos = (double)pos.x + sin(angle) * (double)size;
    double offsetXNeg = (double)pos.x - sin(angle) * (double)size;
    double offsetZPos = (double)pos.z + cos(angle) * (double)size;
    double offsetZNeg = (double)pos.z - cos(angle) * (double)size;
    double offsetYPos = pos.y + rnd.nextInt(rnd.state, 3) - 2;
    double offsetYNeg = pos.y + rnd.nextInt(rnd.state, 3) - 2;
    int startX = pos.x - ceil(size) - amortizedSize;
    int startY = pos.y - 2 - amortizedSize;
    int startZ = pos.z - ceil(size) - amortizedSize;
    int oreSize = 2 * (ceil(size) + amortizedSize);
    int radius = 2 * (2 + amortizedSize);

    for (int x = startX; x <= startX + oreSize; ++x) {
        for (int z = startZ; z <= startZ + oreSize; ++z) {
            float y;
            mapApproxHeight(&y, 0, g, sn, x >> 2, z >> 2, 1, 1);
            if (startY <= (g->dim == DIM_OVERWORLD ? (int) floor(y) : 128)) {
                generateVeinPart(mc, config, rnd, offsetXPos, offsetXNeg, offsetZPos, offsetZNeg, offsetYPos, offsetYNeg, startX, startY, startZ, oreSize, radius, pos3s);
                return;
            }
        }
    }
}

void generateVeinPart(int mc, OreConfig config, RandomSource rnd, double offsetXPos, double offsetXNeg, double offsetZPos, double offsetZNeg, double offsetYPos, double offsetYNeg, int startX, int startY, int startZ, int oreSize, int radius, Pos3List* pos3s)
{
    const int minBuildHeight = mc <= MC_1_17 ? 0 : -64;
    const int maxBuildHeight = mc <= MC_1_17 ? 256 : 320;
    int slots = BITNSLOTS(oreSize * radius * oreSize);
    char bitSet[slots];
    memset(bitSet, 0, slots);
    int size = config.size;
    double store[4 * size];

    for (int i = 0; i < size; ++i) {
        float percent = (float)i / (float)size;
        double x = lerp(percent, offsetXPos, offsetXNeg);
        double y = lerp(percent, offsetYPos, offsetYNeg);
        double z = lerp(percent, offsetZPos, offsetZNeg);
        double length = rnd.nextDouble(rnd.state) * (double)size / 16.0;
        double offset = ((sin((float)PI * percent) + 1.0F) * length + 1.0) / 2.0;
        store[i * 4] = x;
        store[i * 4 + 1] = y;
        store[i * 4 + 2] = z;
        store[i * 4 + 3] = offset;
    }

    for (int i = 0; i < size - 1; ++i) {
        if (store[i * 4 + 3] <= 0.0) continue;
        for (int j = i + 1; j < size; ++j) {
            if (store[j * 4 + 3] <= 0.0) continue;
            double diffX = store[i * 4] - store[j * 4];
            double diffY = store[i * 4 + 1] - store[j * 4 + 1];
            double diffZ = store[i * 4 + 2] - store[j * 4 + 2];
            double offset = store[i * 4 + 3] - store[j * 4 + 3];
            if (offset * offset <= diffX * diffX + diffY * diffY + diffZ * diffZ) continue;
            if (offset > 0.0) {
                store[j * 4 + 3] = -1.0;
            } else {
                store[i * 4 + 3] = -1.0;
            }
        }
    }

    for (int i = 0; i < size; ++i) {
        const double offset = store[i * 4 + 3];
        if (offset < 0.0) continue;
        const double x = store[i * 4];
        const double y = store[i * 4 + 1];
        const double z = store[i * 4 + 2];

        const int floorMinX = floor(x - offset);
        const int minX = MAX(floorMinX, startX);
        const int floorMinY = floor(y - offset);
        const int minY = MAX(floorMinY, startY);
        const int floorMinZ = floor(z - offset);
        const int minZ = MAX(floorMinZ, startZ);

        const int floorMaxX = floor(x + offset);
        const int maxX = MAX(floorMaxX, minX);
        const int floorMaxY = floor(y + offset);
        const int maxY = MAX(floorMaxY, minY);
        const int floorMaxZ = floor(z + offset);
        const int maxZ = MAX(floorMaxZ, minZ);

        for (int X = minX; X <= maxX; ++X) {
            double xSlide = ((double)X + 0.5 - x) / offset;
            if (xSlide * xSlide >= 1.0) continue;
            for (int Y = minY; Y <= maxY; ++Y) {
                double ySlide = ((double)Y + 0.5 - y) / offset;
                if (xSlide * xSlide + ySlide * ySlide >= 1.0) continue;
                for (int Z = minZ; Z <= maxZ; ++Z) {
                    double zSlide = ((double)Z + 0.5 - z) / offset;
                    if (xSlide * xSlide + ySlide * ySlide + zSlide * zSlide >= 1.0) continue;
                    if (Y < minBuildHeight || Y >= maxBuildHeight) continue;
                    int area = X - startX + (Y - startY) * oreSize + (Z - startZ) * oreSize * radius;
                    if (BITTEST(bitSet, area)) continue;
                    BITSET(bitSet, area);
                    Pos3 pos = {X, Y, Z};
                    // TODO: check if the block at pos is contained in config.replaceBlocks
                    if (1) {
                        float chance = config.discardChanceOnAirExposure;
                        int skipAirCheck;
                        if (chance <= 0.0F) {
                            skipAirCheck = 1;
                        } else {
                            skipAirCheck = chance >= 1.0F ? 0 : rnd.nextFloat(rnd.state) >= chance;
                        }
                        if (skipAirCheck) {
                            appendPos3List(pos3s, pos);
                        } else if (1) { // TODO: check if the block at pos is not air-exposed
                            appendPos3List(pos3s, pos);
                        }
                    }
                }
            }
        }
    }
}

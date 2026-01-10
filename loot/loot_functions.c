#include "loot_functions.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "../biomes.h"
#include "../rng.h"

// ----------------------------------------------------------------------------------------
// the actual loot functions

static int is_applicable(const Enchantment enchantment, const ItemType item, const int use_overrides);

static void set_count_uniform_function(uint64_t* rand, ItemStack* is, const void* params)
{
    const int* params_int = (const int*)params;
    const int bound = params_int[1] - params_int[0] + 1;
    const int cnt = nextInt(rand, bound) + params_int[0];
    is->count = cnt;
}

static void set_count_constant_function(uint64_t* rand, ItemStack* is, const void* params)
{
    const int* params_int = (const int*)params;
    is->count = params_int[0];
}

static void set_effect_function(uint64_t* rand, ItemStack* is, const void* params)
{
    skipNextN(rand, 2); // effect type, effect duration
}

static void skip_n_calls_function(uint64_t* rand, ItemStack* is, const void* params)
{
    const int* params_int = (const int*)params;
    skipNextN(rand, params_int[0]);
}

static void skip_one_call_function(uint64_t* rand, ItemStack* is, const void* params)
{
    skipNextN(rand, 1);
}

static void no_op_function(uint64_t* rand, ItemStack* is, const void* params)
{
    // do nothing
}

// enchantments

static void set_enchantment_random_level_function(uint64_t* rand, ItemStack* is, const void* params)
{
    const int* params_int = (const int*)params;
    // params[0] - enchantment id
    // params[1] - max level (min level always 1)

    is->enchantment_count = 1;
    is->enchantments[0].enchantment = params_int[0];

    nextInt(rand, 1); // choose a "random" enchantment, nextInt(1) call
    const int bound = params_int[1];
    is->enchantments[0].level = nextInt(rand, bound) + 1;
}

static void enchant_randomly_function(uint64_t* rand, ItemStack* is, const void* params)
{
    const int* varparams_int = (const int*)params;
    // params[0] - number of enchantments
    // params[2k + 1] - enchantment id
    // params[2k + 2] - nextInt bound for the enchantment level choice

    int numEnchants = varparams_int[0];

    is->enchantment_count = 1;

    int enchantmentID = nextInt(rand, numEnchants);
    is->enchantments[0].enchantment = varparams_int[2 * enchantmentID + 1];

    int maxLevel = varparams_int[2 * enchantmentID + 2];
    is->enchantments[0].level = nextInt(rand, maxLevel) + 1;
}

// enchant with levels helpers

static inline int java_round_positive(float f)
{
    // this should be good enough to emulate Math.round
    return (int)floor(f + 0.5F);
}

static inline int choose_enchantment(uint64_t* rand, int enchantmentVec[], const int vecSize, const int totalWeight)
{
    const int vecCapacity = vecSize * 3;
    int w = nextInt(rand, totalWeight);
    for (int i = 2; i < vecCapacity; i += 3)
    {
        w -= enchantmentVec[i];
        if (w < 0)
            return i - 2;
    }
    return vecCapacity - 3;
}

static int IS_INCOMPATIBLE_ENCHANT[64][64];
static int INCOMPATIBLE_INITIALIZED = 0;
static void fill_incompatible_enchantments()
{
    if (INCOMPATIBLE_INITIALIZED) return;
    INCOMPATIBLE_INITIALIZED = 1;

    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            IS_INCOMPATIBLE_ENCHANT[i][j] = i == j;
        }
    }

    int protections[] = { PROTECTION, FIRE_PROTECTION, BLAST_PROTECTION, PROJECTILE_PROTECTION };
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            IS_INCOMPATIBLE_ENCHANT[protections[i]][protections[j]] = 1;

    int sharpnesses[] = { SHARPNESS, SMITE, BANE_OF_ARTHROPODS };
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            IS_INCOMPATIBLE_ENCHANT[sharpnesses[i]][sharpnesses[j]] = 1;

    int fortunes[] = { FORTUNE, LUCK_OF_THE_SEA, LOOTING };
    for (int i = 0; i < 3; i++)
    {
        IS_INCOMPATIBLE_ENCHANT[fortunes[i]][SILK_TOUCH] = 1;
        IS_INCOMPATIBLE_ENCHANT[SILK_TOUCH][fortunes[i]] = 1;
    }

    IS_INCOMPATIBLE_ENCHANT[DEPTH_STRIDER][FROST_WALKER] = 1;
    IS_INCOMPATIBLE_ENCHANT[FROST_WALKER][DEPTH_STRIDER] = 1;

    IS_INCOMPATIBLE_ENCHANT[MENDING][INFINITY_ENCHANTMENT] = 1;
    IS_INCOMPATIBLE_ENCHANT[INFINITY_ENCHANTMENT][MENDING] = 1;

    IS_INCOMPATIBLE_ENCHANT[RIPTIDE][CHANNELING] = 1;
    IS_INCOMPATIBLE_ENCHANT[CHANNELING][RIPTIDE] = 1;
    IS_INCOMPATIBLE_ENCHANT[RIPTIDE][LOYALTY] = 1;
    IS_INCOMPATIBLE_ENCHANT[LOYALTY][RIPTIDE] = 1;

    IS_INCOMPATIBLE_ENCHANT[PIERCING][MULTISHOT] = 1;
    IS_INCOMPATIBLE_ENCHANT[MULTISHOT][PIERCING] = 1;

    IS_INCOMPATIBLE_ENCHANT[DENSITY][BREACH] = 1;
    IS_INCOMPATIBLE_ENCHANT[BREACH][DENSITY] = 1;
}

static inline void remove_incompatible_enchantments(int enchantmentIndex, int enchantVec[], int* vecSize, int* totalWeight)
{
    int enchantmentID = enchantVec[enchantmentIndex];

    int i = 0;
    int moveBack = 0;

    while (i < *vecSize)
    {
        const int ix = 3 * i;
        const int incompatible = IS_INCOMPATIBLE_ENCHANT[enchantmentID][enchantVec[ix]];

        if (moveBack > 0 && !incompatible)
            memcpy(enchantVec + ix - moveBack, enchantVec + ix, sizeof(int) * 3);

        if (incompatible)
        {
            *totalWeight -= enchantVec[ix + 2]; // subtract the weight of removed enchantment
            moveBack += 3; // make it so that all the following elements are moved back
        }

        i++;
    }

    *vecSize -= (moveBack / 3); // shrink the vector
}

static void enchant_with_levels_function(uint64_t* rand, ItemStack* is, const void* params)
{
    const int** varparams_int_arr = (const int**)params;
    // params[0][0] - item enchantability
    // params[0][1] - min levels
    // params[0][2] - max levels
    //
    // params[i][0]    - number of applicable enchantment instances
    // params[i][1] - total weight of the enchantment instances
    // params[i][3k + 2] - enchantment id
    // params[i][3k + 3] - enchantment level
    // params[i][3k + 4] - enchantment weight

    const int enchantability = varparams_int_arr[0][0];
    const int minLevel = varparams_int_arr[0][1];
    const int maxLevel = varparams_int_arr[0][2];

    // calculate effective level
    int level = minLevel;
    if (minLevel != maxLevel)
        level += nextInt(rand, maxLevel - minLevel + 1);

    const int delta = enchantability / 4 + 1;
    level += 1 + nextInt(rand, delta) + nextInt(rand, delta);
    const float amplifier = (nextFloat(rand) + nextFloat(rand) - 1.0F) * 0.15F;
    level = java_round_positive((float)level + (float)level * amplifier);

    // copy the available enchantment results to a local array
    is->enchantment_count = 0;
    const int enchant_vec_index = level + 1;
    int vecSize = varparams_int_arr[enchant_vec_index][0];     // there was a nasty bug here:
    int totalWeight = varparams_int_arr[enchant_vec_index][1]; // don't forget about the first vector!!!
    if (vecSize == 0) return; // no enchantments available

    int enchantmentVec[128]; // holds triples (id, level, weight), max 42 * 3 = 126 elements
    memcpy(enchantmentVec, varparams_int_arr[enchant_vec_index] + 2, sizeof(int) * vecSize * 3);

    int index = choose_enchantment(rand, enchantmentVec, vecSize, totalWeight);
    is->enchantments[0].enchantment = enchantmentVec[index];
    is->enchantments[0].level = enchantmentVec[index + 1];
    is->enchantment_count++;

    while (nextInt(rand, 50) <= level)
    {
        remove_incompatible_enchantments(index, enchantmentVec, &vecSize, &totalWeight);
        if (vecSize == 0) break;

        index = choose_enchantment(rand, enchantmentVec, vecSize, totalWeight);
        is->enchantments[is->enchantment_count].enchantment = enchantmentVec[index];
        is->enchantments[is->enchantment_count].level = enchantmentVec[index + 1];
        is->enchantment_count++;

        level /= 2;
    }
}

// ----------------------------------------------------------------------------------------
// function creators

static void init_function(LootFunction* lf)
{
    lf->params = NULL;
    lf->varparams_int = NULL;
    lf->varparams_int_arr = NULL;
    lf->varparams_int_arr_size = 0;
}

void create_set_count(LootFunction* lf, const int min, const int max)
{
    init_function(lf);
    lf->params = lf->params_int;
    lf->params_int[0] = min;
    lf->params_int[1] = max;

    if (min == max)
    {
        lf->fun = set_count_constant_function;
    }
    else
    {
        lf->fun = set_count_uniform_function;
    }
}

void create_set_effect(LootFunction* lf)
{
    init_function(lf);
    lf->params = lf->params_int;
    lf->fun = set_effect_function;
}

void create_set_damage(LootFunction* lf)
{
    // item damage is not that important, this should suffice
    create_skip_calls(lf, 1);
}

void create_skip_calls(LootFunction* lf, const int skip_count)
{
    init_function(lf);
    lf->params = lf->params_int;
    lf->params_int[0] = skip_count;

    if (skip_count == 1)
    {
        lf->fun = skip_one_call_function;
    }
    else
    {
        lf->fun = skip_n_calls_function;
    }
}

void create_no_op(LootFunction* lf)
{
    init_function(lf);
    lf->fun = no_op_function;
}

// ----------------------------------------------------------------------------------------
//  Enchantment functions

static int is_applicable(const Enchantment enchantment, const ItemType item, const int use_overrides)
{
    if (enchantment == NO_ENCHANTMENT) return 0;
    if (item == BOOK) return 1; // the wildcard

    switch (enchantment)
    {
    case LUNGE:
        return item == SPEAR;

    case CURSE_OF_VANISHING:
    case UNBREAKING:
    case MENDING:
        return 1;

    case THORNS:
        return item == CHESTPLATE || (use_overrides == 1 && (item == LEGGINGS || item == BOOTS || item == HELMET));
    case CURSE_OF_BINDING:
    case PROTECTION:
    case FIRE_PROTECTION:
    case BLAST_PROTECTION:
    case PROJECTILE_PROTECTION:
        return item == CHESTPLATE || item == LEGGINGS || item == BOOTS || item == HELMET;

    case RESPIRATION:
    case AQUA_AFFINITY:
        return item == HELMET;

    case FEATHER_FALLING:
    case DEPTH_STRIDER:
    case FROST_WALKER:
    case SOUL_SPEED:
        return item == BOOTS;

    case SWIFT_SNEAK:
        return item == LEGGINGS;

    case SHARPNESS:
    case SMITE:
    case BANE_OF_ARTHROPODS:
        return item == SWORD || item == SPEAR || (use_overrides == 1 && item == AXE);
    case KNOCKBACK:
    case FIRE_ASPECT:
    case LOOTING:
        return item == SWORD || item == SPEAR;
    case SWEEPING_EDGE:
        return item == SWORD;

    case EFFICIENCY:
    case SILK_TOUCH:
    case FORTUNE:
        return item == PICKAXE || item == SHOVEL || item == AXE || item == HOE;

    case POWER:
    case PUNCH:
    case FLAME:
    case INFINITY_ENCHANTMENT:
        return item == BOW;

    case MULTISHOT:
    case QUICK_CHARGE:
    case PIERCING:
        return item == CROSSBOW;

    case LUCK_OF_THE_SEA:
    case LURE:
        return item == FISHING_ROD;

    case IMPALING:
    case RIPTIDE:
    case LOYALTY:
    case CHANNELING:
        return item == TRIDENT;

    case DENSITY:
    case BREACH:
    case WIND_BURST:
        return item == MACE;
    }

    return 0;
}

static int test_effective_level(const Enchantment enchantment, const int i, const int n)
{
    switch (enchantment)
    {
    case PROTECTION:
        if ((n < 1 + (i - 1) * 11) || (n > 1 + (i - 1) * 11 + 11))
            return 0;
        break;
    case FIRE_PROTECTION:
        if ((n < 10 + (i - 1) * 8) || (n > 10 + (i - 1) * 8 + 8))
            return 0;
        break;
    case FEATHER_FALLING:
        if ((n < 5 + (i - 1) * 6) || (n > 5 + (i - 1) * 6 + 6))
            return 0;
        break;
    case BLAST_PROTECTION:
        if ((n < 5 + (i - 1) * 8) || (n > 5 + (i - 1) * 8 + 8))
            return 0;
        break;
    case PROJECTILE_PROTECTION:
        if ((n < 3 + (i - 1) * 6) || (n > 3 + (i - 1) * 6 + 6))
            return 0;
        break;
    case RESPIRATION:
        if ((n < 10 * i) || (n > 10 * i + 30))
            return 0;
        break;
    case AQUA_AFFINITY:
        if ((n < 1) || (n > 41))
            return 0;
        break;
    case THORNS:
        if ((n < 10 + (20 * (i - 1))) || (n > 10 + (20 * (i - 1)) + 50))
            return 0;
        break;
    case DEPTH_STRIDER:
        if ((n < i * 10) || (n > i * 10 + 15))
            return 0;
        break;
    case FROST_WALKER:
        if ((n < i * 10) || (n > i * 10 + 15))
            return 0;
        break;
    case CURSE_OF_BINDING:
        if ((n < 25) || (n > 50))
            return 0;
        break;
    case SOUL_SPEED:
        if ((n < i * 10) || (n > i * 10 + 15))
            return 0;
        break;
    case SHARPNESS:
        if ((n < 1 + (i - 1) * 11) || (n > 1 + (i - 1) * 11 + 20))
            return 0;
        break;
    case SMITE:
        if ((n < 5 + (i - 1) * 8) || (n > 5 + (i - 1) * 8 + 20))
            return 0;
        break;
    case BANE_OF_ARTHROPODS:
        if ((n < 5 + (i - 1) * 8) || (n > 5 + (i - 1) * 8 + 20))
            return 0;
        break;
    case KNOCKBACK:
        if ((n < 5 + 20 * (i - 1)) || (n > 1 + (i * 10) + 50))
            return 0;
        break;
    case FIRE_ASPECT:
        if ((n < 10 + 20 * (i - 1)) || (n > 1 + (i * 10) + 50))
            return 0;
        break;
    case LOOTING:
        if ((n < 15 + (i - 1) * 9) || (n > 1 + (i * 10) + 50))
            return 0;
        break;
    case SWEEPING_EDGE:
        if ((n < 5 + (i - 1) * 9) || (n > 5 + (i - 1) * 9 + 15))
            return 0;
        break;
    case EFFICIENCY:
        if ((n < (1 + 10 * (i - 1))) || (n > 1 + (i * 10) + 50))
            return 0;
        break;
    case SILK_TOUCH:
        if ((n < 15) || (n > 1 + (i * 10) + 50))
            return 0;
        break;
    case UNBREAKING:
        if ((n < 5 + (i - 1) * 8) || (n > 1 + (i * 10) + 50))
            return 0;
        break;
    case FORTUNE:
        if ((n < 15 + (i - 1) * 9) || (n > 1 + (i * 10) + 50))
            return 0;
        break;
    case POWER:
        if ((n < 1 + (i - 1) * 10) || (n > 1 + (i - 1) * 10 + 15))
            return 0;
        break;
    case PUNCH:
        if ((n < 12 + (i - 1) * 20) || (n > 12 + (i - 1) * 20 + 25))
            return 0;
        break;
    case FLAME:
        if ((n < 20) || (n > 50))
            return 0;
        break;
    case INFINITY_ENCHANTMENT:
        if ((n < 20) || (n > 50))
            return 0;
        break;
    case LUCK_OF_THE_SEA:
    case LURE:
        if ((n < 15 + (i - 1) * 9) || (n > 1 + (i * 10) + 50))
            return 0;
        break;
    case LOYALTY:
        if ((n < 5 + (i * 7)) || (n > 50))
            return 0;
        break;
    case IMPALING:
        if ((n < 1 + (i - 1) * 8) || (n > 1 + (i - 1) * 8 + 20))
            return 0;
        break;
    case RIPTIDE:
        if ((n < 10 + (i * 7)) || (n > 50))
            return 0;
        break;
    case CHANNELING:
        if ((n < 25) || (n > 50))
            return 0;
        break;
    case MULTISHOT:
        if ((n < 20) || (n > 50))
            return 0;
        break;
    case QUICK_CHARGE:
        if ((n < 12 + (i - 1) * 20) || (n > 50))
            return 0;
        break;
    case PIERCING:
        if ((n < 1 + (i - 1) * 10) || (n > 50))
            return 0;
        break;
    case MENDING:
        if ((n < i * 25) || (n > i * 25 + 50))
            return 0;
        break;
    case CURSE_OF_VANISHING:
        if ((n < 25) || (n > 50))
            return 0;
        break;
    case DENSITY:
    case LUNGE:
        if ((n < 5 + (i - 1) * 8) || (n > 25 + (i - 1) * 8))
            return 0;
        break;
    case BREACH:
    case WIND_BURST:
        if ((n < 15 + (i - 1) * 9) || (n > 65 + (i - 1) * 9))
            return 0;
        break;
    }

    return 1;
}

static int get_weight(const Enchantment enchantment)
{
    switch (enchantment)
    {
    case NO_ENCHANTMENT:
        return 0;

    // common, weight = 10
    case PROTECTION:
    case SHARPNESS:
    case EFFICIENCY:
    case POWER:
    case PIERCING:
        return 10;

    // uncommon, weight = 5
    case FIRE_PROTECTION:
    case FEATHER_FALLING:
    case PROJECTILE_PROTECTION:
    case SMITE:
    case BANE_OF_ARTHROPODS:
    case KNOCKBACK:
    case UNBREAKING:
    case LOYALTY:
    case QUICK_CHARGE:
    case DENSITY:
    case LUNGE:
        return 5;

    // very rare, weight = 1
    case THORNS:
    case CURSE_OF_BINDING:
    case SOUL_SPEED:
    case SILK_TOUCH:
    case INFINITY_ENCHANTMENT:
    case CHANNELING:
    case CURSE_OF_VANISHING:
        return 1;

    // rare, weight = 2
    default:
        return 2;
    }
}

static int get_max_level(const Enchantment enchantment)
{
    static const int MAX_LEVEL[64] = {
        0, // no_enchantment
        4, 4, 4, 4, 3, 1, 3, 3, 4, 3, 2, 3, // armor
        5, 5, 5, 2, 2, 3, 3, // swords
        5, 1, 3, // tools + unbreaking
        3, 3, // fishing rods
        5, 2, 1, 1, // bows
        3, 1, 4, // crossbows
        5, 3, 3, 1, // trident
        5, 4, 3, // mace
        1, 3, 1, 1, // general
        3 // spears
    };

    return MAX_LEVEL[enchantment];
}

static int get_enchantability(const char* item_name)
{
    // I'm truly sorry.
    if (strcmp(item_name, "minecraft:leather_helmet") == 0) return 15;
    if (strcmp(item_name, "minecraft:leather_chestplate") == 0) return 15;
    if (strcmp(item_name, "minecraft:leather_leggings") == 0) return 15;
    if (strcmp(item_name, "minecraft:leather_boots") == 0) return 15;
    if (strcmp(item_name, "minecraft:iron_helmet") == 0) return 9;
    if (strcmp(item_name, "minecraft:iron_chestplate") == 0) return 9;
    if (strcmp(item_name, "minecraft:iron_leggings") == 0) return 9;
    if (strcmp(item_name, "minecraft:iron_boots") == 0) return 9;
    if (strcmp(item_name, "minecraft:golden_helmet") == 0) return 25;
    if (strcmp(item_name, "minecraft:golden_chestplate") == 0) return 25;
    if (strcmp(item_name, "minecraft:golden_leggings") == 0) return 25;
    if (strcmp(item_name, "minecraft:golden_boots") == 0) return 25;
    if (strcmp(item_name, "minecraft:diamond_helmet") == 0) return 10;
    if (strcmp(item_name, "minecraft:diamond_chestplate") == 0) return 10;
    if (strcmp(item_name, "minecraft:diamond_leggings") == 0) return 10;
    if (strcmp(item_name, "minecraft:diamond_boots") == 0) return 10;
    if (strcmp(item_name, "minecraft:fishing_rod") == 0) return 1;
    if (strcmp(item_name, "minecraft:book") == 0) return 1;
    if (strcmp(item_name, "minecraft:iron_pickaxe") == 0) return 14;
    if (strcmp(item_name, "minecraft:iron_axe") == 0) return 14;
    if (strcmp(item_name, "minecraft:iron_hoe") == 0) return 14;
    if (strcmp(item_name, "minecraft:iron_shovel") == 0) return 14;
    if (strcmp(item_name, "minecraft:iron_sword") == 0) return 14;
    if (strcmp(item_name, "minecraft:golden_pickaxe") == 0) return 22;
    if (strcmp(item_name, "minecraft:golden_axe") == 0) return 22;
    if (strcmp(item_name, "minecraft:golden_hoe") == 0) return 22;
    if (strcmp(item_name, "minecraft:golden_shovel") == 0) return 22;
    if (strcmp(item_name, "minecraft:golden_sword") == 0) return 22;
    if (strcmp(item_name, "minecraft:diamond_pickaxe") == 0) return 10;
    if (strcmp(item_name, "minecraft:diamond_axe") == 0) return 10;
    if (strcmp(item_name, "minecraft:diamond_hoe") == 0) return 10;
    if (strcmp(item_name, "minecraft:diamond_shovel") == 0) return 10;
    if (strcmp(item_name, "minecraft:diamond_sword") == 0) return 10;
    if (strcmp(item_name, "minecraft:bow") == 0) return 1;
    return 1;
}

static int get_applicable_enchantments(const ItemType item, const int version, int enchantments[], int use_overrides)
{
    static const int ORDER_V1_13[64] = {
        PROTECTION, FIRE_PROTECTION, FEATHER_FALLING, BLAST_PROTECTION, PROJECTILE_PROTECTION,
        RESPIRATION, AQUA_AFFINITY, THORNS, DEPTH_STRIDER, FROST_WALKER, CURSE_OF_BINDING,
        SHARPNESS, SMITE, BANE_OF_ARTHROPODS, KNOCKBACK, FIRE_ASPECT, LOOTING, SWEEPING_EDGE,
        EFFICIENCY, SILK_TOUCH, UNBREAKING, FORTUNE,
        POWER, PUNCH, FLAME, INFINITY_ENCHANTMENT,
        LUCK_OF_THE_SEA, LURE, LOYALTY, IMPALING, RIPTIDE, CHANNELING,
        MENDING, CURSE_OF_VANISHING, NO_ENCHANTMENT // sentinel
    };
    static const int ORDER_V1_14[64] = {
        PROTECTION, FIRE_PROTECTION, FEATHER_FALLING, BLAST_PROTECTION, PROJECTILE_PROTECTION,
        RESPIRATION, AQUA_AFFINITY, THORNS, DEPTH_STRIDER, FROST_WALKER, CURSE_OF_BINDING,
        SHARPNESS, SMITE, BANE_OF_ARTHROPODS, KNOCKBACK, FIRE_ASPECT, LOOTING, SWEEPING_EDGE,
        EFFICIENCY, SILK_TOUCH, UNBREAKING, FORTUNE,
        POWER, PUNCH, FLAME, INFINITY_ENCHANTMENT,
        LUCK_OF_THE_SEA, LURE, LOYALTY, IMPALING, RIPTIDE, CHANNELING,
        MULTISHOT, QUICK_CHARGE, PIERCING, MENDING, CURSE_OF_VANISHING, NO_ENCHANTMENT // sentinel
    };
    // 1.16 adds soul speed but it's not possible to get it from regular enchanting
    static const int ORDER_V1_21[64] = {
        PROTECTION, FIRE_PROTECTION, FEATHER_FALLING, BLAST_PROTECTION, PROJECTILE_PROTECTION,
        RESPIRATION, AQUA_AFFINITY, THORNS, DEPTH_STRIDER,
        SHARPNESS, SMITE, BANE_OF_ARTHROPODS, KNOCKBACK, FIRE_ASPECT, LOOTING, SWEEPING_EDGE,
        EFFICIENCY, SILK_TOUCH, UNBREAKING, FORTUNE,
        POWER, PUNCH, FLAME, INFINITY_ENCHANTMENT,
        LUCK_OF_THE_SEA, LURE, LOYALTY, IMPALING, RIPTIDE, CHANNELING,
        MULTISHOT, QUICK_CHARGE, PIERCING, DENSITY, BREACH,
        CURSE_OF_BINDING, CURSE_OF_VANISHING, FROST_WALKER, MENDING, NO_ENCHANTMENT // sentinel
    };
    static const int ORDER_V1_21_11[64] = {
        PROTECTION, FIRE_PROTECTION, FEATHER_FALLING, BLAST_PROTECTION, PROJECTILE_PROTECTION,
        RESPIRATION, AQUA_AFFINITY, THORNS, DEPTH_STRIDER,
        SHARPNESS, SMITE, BANE_OF_ARTHROPODS, KNOCKBACK, FIRE_ASPECT, LOOTING, SWEEPING_EDGE,
        EFFICIENCY, SILK_TOUCH, UNBREAKING, FORTUNE,
        POWER, PUNCH, FLAME, INFINITY_ENCHANTMENT,
        LUCK_OF_THE_SEA, LURE, LOYALTY, IMPALING, RIPTIDE, CHANNELING,
        MULTISHOT, QUICK_CHARGE, PIERCING, DENSITY, BREACH, LUNGE,
        CURSE_OF_BINDING, CURSE_OF_VANISHING, FROST_WALKER, MENDING, NO_ENCHANTMENT // sentinel
    };

    const int* order = ORDER_V1_13;
    if (version > MC_1_13) order = ORDER_V1_14;
    if (version > MC_1_20) order = ORDER_V1_21;
    if (version >= MC_1_21_11) order = ORDER_V1_21_11;

    int i = 0, j = 0;
    while (order[i] != NO_ENCHANTMENT)
    {
        if (is_applicable(order[i], item, use_overrides))
        {
            if (enchantments != NULL)
                enchantments[j] = order[i];
            j++;
        }
        i++;
    }

    return j; // return the number of applicable enchantments
}

static int is_treasure_enchantment(const Enchantment ench)
{
    switch (ench)
    {
    case MENDING:
    case FROST_WALKER:
    case CURSE_OF_BINDING:
    case CURSE_OF_VANISHING:
    case SOUL_SPEED:
    case SWIFT_SNEAK:
    case WIND_BURST:
        return 1;
    default:
        return 0;
    }
}

static int is_tag_match(const char *tag, const char *fullName)
{
    if (!tag) return 0;
    if (strcmp(tag, fullName) == 0) return 1;
    if (tag[0] == '#' && strcmp(tag + 1, fullName) == 0) return 1;
    return 0;
}

static int get_non_treasure_1_21(Enchantment out[], const int cap, const ItemType item, const int version)
{
    // Members of vanilla tag '#minecraft:non_treasure' (1.21.x), in tag file order.
    static const Enchantment non_treasure_pre_1_21_11[] = {
        PROTECTION,
        FIRE_PROTECTION,
        FEATHER_FALLING,
        BLAST_PROTECTION,
        PROJECTILE_PROTECTION,
        RESPIRATION,
        AQUA_AFFINITY,
        THORNS,
        DEPTH_STRIDER,
        SHARPNESS,
        SMITE,
        BANE_OF_ARTHROPODS,
        KNOCKBACK,
        FIRE_ASPECT,
        LOOTING,
        SWEEPING_EDGE,
        EFFICIENCY,
        SILK_TOUCH,
        UNBREAKING,
        FORTUNE,
        POWER,
        PUNCH,
        FLAME,
        INFINITY_ENCHANTMENT,
        LUCK_OF_THE_SEA,
        LURE,
        LOYALTY,
        IMPALING,
        RIPTIDE,
        CHANNELING,
        MULTISHOT,
        QUICK_CHARGE,
        PIERCING,
        DENSITY,
        BREACH,
    };
    static const Enchantment non_treasure_1_21_11[] = {
        PROTECTION,
        FIRE_PROTECTION,
        FEATHER_FALLING,
        BLAST_PROTECTION,
        PROJECTILE_PROTECTION,
        RESPIRATION,
        AQUA_AFFINITY,
        THORNS,
        DEPTH_STRIDER,
        SHARPNESS,
        SMITE,
        BANE_OF_ARTHROPODS,
        KNOCKBACK,
        FIRE_ASPECT,
        LOOTING,
        SWEEPING_EDGE,
        EFFICIENCY,
        SILK_TOUCH,
        UNBREAKING,
        FORTUNE,
        POWER,
        PUNCH,
        FLAME,
        INFINITY_ENCHANTMENT,
        LUCK_OF_THE_SEA,
        LURE,
        LOYALTY,
        IMPALING,
        RIPTIDE,
        CHANNELING,
        MULTISHOT,
        QUICK_CHARGE,
        PIERCING,
        DENSITY,
        BREACH,
        LUNGE,
    };

    const Enchantment* non_treasure = version >= MC_1_21_11 ? non_treasure_1_21_11 : non_treasure_pre_1_21_11;
    const size_t non_treasure_len = version >= MC_1_21_11
        ? sizeof(non_treasure_1_21_11) / sizeof(non_treasure_1_21_11[0])
        : sizeof(non_treasure_pre_1_21_11) / sizeof(non_treasure_pre_1_21_11[0]);

    int n = 0;
    for (size_t i = 0; i < non_treasure_len; i++)
    {
        const Enchantment ench = non_treasure[i];
        if (!is_applicable(ench, item, 1))
            continue;
        if (n < cap)
            out[n] = ench;
        n++;
    }
    return n <= cap ? n : cap;
}

static int get_on_random_loot_1_21(Enchantment out[], const int cap, const ItemType item, const int version)
{
    // From vanilla tag expansion (1.21.x client jar):
    //   on_random_loot = #non_treasure + binding_curse + vanishing_curse + frost_walker + mending
    static const Enchantment tail[] = {
        CURSE_OF_BINDING,
        CURSE_OF_VANISHING,
        FROST_WALKER,
        MENDING,
    };

    int n = get_non_treasure_1_21(out, cap, item, version);
    for (size_t i = 0; i < sizeof(tail) / sizeof(tail[0]); i++)
    {
        const Enchantment ench = tail[i];
        if (!is_applicable(ench, item, 1))
            continue;
        if (n < cap)
            out[n] = ench;
        n++;
    }
    return n <= cap ? n : cap;
}

static int get_enchant_level_vector(const int level, const int applicable[], const int num_applicable, int* vec)
{
    int vecSize = 0;
    int totalWeight = 0;

    for (int i = 0; i < num_applicable; i++)
    {
        const int enchantment = applicable[i];
        const int max_level = get_max_level(enchantment);

        for (int ench_level = max_level; ench_level >= 1; ench_level--)
        {
            if (test_effective_level(enchantment, ench_level, level) == 0)
                continue;

            if (vec != NULL)
            {
                const int w = get_weight(enchantment);
                vec[3 * vecSize + 2] = enchantment;
                vec[3 * vecSize + 3] = ench_level;
                vec[3 * vecSize + 4] = w;
                totalWeight += w;
            }

            vecSize++;
            break;
        }
    }

    if (vec == NULL)
        return vecSize;

    vec[0] = vecSize;
    vec[1] = totalWeight;
    return vecSize;
}

//  Enchantment function creators

void create_enchant_randomly_one_enchant(LootFunction* lf, const Enchantment enchantment)
{
    init_function(lf);
    lf->params = lf->params_int;
    lf->params_int[0] = enchantment;
    lf->params_int[1] = get_max_level(enchantment);

    lf->fun = set_enchantment_random_level_function;
}

void create_enchant_randomly_list(LootFunction* lf, const Enchantment* list, const int list_length)
{
    init_function(lf);
    lf->varparams_int = (int*)malloc((2 * list_length + 1) * sizeof(int));
    lf->params = lf->varparams_int;
    lf->varparams_int[0] = list_length;

    for (int i = 0; i < list_length; i++)
    {
        lf->varparams_int[1 + 2 * i] = list[i];
        lf->varparams_int[1 + 2 * i + 1] = get_max_level(list[i]);
    }

    lf->fun = enchant_randomly_function;
}

void create_enchant_randomly(LootFunction* lf, const int version, const ItemType item, const int isTreasure)
{
    int enchantCount = get_applicable_enchantments(item, version, NULL, 1);

    init_function(lf);
    // Allocate for the worst-case; we'll shrink enchantCount after filtering.
    lf->varparams_int = (int*)malloc((2 * enchantCount + 1) * sizeof(int));
    lf->params = lf->varparams_int;

    int applicable[64];
    get_applicable_enchantments(item, version, applicable, 1);

    // copy applicable enchants, along with their max levels
    int out = 0;
    for (int i = 0; i < enchantCount; i++)
    {
        const int ench = applicable[i];
        if (!isTreasure && is_treasure_enchantment((Enchantment)ench))
            continue;
        lf->varparams_int[1 + 2*out] = ench;
        lf->varparams_int[1 + 2*out + 1] = get_max_level(ench);
        out++;
    }

    lf->varparams_int[0] = out;

    lf->fun = enchant_randomly_function;
}

void create_enchant_randomly_tag(LootFunction* lf, const int version, const ItemType item, const char* tag, const int allowTreasure)
{
    Enchantment list[64];
    int n = 0;

    if (is_tag_match(tag, "minecraft:on_random_loot"))
    {
        n = get_on_random_loot_1_21(list, (int)(sizeof(list) / sizeof(list[0])), item, version);
    }
    else if (is_tag_match(tag, "minecraft:non_treasure") || is_tag_match(tag, "minecraft:in_enchanting_table"))
    {
        (void)allowTreasure;
        n = get_non_treasure_1_21(list, (int)(sizeof(list) / sizeof(list[0])), item, version);
    }

    if (n <= 0)
    {
        create_enchant_randomly(lf, version, item, allowTreasure);
        return;
    }

    create_enchant_randomly_list(lf, list, n);
}

void create_enchant_with_levels(LootFunction* lf, const int version, const char* item_name, const ItemType item_type, const int min_level, const int max_level, const int isTreasure)
{
    fill_incompatible_enchantments(); // only need this if we're using the enchant_with_levels function

    // need 2*maxLevel vectors for enchantment instances
    // and a single vector for the initial parameters

    init_function(lf);
    lf->varparams_int_arr = (int**)malloc((2 * max_level + 1) * sizeof(int*));
    lf->varparams_int_arr_size = 2 * max_level + 1;
    lf->params = (void*)lf->varparams_int_arr;

    // basic data vector
    lf->varparams_int_arr[0] = (int*)malloc(3 * sizeof(int));
    lf->varparams_int_arr[0][0] = get_enchantability(item_name);
    lf->varparams_int_arr[0][1] = min_level;
    lf->varparams_int_arr[0][2] = max_level;

    int applicable[64];
    int num_applicable = get_applicable_enchantments(item_type, version, applicable, 0);
    if (!isTreasure)
    {
        int out = 0;
        for (int i = 0; i < num_applicable; i++)
        {
            if (is_treasure_enchantment((Enchantment)applicable[i]))
                continue;
            applicable[out++] = applicable[i];
        }
        num_applicable = out;
    }

    // fill the enchantment instance vector array
    for (int level = 0; level < 2 * max_level; level++)
    {
        // create a vector for the current level
        int vector_size = get_enchant_level_vector(level, applicable, num_applicable, NULL);
        int* vec = malloc((3 * vector_size + 2) * sizeof(int));
        get_enchant_level_vector(level, applicable, num_applicable, vec);
        lf->varparams_int_arr[level+1] = vec;
    }

    lf->fun = enchant_with_levels_function;
}

void create_enchant_with_levels_tag(LootFunction* lf, const int version, const char* item_name, const ItemType item_type, const int min_level, const int max_level, const char* tag, const int allowTreasure)
{
    fill_incompatible_enchantments();

    init_function(lf);
    lf->varparams_int_arr = (int**)malloc((2 * max_level + 1) * sizeof(int*));
    lf->varparams_int_arr_size = 2 * max_level + 1;
    lf->params = (void*)lf->varparams_int_arr;

    lf->varparams_int_arr[0] = (int*)malloc(3 * sizeof(int));
    lf->varparams_int_arr[0][0] = get_enchantability(item_name);
    lf->varparams_int_arr[0][1] = min_level;
    lf->varparams_int_arr[0][2] = max_level;

    int applicable[64];
    int num_applicable = 0;

    if (is_tag_match(tag, "minecraft:on_random_loot"))
    {
        num_applicable = get_on_random_loot_1_21((Enchantment*)applicable, 64, item_type, version);
    }
    else if (is_tag_match(tag, "minecraft:non_treasure") || is_tag_match(tag, "minecraft:in_enchanting_table"))
    {
        num_applicable = get_non_treasure_1_21((Enchantment*)applicable, 64, item_type, version);
    }
    else
    {
        create_enchant_with_levels(lf, version, item_name, item_type, min_level, max_level, allowTreasure);
        return;
    }

    if (!allowTreasure)
    {
        int out = 0;
        for (int i = 0; i < num_applicable; i++)
        {
            if (is_treasure_enchantment((Enchantment)applicable[i]))
                continue;
            applicable[out++] = applicable[i];
        }
        num_applicable = out;
    }

    for (int level = 0; level < 2 * max_level; level++)
    {
        int vector_size = get_enchant_level_vector(level, applicable, num_applicable, NULL);
        int* vec = malloc((3 * vector_size + 2) * sizeof(int));
        get_enchant_level_vector(level, applicable, num_applicable, vec);
        lf->varparams_int_arr[level + 1] = vec;
    }

    lf->fun = enchant_with_levels_function;
}

// ----------------------------------------------------------------------------------------
// Extra utilities

static const char* ENCHANT_NAMES[64] = {
    "no_enchantment",

    "protection",
    "fire_protection",
    "blast_protection",
    "projectile_protection",
    "respiration",
    "aqua_affinity",
    "thorns",
    "swift_sneak",
    "feather_falling",
    "depth_strider",
    "frost_walker",
    "soul_speed",

    "sharpness",
    "smite",
    "bane_of_arthropods",
    "knockback",
    "fire_aspect",
    "looting",
    "sweeping_edge",

    "efficiency",
    "silk_touch",
    "fortune",

    "luck_of_the_sea",
    "lure",

    "power",
    "punch",
    "flame",
    "infinity",

    "quick_charge",
    "multishot",
    "piercing",

    "impaling",
    "riptide",
    "loyalty",
    "channeling",

    "density",
    "breach",
    "wind_burst",

    "mending",
    "unbreaking",
    "curse_of_vanishing",
    "curse_of_binding",
    "lunge"
};

const char* get_enchantment_name(const Enchantment enchantment)
{
    return ENCHANT_NAMES[enchantment];
}

// function testing

void test_enchant_vec()
{
    LootFunction lf;
    create_enchant_with_levels(&lf, MC_1_21, "minecraft:iron_leggings", LEGGINGS, 20, 39, 1);

    printf("const int max_enchants[%d] = { ", lf.varparams_int_arr_size - 1);

    for (int i = 1; i < lf.varparams_int_arr_size; i++) {
        int* vec = lf.varparams_int_arr[i];
        int totalWeight = vec[1];

        int enchantmentVec[128];
        int vecSize = vec[0];
        memcpy(enchantmentVec, vec + 2, sizeof(int) * vecSize * 3);

        int numEnchants = 0;
        uint64_t rand = 0;

        while (vecSize > 0)
        {
            remove_incompatible_enchantments(0, enchantmentVec, &vecSize, &totalWeight);
            choose_enchantment(&rand, enchantmentVec, vecSize, totalWeight);
            numEnchants++;
        }

        printf(i != lf.varparams_int_arr_size - 1 ? "%d, " : "%d ", numEnchants);
        //printf("Level %d: %d enchantments\n", i-1, numEnchants);
    }
    printf("};\n");

    // free the allocated memory
    for (int i = 0; i < lf.varparams_int_arr_size; i++)
        free(lf.varparams_int_arr[i]);
    free(lf.varparams_int_arr);

    //int applicable[64];
    //int num_applicable = get_applicable_enchantments(HELMET, MC_1_21, applicable, 0);

    //int vec[128];
    //int size = get_enchant_level_vector(29, applicable, num_applicable, vec);

    //for (int i = 0; i < size; i++)
    //{
    //    printf("%s %d %d\n", get_enchantment_name((Enchantment)vec[3 * i + 2]), vec[3 * i + 3], vec[3 * i + 4]);
    //}
}

void test_enchant_vec_2()
{
    LootFunction lf;
    create_enchant_randomly(&lf, MC_1_21, BOOK, 1);

    printf("const bool extra_skip[%d] = { ", lf.varparams_int[0]);
    for (int i = 0; i < lf.varparams_int[0]; i++)
    {
        int enchantment = lf.varparams_int[1 + 2 * i];
        int max_level = lf.varparams_int[1 + 2 * i + 1];
        printf(max_level > 1 ? "1, " : "0, ");
    }
    printf("};\n");
}

#ifndef LOOT_LIBRARY_H
#define LOOT_LIBRARY_H

#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    ITEM_NONE,
    OBSIDIAN, FLINT, IRON_NUGGET, FLINT_AND_STEEL, FIRE_CHARGE, GOLDEN_APPLE, GOLD_NUGGET, GOLDEN_SWORD, GOLDEN_AXE, GOLDEN_HOE, GOLDEN_SHOVEL, GOLDEN_PICKAXE, GOLDEN_BOOTS, GOLDEN_CHESTPLATE, GOLDEN_HELMET, GOLDEN_LEGGINGS, GLISTERING_MELON_SLICE, GOLDEN_HORSE_ARMOR, LIGHT_WEIGHTED_PRESSURE_PLATE, GOLDEN_CARROT, CLOCK, GOLD_INGOT, BELL, ENCHANTED_GOLDEN_APPLE, GOLD_BLOCK,
    DIAMOND, IRON_INGOT, EMERALD, BONE, SPIDER_EYE, ROTTEN_FLESH, SADDLE, IRON_HORSE_ARMOR, DIAMOND_HORSE_ARMOUR, ENCHANTED_BOOK,
    GUNPOWDER, STRING, SAND
} Item;

static char *item_names[] = {
    "none", "obsidian", "flint", "iron_nugget", "flint_and_steel", "fire_charge", "golden_apple", "gold_nugget", "golden_sword", "golden_axe", "golden_hoe", "golden_shovel", "golden_pickaxe", "golden_boots", "golden_chestplate", "golden_helmet", "golden_leggings", "glistering_melon_slice", "golden_horse_armor", "light_weighted_pressure_plate", "golden_carrot", "clock", "gold_ingot", "bell", "enchanted_golden_apple", "gold_block",
    "DIAMOND", "IRON_INGOT", "EMERALD", "BONE", "SPIDER_EYE", "ROTTEN_FLESH", "SADDLE", "IRON_HORSE_ARMOR", "DIAMOND_HORSE_ARMOUR", "ENCHANTED_BOOK",
    "GUNPOWDER", "STRING", "SAND"
};

typedef enum {
    ENCHANT_NONE, 
    SHARPNESS, SMITE, BANE_OF_ARTHROPODS, KNOCKBACK, FIRE_ASPECT, LOOTING, 
    SWEEPING, UNBREAKING, MENDING, VANISHING_CURSE, EFFICIENCY, SILK_TOUCH, FORTUNE,
    PROTECTION, FIRE_PROTECTION, FEATHER_FALLING, BLAST_PROTECTION, PROJECTILE_PROTECTION,
    THORNS, DEPTH_STRIDER, FROST_WALKER, BINDING_CURSE, RESPIRATION, AQUA_AFFINITY,
    POWER, PUNCH, FLAME, INFINITY, LUCK_OF_THE_SEA, LURE, LOYALTY, IMPALING, RIPTIDE,
    CHANNELING, MULTISHOT, QUICK_CHARGE, PIERCING, 
} Enchant;

static char *enchant_names[] = {
    "none", "sharpness", "smite", "bane_of_arthropods", "knockback", "fire_aspect",
    "looting", "sweeping", "unbreaking", "mending", "vanishing", "efficiency", "silk_touch",
    "fortune", "protection", "fire_protection", "feather_falling", "blast_protection",
    "projectile_protection", "thorns", "depth_strider", "frost_walker", "binding", "respiration", "aqua_affinity",
    "POWER", "PUNCH", "FLAME", "INFINITY", "LUCK_OF_THE_SEA", "LURE", "LOYALTY", "IMPALING", "RIPTIDE",
    "CHANNELING", "MULTISHOT", "QUICK_CHARGE", "PIERCING", 
};

static int standard_sword_enchantments[][3] = {
    {SHARPNESS, 1, 5}, {SMITE, 1, 5}, {BANE_OF_ARTHROPODS, 1, 5}, {KNOCKBACK, 1, 2}, {FIRE_ASPECT, 1, 2}, {LOOTING, 1, 3}, {SWEEPING, 1, 3}, {UNBREAKING, 1, 3}, {MENDING, 1, 1}, {VANISHING_CURSE, 1, 1}
};

static int standard_axe_enchantments[][3] = {
    {SHARPNESS, 1, 5}, {SMITE, 1, 5}, {BANE_OF_ARTHROPODS, 1, 5}, {EFFICIENCY, 1, 5}, {SILK_TOUCH, 1, 1}, {UNBREAKING, 1, 3}, {FORTUNE, 1, 3}, {MENDING, 1, 1}, {VANISHING_CURSE, 1, 1}
};

static int standard_pickaxe_enchantments[][3] = {
    {EFFICIENCY, 1, 5}, {SILK_TOUCH, 1, 1}, {UNBREAKING, 1, 3}, {FORTUNE, 1, 3}, {MENDING, 1, 1}, {VANISHING_CURSE, 1, 1}
};

static int standard_boot_enchantments[][3] = {
    {PROTECTION, 1, 4}, {FIRE_PROTECTION, 1, 4}, {FEATHER_FALLING, 1, 4}, {BLAST_PROTECTION, 1, 4}, {PROJECTILE_PROTECTION, 1, 4}, {THORNS, 1, 3}, {DEPTH_STRIDER, 1, 3}, {FROST_WALKER, 1, 2}, {BINDING_CURSE, 1, 1}, {UNBREAKING, 1, 3}, {MENDING, 1, 1}, {VANISHING_CURSE, 1, 1}
};

static int standard_leggings_enchantments[][3] = {
    {PROTECTION, 1, 4}, {FIRE_PROTECTION, 1, 4}, {BLAST_PROTECTION, 1, 4}, {PROJECTILE_PROTECTION, 1, 4}, {THORNS, 1, 3}, {BINDING_CURSE, 1, 1}, {UNBREAKING, 1, 3}, {VANISHING_CURSE, 1, 1}, {MENDING, 1, 1}
};

static int standard_chestplate_enchantments[][3] = {
    {PROTECTION, 1, 4}, {FIRE_PROTECTION, 1, 4}, {BLAST_PROTECTION, 1, 4}, {PROJECTILE_PROTECTION, 1, 4}, {THORNS, 1, 3}, {BINDING_CURSE, 1, 1}, {UNBREAKING, 1, 3}, {MENDING, 1, 1}, {VANISHING_CURSE, 1, 1}
};

static int standard_helmet_enchantments[][3] = {
    {PROTECTION, 1, 4}, {FIRE_PROTECTION, 1, 4}, {BLAST_PROTECTION, 1, 4}, {PROJECTILE_PROTECTION, 1, 4}, {RESPIRATION, 1, 3}, {AQUA_AFFINITY, 1, 1}, {THORNS, 1, 3}, {BINDING_CURSE, 1, 1}, {UNBREAKING, 1, 3}, {MENDING, 1, 1}, {VANISHING_CURSE, 1, 1}
}; 

static int standard_hoe_enchantments[][3] = {
    {EFFICIENCY, 1, 5}, {SILK_TOUCH, 1, 1}, {UNBREAKING, 1, 3}, {FORTUNE, 1, 3}, {MENDING, 1, 1}, {VANISHING_CURSE, 1, 1}
};

static int standard_shovel_enchantments[][3] = {
    {EFFICIENCY, 1, 5}, {SILK_TOUCH, 1, 1}, {UNBREAKING, 1, 3}, {FORTUNE, 1, 3}, {MENDING, 1, 1}, {VANISHING_CURSE, 1, 1}
};

static int standard_enchanted_book_enchantments[][3] = {
    {PROTECTION, 1, 4}, {FIRE_PROTECTION, 1, 4}, {FEATHER_FALLING, 1, 4},
    {BLAST_PROTECTION, 1, 4}, {PROJECTILE_PROTECTION, 1, 4}, {RESPIRATION, 1, 3},
    {AQUA_AFFINITY, 1, 1}, {THORNS, 1, 3}, {DEPTH_STRIDER, 1, 3}, {FROST_WALKER, 1, 2},
    {BINDING_CURSE, 1, 1}, {SHARPNESS, 1, 5}, {SMITE, 1, 5}, {BANE_OF_ARTHROPODS, 1, 5},
    {KNOCKBACK, 1, 2}, {FIRE_ASPECT, 1, 2}, {LOOTING, 1, 3}, {SWEEPING, 1, 3},
    {EFFICIENCY, 1, 5}, {SILK_TOUCH, 1, 1}, {UNBREAKING, 1, 3}, {FORTUNE, 1, 3},
    {POWER, 1, 5}, {PUNCH, 1, 2}, {FLAME, 1, 1}, {INFINITY, 1, 1}, {LUCK_OF_THE_SEA, 1, 3},
    {LURE, 1, 3}, {LOYALTY, 1, 3}, {IMPALING, 1, 5}, {RIPTIDE, 1, 3},
    {CHANNELING, 1, 1}, {MULTISHOT, 1, 1}, {QUICK_CHARGE, 1, 3}, {PIERCING, 1, 4},
    {MENDING, 1, 1}, {VANISHING_CURSE, 1, 1}
};

typedef enum {
    NO_ENCHANTS, SWORD_ENCHANTS, AXE_ENCHANTS, PICKAXE_ENCHANTS, HOE_ENCHANTS, BOOTS_ENCHANTS, LEGGINGS_ENCHANTS, CHESTPLATE_ENCHANTS, HELMET_ENCHANTS, SHOVEL_ENCHANTS, ENCHANTED_BOOK_ENCHANTS
} ItemEnchants;

typedef struct {
    // pool index, item index
    size_t current_table;
    int table_counts[4][128][2];
    ItemEnchants enchant_table[4][128];
    Item table[4][128];
} LootTable;

typedef struct {
    Item item; 
    int quantity;

    bool enchanted;
    Enchant enchant;
    int enchant_level;
} LootItem;

#endif

#ifdef LOOT_LIBRARY

#include "rng.c"

LootItem enchanted_loot_item_new(Item item, int quantity, Enchant enchant, int enchant_level) {
    return (LootItem) {
        .item = item,
        .quantity = quantity,
        .enchanted = true, 
        .enchant = enchant,
        .enchant_level = enchant_level
    };
}

LootItem loot_item_new(Item item, int quantity) {
    return (LootItem) {
        .item = item,
        .quantity = quantity,
        .enchanted = false, 
        .enchant = ENCHANT_NONE,
        .enchant_level = -1,
    };
}

LootItem provide_loot_no_function(LootTable *table, int item_index, uint64_t *internal) {
    (void)internal;
    size_t current_table = table->current_table;
    return loot_item_new(table->table[current_table][item_index], 1);
}   

LootItem provide_loot_uniform_roll(LootTable *table, int item_index, uint64_t *internal) {
    size_t current_table = table->current_table;
    int min = table->table_counts[current_table][item_index][0];
    int max = table->table_counts[current_table][item_index][1];
    int amount = next_int_bounded(internal, min, max);
    return loot_item_new(table->table[current_table][item_index], amount);
}

LootItem enchant(LootTable *table, int item_index, uint64_t *internal, int enchant_table[][3], size_t num_enchants) {
    int enchant_num = next_int(internal, num_enchants);
    int *enchant = enchant_table[enchant_num]; 
    
    int level = 1;
    if (enchant[1] != enchant[2]) {
        level = next_int(internal, enchant[2]) + 1; 
    }

    return enchanted_loot_item_new(table->table[table->current_table][item_index], 1, enchant[0], level); 
}

#define LENGTH(x) sizeof(x) / sizeof(x[0])

LootItem provide_loot_random_enchant(LootTable *table, int item_index, uint64_t *internal) {
    size_t current_table = table->current_table;
    ItemEnchants enchants = table->enchant_table[current_table][item_index];
    
    switch (enchants) {
        case NO_ENCHANTS:
            assert(false);
            break;
        case SWORD_ENCHANTS:
            return enchant(table, item_index, internal, standard_sword_enchantments, LENGTH(standard_sword_enchantments));
        case AXE_ENCHANTS:
            return enchant(table, item_index, internal, standard_axe_enchantments, LENGTH(standard_axe_enchantments));
        case PICKAXE_ENCHANTS:
            return enchant(table, item_index, internal, standard_pickaxe_enchantments, LENGTH(standard_pickaxe_enchantments));
        case SHOVEL_ENCHANTS:
            return enchant(table, item_index, internal, standard_shovel_enchantments, LENGTH(standard_shovel_enchantments));
        case HOE_ENCHANTS:
            return enchant(table, item_index, internal, standard_hoe_enchantments, LENGTH(standard_hoe_enchantments));
        case BOOTS_ENCHANTS:
            return enchant(table, item_index, internal, standard_boot_enchantments, LENGTH(standard_boot_enchantments));
        case LEGGINGS_ENCHANTS:
            return enchant(table, item_index, internal, standard_leggings_enchantments, LENGTH(standard_leggings_enchantments));
        case CHESTPLATE_ENCHANTS:
            return enchant(table, item_index, internal, standard_chestplate_enchantments, LENGTH(standard_chestplate_enchantments));
        case HELMET_ENCHANTS:
            return enchant(table, item_index, internal, standard_helmet_enchantments, LENGTH(standard_helmet_enchantments));
       case ENCHANTED_BOOK_ENCHANTS:
            return enchant(table, item_index, internal, standard_enchanted_book_enchantments, LENGTH(standard_enchanted_book_enchantments));
    }

    return enchanted_loot_item_new(table->table[current_table][item_index], 1, -1, -1);
}

#define RUINED_PORTAL
#include "ruined_portal.h"

#define DESERT_TEMPLE
#include "desert_temple.h"

#endif

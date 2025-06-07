#ifndef RUINED_PORTAL_H
#define RUINED_PORTAL_H

static int ruined_portal_loot_table_counts[25][2] = {
    {1, 2}, {1, 4}, {9, 18}, 
    {-1, -1}, {-1, -1}, {-1, -1},
    {4, 24},
    {-1, -1}, {-1, -1}, {-1, -1},
    {-1, -1}, {-1, -1}, {-1, -1},
    {-1, -1}, {-1, -1}, {-1, -1},
    {4, 12},
    {-1, -1}, {-1, -1},
    {4, 12},
    {-1, -1},
    {2, 8},
    {-1, -1}, {-1, -1},
    {1, 2}
};

static Item ruined_portal_loot_table[25] = {
    OBSIDIAN, FLINT, IRON_NUGGET, 
    FLINT_AND_STEEL, FIRE_CHARGE, GOLDEN_APPLE, 
    GOLD_NUGGET, GOLDEN_SWORD, GOLDEN_AXE, 
    GOLDEN_HOE, GOLDEN_SHOVEL, GOLDEN_PICKAXE, 
    GOLDEN_BOOTS, GOLDEN_CHESTPLATE, GOLDEN_HELMET, 
    GOLDEN_LEGGINGS, GLISTERING_MELON_SLICE, GOLDEN_HORSE_ARMOR, 
    LIGHT_WEIGHTED_PRESSURE_PLATE, GOLDEN_CARROT, CLOCK, 
    GOLD_INGOT, BELL, ENCHANTED_GOLDEN_APPLE, 
    GOLD_BLOCK,      
};

static ItemEnchants ruined_portal_enchant_table[25] = {
    NO_ENCHANTS, NO_ENCHANTS, NO_ENCHANTS,
    NO_ENCHANTS, NO_ENCHANTS, NO_ENCHANTS,
    NO_ENCHANTS, SWORD_ENCHANTS, AXE_ENCHANTS,
    HOE_ENCHANTS, SHOVEL_ENCHANTS, PICKAXE_ENCHANTS, 
    BOOTS_ENCHANTS, CHESTPLATE_ENCHANTS, HELMET_ENCHANTS,
    LEGGINGS_ENCHANTS, NO_ENCHANTS, NO_ENCHANTS, 
    NO_ENCHANTS, NO_ENCHANTS, NO_ENCHANTS,
    NO_ENCHANTS, NO_ENCHANTS, NO_ENCHANTS,
    NO_ENCHANTS
};

int precomputedRPLootTable[398] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,16,16,16,16,16,17,17,17,17,17,18,18,18,18,18,19,19,19,19,19,20,20,20,20,20,21,21,21,21,21,22,23,24
};

LootItem (*ruined_portal_loot_providers[25]) (LootTable *table, int item_index, uint64_t *internal) = {
    provide_loot_uniform_roll, provide_loot_uniform_roll, provide_loot_uniform_roll, 
    provide_loot_no_function, provide_loot_no_function, provide_loot_no_function, 
    provide_loot_uniform_roll, provide_loot_random_enchant, provide_loot_random_enchant, 
    provide_loot_random_enchant, provide_loot_random_enchant, provide_loot_random_enchant, 
    provide_loot_random_enchant, provide_loot_random_enchant, provide_loot_random_enchant, 
    provide_loot_random_enchant, provide_loot_uniform_roll, provide_loot_no_function, 
    provide_loot_no_function, provide_loot_uniform_roll, provide_loot_no_function, 
    provide_loot_uniform_roll, provide_loot_no_function, provide_loot_no_function,
    provide_loot_uniform_roll,
};

LootTable init_ruined_portal_loot_table();
void ruined_portal_loot(LootTable *table, uint64_t loot_seed, LootItem *items, size_t *num_items);

#endif

#ifdef RUINED_PORTAL

LootTable init_ruined_portal_loot_table() {
    LootTable ruined_portal_table;
    
    ruined_portal_table.current_table = 0;
    memcpy(ruined_portal_table.table[0], ruined_portal_loot_table, sizeof(ruined_portal_loot_table));
    memcpy(ruined_portal_table.table_counts[0], ruined_portal_loot_table_counts, sizeof(ruined_portal_loot_table_counts));
    memcpy(ruined_portal_table.enchant_table[0], ruined_portal_enchant_table, sizeof(ruined_portal_enchant_table));

    return ruined_portal_table;
}

void ruined_portal_loot(LootTable *table, uint64_t loot_seed, LootItem *items, size_t *num_items) {
    uint64_t internal;
    set_seed(&internal, loot_seed);
    *(num_items) = 0;

    int rolls = next_int_bounded(&internal, 4, 8);

    table->current_table = 0;
    for (int i = 0; i < rolls; i++) {
        int item = precomputedRPLootTable[next_int(&internal, 398)];
        LootItem loot_item = ruined_portal_loot_providers[item](table, item, &internal);
        items[*num_items] = loot_item;
        (*num_items)++;
    }
}

#endif

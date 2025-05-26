#ifndef DESERT_TEMPLE_H
#define DESERT_TEMPLE_H

#include <stdlib.h>

static int desert_temple_loot_table_counts_0[25][2] = {
    {1, 3}, {1, 5}, {2, 7}, {1, 3}, {4, 6}, {1, 3}, {3, 7},
    {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, 1}
};

static int desert_temple_loot_table_counts_1[25][2] = {
    {1, 8}, {1, 8}, {1, 8}, {1, 8}, {1, 8}
};

static Item desert_temple_loot_table_0[25] = {
    DIAMOND, IRON_INGOT, GOLD_INGOT, EMERALD,
    BONE, SPIDER_EYE, ROTTEN_FLESH, SADDLE, IRON_HORSE_ARMOR,
    GOLDEN_HORSE_ARMOR, DIAMOND_HORSE_ARMOUR, ENCHANTED_BOOK, GOLDEN_APPLE, ENCHANTED_GOLDEN_APPLE, ITEM_NONE
};

static Item desert_temple_loot_table_1[25] = {
    BONE, GUNPOWDER, ROTTEN_FLESH, STRING, SAND
};

static ItemEnchants desert_temple_enchant_table_0[25] = {
    NO_ENCHANTS, NO_ENCHANTS, NO_ENCHANTS, NO_ENCHANTS, NO_ENCHANTS,
    NO_ENCHANTS, NO_ENCHANTS, NO_ENCHANTS, NO_ENCHANTS, NO_ENCHANTS,
    NO_ENCHANTS, ENCHANTED_BOOK_ENCHANTS, NO_ENCHANTS, NO_ENCHANTS, NO_ENCHANTS,
};

static ItemEnchants desert_temple_enchant_table_1[25] = {
    NO_ENCHANTS, NO_ENCHANTS, NO_ENCHANTS, NO_ENCHANTS, NO_ENCHANTS,
};


int precomputed_desert_loot_table_0[232] = {0};
int precomputed_desert_loot_table_1[50] = {0};

int desert_temple_table_weights_0[] = {
    5, 15, 15, 15, 25, 25, 25, 20, 15, 10, 5, 20, 20, 2, 15
};

int desert_temple_table_weights_1[] = {
    10, 10, 10, 10, 10
};

LootItem (*desert_temple_loot_providers_0[25]) (LootTable *table, int item_index, uint64_t *internal) = {
    provide_loot_uniform_roll, provide_loot_uniform_roll, provide_loot_uniform_roll, 
    provide_loot_uniform_roll, provide_loot_uniform_roll, provide_loot_uniform_roll, 
    provide_loot_uniform_roll, 
    provide_loot_no_function, provide_loot_no_function, provide_loot_no_function, provide_loot_no_function,
    provide_loot_random_enchant, provide_loot_no_function, provide_loot_no_function, provide_loot_no_function 
};

LootItem (*desert_temple_loot_providers_1[25]) (LootTable *table, int item_index, uint64_t *internal) = {
    provide_loot_uniform_roll, provide_loot_uniform_roll, provide_loot_uniform_roll,
    provide_loot_uniform_roll, provide_loot_uniform_roll 
};

LootTable init_desert_temple_loot_table();
void desert_temple_loot(LootTable *table, uint64_t loot_seed, LootItem *items, size_t *num_items);

#endif

#ifdef RUINED_PORTAL

LootTable init_desert_temple_loot_table() {
    LootTable desert_temple_table;

    int k = 0;
    for (size_t i = 0; i < 15; i++) {
        for (size_t j = 0; j < (size_t)desert_temple_table_weights_0[i]; j++) {
            precomputed_desert_loot_table_0[k] = i;
            k++;
        }
    }

    k = 0;
    for (size_t i = 0; i < 5; i++) {
        for (size_t j = 0; j < (size_t)desert_temple_table_weights_1[i]; j++) {
            precomputed_desert_loot_table_1[k] = i;
            k++;
        }
    }
    
    memcpy(desert_temple_table.table[0], desert_temple_loot_table_0, sizeof(desert_temple_loot_table_0));
    memcpy(desert_temple_table.table_counts[0], desert_temple_loot_table_counts_0, sizeof(desert_temple_loot_table_counts_0));
    memcpy(desert_temple_table.enchant_table[0], desert_temple_enchant_table_0, sizeof(desert_temple_enchant_table_0));

    memcpy(desert_temple_table.table[1], desert_temple_loot_table_1, sizeof(desert_temple_loot_table_1));
    memcpy(desert_temple_table.table_counts[1], desert_temple_loot_table_counts_1, sizeof(desert_temple_loot_table_counts_1));
    memcpy(desert_temple_table.enchant_table[1], desert_temple_enchant_table_1, sizeof(desert_temple_enchant_table_1));


    return desert_temple_table;
}

void desert_temple_loot(LootTable *table, uint64_t loot_seed, LootItem *items, size_t *num_items) {
    uint64_t internal;
    set_seed(&internal, loot_seed);
    *(num_items) = 0;

    int rolls = next_int_bounded(&internal, 2, 4);

    table->current_table = 0;
    for (int i = 0; i < rolls; i++) {
        int weight = next_int(&internal, 232);
        int item = precomputed_desert_loot_table_0[weight];
        printf("item = %d\n", item);
        LootItem loot_item = desert_temple_loot_providers_0[item](table, item, &internal);
        items[*num_items] = loot_item;
        (*num_items)++;
    }

    table->current_table = 1;
    for (int i = 0; i < 4; i++) {
        int weight = next_int(&internal, 50);
        int item = precomputed_desert_loot_table_1[weight];
        LootItem loot_item = desert_temple_loot_providers_1[item](table, item, &internal);
        items[*num_items] = loot_item;
        (*num_items)++;   
    }
}

#endif

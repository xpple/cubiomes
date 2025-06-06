#include "igloo_chest_1_13.h"

#include "../../biomes.h"

#include "../loot_table_context.h"
#include "../loot_table_parser.h"

int init_igloo_chest_1_13(LootTableContext* context) {
    static char* item_names[8] = {"minecraft:apple", "minecraft:coal", "minecraft:gold_nugget", "minecraft:stone_axe", "minecraft:rotten_flesh", "minecraft:emerald", "minecraft:wheat", "minecraft:golden_apple"};
    static int precomputed_loot__0[63] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6};
    static int entry_to_item__0[7] = {0, 1, 2, 3, 4, 5, 6};
    static int entry_functions_count__0[7] = {1, 1, 1, 0, 0, 0, 1};
    static int entry_functions_index__0[7] = {0, 1, 2, 3, 3, 3, 3};
    static LootPool igloo_chest_1_13__0 = {
        .min_rolls = 2,
        .max_rolls = 8,
        .roll_count_function = roll_count_uniform,
        .entry_count = 7,
        .total_weight = 63,
        .precomputed_loot = precomputed_loot__0,
        .entry_to_item = entry_to_item__0,
        .entry_functions_count = entry_functions_count__0,
        .entry_functions_index = entry_functions_index__0,
        .loot_functions = NULL, // initialised later
    };
    static int precomputed_loot__1[1] = {0};
    static int entry_to_item__1[1] = {7};
    static int entry_functions_count__1[1] = {0};
    static int entry_functions_index__1[1] = {0};
    static LootPool igloo_chest_1_13__1 = {
        .min_rolls = 1,
        .max_rolls = 1,
        .roll_count_function = roll_count_constant,
        .entry_count = 1,
        .total_weight = 1,
        .precomputed_loot = precomputed_loot__1,
        .entry_to_item = entry_to_item__1,
        .entry_functions_count = entry_functions_count__1,
        .entry_functions_index = entry_functions_index__1,
        .loot_functions = NULL, // initialised later
    };

    context->version = MC_1_13;
    context->item_count = 8;
    context->item_names = item_names;

    context->unresolved_subtable_count = 0;
    context->subtable_count = 0;
    context->subtable_pool_offset = NULL;
    context->subtable_pool_count = NULL;

    context->pool_count = 2;
    context->loot_pools = malloc(context->pool_count * sizeof(LootPool));
    if (context->loot_pools == NULL) {
        return 0;
    }
    context->loot_pools[0] = igloo_chest_1_13__0;
    context->loot_pools[1] = igloo_chest_1_13__1;
    LootPool* loot_pool__0 = &(context->loot_pools[0]);
    loot_pool__0->loot_functions = malloc(4 * sizeof(LootFunction));
    create_set_count(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[0] + loot_pool__0->entry_functions_count[0]]), 1, 3);
    create_set_count(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[1] + loot_pool__0->entry_functions_count[1]]), 1, 4);
    create_set_count(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[2] + loot_pool__0->entry_functions_count[2]]), 1, 3);
    create_set_count(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[6] + loot_pool__0->entry_functions_count[6]]), 2, 3);
    LootPool* loot_pool__1 = &(context->loot_pools[1]);
    loot_pool__1->loot_functions = malloc(0 * sizeof(LootFunction));
    return 1;
}

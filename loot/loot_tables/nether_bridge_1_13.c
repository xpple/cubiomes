#include "nether_bridge_1_13.h"

#include "../../biomes.h"

#include "../loot_table_context.h"
#include "../loot_table_parser.h"

int init_nether_bridge_1_13(LootTableContext* context) {
    static char* item_names[12] = {"minecraft:diamond", "minecraft:iron_ingot", "minecraft:gold_ingot", "minecraft:golden_sword", "minecraft:golden_chestplate", "minecraft:flint_and_steel", "minecraft:nether_wart", "minecraft:saddle", "minecraft:golden_horse_armor", "minecraft:iron_horse_armor", "minecraft:diamond_horse_armor", "minecraft:obsidian"};
    static int precomputed_loot__0[73] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 10, 10, 10, 11, 11};
    static int entry_to_item__0[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    static int entry_functions_count__0[12] = {1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    static int entry_functions_index__0[12] = {0, 1, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4};
    static LootPool nether_bridge_1_13__0 = {
        .min_rolls = 2,
        .max_rolls = 4,
        .roll_count_function = roll_count_uniform,
        .entry_count = 12,
        .total_weight = 73,
        .precomputed_loot = precomputed_loot__0,
        .entry_to_item = entry_to_item__0,
        .entry_functions_count = entry_functions_count__0,
        .entry_functions_index = entry_functions_index__0,
        .loot_functions = NULL, // initialised later
    };

    context->version = MC_1_13;
    context->item_count = 12;
    context->item_names = item_names;

    context->unresolved_subtable_count = 0;
    context->subtable_count = 0;
    context->subtable_pool_offset = NULL;
    context->subtable_pool_count = NULL;

    context->pool_count = 1;
    context->loot_pools = malloc(context->pool_count * sizeof(LootPool));
    if (context->loot_pools == NULL) {
        return 0;
    }
    context->loot_pools[0] = nether_bridge_1_13__0;
    LootPool* loot_pool__0 = &(context->loot_pools[0]);
    loot_pool__0->loot_functions = malloc(5 * sizeof(LootFunction));
    create_set_count(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[0] + loot_pool__0->entry_functions_count[0]]), 1, 3);
    create_set_count(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[1] + loot_pool__0->entry_functions_count[1]]), 1, 5);
    create_set_count(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[2] + loot_pool__0->entry_functions_count[2]]), 1, 3);
    create_set_count(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[6] + loot_pool__0->entry_functions_count[6]]), 3, 7);
    create_set_count(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[11] + loot_pool__0->entry_functions_count[11]]), 2, 4);
    return 1;
}

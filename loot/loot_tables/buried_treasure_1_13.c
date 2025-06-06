#include "buried_treasure_1_13.h"

#include "../../biomes.h"

#include "../loot_table_context.h"
#include "../loot_table_parser.h"

int init_buried_treasure_1_13(LootTableContext* context) {
    static char* item_names[11] = {"minecraft:heart_of_the_sea", "minecraft:iron_ingot", "minecraft:gold_ingot", "minecraft:tnt", "minecraft:emerald", "minecraft:diamond", "minecraft:prismarine_crystals", "minecraft:leather_chestplate", "minecraft:iron_sword", "minecraft:cooked_cod", "minecraft:cooked_salmon"};
    static int precomputed_loot__0[1] = {0};
    static int entry_to_item__0[1] = {0};
    static int entry_functions_count__0[1] = {0};
    static int entry_functions_index__0[1] = {0};
    static LootPool buried_treasure_1_13__0 = {
        .min_rolls = 1,
        .max_rolls = 1,
        .roll_count_function = roll_count_constant,
        .entry_count = 1,
        .total_weight = 1,
        .precomputed_loot = precomputed_loot__0,
        .entry_to_item = entry_to_item__0,
        .entry_functions_count = entry_functions_count__0,
        .entry_functions_index = entry_functions_index__0,
        .loot_functions = NULL, // initialised later
    };
    static int precomputed_loot__1[35] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2};
    static int entry_to_item__1[3] = {1, 2, 3};
    static int entry_functions_count__1[3] = {1, 1, 1};
    static int entry_functions_index__1[3] = {0, 1, 2};
    static LootPool buried_treasure_1_13__1 = {
        .min_rolls = 5,
        .max_rolls = 8,
        .roll_count_function = roll_count_uniform,
        .entry_count = 3,
        .total_weight = 35,
        .precomputed_loot = precomputed_loot__1,
        .entry_to_item = entry_to_item__1,
        .entry_functions_count = entry_functions_count__1,
        .entry_functions_index = entry_functions_index__1,
        .loot_functions = NULL, // initialised later
    };
    static int precomputed_loot__2[15] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2};
    static int entry_to_item__2[3] = {4, 5, 6};
    static int entry_functions_count__2[3] = {1, 1, 1};
    static int entry_functions_index__2[3] = {0, 1, 2};
    static LootPool buried_treasure_1_13__2 = {
        .min_rolls = 1,
        .max_rolls = 3,
        .roll_count_function = roll_count_uniform,
        .entry_count = 3,
        .total_weight = 15,
        .precomputed_loot = precomputed_loot__2,
        .entry_to_item = entry_to_item__2,
        .entry_functions_count = entry_functions_count__2,
        .entry_functions_index = entry_functions_index__2,
        .loot_functions = NULL, // initialised later
    };
    static int precomputed_loot__3[2] = {0, 1};
    static int entry_to_item__3[2] = {7, 8};
    static int entry_functions_count__3[2] = {0, 0};
    static int entry_functions_index__3[2] = {0, 0};
    static LootPool buried_treasure_1_13__3 = {
        .min_rolls = 0,
        .max_rolls = 1,
        .roll_count_function = roll_count_uniform,
        .entry_count = 2,
        .total_weight = 2,
        .precomputed_loot = precomputed_loot__3,
        .entry_to_item = entry_to_item__3,
        .entry_functions_count = entry_functions_count__3,
        .entry_functions_index = entry_functions_index__3,
        .loot_functions = NULL, // initialised later
    };
    static int precomputed_loot__4[2] = {0, 1};
    static int entry_to_item__4[2] = {9, 10};
    static int entry_functions_count__4[2] = {1, 1};
    static int entry_functions_index__4[2] = {0, 1};
    static LootPool buried_treasure_1_13__4 = {
        .min_rolls = 2,
        .max_rolls = 2,
        .roll_count_function = roll_count_constant,
        .entry_count = 2,
        .total_weight = 2,
        .precomputed_loot = precomputed_loot__4,
        .entry_to_item = entry_to_item__4,
        .entry_functions_count = entry_functions_count__4,
        .entry_functions_index = entry_functions_index__4,
        .loot_functions = NULL, // initialised later
    };

    context->version = MC_1_13;
    context->item_count = 11;
    context->item_names = item_names;

    context->unresolved_subtable_count = 0;
    context->subtable_count = 0;
    context->subtable_pool_offset = NULL;
    context->subtable_pool_count = NULL;

    context->pool_count = 5;
    context->loot_pools = malloc(context->pool_count * sizeof(LootPool));
    if (context->loot_pools == NULL) {
        return 0;
    }
    context->loot_pools[0] = buried_treasure_1_13__0;
    context->loot_pools[1] = buried_treasure_1_13__1;
    context->loot_pools[2] = buried_treasure_1_13__2;
    context->loot_pools[3] = buried_treasure_1_13__3;
    context->loot_pools[4] = buried_treasure_1_13__4;
    LootPool* loot_pool__0 = &(context->loot_pools[0]);
    loot_pool__0->loot_functions = malloc(0 * sizeof(LootFunction));
    LootPool* loot_pool__1 = &(context->loot_pools[1]);
    loot_pool__1->loot_functions = malloc(3 * sizeof(LootFunction));
    create_set_count(&(loot_pool__1->loot_functions[loot_pool__1->entry_functions_index[0] + loot_pool__1->entry_functions_count[0]]), 1, 4);
    create_set_count(&(loot_pool__1->loot_functions[loot_pool__1->entry_functions_index[1] + loot_pool__1->entry_functions_count[1]]), 1, 4);
    create_set_count(&(loot_pool__1->loot_functions[loot_pool__1->entry_functions_index[2] + loot_pool__1->entry_functions_count[2]]), 1, 2);
    LootPool* loot_pool__2 = &(context->loot_pools[2]);
    loot_pool__2->loot_functions = malloc(3 * sizeof(LootFunction));
    create_set_count(&(loot_pool__2->loot_functions[loot_pool__2->entry_functions_index[0] + loot_pool__2->entry_functions_count[0]]), 4, 8);
    create_set_count(&(loot_pool__2->loot_functions[loot_pool__2->entry_functions_index[1] + loot_pool__2->entry_functions_count[1]]), 1, 2);
    create_set_count(&(loot_pool__2->loot_functions[loot_pool__2->entry_functions_index[2] + loot_pool__2->entry_functions_count[2]]), 1, 5);
    LootPool* loot_pool__3 = &(context->loot_pools[3]);
    loot_pool__3->loot_functions = malloc(0 * sizeof(LootFunction));
    LootPool* loot_pool__4 = &(context->loot_pools[4]);
    loot_pool__4->loot_functions = malloc(2 * sizeof(LootFunction));
    create_set_count(&(loot_pool__4->loot_functions[loot_pool__4->entry_functions_index[0] + loot_pool__4->entry_functions_count[0]]), 2, 4);
    create_set_count(&(loot_pool__4->loot_functions[loot_pool__4->entry_functions_index[1] + loot_pool__4->entry_functions_count[1]]), 2, 4);
    return 1;
}

#include "ruined_portal_1_21.h"

#include "../../biomes.h"

#include "../loot_table_context.h"
#include "../loot_table_parser.h"

int init_ruined_portal_1_21(LootTableContext* context) {
    static char* item_names[26] = {"minecraft:obsidian", "minecraft:flint", "minecraft:iron_nugget", "minecraft:flint_and_steel", "minecraft:fire_charge", "minecraft:golden_apple", "minecraft:gold_nugget", "minecraft:golden_sword", "minecraft:golden_axe", "minecraft:golden_hoe", "minecraft:golden_shovel", "minecraft:golden_pickaxe", "minecraft:golden_boots", "minecraft:golden_chestplate", "minecraft:golden_helmet", "minecraft:golden_leggings", "minecraft:glistering_melon_slice", "minecraft:golden_horse_armor", "minecraft:light_weighted_pressure_plate", "minecraft:golden_carrot", "minecraft:clock", "minecraft:gold_ingot", "minecraft:bell", "minecraft:enchanted_golden_apple", "minecraft:gold_block", "minecraft:lodestone"};
    static int precomputed_loot__0[398] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 22, 23, 24};
    static int entry_to_item__0[25] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
    static int entry_functions_count__0[25] = {1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1};
    static int entry_functions_index__0[25] = {0, 1, 2, 3, 3, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 15, 15, 16, 16, 16};
    static LootPool ruined_portal_1_21__0 = {
        .min_rolls = 4,
        .max_rolls = 8,
        .roll_count_function = roll_count_uniform,
        .entry_count = 25,
        .total_weight = 398,
        .precomputed_loot = precomputed_loot__0,
        .entry_to_item = entry_to_item__0,
        .entry_functions_count = entry_functions_count__0,
        .entry_functions_index = entry_functions_index__0,
        .loot_functions = NULL, // initialised later
    };
    static int precomputed_loot__1[3] = {0, 1, 1};
    static int entry_to_item__1[2] = {-1, 25};
    static int entry_functions_count__1[2] = {0, 1};
    static int entry_functions_index__1[2] = {0, 0};
    static LootPool ruined_portal_1_21__1 = {
        .min_rolls = 1,
        .max_rolls = 1,
        .roll_count_function = roll_count_constant,
        .entry_count = 2,
        .total_weight = 3,
        .precomputed_loot = precomputed_loot__1,
        .entry_to_item = entry_to_item__1,
        .entry_functions_count = entry_functions_count__1,
        .entry_functions_index = entry_functions_index__1,
        .loot_functions = NULL, // initialised later
    };

    context->version = MC_1_21;
    context->item_count = 26;
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
    context->loot_pools[0] = ruined_portal_1_21__0;
    context->loot_pools[1] = ruined_portal_1_21__1;
    LootPool* loot_pool__0 = &(context->loot_pools[0]);
    loot_pool__0->loot_functions = malloc(17 * sizeof(LootFunction));
    create_set_count(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[0] + loot_pool__0->entry_functions_count[0]]), 1, 2);
    create_set_count(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[1] + loot_pool__0->entry_functions_count[1]]), 1, 4);
    create_set_count(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[2] + loot_pool__0->entry_functions_count[2]]), 9, 18);
    create_set_count(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[6] + loot_pool__0->entry_functions_count[6]]), 4, 24);
    create_enchant_randomly(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[7] + loot_pool__0->entry_functions_count[7]]), MC_1_21, get_item_type("minecraft:golden_sword"), 1);
    create_enchant_randomly(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[8] + loot_pool__0->entry_functions_count[8]]), MC_1_21, get_item_type("minecraft:golden_axe"), 1);
    create_enchant_randomly(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[9] + loot_pool__0->entry_functions_count[9]]), MC_1_21, get_item_type("minecraft:golden_hoe"), 1);
    create_enchant_randomly(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[10] + loot_pool__0->entry_functions_count[10]]), MC_1_21, get_item_type("minecraft:golden_shovel"), 1);
    create_enchant_randomly(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[11] + loot_pool__0->entry_functions_count[11]]), MC_1_21, get_item_type("minecraft:golden_pickaxe"), 1);
    create_enchant_randomly(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[12] + loot_pool__0->entry_functions_count[12]]), MC_1_21, get_item_type("minecraft:golden_boots"), 1);
    create_enchant_randomly(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[13] + loot_pool__0->entry_functions_count[13]]), MC_1_21, get_item_type("minecraft:golden_chestplate"), 1);
    create_enchant_randomly(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[14] + loot_pool__0->entry_functions_count[14]]), MC_1_21, get_item_type("minecraft:golden_helmet"), 1);
    create_enchant_randomly(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[15] + loot_pool__0->entry_functions_count[15]]), MC_1_21, get_item_type("minecraft:golden_leggings"), 1);
    create_set_count(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[16] + loot_pool__0->entry_functions_count[16]]), 4, 12);
    create_set_count(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[19] + loot_pool__0->entry_functions_count[19]]), 4, 12);
    create_set_count(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[21] + loot_pool__0->entry_functions_count[21]]), 2, 8);
    create_set_count(&(loot_pool__0->loot_functions[loot_pool__0->entry_functions_index[24] + loot_pool__0->entry_functions_count[24]]), 1, 2);
    LootPool* loot_pool__1 = &(context->loot_pools[1]);
    loot_pool__1->loot_functions = malloc(1 * sizeof(LootFunction));
    create_set_count(&(loot_pool__1->loot_functions[loot_pool__1->entry_functions_index[1] + loot_pool__1->entry_functions_count[1]]), 1, 2);
    return 1;
}

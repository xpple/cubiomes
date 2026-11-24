#include "testing.h"
#include "../biomes.h"
#include "../loot/items.h"
#include "../loot/loot_tables.h"

static int same_enchantment_instance(const EnchantInstance *actual, const EnchantInstance *expected) {
    if (actual->enchantment != expected->enchantment) {
        return 0;
    }
    if (actual->level != expected->level) {
        return 0;
    }
    return 1;
}

static int same_item(const ItemStack *actual, const ItemStack *expected) {
    if (actual->item != expected->item) {
        return 0;
    }
    if (actual->enchantment_count != expected->enchantment_count) {
        return 0;
    }
    // order independent comparison
    for (int i = 0; i < expected->enchantment_count; i++) {
        int j;
        for (j = 0; j < actual->enchantment_count; j++) {
            if (same_enchantment_instance(&actual->enchantments[j], &expected->enchantments[i])) {
                break;
            }
        }
        if (j == actual->enchantment_count) {
            return 0;
        }
    }

    return 1;
}

static int test_loot_gen_26_3() {
    int ret = 0;

    const struct { const char *table; uint64_t seed; int itemCount; ItemStack items[5]; } data[] = {
        {"chests/stronghold_corridor", 3447489880046750390ULL, 4, {
            {.item = ITEM_BREAD, .count = 3},
            {.item = ITEM_ENDER_PEARL, .count = 1},
            {.item = ITEM_APPLE, .count = 2},
            {.item = ITEM_EYE_ARMOR_TRIM_SMITHING_TEMPLATE, .count = 1}}},
        {"chests/stronghold_library", -3936241895135843426ULL, 5, {
            {.item = ITEM_BOOK, .count = 9},
            {.item = ITEM_PAPER, .count = 15},
            {.item = ITEM_BOOK, .count = 1, .enchantment_count = 2, .enchantments = {{MENDING, 1}, {SMITE, 4}}},
            {.item = ITEM_BOOK, .count = 1, .enchantment_count = 1, .enchantments = {{LOOTING, 2}}},
            {.item = ITEM_EYE_ARMOR_TRIM_SMITHING_TEMPLATE, .count = 1}}},
        {"chests/stronghold_library", -8810524954827231578ULL, 5, {
            {.item = ITEM_PAPER, .count = 17},
            {.item = ITEM_BOOK, .count = 5},
            {.item = ITEM_COMPASS, .count = 1},
            {.item = ITEM_BOOK, .count = 1, .enchantment_count = 2, .enchantments = {{EFFICIENCY, 3}, {PIERCING, 3}}},
            {.item = ITEM_EYE_ARMOR_TRIM_SMITHING_TEMPLATE, .count = 1}}},
        {"chests/stronghold_crossing", 1348204196818699851ULL, 2, {
            {.item = ITEM_COAL, .count = 7},
            {.item = ITEM_IRON_INGOT, .count = 2}}},
        {"chests/stronghold_crossing", 8579856192373242195ULL, 3, {
            {.item = ITEM_REDSTONE, .count = 11},
            {.item = ITEM_IRON_INGOT, .count = 4},
            {.item = ITEM_BREAD, .count = 3}}},
        {"chests/stronghold_corridor", -2669918015300919170ULL, 3, {
            {.item = ITEM_IRON_LEGGINGS, .count = 1},
            {.item = ITEM_APPLE, .count = 3},
            {.item = ITEM_ENDER_PEARL, .count = 1}}},
        {"chests/stronghold_corridor", -1839377622803992734ULL, 2, {
            {.item = ITEM_IRON_PICKAXE, .count = 1},
            {.item = ITEM_BREAD, .count = 1}}},
        {"chests/stronghold_corridor", 8968825157336189011ULL, 3, {
            {.item = ITEM_IRON_LEGGINGS, .count = 1},
            {.item = ITEM_ENDER_PEARL, .count = 1},
            {.item = ITEM_APPLE, .count = 1}}},
        {"chests/stronghold_library", -3204400700003780750ULL, 5, {
            {.item = ITEM_BOOK, .count = 4},
            {.item = ITEM_COMPASS, .count = 1},
            {.item = ITEM_BOOK, .count = 1, .enchantment_count = 3, .enchantments = {{LOYALTY, 3}, {POWER, 3}, {IMPALING, 4}}},
            {.item = ITEM_BOOK, .count = 1, .enchantment_count = 2, .enchantments = {{KNOCKBACK, 2}, {QUICK_CHARGE, 1}}},
            {.item = ITEM_EYE_ARMOR_TRIM_SMITHING_TEMPLATE, .count = 1}}},
        {"chests/stronghold_library", 1484191649403803537ULL, 4, {
            {.item = ITEM_PAPER, .count = 6},
            {.item = ITEM_BOOK, .count = 6},
            {.item = ITEM_BOOK, .count = 1, .enchantment_count = 3, .enchantments = {{LOYALTY, 3}, {BREACH, 2}, {FLAME, 1}}},
            {.item = ITEM_EYE_ARMOR_TRIM_SMITHING_TEMPLATE, .count = 1}}},
    };

    for (size_t i = 0; i < sizeof(data) / sizeof(*data); i++) {
        LootTableContext *ltc;
        if (!init_loot_table_name(&ltc, data[i].table, MC_26_3)) {
            ASSERTION_FAILED;
            ret++;
            continue;
        }
        set_loot_prng_type(ltc, JAVA_RANDOM);
        set_loot_seed(ltc, data[i].seed);
        generate_loot(ltc);

        int counts[5] = {0};
        for (int j = 0; j < ltc->generated_item_count; j++) {
            ItemStack item = ltc->generated_items[j];
            item.item = get_global_item_id(ltc, item.item);

            int k;
            for (k = 0; k < data[i].itemCount; k++) {
                if (same_item(&item, &data[i].items[k])) {
                    break;
                }
            }
            ASSERT_LT(ret, k, data[i].itemCount);
            if (k < data[i].itemCount) {
                counts[k] += item.count;
            }
        }
        for (int j = 0; j < data[i].itemCount; j++) {
            ASSERT_EQ(ret, counts[j], data[i].items[j].count);
        }
    }

    return ret;
}

int main() {
    int ret = 0;

    ret += test_loot_gen_26_3();

    return ret;
}

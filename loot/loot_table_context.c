#include "items.h"
#include "loot_table_context.h"
#include <string.h>

void set_loot_prng_type(LootTableContext* context, int type)
{
    context->prng_state.type = type;
}

void set_loot_seed(LootTableContext* context, uint64_t seed)
{
    absSetSeed(&context->prng_state, seed);
}

void set_internal_loot_seed(LootTableContext* context, RandomState internal_seed) {
    absSetSeedInternal(&context->prng_state, internal_seed);
}

int get_item_id(LootTableContext* context, const char* item_name)
{
    for (int i = 0; i < context->item_count; i++)
    {
        if (strcmp(context->item_names[i], item_name) == 0)
            return i;
    }
    return -1;
}

int get_global_item_id(LootTableContext* context, int item_id) {
    if (item_id < 0 || item_id >= context->item_count) {
        return ITEM_UNKNOWN;
    }
    return context->global_item_ids[item_id];
}

const char* get_item_name(LootTableContext* context, int item_id)
{
    if (item_id < 0 || item_id >= context->item_count)
        return NULL;
    return context->item_names[item_id];
}

int has_item(LootTableContext* context, int global_item_id) {
    for (int i = 0; i < context->item_count; ++i) {
        if (context->global_item_ids[i] == global_item_id) {
            return 1;
        }
    }
    return 0;
}

// ------------------------------------------------------------------------

static void generate_loot_pool(LootTableContext* context, int pool_index);
static void generate_subtable_loot(LootTableContext* context, int subtable_index);


static void generate_subtable_loot(LootTableContext* context, int subtable_index)
{
    int pool_count = context->subtable_pool_count[subtable_index];

    for (int i = 0; i < pool_count; i++)
    {
        const int pool_index = context->subtable_pool_offset[subtable_index] + i;
        generate_loot_pool(context, pool_index);
    }
}

static void generate_loot_pool(LootTableContext* context, int pool_index)
{
    const LootPool* pool = &(context->loot_pools[pool_index]);

    const int condition_count = pool->pool_condition_count;
    for (int i = 0; i < condition_count; i++) {
        const LootItemCondition* condition = &pool->conditions[i];
        if (!condition->fun(&context->prng_state, condition->params)) {
            return;
        }
    }

    // each loot pool first chooses the number of rolls
    RollCountFunction roll_choice = pool->roll_count_function;
    const int rolls = roll_choice(&(context->prng_state), pool->min_rolls, pool->max_rolls);

    if (rolls <= 0 || pool->entry_count == 0) {
        return;
    }

    const int last = pool->entry_count - 1;
    const int has_entry_conditions = pool->entry_conditions_index[last] + pool->entry_conditions_count[last] > pool->pool_condition_count;

    // recover entry weights only for pools with conditional entries
    int entry_weights[has_entry_conditions ? pool->entry_count : 1];
    if (has_entry_conditions) {
        memset(entry_weights, 0, sizeof(entry_weights));
        for (int w = 0; w < pool->total_weight; w++) {
            entry_weights[pool->precomputed_loot[w]]++;
        }
    }

    for (int r = 0; r < rolls; r++)
    {
        int entry = -1;
        if (!has_entry_conditions) {
            // fast path
            if (pool->total_weight == 0) {
                continue;
            }

            int multiple_entries = pool->precomputed_loot[0] != pool->precomputed_loot[pool->total_weight - 1];
            int w = multiple_entries ? absNextInt(&context->prng_state, pool->total_weight) : 0;
            entry = pool->precomputed_loot[w];
        } else {
            // check entry conditions
            int cumulative_weights[pool->entry_count];
            int total_weight = 0;
            int entry_count = 0;
            for (int e = 0; e < pool->entry_count; e++) {
                int passes = 1;
                const int count = pool->entry_conditions_count[e];
                const int index = pool->entry_conditions_index[e];
                for (int c = 0; c < count; c++) {
                    const LootItemCondition* condition = &pool->conditions[index + c];
                    if (!condition->fun(&context->prng_state, condition->params)) {
                        passes = 0;
                        break;
                    }
                }
                if (passes && entry_weights[e] > 0) {
                    total_weight += entry_weights[e];
                    entry_count++;
                    entry = e;
                }
                cumulative_weights[e] = total_weight;
            }

            if (entry_count == 0) {
                continue;
            }

            if (entry_count > 1) {
                int w = absNextInt(&context->prng_state, total_weight);
                for (entry = 0; w >= cumulative_weights[entry]; entry++);
            }
        }
        int item = pool->entry_to_item[entry];
        if (item == -1)
            continue;

        if (item < -1)
        {
            generate_subtable_loot(context, -(item+2));
            continue;
        }

        // add the non-empty entry as a generated item
        ItemStack* item_stack = &context->generated_items[context->generated_item_count];
        (context->generated_item_count)++;
        item_stack->item = item;
        item_stack->count = 1;
        item_stack->enchantment_count = 0;
        item_stack->mob_effect = (MobEffectInstance) {-1, -1};

        // sequentially apply all the loot functions to the item stack
        const int function_count = pool->entry_functions_count[entry];
        const int function_index = pool->entry_functions_index[entry];
        for (int f = 0; f < function_count; f++)
        {
            LootFunction* lf = &pool->loot_functions[function_index + f];
            lf->fun(&context->prng_state, item_stack, lf->params);
        }
    }
}

void generate_loot(LootTableContext* context)
{
    // this is the entire point of the library.
    context->generated_item_count = 0;

    for (int loot_pool_id = 0; loot_pool_id < context->pool_count; loot_pool_id++)
    {
        generate_loot_pool(context, loot_pool_id);
    }
}

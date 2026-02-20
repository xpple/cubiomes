#ifndef _LOOT_TABLE_CONTEXT_H
#define _LOOT_TABLE_CONTEXT_H

#include <stdint.h>

#include "loot_functions.h"


// ---------------------------------------------------------

enum GenerationMode {
    GENERATE_NATURAL = 0,
    GENERATE_INDEXED = 1,
    GENERATE_AGGREGATED = 2,
    SKIP_ENCHANTMENTS = 4,
    STOP_AT_PREDICATE_MATCH = 8
};
typedef enum GenerationMode GenerationMode;

typedef struct LootPool LootPool;
struct LootPool {
    // roll count choice function
    int min_rolls;
    int max_rolls;
    RollCountFunction roll_count_function;

    // precomputed loot array, which maps possible rng ouputs
    // to indices of loot entries they correspond to
    int total_weight;
    int* precomputed_loot;

    // loot entry arrays
    int entry_count;
    int* entry_to_item;         // entryID -> context itemID
    int* entry_functions_index; // entryID -> ID of first loot function in functions array
    int* entry_functions_count; // entryID -> number of functions applied to the item

    // loot function array, containing all the extra processors that get applied to the item
    LootFunction* loot_functions;
};

typedef struct LootTableContext LootTableContext;
struct LootTableContext {
    // "constants" defining the loot table
    int version;
    int item_count;    // how many different items can this loot table generate
    char** item_names; // names of the different items
    int* global_item_ids; // consistent item ids

    int pool_count;
    LootPool* loot_pools;

    int unresolved_subtable_count;      // how many subtables are missing
    char* unresolved_subtable_names[8]; // names of the unresolved subtables (future-safe max size)

    int subtable_count;        // how many subtables the loot table has
    int* subtable_pool_offset; // subtable index to index of first loot pool
    int* subtable_pool_count;  // subtable index to how many pools it has

    // holding data related with generating the loot table within the context is a debatable idea
    uint64_t prng_state;
    int generated_item_count;
    ItemStack generated_items[27];

    // TODO:
    // GenerationMode generation_mode;
    // use LootTableOutput* loot_output;
    // instead of generated_items
};

typedef struct LootTableOutput LootTableOutput;
struct LootTableOutput {
    ItemStack natural_loot[27];
    ItemStack indexed_loot[27];
    int aggregated_loot[64];
};


// ---------------------------------------------------------
// loot generation functions

void set_loot_seed(LootTableContext* context, uint64_t seed);
void set_internal_loot_seed(LootTableContext* context, uint64_t internal_seed);

int get_item_id(LootTableContext* context, const char* item_name);
int get_global_item_id(LootTableContext* context, int item_id);
const char* get_item_name(LootTableContext* context, int item_id);
int has_item(LootTableContext* context, int global_item_id);

void generate_loot(LootTableContext* context);

#endif // _LOOT_TABLE_CONTEXT_H

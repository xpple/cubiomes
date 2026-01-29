#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "items.h"
#include "mc_loot.h"
#include "loot_functions.h"
#include "loot_table_parser.h"
#include "logging.h"

#include "cjson/cJSON.h"
#include "../biomes.h"


ItemType get_item_type(const char* item_name)
{
    if (strstr(item_name, "_pickaxe") != NULL) return PICKAXE;
    if (strstr(item_name, "_axe") != NULL) return AXE;
    if (strstr(item_name, "_shovel") != NULL) return SHOVEL;
    if (strstr(item_name, "_hoe") != NULL) return HOE;
    if (strstr(item_name, "_sword") != NULL) return SWORD;
    if (strstr(item_name, "_spear") != NULL) return SPEAR;
    if (strstr(item_name, "_helmet") != NULL) return HELMET;
    if (strstr(item_name, "_chestplate") != NULL) return CHESTPLATE;
    if (strstr(item_name, "_leggings") != NULL) return LEGGINGS;
    if (strstr(item_name, "_boots") != NULL) return BOOTS;

    if (strcmp(item_name, "minecraft:fishing_rod") == 0) return FISHING_ROD;
    if (strcmp(item_name, "minecraft:crossbow") == 0) return CROSSBOW;
    if (strcmp(item_name, "minecraft:trident") == 0) return TRIDENT;
    if (strcmp(item_name, "minecraft:bow") == 0) return BOW;
    if (strcmp(item_name, "minecraft:book") == 0) return BOOK;
    if (strcmp(item_name, "minecraft:mace") == 0) return MACE;

    return NO_ITEM;
}

Enchantment get_enchantment_from_name(const char* ench)
{
    // I'm sorry.
    if (strcmp(ench, "minecraft:protection") == 0) return PROTECTION;
    if (strcmp(ench, "minecraft:fire_protection") == 0) return FIRE_PROTECTION;
    if (strcmp(ench, "minecraft:feather_falling") == 0) return FEATHER_FALLING;
    if (strcmp(ench, "minecraft:blast_protection") == 0) return BLAST_PROTECTION;
    if (strcmp(ench, "minecraft:projectile_protection") == 0) return PROJECTILE_PROTECTION;
    if (strcmp(ench, "minecraft:respiration") == 0) return RESPIRATION;
    if (strcmp(ench, "minecraft:aqua_affinity") == 0) return AQUA_AFFINITY;
    if (strcmp(ench, "minecraft:thorns") == 0) return THORNS;
    if (strcmp(ench, "minecraft:soul_speed") == 0) return SOUL_SPEED;
    if (strcmp(ench, "minecraft:depth_strider") == 0) return DEPTH_STRIDER;
    if (strcmp(ench, "minecraft:frost_walker") == 0) return FROST_WALKER;
    if (strcmp(ench, "minecraft:swift_sneak") == 0) return SWIFT_SNEAK;

    if (strcmp(ench, "minecraft:sharpness") == 0) return SHARPNESS;
    if (strcmp(ench, "minecraft:smite") == 0) return SMITE;
    if (strcmp(ench, "minecraft:bane_of_arthropods") == 0) return BANE_OF_ARTHROPODS;
    if (strcmp(ench, "minecraft:knockback") == 0) return KNOCKBACK;
    if (strcmp(ench, "minecraft:fire_aspect") == 0) return FIRE_ASPECT;
    if (strcmp(ench, "minecraft:looting") == 0) return LOOTING;
    if (strstr(ench, "sweeping") != NULL) return SWEEPING_EDGE;

    if (strcmp(ench, "minecraft:efficiency") == 0) return EFFICIENCY;
    if (strcmp(ench, "minecraft:silk_touch") == 0) return SILK_TOUCH;
    if (strcmp(ench, "minecraft:fortune") == 0) return FORTUNE;

    if (strcmp(ench, "minecraft:power") == 0) return POWER;
    if (strcmp(ench, "minecraft:flame") == 0) return FLAME;
    if (strcmp(ench, "minecraft:infinity") == 0) return INFINITY_ENCHANTMENT;
    if (strcmp(ench, "minecraft:punch") == 0) return PUNCH;

    if (strcmp(ench, "minecraft:multishot") == 0) return MULTISHOT;
    if (strcmp(ench, "minecraft:quick_charge") == 0) return QUICK_CHARGE;
    if (strcmp(ench, "minecraft:piercing") == 0) return PIERCING;

    if (strcmp(ench, "minecraft:impaling") == 0) return IMPALING;
    if (strcmp(ench, "minecraft:loyalty") == 0) return LOYALTY;
    if (strcmp(ench, "minecraft:riptide") == 0) return RIPTIDE;
    if (strcmp(ench, "minecraft:channeling") == 0) return CHANNELING;

    if (strcmp(ench, "minecraft:luck_of_the_sea") == 0) return LUCK_OF_THE_SEA;
    if (strcmp(ench, "minecraft:lure") == 0) return LURE;
    if (strcmp(ench, "minecraft:lunge") == 0) return LUNGE;

    if (strcmp(ench, "minecraft:density") == 0) return DENSITY;
    if (strcmp(ench, "minecraft:breach") == 0) return BREACH;
    if (strcmp(ench, "minecraft:wind_burst") == 0) return WIND_BURST;

    if (strcmp(ench, "minecraft:mending") == 0) return MENDING;
    if (strcmp(ench, "minecraft:unbreaking") == 0) return UNBREAKING;
    if (strstr(ench, "binding") != NULL) return CURSE_OF_BINDING;
    if (strstr(ench, "vanishing") != NULL) return CURSE_OF_VANISHING;

    return NO_ENCHANTMENT;
}

// ----------------------------------------------
// loot function parsers

static int parse_set_count(LootFunction* loot_function, const cJSON* function_data)
{
    cJSON* count = cJSON_GetObjectItem(function_data, "count");

    if (cJSON_IsNumber(count))
    {
        // constant count
        create_set_count(loot_function, count->valueint, count->valueint);
        return 0;
    }
    else
    {
        // uniformly rolled count
        const int min = cJSON_GetObjectItem(count, "min")->valueint;
        const int max = cJSON_GetObjectItem(count, "max")->valueint;
        create_set_count(loot_function, min, max);
        return 0;
    }
}

static void parse_enchant_randomly(LootTableContext* ctx, LootFunction* loot_function, const cJSON* function_data, const char* item_name)
{
    const ItemType item_type = get_item_type(item_name);
    int is_treasure = 0;
    cJSON* treasure = cJSON_GetObjectItem(function_data, "treasure");
    if (treasure != NULL)
        is_treasure = (int)cJSON_IsTrue(treasure);

    // if the "enchantments" field is present, we need to extract the enchantment name.
    // otherwise, we can simply create the enchant randomly function
    cJSON* defined_enchants_legacy = cJSON_GetObjectItem(function_data, "enchantments");
    cJSON* defined_enchants = cJSON_GetObjectItem(function_data, "options");
    if (defined_enchants == NULL && defined_enchants_legacy == NULL)
    {
        if (ctx->version >= MC_1_21_9)
            create_enchant_randomly_tag(loot_function, ctx->version, item_type, "#minecraft:in_enchanting_table", is_treasure);
        else
            create_enchant_randomly(loot_function, ctx->version, item_type, is_treasure);
        return;
    }
    if (defined_enchants == NULL) {
        defined_enchants = defined_enchants_legacy;
    }
    // check if the field actually defines an enchantment (list) or something like #minecraft:on_random_loot
    if (cJSON_IsString(defined_enchants) && defined_enchants->valuestring && defined_enchants->valuestring[0] == '#') {
        create_enchant_randomly_tag(loot_function, ctx->version, item_type, defined_enchants->valuestring, is_treasure);
        return;
    }
    if (cJSON_IsString(defined_enchants) && get_enchantment_from_name(defined_enchants->valuestring) == NO_ENCHANTMENT) {
        create_enchant_randomly(loot_function, ctx->version, item_type, is_treasure);
        return;
    }

    // enchant randomly with a given list of enchantments
    // sometimes the field's value can be a single enchantment name, handle that case first
    if (cJSON_IsString(defined_enchants)) {
        const char* ench_name = defined_enchants->valuestring;
        const Enchantment ench = get_enchantment_from_name(ench_name);
        create_enchant_randomly_one_enchant(loot_function, ench);
        return;
    }

    const int ench_count = cJSON_GetArraySize(defined_enchants);
    if (ench_count == 1)
    {
        // one-enchant variant
        const char* ench_name = cJSON_GetArrayItem(defined_enchants, 0)->valuestring;
        const Enchantment ench = get_enchantment_from_name(ench_name);
        create_enchant_randomly_one_enchant(loot_function, ench);
    }
    else
    {
        // multi-enchant variant
        Enchantment* enchs = (Enchantment*)malloc(ench_count * sizeof(Enchantment)); // 1
        if (enchs == NULL) return; // FIXME add logging

        int i = 0;
        cJSON* element = NULL;
        cJSON_ArrayForEach(element, defined_enchants)
        {
            const char* ench_name = cJSON_GetStringValue(element);
            const Enchantment ench = get_enchantment_from_name(ench_name);
            enchs[i] = ench;
            i++;
        }

        create_enchant_randomly_list(loot_function, enchs, ench_count);
        free(enchs); // 0
    }
}

static void parse_enchant_with_levels(LootTableContext* ctx, LootFunction* loot_function, const cJSON* function_data, const char* item_name)
{
    const ItemType item_type = get_item_type(item_name);

    int min_level = 0;
    int max_level = 0;
    int is_treasure = 1;

    cJSON* levels = cJSON_GetObjectItem(function_data, "levels");
    if (cJSON_IsNumber(levels))
    {
        min_level = levels->valueint;
        max_level = levels->valueint;
    }
    else if (levels != NULL)
    {
        min_level = cJSON_GetObjectItem(levels, "min")->valueint;
        max_level = cJSON_GetObjectItem(levels, "max")->valueint;
    }

    cJSON* treasure = cJSON_GetObjectItem(function_data, "treasure");
    if (treasure != NULL)
        is_treasure = (int)cJSON_IsTrue(treasure);

    cJSON* options = cJSON_GetObjectItem(function_data, "options");
    if (cJSON_IsString(options))
    {
        const char* opt = cJSON_GetStringValue(options);
        if (opt != NULL && opt[0] == '#')
        {
            create_enchant_with_levels_tag(
                loot_function,
                ctx->version,
                item_name, item_type,
                min_level, max_level,
                opt,
                is_treasure
            );
            return;
        }
    }

    create_enchant_with_levels(
        loot_function,
        ctx->version,
        item_name, item_type,
        min_level, max_level,
        is_treasure
    );
}

// ----------------------------------------------

static void init_loot_table_items(const cJSON* loot_table, LootTableContext* ctx)
{
    // count how many item entries there are in the loot table
    int item_count = 0;

    const cJSON* pools = cJSON_GetObjectItem(loot_table, "pools");
    cJSON* pool = NULL;

    cJSON_ArrayForEach(pool, pools)
    {
        cJSON* entries = cJSON_GetObjectItem(pool, "entries");
        cJSON* entry = NULL;

        cJSON_ArrayForEach(entry, entries)
        {
            cJSON* name = cJSON_GetObjectItem(entry, "name");
            if (name == NULL) continue;
            item_count++;
        }
    }

    // allocate memory for item names
    ctx->item_count = item_count;
    ctx->item_names = (char**)malloc(item_count * sizeof(char*));
    ctx->global_item_ids = malloc(item_count * sizeof(int));
    if (ctx->item_names == NULL || ctx->global_item_ids == NULL) return; // FIXME add logging

    // fill the item names array
    int ix = 0;
    cJSON_ArrayForEach(pool, pools)
    {
        cJSON* entries = cJSON_GetObjectItem(pool, "entries");
        cJSON* entry = NULL;

        cJSON_ArrayForEach(entry, entries)
        {
            cJSON* name = cJSON_GetObjectItem(entry, "name");
            if (name == NULL) continue;

            // copy the name to the LootContext structure
            const char* item_name = cJSON_GetStringValue(name);
            ctx->item_names[ix] = (char*)malloc(strlen(item_name) + 1);
            if (ctx->item_names[ix] == NULL || item_name == NULL) return; // FIXME add logging
            strcpy(ctx->item_names[ix], item_name);
            ctx->global_item_ids[ix] = item_name2global_id(item_name);
            ix++;
            if (ix >= item_count) break;
        }
    }
}

static void init_rolls(const cJSON* pool_data, LootPool* pool)
{
    cJSON* rolls = cJSON_GetObjectItem(pool_data, "rolls");

    if (cJSON_IsNumber(rolls))
    {
        // constant roll
        pool->min_rolls = rolls->valueint;
        pool->max_rolls = rolls->valueint;
        pool->roll_count_function = roll_count_constant;
    }
    else
    {
        // uniform roll
        pool->min_rolls = cJSON_GetObjectItem(rolls, "min")->valueint;
        pool->max_rolls = cJSON_GetObjectItem(rolls, "max")->valueint;
        pool->roll_count_function = roll_count_uniform;
    }
}

static void map_entry_to_item(const cJSON* entry_data, LootTableContext* ctx, int* item_id)
{
    cJSON* name = cJSON_GetObjectItem(entry_data, "name");
    if (name == NULL) {
        *item_id = -1; // empty entry
        return;
    }

    const char* name_str = cJSON_GetStringValue(name);
    for (int i = 0; i < ctx->item_count; i++) {
        if (strcmp(ctx->item_names[i], name_str) == 0) {
            *item_id = i;
            break;
        }
    }
}

static int get_subtable_entry_id(int index)
{
    return -index - 2;
    // index 0 maps to -2, index 1 to -3, etc.
    // this is done because all positive IDs are reserved for item entries
    // and the id -1 is reserved for empty entries.
}

static void init_entry(const cJSON* entry_data, LootPool* pool, const int entry_id, LootTableContext* ctx)
{
    cJSON* weight_element = cJSON_GetObjectItem(entry_data, "weight");
    pool->total_weight += weight_element == NULL ? 1 : weight_element->valueint;

    cJSON* type_field = cJSON_GetObjectItem(entry_data, "type");
    if (strcmp(type_field->valuestring, "minecraft:loot_table") == 0)
    {
        cJSON* value_field = cJSON_GetObjectItem(entry_data, "value");

        // search the array of unresolved loot tables for this entry
        int ltid = 0;
        for (int i = 0; i < ctx->unresolved_subtable_count; i++) {
            if (strcmp(ctx->unresolved_subtable_names[i], value_field->valuestring) == 0) {
                ltid = get_subtable_entry_id(i);
                break;
            }
        }
        if (ltid == 0) {
            // add the loot table to the unresolved list
            int aid = ctx->unresolved_subtable_count;
            ctx->unresolved_subtable_names[aid] = (char*)malloc(strlen(value_field->valuestring) + 1);
            strcpy(ctx->unresolved_subtable_names[aid], value_field->valuestring);
            ltid = get_subtable_entry_id(aid);

            ctx->unresolved_subtable_count++;
        }

        // initialize entry to item (or loot table in this case) mapping field
        pool->entry_to_item[entry_id] = ltid;

        // initialize loot function fields
        pool->entry_functions_count[entry_id] = 0;
        int new_index = 0;
        if (entry_id > 0)
            new_index = pool->entry_functions_index[entry_id - 1] + pool->entry_functions_count[entry_id - 1];
        pool->entry_functions_index[entry_id] = new_index;
    }
    else // minecraft:item
    {
        int functions = 0;
        cJSON* functions_field = cJSON_GetObjectItem(entry_data, "functions");
        if (functions_field != NULL) {
            functions = cJSON_GetArraySize(functions_field);
        }

        // initialize loot function fields

        pool->entry_functions_count[entry_id] = functions;
        int new_index = 0;
        if (entry_id > 0)
            new_index = pool->entry_functions_index[entry_id - 1] + pool->entry_functions_count[entry_id - 1];
        pool->entry_functions_index[entry_id] = new_index;

        // initialize entry to item mapping field

        pool->entry_to_item[entry_id] = -1; // assume it's an empty entry for now
        map_entry_to_item(entry_data, ctx, &(pool->entry_to_item[entry_id]));

        // loot function initialization will be done after all entries are processed
        // because we need to know the total number of functions to allocate the array
    }
}

static void init_entry_functions(const cJSON* entry_data, LootPool* pool, const int entry_id, LootTableContext* ctx)
{
    cJSON* functions_field = cJSON_GetObjectItem(entry_data, "functions");
    if (functions_field == NULL)
        return;

    const int entry_item = pool->entry_to_item[entry_id];
    const char* entry_item_name = entry_item == -1 ? NULL : ctx->item_names[entry_item];
    if (entry_item_name == NULL)
        return; // empty entry

    int findex = pool->entry_functions_index[entry_id];
    int fcount = pool->entry_functions_count[entry_id];

    for (int i = 0; i < fcount; i++) {
        int index_in_array = findex + i;
        LootFunction* loot_function = &(pool->loot_functions[index_in_array]);

        cJSON* function_data = cJSON_GetArrayItem(functions_field, i);
        char* function_name = cJSON_GetStringValue(cJSON_GetObjectItem(function_data, "function"));

        if (strcmp(function_name, "minecraft:set_count") == 0) {
            parse_set_count(loot_function, function_data);
        }
        else if (strcmp(function_name, "minecraft:enchant_with_levels") == 0) {
            parse_enchant_with_levels(ctx, loot_function, function_data, entry_item_name);
        }
        else if (strcmp(function_name, "minecraft:enchant_randomly") == 0) {
            parse_enchant_randomly(ctx, loot_function, function_data, entry_item_name);
        }
        else if (strcmp(function_name, "minecraft:set_damage") == 0) {
            create_set_damage(loot_function);
        }
        else if (strcmp(function_name, "minecraft:set_stew_effect") == 0) {
            create_set_effect(loot_function);
        }
        else if (strcmp(function_name, "minecraft:set_ominous_bottle_amplifier") == 0) {
            create_skip_calls(loot_function, 1);
        }
        else {
            create_no_op(loot_function);
        }
    }
}

static void precompute_loot_pool(LootPool* pool, const cJSON* entries_field)
{
    int index = 0;

    for (int i = 0; i < pool->entry_count; i++) {
        cJSON* entry = cJSON_GetArrayItem(entries_field, i);
        cJSON* entry_weight = cJSON_GetObjectItem(entry, "weight");
        int w = entry_weight == NULL ? 1 : entry_weight->valueint;

        for (int j = 0; j < w; j++) {
            pool->precomputed_loot[index] = i;
            index++;
        }
    }
}

static int init_loot_pool(const cJSON* pool_data, const int pool_id, LootTableContext* ctx)
{
    LootPool* pool = &(ctx->loot_pools[pool_id]);
    pool->total_weight = 0;
    init_rolls(pool_data, pool);

    // count entries
    cJSON* entries = cJSON_GetObjectItem(pool_data, "entries");
    pool->entry_count = cJSON_GetArraySize(entries);

    // allocate memory for entry data
    pool->entry_to_item = (int*)malloc(pool->entry_count * sizeof(int));
    pool->entry_functions_index = (int*)malloc(pool->entry_count * sizeof(int));
    pool->entry_functions_count = (int*)malloc(pool->entry_count * sizeof(int));
    if (pool->entry_to_item == NULL || pool->entry_functions_index == NULL || pool->entry_functions_count == NULL)
        return -1;

    // first pass: count total loot functions and create simple entry mappings
    for (int i = 0; i < pool->entry_count; i++) {
        cJSON* entry = cJSON_GetArrayItem(entries, i);
        init_entry(entry, pool, i, ctx);
    }

    // second pass: initialize loot functions
    int last_ix = pool->entry_count - 1;
    int function_count = pool->entry_functions_index[last_ix] + pool->entry_functions_count[last_ix];
    pool->loot_functions = (LootFunction*)malloc(function_count * sizeof(LootFunction));
    if (pool->loot_functions == NULL)
        return -1; // FIXME add logging

    for (int i = 0; i < pool->entry_count; i++) {
        cJSON* entry = cJSON_GetArrayItem(entries, i);
        init_entry_functions(entry, pool, i, ctx);
    }

    // final pass: precompute loot table
    pool->precomputed_loot = (int*)malloc(pool->total_weight * sizeof(int));
    if (pool->precomputed_loot == NULL)
        return -1; // FIXME add logging

    precompute_loot_pool(pool, entries);

    return 0;
}

void free_loot_function(LootFunction* lf)
{
    if (lf->varparams_int != NULL)
        free(lf->varparams_int);

    if (lf->varparams_int_arr != NULL)
    {
        for (int i = 0; i < lf->varparams_int_arr_size; i++)
            free(lf->varparams_int_arr[i]);
        free(lf->varparams_int_arr);
        lf->varparams_int_arr_size = 0;
    }

    lf->params = NULL;
    lf->fun = NULL;
}

static void free_loot_pool(LootPool* pool)
{
    free(pool->precomputed_loot);

    free(pool->entry_to_item);

    int last = pool->entry_count - 1;
    int functionCount = pool->entry_functions_index[last] + pool->entry_functions_count[last];
    free(pool->entry_functions_index);
    free(pool->entry_functions_count);

    for (int i = 0; i < functionCount; i++)
        free_loot_function(&(pool->loot_functions[i]));

    free(pool->loot_functions);
}

// -------------------------------------------------------------------------------------

static char* get_loot_table_from_file(FILE* file)
{
    if (file == NULL)
        return NULL;

    int file_size = 0;
    for (char c = getc(file); c != EOF; c = getc(file))
        file_size++;

    fseek(file, 0, SEEK_SET);        // go back to the beginning of the file

    // allocate memory for file contents
    char* file_content = (char*)malloc(file_size + 1); // 1
    if (file_content == NULL) {
        return NULL;
    }

    // read the file content
    fread(file_content, 1, file_size, file);
    file_content[file_size] = '\0';
    return file_content;
}

int init_loot_table(const char* loot_table_string, LootTableContext* context, const int version)
{
    context->version = version;

    cJSON* loot_table = cJSON_Parse(loot_table_string);
    if (loot_table == NULL) {
        return -1; // failed to parse string into JSON
    }

    // initialize item names
    init_loot_table_items(loot_table, context);

    // initialize subtables
    context->unresolved_subtable_count = 0;
    context->subtable_count = 0;
    context->subtable_pool_offset = NULL;
    context->subtable_pool_count = NULL;

    // count loot pools
    const cJSON* pools = cJSON_GetObjectItem(loot_table, "pools");
    context->pool_count = cJSON_GetArraySize(pools);

    // allocate memory for loot pools
    context->loot_pools = (LootPool*)malloc(context->pool_count * sizeof(LootPool));
    if (context->loot_pools == NULL) {
        cJSON_Delete(loot_table);
        return -1;
    }

    // initialize loot pools
    for (int i = 0; i < context->pool_count; i++) {
        if (init_loot_pool(cJSON_GetArrayItem(pools, i), i, context) != 0) {
            cJSON_Delete(loot_table);
            free_loot_table(context);
            return -1;
        }
    }

    // allocate memory (if needed) for subtable pool offset and count arrays
    if (context->unresolved_subtable_count > 0)
    {
        context->subtable_pool_offset = (int*)malloc(context->unresolved_subtable_count * sizeof(int));
        context->subtable_pool_count = (int*)malloc(context->unresolved_subtable_count * sizeof(int));
    }

    cJSON_Delete(loot_table);
    return 0;
}

int init_loot_table_file(FILE* loot_table_file, LootTableContext* context, const int version)
{
    char* file_content = get_loot_table_from_file(loot_table_file);
    if (file_content == NULL) {
        LOG_ERROR("Failed to read loot table file.");
        return -1;
    }
    int ret = init_loot_table(file_content, context, version);
    free(file_content);
    return ret;
}

// -------------------------------------------------------------------------------------

static int merge_item_lists(LootTableContext* ctx, LootTableContext* sub_ctx)
{
    int total_unique_items = ctx->item_count;
    for (int i = 0; i < sub_ctx->item_count; i++)
    {
        int found = 0;
        for (int j = 0; j < ctx->item_count; j++)
        {
            if (strcmp(ctx->item_names[j], sub_ctx->item_names[i]) == 0)
            {
                found = 1;
                break;
            }
        }
        if (!found)
            total_unique_items++;
    }

    // allocate memory for the new item names
    char** new_item_names = (char**)malloc(total_unique_items * sizeof(char*));
    int* new_global_item_ids = malloc(total_unique_items * sizeof(int));
    if (new_item_names == NULL || new_global_item_ids == NULL)
        return -1; // failed to allocate memory

    // copy the original item names without changes
    for (int i = 0; i < ctx->item_count; i++)
    {
        new_item_names[i] = (char*)malloc(strlen(ctx->item_names[i]) + 1);
        if (new_item_names[i] == NULL)
            return -1; // failed to allocate memory
        strcpy(new_item_names[i], ctx->item_names[i]);
        new_global_item_ids[i] = ctx->global_item_ids[i];
    }

    // copy subtable item names not present in the original context
    int new_item_id = ctx->item_count;
    for (int i = 0; i < sub_ctx->item_count; i++)
    {
        int found = 0;
        for (int j = 0; j < ctx->item_count; j++)
        {
            if (strcmp(ctx->item_names[j], sub_ctx->item_names[i]) == 0)
            {
                found = 1;
                break;
            }
        }
        if (!found)
        {
            new_item_names[new_item_id] = (char*)malloc(strlen(sub_ctx->item_names[i]) + 1);
            if (new_item_names[new_item_id] == NULL)
                return -1; // failed to allocate memory
            strcpy(new_item_names[new_item_id], sub_ctx->item_names[i]);
            new_global_item_ids[new_item_id] = sub_ctx->global_item_ids[i];
            new_item_id++;
        }
    }

    // replace the old item names in the original context
    for (int i = 0; i < ctx->item_count; i++)
        free(ctx->item_names[i]);
    free(ctx->item_names);
    free(ctx->global_item_ids);
    ctx->item_names = new_item_names;
    ctx->global_item_ids = new_global_item_ids;
    ctx->item_count = total_unique_items;

    return 0;
}

static int merge_loot_pools(LootTableContext* ctx, LootTableContext* sub_ctx)
{
    // allocate extra memory for the new loot pools
    int total_pools = ctx->pool_count;
    for (int i = 0; i < ctx->subtable_count; i++)
        total_pools += ctx->subtable_pool_count[i];
    total_pools += sub_ctx->pool_count;

    ctx->loot_pools = (LootPool*)realloc(ctx->loot_pools, total_pools * sizeof(LootPool));
    if (ctx->loot_pools == NULL)
        return -1; // failed to allocate memory

    // copy the loot pools from the subtable context, updating the item IDs
    for (int i = 0; i < sub_ctx->pool_count; i++)
    {
        LootPool* old_pool = &(sub_ctx->loot_pools[i]);
        const int new_index = total_pools - sub_ctx->pool_count + i;
        LootPool* new_pool = &(ctx->loot_pools[new_index]);
        memcpy(new_pool, old_pool, sizeof(LootPool));

        // update the item IDs in the loot functions
        for (int j = 0; j < new_pool->entry_count; j++)
        {
            int item_id = new_pool->entry_to_item[j];
            const char* item_name = item_id == -1 ? NULL : sub_ctx->item_names[item_id];
            if (item_name == NULL)
                continue; // empty entry, ignore

            // search for the item name in the original context
            int found = 0;
            for (int k = 0; k < ctx->item_count; k++)
            {
                if (strcmp(ctx->item_names[k], item_name) == 0)
                {
                    new_pool->entry_to_item[j] = k;
                    found = 1;
                    break;
                }
            }
            if (!found)
            {
                // item should always be found, this is an error.
                LOG_ERROR("Subtable item failed to be mapped in the parent context:");
                fprintf(stderr, "Item: %s (id = %d)\n", item_name, item_id);
                return -1;
            }
        }
    }

    // free the subtable loot pool array - its contents were copied to the parent context
    free(sub_ctx->loot_pools);

    return 0;
}

int resolve_subtable(LootTableContext* context, const char* subtable_name, const char* subtable_string)
{
    if (context->unresolved_subtable_count == 0)
    {
        LOG_ERROR("The loot table has no unresolved subtables");
        return -1;
    }

    LootTableContext subtable_context;
    if (init_loot_table(subtable_string, &subtable_context, context->version) != 0)
    {
        LOG_ERROR("Could not parse subtable.");
        return -1;
    }

    // make sure the subtable doesn't list any subtables itself
    if (subtable_context.unresolved_subtable_count > 0 || subtable_context.subtable_count > 0)
    {
        LOG_ERROR("Doubly nested subtables are not supported.");
        free_loot_table(&subtable_context);
        return -1;
    }

    // check if the subtable has the correct index (it must always be the first
    // unresolved subtable in the context, we need to check for safety)
    int first_unresolved = context->subtable_count;
    if (strcmp(context->unresolved_subtable_names[first_unresolved], subtable_name) != 0)
    {
        LOG_ERROR("Incorrect subtable resolvement order.\nAll subtables must be resolved in order of appearance in the loot table:");
        fprintf(stderr, "Expected: %s\n", context->unresolved_subtable_names[first_unresolved]);
        fprintf(stderr, "Got: %s\n", subtable_name);
        free_loot_table(&subtable_context);
        return -1;
    }
    int subtable_index = first_unresolved;

    // merge item name lists
    if (merge_item_lists(context, &subtable_context) != 0)
    {
        free_loot_table(&subtable_context);
        LOG_ERROR("Failed to merge item lists");
        return -1;
    }

    // merge loot pools (most complex step)
    if (merge_loot_pools(context, &subtable_context) != 0)
    {
        free_loot_table(&subtable_context);
        LOG_ERROR("Failed to merge loot pools");
        return -1;
    }

    // mark subtable as resolved and update the context
    // to properly link the added loot pools
    free(context->unresolved_subtable_names[subtable_index]);
    context->unresolved_subtable_names[subtable_index] = NULL;
    context->unresolved_subtable_count--;

    if (subtable_index == 0)
        context->subtable_pool_offset[subtable_index] = context->pool_count;
    else
        context->subtable_pool_offset[subtable_index] = context->subtable_pool_offset[subtable_index - 1] + context->subtable_pool_count[subtable_index - 1];

    context->subtable_pool_count[subtable_index] = subtable_context.pool_count;
    context->subtable_count++;

    // clean up the subtable context memory.
    // - item names were copied during merge, so free them
    // - loot pools were freed properly in the merge function
    // - subtable fields were not allocated, so ignore them
    for (int i = 0; i < subtable_context.item_count; i++)
        free(subtable_context.item_names[i]);
    free(subtable_context.item_names);
    free(subtable_context.global_item_ids);

    return 0;
}

int resolve_subtable_file(LootTableContext* context, const char* subtable_name, FILE* subtable_file)
{
    char* file_content = get_loot_table_from_file(subtable_file);
    if (file_content == NULL) {
        LOG_ERROR("Failed to read loot table file.");
        return -1;
    }
    int ret = resolve_subtable(context, subtable_name, file_content);
    free(file_content);
    return ret;
}

void free_loot_table(LootTableContext* context)
{
    // free item name arrays
    for (int i = 0; i < context->item_count; i++)
        free(context->item_names[i]);
    free(context->item_names);
    free(context->global_item_ids);

    // free unresolved subtable names
    for (int i = 0; i < context->unresolved_subtable_count; i++)
        if (context->unresolved_subtable_names[i] != NULL)
            free(context->unresolved_subtable_names[i]);

    // count total loot pools (incl. subtables)
    int total_pools = context->pool_count;
    for (int i = 0; i < context->subtable_count; i++)
        total_pools += context->subtable_pool_count[i];

    // free loot pools
    for (int i = 0; i < total_pools; i++)
        free_loot_pool(&(context->loot_pools[i]));
    free(context->loot_pools);

    // free subtable pool offset and count arrays
    if (context->subtable_pool_offset != NULL)
        free(context->subtable_pool_offset);
    if (context->subtable_pool_count != NULL)
        free(context->subtable_pool_count);
}

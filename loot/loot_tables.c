#include <stdio.h>
#include <string.h>

#include "loot_tables.h"
#include "loot_table_context.h"

#include "loot_tables/desert_pyramid_1_13.h"
#include "loot_tables/ruined_portal_1_21.h"

int init_loot_table_name(LootTableContext* context, const char* loot_table, int version) {
    if (strcmp(loot_table, "ruined_portal") == 0) {
        return init_ruined_portal(context, version);
    }
    if (strcmp(loot_table, "desert_pyramid") == 0) {
        return init_desert_pyramid(context, version);
    }
    fprintf(stderr, "ERR init_loot_table_name: unsupported loot_table %s\n", loot_table);
    memset(context, 0, sizeof(LootTableContext));
    return 0;
}

int init_desert_pyramid(LootTableContext* context, int version) {
    if (1) {
        return init_desert_pyramid_1_13(context);
    }
}

int init_ruined_portal(LootTableContext* context, int version) {
    if (1) {
        return init_ruined_portal_1_21(context);
    }
}

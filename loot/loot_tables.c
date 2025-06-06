#include <stdio.h>
#include <string.h>

#include "loot_tables.h"
#include "loot_table_context.h"
#include "../biomes.h"

#include "loot_tables/buried_treasure_1_13.h"
#include "loot_tables/buried_treasure_1_18.h"
#include "loot_tables/desert_pyramid_1_13.h"
#include "loot_tables/desert_pyramid_1_20.h"
#include "loot_tables/igloo_chest_1_13.h"
#include "loot_tables/nether_bridge_1_13.h"
#include "loot_tables/nether_bridge_1_20.h"
#include "loot_tables/ruined_portal_1_16_1.h"
#include "loot_tables/ruined_portal_1_21_5.h"

int init_loot_table_name(LootTableContext* context, const char* loot_table, int version) {
    if (strcmp(loot_table, "buried_treasure") == 0) {
        return init_buried_treasure(context, version);
    }
    if (strcmp(loot_table, "desert_pyramid") == 0) {
        return init_desert_pyramid(context, version);
    }
    if (strcmp(loot_table, "igloo_chest") == 0) {
        return init_igloo_chest(context, version);
    }
    if (strcmp(loot_table, "nether_bridge") == 0) {
        return init_nether_bridge(context, version);
    }
    if (strcmp(loot_table, "ruined_portal") == 0) {
        return init_ruined_portal(context, version);
    }
    fprintf(stderr, "ERR init_loot_table_name: unsupported loot_table %s\n", loot_table);
    memset(context, 0, sizeof(LootTableContext));
    return 0;
}

int init_buried_treasure(LootTableContext* context, int version) {
    if (version < MC_1_18) init_buried_treasure_1_13(context);
    else init_buried_treasure_1_18(context);
    return version > MC_1_12;
}

int init_desert_pyramid(LootTableContext* context, int version) {
    if (version < MC_1_20) init_desert_pyramid_1_13(context);
    else init_desert_pyramid_1_20(context);
    return version > MC_1_12;
}

int init_igloo_chest(LootTableContext* context, int version) {
    init_igloo_chest_1_13(context);
    return version > MC_1_12;
}

int init_nether_bridge(LootTableContext* context, int version) {
    if (version < MC_1_20) init_nether_bridge_1_13(context);
    else init_nether_bridge_1_20(context);
    return version > MC_1_12;
}

int init_ruined_portal(LootTableContext* context, int version) {
    if (version < MC_1_21_5) init_ruined_portal_1_16_1(context);
    else init_ruined_portal_1_21_5(context);
    return version > MC_1_15;
}

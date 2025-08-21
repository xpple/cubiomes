#include <stdio.h>
#include <string.h>

#include "loot_tables.h"
#include "loot_table_context.h"
#include "loot_table_parser.h"
#include "../biomes.h"

#include "loot_tables/bastion_bridge_1_16_1.h"
#include "loot_tables/bastion_bridge_1_16_5.h"
#include "loot_tables/bastion_bridge_1_20.h"
#include "loot_tables/bastion_other_1_16_1.h"
#include "loot_tables/bastion_other_1_16_5.h"
#include "loot_tables/bastion_other_1_20.h"
#include "loot_tables/bastion_other_1_21_1.h"
#include "loot_tables/buried_treasure_1_13.h"
#include "loot_tables/buried_treasure_1_18.h"
#include "loot_tables/desert_pyramid_1_13.h"
#include "loot_tables/desert_pyramid_1_20.h"
#include "loot_tables/end_city_treasure_1_13.h"
#include "loot_tables/end_city_treasure_1_20.h"
#include "loot_tables/igloo_chest_1_13.h"
#include "loot_tables/jungle_temple_1_13.h"
#include "loot_tables/jungle_temple_1_14.h"
#include "loot_tables/jungle_temple_1_20.h"
#include "loot_tables/jungle_temple_dispenser_1_13.h"
#include "loot_tables/nether_bridge_1_13.h"
#include "loot_tables/nether_bridge_1_20.h"
#include "loot_tables/pillager_outpost_1_14.h"
#include "loot_tables/pillager_outpost_1_19_2.h"
#include "loot_tables/pillager_outpost_1_20.h"
#include "loot_tables/ruined_portal_1_16_1.h"
#include "loot_tables/ruined_portal_1_21_5.h"
#include "loot_tables/shipwreck_map_1_13.h"
#include "loot_tables/shipwreck_map_1_18.h"
#include "loot_tables/shipwreck_map_1_20.h"
#include "loot_tables/shipwreck_supply_1_13.h"
#include "loot_tables/shipwreck_supply_1_14.h"
#include "loot_tables/shipwreck_supply_1_17.h"
#include "loot_tables/shipwreck_supply_1_20.h"
#include "loot_tables/shipwreck_treasure_1_13.h"
#include "loot_tables/shipwreck_treasure_1_20.h"

int init_loot_table_name(LootTableContext* context, const char* loot_table, int version) {
    if (strcmp(loot_table, "bastion_bridge") == 0) {
        return init_bastion_bridge(context, version);
    }
    if (strcmp(loot_table, "bastion_other") == 0) {
        return init_bastion_other(context, version);
    }
    if (strcmp(loot_table, "buried_treasure") == 0) {
        return init_buried_treasure(context, version);
    }
    if (strcmp(loot_table, "desert_pyramid") == 0) {
        return init_desert_pyramid(context, version);
    }
    if (strcmp(loot_table, "end_city_treasure") == 0) {
        return init_end_city_treasure(context, version);
    }
    if (strcmp(loot_table, "igloo_chest") == 0) {
        return init_igloo_chest(context, version);
    }
    if (strcmp(loot_table, "jungle_temple") == 0) {
        return init_jungle_temple(context, version);
    }
    if (strcmp(loot_table, "jungle_temple_dispenser") == 0) {
        return init_jungle_temple_dispenser(context, version);
    }
    if (strcmp(loot_table, "nether_bridge") == 0) {
        return init_nether_bridge(context, version);
    }
    if (strcmp(loot_table, "pillager_outpost") == 0) {
        return init_pillager_outpost(context, version);
    }
    if (strcmp(loot_table, "ruined_portal") == 0) {
        return init_ruined_portal(context, version);
    }
    if (strcmp(loot_table, "shipwreck_map") == 0) {
        return init_shipwreck_map(context, version);
    }
    if (strcmp(loot_table, "shipwreck_supply") == 0) {
        return init_shipwreck_supply(context, version);
    }
    if (strcmp(loot_table, "shipwreck_treasure") == 0) {
        return init_shipwreck_treasure(context, version);
    }
    fprintf(stderr, "ERR init_loot_table_name: unsupported loot_table %s\n", loot_table);
    memset(context, 0, sizeof(LootTableContext));
    return 0;
}

int init_bastion_bridge(LootTableContext* context, int version) {
    if (version < MC_1_16_5) init_bastion_bridge_1_16_1(context);
    else if (version < MC_1_20) init_bastion_bridge_1_16_5(context);
    else init_bastion_bridge_1_20(context);
    return version > MC_1_15;
}

int init_bastion_other(LootTableContext* context, int version) {
    if (version < MC_1_16_5) init_bastion_other_1_16_1(context);
    else if (version < MC_1_20) init_bastion_other_1_16_5(context);
    else if (version < MC_1_21_1) init_bastion_other_1_20(context);
    else init_bastion_other_1_21_1(context);
    return version > MC_1_15;
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

int init_end_city_treasure(LootTableContext * context, int version) {
    if (version < MC_1_20) init_end_city_treasure_1_13(context);
    else init_end_city_treasure_1_20(context);
    return version > MC_1_12;
}

int init_igloo_chest(LootTableContext* context, int version) {
    init_igloo_chest_1_13(context);
    return version > MC_1_12;
}

int init_jungle_temple(LootTableContext* context, int version) {
    if (version < MC_1_14) init_jungle_temple_1_13(context);
    else if (version < MC_1_20) init_jungle_temple_1_14(context);
    else init_jungle_temple_1_20(context);
    return version > MC_1_12;
}

int init_jungle_temple_dispenser(LootTableContext* context, int version) {
    init_jungle_temple_dispenser_1_13(context);
    return version > MC_1_12;
}

int init_nether_bridge(LootTableContext* context, int version) {
    if (version < MC_1_20) init_nether_bridge_1_13(context);
    else init_nether_bridge_1_20(context);
    return version > MC_1_12;
}

int init_pillager_outpost(LootTableContext* context, int version) {
    if (version < MC_1_19_2) init_pillager_outpost_1_14(context);
    else if (version < MC_1_20) init_pillager_outpost_1_19_2(context);
    else init_pillager_outpost_1_20(context);
    return version > MC_1_13;
}

int init_ruined_portal(LootTableContext* context, int version) {
    if (version < MC_1_21_5) init_ruined_portal_1_16_1(context);
    else init_ruined_portal_1_21_5(context);
    return version > MC_1_15;
}

int init_shipwreck_map(LootTableContext* context, int version) {
    if (version < MC_1_18) init_shipwreck_map_1_13(context);
    else if (version < MC_1_20) init_shipwreck_map_1_18(context);
    else init_shipwreck_map_1_20(context);
    return version > MC_1_12;
}
int init_shipwreck_supply(LootTableContext* context, int version) {
    if (version < MC_1_14) init_shipwreck_supply_1_13(context);
    else if (version < MC_1_17) init_shipwreck_supply_1_14(context);
    else if (version < MC_1_20) init_shipwreck_supply_1_17(context);
    else init_shipwreck_supply_1_20(context);
    return version > MC_1_12;
}
int init_shipwreck_treasure(LootTableContext* context, int version) {
    if (version < MC_1_20) init_shipwreck_treasure_1_13(context);
    else init_shipwreck_treasure_1_20(context);
    return version > MC_1_12;
}

void free_loot_table_pools(LootTableContext* context) {
    for (int i = 0; i < context->pool_count; ++i) {
        LootPool* pool = &context->loot_pools[i];
        int last = pool->entry_count - 1;
        int functionCount = pool->entry_functions_index[last] + pool->entry_functions_count[last];
        for (int j = 0; j < functionCount; ++j) {
            free_loot_function(&(pool->loot_functions[j]));
        }
        free(pool->loot_functions);
    }
    free(context->loot_pools);
}

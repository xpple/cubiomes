#ifndef LOOT_TABLES_H
#define LOOT_TABLES_H

#include "loot_table_context.h"

int init_loot_table_name(LootTableContext* context, const char* loot_table, int version);

int init_buried_treasure(LootTableContext* context, int version);
int init_desert_pyramid(LootTableContext* context, int version);
int init_igloo_chest(LootTableContext* context, int version);
int init_nether_bridge(LootTableContext* context, int version);
int init_ruined_portal(LootTableContext* context, int version);

#endif //LOOT_TABLES_H

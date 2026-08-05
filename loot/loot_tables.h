#ifndef LOOT_TABLES_H
#define LOOT_TABLES_H

#include "loot_table_context.h"

int init_loot_table_name(LootTableContext** context, const char* loot_table, int version);

int init_bastion_bridge(LootTableContext** context, int version);
int init_bastion_hoglin_stable(LootTableContext** context, int version);
int init_bastion_other(LootTableContext** context, int version);
int init_bastion_treasure(LootTableContext** context, int version);
int init_buried_treasure(LootTableContext** context, int version);
int init_desert_pyramid(LootTableContext** context, int version);
int init_end_city_treasure(LootTableContext ** context, int version);
int init_igloo_chest(LootTableContext** context, int version);
int init_jungle_temple(LootTableContext** context, int version);
int init_jungle_temple_dispenser(LootTableContext** context, int version);
int init_nether_bridge(LootTableContext** context, int version);
int init_pillager_outpost(LootTableContext** context, int version);
int init_ruined_portal(LootTableContext** context, int version);
int init_shipwreck_map(LootTableContext** context, int version);
int init_shipwreck_supply(LootTableContext** context, int version);
int init_shipwreck_treasure(LootTableContext** context, int version);
int init_simple_dungeon(LootTableContext** context, int version);
int init_village_armorer(LootTableContext** context, int version);
int init_village_butcher(LootTableContext** context, int version);
int init_village_cartographer(LootTableContext** context, int version);
int init_village_desert_house(LootTableContext** context, int version);
int init_village_fisher(LootTableContext** context, int version);
int init_village_fletcher(LootTableContext** context, int version);
int init_village_mason(LootTableContext** context, int version);
int init_village_plains_house(LootTableContext** context, int version);
int init_village_savanna_house(LootTableContext** context, int version);
int init_village_shepherd(LootTableContext** context, int version);
int init_village_snowy_house(LootTableContext** context, int version);
int init_village_taiga_house(LootTableContext** context, int version);
int init_village_tannery(LootTableContext** context, int version);
int init_village_temple(LootTableContext** context, int version);
int init_village_toolsmith(LootTableContext** context, int version);
int init_village_weaponsmith(LootTableContext** context, int version);
int init_stronghold_corridor(LootTableContext** context, int version);
int init_stronghold_crossing(LootTableContext** context, int version);
int init_stronghold_library(LootTableContext** context, int version);

#endif //LOOT_TABLES_H

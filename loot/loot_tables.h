#ifndef LOOT_TABLES_H
#define LOOT_TABLES_H

#include "loot_table_context.h"

/// Selects a built-in loot table by name, e.g. "chests/buried_treasure", for
/// the given Minecraft version. The returned context is shared static storage
/// and must not be passed to `free_loot_table`. Set its seed before generation.
/// Repeated calls may return the same mutable context. Thus, synchronize access
/// when sharing it between threads.
/// \param[out] context pointer receiving the selected context
/// \param[in] loot_table loot table name without the "minecraft:" prefix
/// \param version Minecraft version
/// \return nonzero if the table and version are supported
int init_loot_table_name(LootTableContext** context, const char* loot_table, int version);

int init_abandoned_camp_barrel(LootTableContext** context, int version);
int init_abandoned_camp_common_chest(LootTableContext** context, int version);
int init_abandoned_camp_secret_chest(LootTableContext** context, int version);
int init_abandoned_mineshaft(LootTableContext** context, int version);
int init_bastion_bridge(LootTableContext** context, int version);
int init_bastion_other(LootTableContext** context, int version);
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
int init_stronghold_corridor(LootTableContext** context, int version);
int init_stronghold_crossing(LootTableContext** context, int version);
int init_stronghold_library(LootTableContext** context, int version);
int init_reward(LootTableContext** context, int version);
int init_reward_common(LootTableContext** context, int version);
int init_reward_ominous(LootTableContext** context, int version);
int init_reward_ominous_common(LootTableContext** context, int version);
int init_reward_ominous_rare(LootTableContext** context, int version);
int init_reward_ominous_unique(LootTableContext** context, int version);
int init_reward_rare(LootTableContext** context, int version);
int init_reward_unique(LootTableContext** context, int version);

#endif //LOOT_TABLES_H

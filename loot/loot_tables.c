#include <stdio.h>
#include <string.h>

#include "loot_tables.h"
#include "loot_table_context.h"
#include "../biomes.h"

#include "loot_tables/bastion_bridge_1_16_1.h"
#include "loot_tables/bastion_bridge_1_16_5.h"
#include "loot_tables/bastion_bridge_1_20.h"
#include "loot_tables/bastion_hoglin_stable_1_16_1.h"
#include "loot_tables/bastion_hoglin_stable_1_16_5.h"
#include "loot_tables/bastion_other_1_16_1.h"
#include "loot_tables/bastion_other_1_16_5.h"
#include "loot_tables/bastion_other_1_20.h"
#include "loot_tables/bastion_other_1_21_1.h"
#include "loot_tables/bastion_other_1_21_9.h"
#include "loot_tables/bastion_treasure_1_16_1.h"
#include "loot_tables/bastion_treasure_1_16_5.h"
#include "loot_tables/buried_treasure_1_13.h"
#include "loot_tables/buried_treasure_1_18.h"
#include "loot_tables/buried_treasure_1_21_11.h"
#include "loot_tables/desert_pyramid_1_13.h"
#include "loot_tables/desert_pyramid_1_20.h"
#include "loot_tables/desert_pyramid_1_21_6.h"
#include "loot_tables/desert_pyramid_1_21_9.h"
#include "loot_tables/desert_pyramid_1_21_11.h"
#include "loot_tables/end_city_treasure_1_13.h"
#include "loot_tables/end_city_treasure_1_20.h"
#include "loot_tables/end_city_treasure_1_21_9.h"
#include "loot_tables/end_city_treasure_1_21_11.h"
#include "loot_tables/igloo_chest_1_13.h"
#include "loot_tables/jungle_temple_1_13.h"
#include "loot_tables/jungle_temple_1_14.h"
#include "loot_tables/jungle_temple_1_20.h"
#include "loot_tables/jungle_temple_1_21_6.h"
#include "loot_tables/jungle_temple_1_21_9.h"
#include "loot_tables/jungle_temple_1_21_11.h"
#include "loot_tables/jungle_temple_dispenser_1_13.h"
#include "loot_tables/nether_bridge_1_13.h"
#include "loot_tables/nether_bridge_1_20.h"
#include "loot_tables/nether_bridge_1_21_9.h"
#include "loot_tables/pillager_outpost_1_14.h"
#include "loot_tables/pillager_outpost_1_19_2.h"
#include "loot_tables/pillager_outpost_1_20.h"
#include "loot_tables/pillager_outpost_1_21_11.h"
#include "loot_tables/ruined_portal_1_16_1.h"
#include "loot_tables/ruined_portal_1_21_5.h"
#include "loot_tables/shipwreck_map_1_13.h"
#include "loot_tables/shipwreck_map_1_18.h"
#include "loot_tables/shipwreck_map_1_20.h"
#include "loot_tables/shipwreck_map_1_21_11.h"
#include "loot_tables/shipwreck_supply_1_13.h"
#include "loot_tables/shipwreck_supply_1_14.h"
#include "loot_tables/shipwreck_supply_1_17.h"
#include "loot_tables/shipwreck_supply_1_20.h"
#include "loot_tables/shipwreck_supply_1_21_11.h"
#include "loot_tables/shipwreck_treasure_1_13.h"
#include "loot_tables/shipwreck_treasure_1_20.h"
#include "loot_tables/shipwreck_treasure_1_21_11.h"
#include "loot_tables/simple_dungeon_1_14.h"
#include "loot_tables/village_armorer_1_16_1.h"
#include "loot_tables/village_butcher_1_16_1.h"
#include "loot_tables/village_cartographer_1_16_1.h"
#include "loot_tables/village_desert_house_1_16_1.h"
#include "loot_tables/village_fisher_1_16_1.h"
#include "loot_tables/village_fletcher_1_16_1.h"
#include "loot_tables/village_mason_1_16_1.h"
#include "loot_tables/village_plains_house_1_16_1.h"
#include "loot_tables/village_savanna_house_1_16_1.h"
#include "loot_tables/village_shepherd_1_16_1.h"
#include "loot_tables/village_snowy_house_1_16_1.h"
#include "loot_tables/village_taiga_house_1_16_1.h"
#include "loot_tables/village_tannery_1_16_1.h"
#include "loot_tables/village_temple_1_16_1.h"
#include "loot_tables/village_toolsmith_1_16_1.h"
#include "loot_tables/village_weaponsmith_1_16_1.h"
#include "loot_tables/stronghold_corridor_1_13.h"
#include "loot_tables/stronghold_corridor_1_18.h"
#include "loot_tables/stronghold_corridor_1_20.h"
#include "loot_tables/stronghold_corridor_1_21_6.h"
#include "loot_tables/stronghold_corridor_1_21_9.h"
#include "loot_tables/stronghold_crossing_1_13.h"
#include "loot_tables/stronghold_library_1_13.h"
#include "loot_tables/stronghold_library_1_20.h"

int init_loot_table_name(LootTableContext** context, const char* loot_table, int version) {
    if (strcmp(loot_table, "bastion_bridge") == 0) {
        return init_bastion_bridge(context, version);
    }
    if (strcmp(loot_table, "bastion_hoglin_stable") == 0) {
        return init_bastion_hoglin_stable(context, version);
    }
    if (strcmp(loot_table, "bastion_other") == 0) {
        return init_bastion_other(context, version);
    }
    if (strcmp(loot_table, "bastion_treasure") == 0) {
        return init_bastion_treasure(context, version);
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
    if (strcmp(loot_table, "simple_dungeon") == 0) {
        return init_simple_dungeon(context, version);
    }
    if (strcmp(loot_table, "village_armorer") == 0) {
        return init_village_armorer(context, version);
    }
    if (strcmp(loot_table, "village_butcher") == 0) {
        return init_village_butcher(context, version);
    }
    if (strcmp(loot_table, "village_cartographer") == 0) {
        return init_village_cartographer(context, version);
    }
    if (strcmp(loot_table, "village_desert_house") == 0) {
        return init_village_desert_house(context, version);
    }
    if (strcmp(loot_table, "village_fisher") == 0) {
        return init_village_fisher(context, version);
    }
    if (strcmp(loot_table, "village_fletcher") == 0) {
        return init_village_fletcher(context, version);
    }
    if (strcmp(loot_table, "village_mason") == 0) {
        return init_village_mason(context, version);
    }
    if (strcmp(loot_table, "village_plains_house") == 0) {
        return init_village_plains_house(context, version);
    }
    if (strcmp(loot_table, "village_savanna_house") == 0) {
        return init_village_savanna_house(context, version);
    }
    if (strcmp(loot_table, "village_shepherd") == 0) {
        return init_village_shepherd(context, version);
    }
    if (strcmp(loot_table, "village_snowy_house") == 0) {
        return init_village_snowy_house(context, version);
    }
    if (strcmp(loot_table, "village_taiga_house") == 0) {
        return init_village_taiga_house(context, version);
    }
    if (strcmp(loot_table, "village_tannery") == 0) {
        return init_village_tannery(context, version);
    }
    if (strcmp(loot_table, "village_temple") == 0) {
        return init_village_temple(context, version);
    }
    if (strcmp(loot_table, "village_toolsmith") == 0) {
        return init_village_toolsmith(context, version);
    }
    if (strcmp(loot_table, "village_weaponsmith") == 0) {
        return init_village_weaponsmith(context, version);
    }
    if (strcmp(loot_table, "stronghold_corridor") == 0) {
        return init_stronghold_corridor(context, version);
    }
    if (strcmp(loot_table, "stronghold_crossing") == 0) {
        return init_stronghold_crossing(context, version);
    }
    if (strcmp(loot_table, "stronghold_library") == 0) {
        return init_stronghold_library(context, version);
    }
    fprintf(stderr, "ERR init_loot_table_name: unsupported loot_table %s\n", loot_table);
    *context = NULL;
    return 0;
}

int init_bastion_bridge(LootTableContext** context, int version) {
    if (version < MC_1_16_5) *context = init_bastion_bridge_1_16_1(version);
    else if (version < MC_1_20) *context = init_bastion_bridge_1_16_5(version);
    else *context = init_bastion_bridge_1_20(version);
    return version > MC_1_15;
}

int init_bastion_hoglin_stable(LootTableContext** context, int version) {
    if (version < MC_1_16_5) *context = init_bastion_hoglin_stable_1_16_1(version);
    else *context = init_bastion_hoglin_stable_1_16_5(version);
    return version > MC_1_15;
}

int init_bastion_treasure(LootTableContext** context, int version) {
    if (version < MC_1_16_5) *context = init_bastion_treasure_1_16_1(version);
    else *context = init_bastion_treasure_1_16_5(version);
    return version > MC_1_15;
}

int init_bastion_other(LootTableContext** context, int version) {
    if (version < MC_1_16_5) *context = init_bastion_other_1_16_1(version);
    else if (version < MC_1_20) *context = init_bastion_other_1_16_5(version);
    else if (version < MC_1_21_1) *context = init_bastion_other_1_20(version);
    else if (version < MC_1_21_9) *context = init_bastion_other_1_21_1(version);
    else *context = init_bastion_other_1_21_9(version);
    return version > MC_1_15;
}

int init_buried_treasure(LootTableContext** context, int version) {
    if (version < MC_1_18) *context = init_buried_treasure_1_13(version);
    else if (version < MC_1_21_11) *context = init_buried_treasure_1_18(version);
    else *context = init_buried_treasure_1_21_11(version);
    return version > MC_1_12;
}

int init_desert_pyramid(LootTableContext** context, int version) {
    if (version < MC_1_20) *context = init_desert_pyramid_1_13(version);
    else if (version < MC_1_21_6) *context = init_desert_pyramid_1_20(version);
    else if (version < MC_1_21_9) *context = init_desert_pyramid_1_21_6(version);
    else if (version < MC_1_21_11) *context = init_desert_pyramid_1_21_9(version);
    else *context = init_desert_pyramid_1_21_11(version);
    return version > MC_1_12;
}

int init_end_city_treasure(LootTableContext** context, int version) {
    if (version < MC_1_20) *context = init_end_city_treasure_1_13(version);
    else if (version < MC_1_21_9) *context = init_end_city_treasure_1_20(version);
    else if (version < MC_1_21_11) *context = init_end_city_treasure_1_21_9(version);
    else *context = init_end_city_treasure_1_21_11(version);
    return version > MC_1_12;
}

int init_igloo_chest(LootTableContext** context, int version) {
    *context = init_igloo_chest_1_13(version);
    return version > MC_1_12;
}

int init_jungle_temple(LootTableContext** context, int version) {
    if (version < MC_1_14) *context = init_jungle_temple_1_13(version);
    else if (version < MC_1_20) *context = init_jungle_temple_1_14(version);
    else if (version < MC_1_21_6) *context = init_jungle_temple_1_20(version);
    else if (version < MC_1_21_9) *context = init_jungle_temple_1_21_6(version);
    else if (version < MC_1_21_11) *context = init_jungle_temple_1_21_9(version);
    else *context = init_jungle_temple_1_21_11(version);
    return version > MC_1_12;
}

int init_jungle_temple_dispenser(LootTableContext** context, int version) {
    *context = init_jungle_temple_dispenser_1_13(version);
    return version > MC_1_12;
}

int init_nether_bridge(LootTableContext** context, int version) {
    if (version < MC_1_20) *context = init_nether_bridge_1_13(version);
    else if (version < MC_1_21_9) *context = init_nether_bridge_1_20(version);
    else *context = init_nether_bridge_1_21_9(version);
    return version > MC_1_12;
}

int init_pillager_outpost(LootTableContext** context, int version) {
    if (version < MC_1_19_2) *context = init_pillager_outpost_1_14(version);
    else if (version < MC_1_20) *context = init_pillager_outpost_1_19_2(version);
    else if (version < MC_1_21_9) *context = init_pillager_outpost_1_20(version);
    else *context = init_pillager_outpost_1_21_11(version);
    return version > MC_1_13;
}

int init_ruined_portal(LootTableContext** context, int version) {
    if (version < MC_1_21_5) *context = init_ruined_portal_1_16_1(version);
    else *context = init_ruined_portal_1_21_5(version);
    return version > MC_1_15;
}

int init_shipwreck_map(LootTableContext** context, int version) {
    if (version < MC_1_18) *context = init_shipwreck_map_1_13(version);
    else if (version < MC_1_20) *context = init_shipwreck_map_1_18(version);
    else if (version < MC_1_21_11) *context = init_shipwreck_map_1_20(version);
    else *context = init_shipwreck_map_1_21_11(version);
    return version > MC_1_12;
}

int init_shipwreck_supply(LootTableContext** context, int version) {
    if (version < MC_1_14) *context = init_shipwreck_supply_1_13(version);
    else if (version < MC_1_17) *context = init_shipwreck_supply_1_14(version);
    else if (version < MC_1_20) *context = init_shipwreck_supply_1_17(version);
    else if (version < MC_1_21_11) *context = init_shipwreck_supply_1_20(version);
    else *context = init_shipwreck_supply_1_21_11(version);
    return version > MC_1_12;
}

int init_shipwreck_treasure(LootTableContext** context, int version) {
    if (version < MC_1_20) *context = init_shipwreck_treasure_1_13(version);
    else if (version < MC_1_21_11) *context = init_shipwreck_treasure_1_20(version);
    else *context = init_shipwreck_treasure_1_21_11(version);
    return version > MC_1_12;
}

int init_simple_dungeon(LootTableContext** context, int version) {
    *context = init_simple_dungeon_1_14(version);
    return version > MC_1_13_2;
}

int init_village_armorer(LootTableContext** context, int version) {
    *context = init_village_armorer_1_16_1(version);
    return version >= MC_1_16_1;
}
int init_village_butcher(LootTableContext** context, int version) {
    *context = init_village_butcher_1_16_1(version);
    return version >= MC_1_16_1;
}
int init_village_cartographer(LootTableContext** context, int version) {
    *context = init_village_cartographer_1_16_1(version);
    return version >= MC_1_16_1;
}
int init_village_desert_house(LootTableContext** context, int version) {
    *context = init_village_desert_house_1_16_1(version);
    return version >= MC_1_16_1;
}
int init_village_fisher(LootTableContext** context, int version) {
    *context = init_village_fisher_1_16_1(version);
    return version >= MC_1_16_1;
}
int init_village_fletcher(LootTableContext** context, int version) {
    *context = init_village_fletcher_1_16_1(version);
    return version >= MC_1_16_1;
}
int init_village_mason(LootTableContext** context, int version) {
    *context = init_village_mason_1_16_1(version);
    return version >= MC_1_16_1;
}
int init_village_plains_house(LootTableContext** context, int version) {
    *context = init_village_plains_house_1_16_1(version);
    return version >= MC_1_16_1;
}
int init_village_savanna_house(LootTableContext** context, int version) {
    *context = init_village_savanna_house_1_16_1(version);
    return version >= MC_1_16_1;
}
int init_village_shepherd(LootTableContext** context, int version) {
    *context = init_village_shepherd_1_16_1(version);
    return version >= MC_1_16_1;
}
int init_village_snowy_house(LootTableContext** context, int version) {
    *context = init_village_snowy_house_1_16_1(version);
    return version >= MC_1_16_1;
}
int init_village_taiga_house(LootTableContext** context, int version) {
    *context = init_village_taiga_house_1_16_1(version);
    return version >= MC_1_16_1;
}
int init_village_tannery(LootTableContext** context, int version) {
    *context = init_village_tannery_1_16_1(version);
    return version >= MC_1_16_1;
}
int init_village_temple(LootTableContext** context, int version) {
    *context = init_village_temple_1_16_1(version);
    return version >= MC_1_16_1;
}
int init_village_toolsmith(LootTableContext** context, int version) {
    *context = init_village_toolsmith_1_16_1(version);
    return version >= MC_1_16_1;
}
int init_village_weaponsmith(LootTableContext** context, int version) {
    *context = init_village_weaponsmith_1_16_1(version);
    return version >= MC_1_16_1;
}

int init_stronghold_corridor(LootTableContext** context, int version) {
    if (version < MC_1_18) *context = init_stronghold_corridor_1_13(version);
    else if (version < MC_1_20) *context = init_stronghold_corridor_1_18(version);
    else if (version < MC_1_21_6) *context = init_stronghold_corridor_1_20(version);
    else if (version < MC_1_21_9) *context = init_stronghold_corridor_1_21_6(version);
    else *context = init_stronghold_corridor_1_21_9(version);
    return version > MC_1_12;
}

int init_stronghold_crossing(LootTableContext** context, int version) {
    *context = init_stronghold_crossing_1_13(version);
    return version > MC_1_12;
}

int init_stronghold_library(LootTableContext** context, int version) {
    if (version < MC_1_20) *context = init_stronghold_library_1_13(version);
    else *context = init_stronghold_library_1_20(version);
    return version > MC_1_12;
}

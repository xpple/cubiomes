#include <stdio.h>
#include <string.h>

#include "../biomes.h"
#include "items.h"

int item_name2global_id(const char* name) {
    static const struct { const char *name; int id; } items[] = {
        {"minecraft:abandoned_camp_map", ITEM_ABANDONED_CAMP_MAP},
        {"minecraft:activator_rail", ITEM_ACTIVATOR_RAIL},
        {"minecraft:ancient_debris", ITEM_ANCIENT_DEBRIS},
        {"minecraft:apple", ITEM_APPLE},
        {"minecraft:arrow", ITEM_ARROW},
        {"minecraft:bamboo", ITEM_BAMBOO},
        {"minecraft:beetroot_seeds", ITEM_BEETROOT_SEEDS},
        {"minecraft:bell", ITEM_BELL},
        {"minecraft:bolt_armor_trim_smithing_template", ITEM_BOLT_ARMOR_TRIM_SMITHING_TEMPLATE},
        {"minecraft:bone", ITEM_BONE},
        {"minecraft:bone_block", ITEM_BONE_BLOCK},
        {"minecraft:book", ITEM_BOOK},
        {"minecraft:bow", ITEM_BOW},
        {"minecraft:bowl", ITEM_BOWL},
        {"minecraft:bread", ITEM_BREAD},
        {"minecraft:bucket", ITEM_BUCKET},
        {"minecraft:bundle", ITEM_BUNDLE},
        {"minecraft:buried_ancient_city_map", ITEM_BURIED_ANCIENT_CITY_MAP},
        {"minecraft:buried_mineshaft_map", ITEM_BURIED_MINESHAFT_MAP},
        {"minecraft:buried_trial_chambers_map", ITEM_BURIED_TRIAL_CHAMBERS_MAP},
        {"minecraft:carrot", ITEM_CARROT},
        {"minecraft:chain", ITEM_CHAIN},
        {"minecraft:clock", ITEM_CLOCK},
        {"minecraft:coal", ITEM_COAL},
        {"minecraft:coast_armor_trim_smithing_template", ITEM_COAST_ARMOR_TRIM_SMITHING_TEMPLATE},
        {"minecraft:cobweb", ITEM_COBWEB},
        {"minecraft:compass", ITEM_COMPASS},
        {"minecraft:cooked_cod", ITEM_COOKED_COD},
        {"minecraft:cooked_porkchop", ITEM_COOKED_PORKCHOP},
        {"minecraft:cooked_salmon", ITEM_COOKED_SALMON},
        {"minecraft:copper_axe", ITEM_COPPER_AXE},
        {"minecraft:copper_boots", ITEM_COPPER_BOOTS},
        {"minecraft:copper_chestplate", ITEM_COPPER_CHESTPLATE},
        {"minecraft:copper_horse_armor", ITEM_COPPER_HORSE_ARMOR},
        {"minecraft:copper_ingot", ITEM_COPPER_INGOT},
        {"minecraft:copper_leggings", ITEM_COPPER_LEGGINGS},
        {"minecraft:copper_nautilus_armor", ITEM_COPPER_NAUTILUS_ARMOR},
        {"minecraft:copper_spear", ITEM_COPPER_SPEAR},
        {"minecraft:copper_sword", ITEM_COPPER_SWORD},
        {"minecraft:crossbow", ITEM_CROSSBOW},
        {"minecraft:crying_obsidian", ITEM_CRYING_OBSIDIAN},
        {"minecraft:dark_oak_log", ITEM_DARK_OAK_LOG},
        {"minecraft:desert_pyramid_map", ITEM_DESERT_PYRAMID_MAP},
        {"minecraft:detector_rail", ITEM_DETECTOR_RAIL},
        {"minecraft:diamond", ITEM_DIAMOND},
        {"minecraft:diamond_axe", ITEM_DIAMOND_AXE},
        {"minecraft:diamond_block", ITEM_DIAMOND_BLOCK},
        {"minecraft:diamond_boots", ITEM_DIAMOND_BOOTS},
        {"minecraft:diamond_chestplate", ITEM_DIAMOND_CHESTPLATE},
        {"minecraft:diamond_helmet", ITEM_DIAMOND_HELMET},
        {"minecraft:diamond_horse_armor", ITEM_DIAMOND_HORSE_ARMOR},
        {"minecraft:diamond_leggings", ITEM_DIAMOND_LEGGINGS},
        {"minecraft:diamond_nautilus_armor", ITEM_DIAMOND_NAUTILUS_ARMOR},
        {"minecraft:diamond_pickaxe", ITEM_DIAMOND_PICKAXE},
        {"minecraft:diamond_shovel", ITEM_DIAMOND_SHOVEL},
        {"minecraft:diamond_spear", ITEM_DIAMOND_SPEAR},
        {"minecraft:diamond_sword", ITEM_DIAMOND_SWORD},
        {"minecraft:dune_armor_trim_smithing_template", ITEM_DUNE_ARMOR_TRIM_SMITHING_TEMPLATE},
        {"minecraft:emerald", ITEM_EMERALD},
        {"minecraft:emerald_block", ITEM_EMERALD_BLOCK},
        {"minecraft:enchanted_golden_apple", ITEM_ENCHANTED_GOLDEN_APPLE},
        {"minecraft:ender_pearl", ITEM_ENDER_PEARL},
        {"minecraft:experience_bottle", ITEM_EXPERIENCE_BOTTLE},
        {"minecraft:eye_armor_trim_smithing_template", ITEM_EYE_ARMOR_TRIM_SMITHING_TEMPLATE},
        {"minecraft:feather", ITEM_FEATHER},
        {"minecraft:fire_charge", ITEM_FIRE_CHARGE},
        {"minecraft:firework_rocket", ITEM_FIREWORK_ROCKET},
        {"minecraft:fishing_rod", ITEM_FISHING_ROD},
        {"minecraft:flint", ITEM_FLINT},
        {"minecraft:flint_and_steel", ITEM_FLINT_AND_STEEL},
        {"minecraft:flow_armor_trim_smithing_template", ITEM_FLOW_ARMOR_TRIM_SMITHING_TEMPLATE},
        {"minecraft:flow_banner_pattern", ITEM_FLOW_BANNER_PATTERN},
        {"minecraft:gilded_blackstone", ITEM_GILDED_BLACKSTONE},
        {"minecraft:glass_bottle", ITEM_GLASS_BOTTLE},
        {"minecraft:glistering_melon_slice", ITEM_GLISTERING_MELON_SLICE},
        {"minecraft:glow_berries", ITEM_GLOW_BERRIES},
        {"minecraft:goat_horn", ITEM_GOAT_HORN},
        {"minecraft:gold_block", ITEM_GOLD_BLOCK},
        {"minecraft:gold_ingot", ITEM_GOLD_INGOT},
        {"minecraft:gold_nugget", ITEM_GOLD_NUGGET},
        {"minecraft:golden_apple", ITEM_GOLDEN_APPLE},
        {"minecraft:golden_axe", ITEM_GOLDEN_AXE},
        {"minecraft:golden_boots", ITEM_GOLDEN_BOOTS},
        {"minecraft:golden_carrot", ITEM_GOLDEN_CARROT},
        {"minecraft:golden_chestplate", ITEM_GOLDEN_CHESTPLATE},
        {"minecraft:golden_helmet", ITEM_GOLDEN_HELMET},
        {"minecraft:golden_hoe", ITEM_GOLDEN_HOE},
        {"minecraft:golden_horse_armor", ITEM_GOLDEN_HORSE_ARMOR},
        {"minecraft:golden_leggings", ITEM_GOLDEN_LEGGINGS},
        {"minecraft:golden_nautilus_armor", ITEM_GOLDEN_NAUTILUS_ARMOR},
        {"minecraft:golden_pickaxe", ITEM_GOLDEN_PICKAXE},
        {"minecraft:golden_shovel", ITEM_GOLDEN_SHOVEL},
        {"minecraft:golden_sword", ITEM_GOLDEN_SWORD},
        {"minecraft:gunpowder", ITEM_GUNPOWDER},
        {"minecraft:guster_banner_pattern", ITEM_GUSTER_BANNER_PATTERN},
        {"minecraft:heart_of_the_sea", ITEM_HEART_OF_THE_SEA},
        {"minecraft:heavy_core", ITEM_HEAVY_CORE},
        {"minecraft:honey_bottle", ITEM_HONEY_BOTTLE},
        {"minecraft:iron_axe", ITEM_IRON_AXE},
        {"minecraft:iron_block", ITEM_IRON_BLOCK},
        {"minecraft:iron_boots", ITEM_IRON_BOOTS},
        {"minecraft:iron_chain", ITEM_IRON_CHAIN},
        {"minecraft:iron_chestplate", ITEM_IRON_CHESTPLATE},
        {"minecraft:iron_helmet", ITEM_IRON_HELMET},
        {"minecraft:iron_horse_armor", ITEM_IRON_HORSE_ARMOR},
        {"minecraft:iron_ingot", ITEM_IRON_INGOT},
        {"minecraft:iron_leggings", ITEM_IRON_LEGGINGS},
        {"minecraft:iron_nautilus_armor", ITEM_IRON_NAUTILUS_ARMOR},
        {"minecraft:iron_nugget", ITEM_IRON_NUGGET},
        {"minecraft:iron_pickaxe", ITEM_IRON_PICKAXE},
        {"minecraft:iron_shovel", ITEM_IRON_SHOVEL},
        {"minecraft:iron_spear", ITEM_IRON_SPEAR},
        {"minecraft:iron_sword", ITEM_IRON_SWORD},
        {"minecraft:jungle_pyramid_map", ITEM_JUNGLE_PYRAMID_MAP},
        {"minecraft:lapis_lazuli", ITEM_LAPIS_LAZULI},
        {"minecraft:lead", ITEM_LEAD},
        {"minecraft:leather", ITEM_LEATHER},
        {"minecraft:leather_boots", ITEM_LEATHER_BOOTS},
        {"minecraft:leather_chestplate", ITEM_LEATHER_CHESTPLATE},
        {"minecraft:leather_helmet", ITEM_LEATHER_HELMET},
        {"minecraft:leather_leggings", ITEM_LEATHER_LEGGINGS},
        {"minecraft:light_weighted_pressure_plate", ITEM_LIGHT_WEIGHTED_PRESSURE_PLATE},
        {"minecraft:lodestone", ITEM_LODESTONE},
        {"minecraft:magma_cream", ITEM_MAGMA_CREAM},
        {"minecraft:map", ITEM_MAP},
        {"minecraft:melon_seeds", ITEM_MELON_SEEDS},
        {"minecraft:moss_block", ITEM_MOSS_BLOCK},
        {"minecraft:music_disc_bounce", ITEM_MUSIC_DISC_BOUNCE},
        {"minecraft:music_disc_creator", ITEM_MUSIC_DISC_CREATOR},
        {"minecraft:music_disc_otherside", ITEM_MUSIC_DISC_OTHERSIDE},
        {"minecraft:music_disc_pigstep", ITEM_MUSIC_DISC_PIGSTEP},
        {"minecraft:music_disc_precipice", ITEM_MUSIC_DISC_PRECIPICE},
        {"minecraft:name_tag", ITEM_NAME_TAG},
        {"minecraft:nether_wart", ITEM_NETHER_WART},
        {"minecraft:netherite_scrap", ITEM_NETHERITE_SCRAP},
        {"minecraft:netherite_upgrade_smithing_template", ITEM_NETHERITE_UPGRADE_SMITHING_TEMPLATE},
        {"minecraft:obsidian", ITEM_OBSIDIAN},
        {"minecraft:ominous_bottle", ITEM_OMINOUS_BOTTLE},
        {"minecraft:paper", ITEM_PAPER},
        {"minecraft:piglin_banner_pattern", ITEM_PIGLIN_BANNER_PATTERN},
        {"minecraft:poisonous_potato", ITEM_POISONOUS_POTATO},
        {"minecraft:potato", ITEM_POTATO},
        {"minecraft:potion", ITEM_POTION},
        {"minecraft:powered_rail", ITEM_POWERED_RAIL},
        {"minecraft:prismarine_crystals", ITEM_PRISMARINE_CRYSTALS},
        {"minecraft:pumpkin", ITEM_PUMPKIN},
        {"minecraft:pumpkin_seeds", ITEM_PUMPKIN_SEEDS},
        {"minecraft:rabbit_hide", ITEM_RABBIT_HIDE},
        {"minecraft:rail", ITEM_RAIL},
        {"minecraft:redstone", ITEM_REDSTONE},
        {"minecraft:rib_armor_trim_smithing_template", ITEM_RIB_ARMOR_TRIM_SMITHING_TEMPLATE},
        {"minecraft:rotten_flesh", ITEM_ROTTEN_FLESH},
        {"minecraft:saddle", ITEM_SADDLE},
        {"minecraft:sand", ITEM_SAND},
        {"minecraft:sentry_armor_trim_smithing_template", ITEM_SENTRY_ARMOR_TRIM_SMITHING_TEMPLATE},
        {"minecraft:shears", ITEM_SHEARS},
        {"minecraft:shield", ITEM_SHIELD},
        {"minecraft:snout_armor_trim_smithing_template", ITEM_SNOUT_ARMOR_TRIM_SMITHING_TEMPLATE},
        {"minecraft:spectral_arrow", ITEM_SPECTRAL_ARROW},
        {"minecraft:spider_eye", ITEM_SPIDER_EYE},
        {"minecraft:spire_armor_trim_smithing_template", ITEM_SPIRE_ARMOR_TRIM_SMITHING_TEMPLATE},
        {"minecraft:spyglass", ITEM_SPYGLASS},
        {"minecraft:stone_axe", ITEM_STONE_AXE},
        {"minecraft:straw_bed", ITEM_STRAW_BED},
        {"minecraft:string", ITEM_STRING},
        {"minecraft:suspicious_stew", ITEM_SUSPICIOUS_STEW},
        {"minecraft:tipped_arrow", ITEM_TIPPED_ARROW},
        {"minecraft:tnt", ITEM_TNT},
        {"minecraft:torch", ITEM_TORCH},
        {"minecraft:trident", ITEM_TRIDENT},
        {"minecraft:tripwire_hook", ITEM_TRIPWIRE_HOOK},
        {"minecraft:warm_ocean_ruins_map", ITEM_WARM_OCEAN_RUINS_MAP},
        {"minecraft:wheat", ITEM_WHEAT},
        {"minecraft:white_candle", ITEM_WHITE_CANDLE},
        {"minecraft:white_cushion", ITEM_WHITE_CUSHION},
        {"minecraft:wild_armor_trim_smithing_template", ITEM_WILD_ARMOR_TRIM_SMITHING_TEMPLATE},
        {"minecraft:wind_charge", ITEM_WIND_CHARGE},
        {"minecraft:wooden_axe", ITEM_WOODEN_AXE},
        {"minecraft:woodland_mansion_map", ITEM_WOODLAND_MANSION_MAP},
    };
    size_t lo = 0, hi = sizeof(items) / sizeof(*items);
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        int cmp = strcmp(name, items[mid].name);
        if (cmp == 0) {
            return items[mid].id;
        }
        if (cmp < 0) {
            hi = mid;
        } else {
            lo = mid + 1;
        }
    }
    fprintf(stderr, "ERR item_name2global_id: unknown item %s\n", name);
    return ITEM_UNKNOWN;
}

const char* global_id2item_name(int global_id, int mc) {
    if (mc >= MC_1_21_9) {
        if (global_id == ITEM_IRON_CHAIN) return "minecraft:iron_chain";
    }
    switch (global_id) {
    case ITEM_ABANDONED_CAMP_MAP: return "minecraft:abandoned_camp_map";
    case ITEM_ACTIVATOR_RAIL: return "minecraft:activator_rail";
    case ITEM_ANCIENT_DEBRIS: return "minecraft:ancient_debris";
    case ITEM_APPLE: return "minecraft:apple";
    case ITEM_ARROW: return "minecraft:arrow";
    case ITEM_BAMBOO: return "minecraft:bamboo";
    case ITEM_BEETROOT_SEEDS: return "minecraft:beetroot_seeds";
    case ITEM_BELL: return "minecraft:bell";
    case ITEM_BOLT_ARMOR_TRIM_SMITHING_TEMPLATE: return "minecraft:bolt_armor_trim_smithing_template";
    case ITEM_BONE: return "minecraft:bone";
    case ITEM_BONE_BLOCK: return "minecraft:bone_block";
    case ITEM_BOOK: return "minecraft:book";
    case ITEM_BOW: return "minecraft:bow";
    case ITEM_BOWL: return "minecraft:bowl";
    case ITEM_BREAD: return "minecraft:bread";
    case ITEM_BUCKET: return "minecraft:bucket";
    case ITEM_BUNDLE: return "minecraft:bundle";
    case ITEM_BURIED_ANCIENT_CITY_MAP: return "minecraft:buried_ancient_city_map";
    case ITEM_BURIED_MINESHAFT_MAP: return "minecraft:buried_mineshaft_map";
    case ITEM_BURIED_TRIAL_CHAMBERS_MAP: return "minecraft:buried_trial_chambers_map";
    case ITEM_CARROT: return "minecraft:carrot";
    case ITEM_CHAIN: return "minecraft:chain";
    case ITEM_CLOCK: return "minecraft:clock";
    case ITEM_COAL: return "minecraft:coal";
    case ITEM_COAST_ARMOR_TRIM_SMITHING_TEMPLATE: return "minecraft:coast_armor_trim_smithing_template";
    case ITEM_COBWEB: return "minecraft:cobweb";
    case ITEM_COMPASS: return "minecraft:compass";
    case ITEM_COOKED_COD: return "minecraft:cooked_cod";
    case ITEM_COOKED_PORKCHOP: return "minecraft:cooked_porkchop";
    case ITEM_COOKED_SALMON: return "minecraft:cooked_salmon";
    case ITEM_COPPER_AXE: return "minecraft:copper_axe";
    case ITEM_COPPER_BOOTS: return "minecraft:copper_boots";
    case ITEM_COPPER_CHESTPLATE: return "minecraft:copper_chestplate";
    case ITEM_COPPER_HORSE_ARMOR: return "minecraft:copper_horse_armor";
    case ITEM_COPPER_INGOT: return "minecraft:copper_ingot";
    case ITEM_COPPER_LEGGINGS: return "minecraft:copper_leggings";
    case ITEM_COPPER_NAUTILUS_ARMOR: return "minecraft:copper_nautilus_armor";
    case ITEM_COPPER_SPEAR: return "minecraft:copper_spear";
    case ITEM_COPPER_SWORD: return "minecraft:copper_sword";
    case ITEM_CROSSBOW: return "minecraft:crossbow";
    case ITEM_CRYING_OBSIDIAN: return "minecraft:crying_obsidian";
    case ITEM_DARK_OAK_LOG: return "minecraft:dark_oak_log";
    case ITEM_DESERT_PYRAMID_MAP: return "minecraft:desert_pyramid_map";
    case ITEM_DETECTOR_RAIL: return "minecraft:detector_rail";
    case ITEM_DIAMOND: return "minecraft:diamond";
    case ITEM_DIAMOND_AXE: return "minecraft:diamond_axe";
    case ITEM_DIAMOND_BLOCK: return "minecraft:diamond_block";
    case ITEM_DIAMOND_BOOTS: return "minecraft:diamond_boots";
    case ITEM_DIAMOND_CHESTPLATE: return "minecraft:diamond_chestplate";
    case ITEM_DIAMOND_HELMET: return "minecraft:diamond_helmet";
    case ITEM_DIAMOND_HORSE_ARMOR: return "minecraft:diamond_horse_armor";
    case ITEM_DIAMOND_LEGGINGS: return "minecraft:diamond_leggings";
    case ITEM_DIAMOND_NAUTILUS_ARMOR: return "minecraft:diamond_nautilus_armor";
    case ITEM_DIAMOND_PICKAXE: return "minecraft:diamond_pickaxe";
    case ITEM_DIAMOND_SHOVEL: return "minecraft:diamond_shovel";
    case ITEM_DIAMOND_SPEAR: return "minecraft:diamond_spear";
    case ITEM_DIAMOND_SWORD: return "minecraft:diamond_sword";
    case ITEM_DUNE_ARMOR_TRIM_SMITHING_TEMPLATE: return "minecraft:dune_armor_trim_smithing_template";
    case ITEM_EMERALD: return "minecraft:emerald";
    case ITEM_EMERALD_BLOCK: return "minecraft:emerald_block";
    case ITEM_ENCHANTED_GOLDEN_APPLE: return "minecraft:enchanted_golden_apple";
    case ITEM_ENDER_PEARL: return "minecraft:ender_pearl";
    case ITEM_EXPERIENCE_BOTTLE: return "minecraft:experience_bottle";
    case ITEM_EYE_ARMOR_TRIM_SMITHING_TEMPLATE: return "minecraft:eye_armor_trim_smithing_template";
    case ITEM_FEATHER: return "minecraft:feather";
    case ITEM_FIRE_CHARGE: return "minecraft:fire_charge";
    case ITEM_FIREWORK_ROCKET: return "minecraft:firework_rocket";
    case ITEM_FISHING_ROD: return "minecraft:fishing_rod";
    case ITEM_FLINT: return "minecraft:flint";
    case ITEM_FLINT_AND_STEEL: return "minecraft:flint_and_steel";
    case ITEM_FLOW_ARMOR_TRIM_SMITHING_TEMPLATE: return "minecraft:flow_armor_trim_smithing_template";
    case ITEM_FLOW_BANNER_PATTERN: return "minecraft:flow_banner_pattern";
    case ITEM_GILDED_BLACKSTONE: return "minecraft:gilded_blackstone";
    case ITEM_GLASS_BOTTLE: return "minecraft:glass_bottle";
    case ITEM_GLISTERING_MELON_SLICE: return "minecraft:glistering_melon_slice";
    case ITEM_GLOW_BERRIES: return "minecraft:glow_berries";
    case ITEM_GOAT_HORN: return "minecraft:goat_horn";
    case ITEM_GOLDEN_APPLE: return "minecraft:golden_apple";
    case ITEM_GOLDEN_AXE: return "minecraft:golden_axe";
    case ITEM_GOLDEN_BOOTS: return "minecraft:golden_boots";
    case ITEM_GOLDEN_CARROT: return "minecraft:golden_carrot";
    case ITEM_GOLDEN_CHESTPLATE: return "minecraft:golden_chestplate";
    case ITEM_GOLDEN_HELMET: return "minecraft:golden_helmet";
    case ITEM_GOLDEN_HOE: return "minecraft:golden_hoe";
    case ITEM_GOLDEN_HORSE_ARMOR: return "minecraft:golden_horse_armor";
    case ITEM_GOLDEN_LEGGINGS: return "minecraft:golden_leggings";
    case ITEM_GOLDEN_NAUTILUS_ARMOR: return "minecraft:golden_nautilus_armor";
    case ITEM_GOLDEN_PICKAXE: return "minecraft:golden_pickaxe";
    case ITEM_GOLDEN_SHOVEL: return "minecraft:golden_shovel";
    case ITEM_GOLDEN_SWORD: return "minecraft:golden_sword";
    case ITEM_GOLD_BLOCK: return "minecraft:gold_block";
    case ITEM_GOLD_INGOT: return "minecraft:gold_ingot";
    case ITEM_GOLD_NUGGET: return "minecraft:gold_nugget";
    case ITEM_GUNPOWDER: return "minecraft:gunpowder";
    case ITEM_GUSTER_BANNER_PATTERN: return "minecraft:guster_banner_pattern";
    case ITEM_HEART_OF_THE_SEA: return "minecraft:heart_of_the_sea";
    case ITEM_HEAVY_CORE: return "minecraft:heavy_core";
    case ITEM_HONEY_BOTTLE: return "minecraft:honey_bottle";
    case ITEM_IRON_AXE: return "minecraft:iron_axe";
    case ITEM_IRON_BLOCK: return "minecraft:iron_block";
    case ITEM_IRON_BOOTS: return "minecraft:iron_boots";
    case ITEM_IRON_CHESTPLATE: return "minecraft:iron_chestplate";
    case ITEM_IRON_HELMET: return "minecraft:iron_helmet";
    case ITEM_IRON_HORSE_ARMOR: return "minecraft:iron_horse_armor";
    case ITEM_IRON_INGOT: return "minecraft:iron_ingot";
    case ITEM_IRON_LEGGINGS: return "minecraft:iron_leggings";
    case ITEM_IRON_NAUTILUS_ARMOR: return "minecraft:iron_nautilus_armor";
    case ITEM_IRON_NUGGET: return "minecraft:iron_nugget";
    case ITEM_IRON_PICKAXE: return "minecraft:iron_pickaxe";
    case ITEM_IRON_SHOVEL: return "minecraft:iron_shovel";
    case ITEM_IRON_SPEAR: return "minecraft:iron_spear";
    case ITEM_IRON_SWORD: return "minecraft:iron_sword";
    case ITEM_JUNGLE_PYRAMID_MAP: return "minecraft:jungle_pyramid_map";
    case ITEM_LAPIS_LAZULI: return "minecraft:lapis_lazuli";
    case ITEM_LEAD: return "minecraft:lead";
    case ITEM_LEATHER: return "minecraft:leather";
    case ITEM_LEATHER_BOOTS: return "minecraft:leather_boots";
    case ITEM_LEATHER_CHESTPLATE: return "minecraft:leather_chestplate";
    case ITEM_LEATHER_HELMET: return "minecraft:leather_helmet";
    case ITEM_LEATHER_LEGGINGS: return "minecraft:leather_leggings";
    case ITEM_LIGHT_WEIGHTED_PRESSURE_PLATE: return "minecraft:light_weighted_pressure_plate";
    case ITEM_LODESTONE: return "minecraft:lodestone";
    case ITEM_MAGMA_CREAM: return "minecraft:magma_cream";
    case ITEM_MAP: return "minecraft:map";
    case ITEM_MELON_SEEDS: return "minecraft:melon_seeds";
    case ITEM_MOSS_BLOCK: return "minecraft:moss_block";
    case ITEM_MUSIC_DISC_BOUNCE: return "minecraft:music_disc_bounce";
    case ITEM_MUSIC_DISC_CREATOR: return "minecraft:music_disc_creator";
    case ITEM_MUSIC_DISC_OTHERSIDE: return "minecraft:music_disc_otherside";
    case ITEM_MUSIC_DISC_PIGSTEP: return "minecraft:music_disc_pigstep";
    case ITEM_MUSIC_DISC_PRECIPICE: return "minecraft:music_disc_precipice";
    case ITEM_NAME_TAG: return "minecraft:name_tag";
    case ITEM_NETHERITE_SCRAP: return "minecraft:netherite_scrap";
    case ITEM_NETHERITE_UPGRADE_SMITHING_TEMPLATE: return "minecraft:netherite_upgrade_smithing_template";
    case ITEM_NETHER_WART: return "minecraft:nether_wart";
    case ITEM_OBSIDIAN: return "minecraft:obsidian";
    case ITEM_OMINOUS_BOTTLE: return "minecraft:ominous_bottle";
    case ITEM_PAPER: return "minecraft:paper";
    case ITEM_PIGLIN_BANNER_PATTERN: return "minecraft:piglin_banner_pattern";
    case ITEM_POISONOUS_POTATO: return "minecraft:poisonous_potato";
    case ITEM_POTATO: return "minecraft:potato";
    case ITEM_POTION: return "minecraft:potion";
    case ITEM_POWERED_RAIL: return "minecraft:powered_rail";
    case ITEM_PRISMARINE_CRYSTALS: return "minecraft:prismarine_crystals";
    case ITEM_PUMPKIN: return "minecraft:pumpkin";
    case ITEM_PUMPKIN_SEEDS: return "minecraft:pumpkin_seeds";
    case ITEM_RABBIT_HIDE: return "minecraft:rabbit_hide";
    case ITEM_RAIL: return "minecraft:rail";
    case ITEM_REDSTONE: return "minecraft:redstone";
    case ITEM_RIB_ARMOR_TRIM_SMITHING_TEMPLATE: return "minecraft:rib_armor_trim_smithing_template";
    case ITEM_ROTTEN_FLESH: return "minecraft:rotten_flesh";
    case ITEM_SADDLE: return "minecraft:saddle";
    case ITEM_SAND: return "minecraft:sand";
    case ITEM_SENTRY_ARMOR_TRIM_SMITHING_TEMPLATE: return "minecraft:sentry_armor_trim_smithing_template";
    case ITEM_SHEARS: return "minecraft:shears";
    case ITEM_SHIELD: return "minecraft:shield";
    case ITEM_SNOUT_ARMOR_TRIM_SMITHING_TEMPLATE: return "minecraft:snout_armor_trim_smithing_template";
    case ITEM_SPECTRAL_ARROW: return "minecraft:spectral_arrow";
    case ITEM_SPIDER_EYE: return "minecraft:spider_eye";
    case ITEM_SPIRE_ARMOR_TRIM_SMITHING_TEMPLATE: return "minecraft:spire_armor_trim_smithing_template";
    case ITEM_SPYGLASS: return "minecraft:spyglass";
    case ITEM_STONE_AXE: return "minecraft:stone_axe";
    case ITEM_STRAW_BED: return "minecraft:straw_bed";
    case ITEM_STRING: return "minecraft:string";
    case ITEM_SUSPICIOUS_STEW: return "minecraft:suspicious_stew";
    case ITEM_TIPPED_ARROW: return "minecraft:tipped_arrow";
    case ITEM_TNT: return "minecraft:tnt";
    case ITEM_TORCH: return "minecraft:torch";
    case ITEM_TRIDENT: return "minecraft:trident";
    case ITEM_TRIPWIRE_HOOK: return "minecraft:tripwire_hook";
    case ITEM_WARM_OCEAN_RUINS_MAP: return "minecraft:warm_ocean_ruins_map";
    case ITEM_WHEAT: return "minecraft:wheat";
    case ITEM_WHITE_CANDLE: return "minecraft:white_candle";
    case ITEM_WHITE_CUSHION: return "minecraft:white_cushion";
    case ITEM_WILD_ARMOR_TRIM_SMITHING_TEMPLATE: return "minecraft:wild_armor_trim_smithing_template";
    case ITEM_WIND_CHARGE: return "minecraft:wind_charge";
    case ITEM_WOODEN_AXE: return "minecraft:wooden_axe";
    case ITEM_WOODLAND_MANSION_MAP: return "minecraft:woodland_mansion_map";
    default:
        fprintf(stderr, "ERR global_id2item_name: unknown item type: %d\n", global_id);
        return NULL;
    }
}

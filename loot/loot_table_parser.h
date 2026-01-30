#ifndef LOOT_TABLE_PARSER_H
#define LOOT_TABLE_PARSER_H

ItemType get_item_type(const char* item_name);
const MobEffect* get_mob_effect_from_name(const char* mob_effect);
Enchantment get_enchantment_from_name(const char* ench);

void free_loot_function(LootFunction* lf);

#endif //LOOT_TABLE_PARSER_H

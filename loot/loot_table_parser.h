#ifndef LOOT_TABLE_PARSER_H
#define LOOT_TABLE_PARSER_H

ItemType get_item_type(const char* item_name);
const MobEffect* get_mob_effect_from_name(const char* mob_effect);
const Potion* get_potion_from_name(const char* potion);
Enchantment get_enchantment_from_name(const char* ench);

int merge_item_lists(LootTableContext* ctx, const LootTableContext* sub_ctx, int is_ctx_static);
int merge_loot_pools(LootTableContext* ctx, const LootTableContext* sub_ctx, int is_ctx_static, int is_sub_ctx_static);

void free_loot_function(LootFunction* lf);

#endif //LOOT_TABLE_PARSER_H

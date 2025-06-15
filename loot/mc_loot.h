#ifndef _LOOT_TABLE_PARSER_H
#define _LOOT_TABLE_PARSER_H

#include "loot_table_context.h"
#include <stdio.h>

int init_loot_table(const char* loot_table_string, LootTableContext* context, const int version);
int init_loot_table_file(FILE* file, LootTableContext* context, const int version);

int resolve_subtable(LootTableContext* context, const char* subtable_name, const char* subtable_string);
int resolve_subtable_file(LootTableContext* context, const char* subtable_name, FILE* subtable_file);

void free_loot_table(LootTableContext* context);

#endif

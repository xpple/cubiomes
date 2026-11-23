#ifndef _LOOT_TABLE_PARSER_H
#define _LOOT_TABLE_PARSER_H

#include "loot_table_context.h"
#include <stdio.h>

/// Parses a JSON loot table into a caller-provided context for the given
/// Minecraft version. Resolve any subtables before generating loot, and
/// release the allocated contents with `free_loot_table` when finished.
/// \return zero upon success, or -1 upon failure
int init_loot_table(const char* loot_table_string, LootTableContext* context, const int version);
/// Initializes a loot table as in init_loot_table(), reading JSON from a
/// file. The file is not closed. Returns zero upon success, else -1.
int init_loot_table_file(FILE* file, LootTableContext* context, const int version);

/// Resolves a referenced subtable from JSON. Subtables must be resolved in
/// their order in unresolved_subtable_names and may not contain subtables
/// themselves.
/// \return zero upon success, or -1 upon failure.
int resolve_subtable(LootTableContext* context, const char* subtable_name, const char* subtable_string);
/// Resolves a subtable as in `resolve_subtable`, reading JSON from a file.
/// The file is not closed. Returns zero upon success, else -1.
int resolve_subtable_file(LootTableContext* context, const char* subtable_name, FILE* subtable_file);

/// Frees the contents of a context parsed with `init_loot_table` or
/// `init_loot_table_file`. The context itself is not freed. Do not use this
/// for the static contexts returned by the built-in loot table initializers.
void free_loot_table(LootTableContext* context);

#endif

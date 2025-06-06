import glob
import json

from abc import abstractmethod
from textwrap import dedent


class LootFunction:
    def __init__(self):
        pass

    @abstractmethod
    def to_function_call(self, arg: str, version: str):
        pass


class SetCountFunction(LootFunction):
    def __init__(self, min: int, max: int):
        super().__init__()
        self.min = min
        self.max = max


    def to_function_call(self, arg: str, version: str):
        return f"create_set_count({arg}, {self.min}, {self.max})"


class SetEffectFunction(LootFunction):
    def __init__(self):
        super().__init__()


    def to_function_call(self, arg: str, version: str):
        return f"create_set_effect({arg})"


class SetDamageFunction(LootFunction):
    def __init__(self):
        super().__init__()


    def to_function_call(self, arg: str, version: str):
        return f"create_skip_calls({arg}, 1)"


class SkipCallsFunction(LootFunction):
    def __init__(self, skip_count: int):
        super().__init__()
        self.skip_count = skip_count


    def to_function_call(self, arg: str, version: str):
        return f"create_skip_calls({arg}, {self.skip_count})"


class NoOpFunction(LootFunction):
    def __init__(self):
        super().__init__()

    def to_function_call(self, arg: str, version: str):
        return f"create_no_op({arg})"


class EnchantRandomlyFunction(LootFunction):
    def __init__(self, enchantments: list[str], item_name: str):
        super().__init__()
        self.enchantments = enchantments
        self.item_name = item_name


    def to_function_call(self, arg: str, version: str):
        if self.enchantments is None:
            return f"create_enchant_randomly({arg}, MC_{version}, get_item_type(\"{self.item_name}\"), 1)"
        elif len(self.enchantments) == 1:
            return f"create_enchant_randomly_one_enchant({arg}, get_enchantment_from_name(\"{self.enchantments[0]}\"))"
        else:
            enchantments_str = ", ".join(f"get_enchantment_from_name(\"{enchantment}\")" for enchantment in self.enchantments)
            return f"create_enchant_randomly_list({arg}, (Enchantment[]){{{enchantments_str}}}, {len(self.enchantments)})"


class EnchantWithLevelsFunction(LootFunction):
    def __init__(self, item_name: str, min: int, max: int, is_treasure: int):
        super().__init__()
        self.item_name = item_name
        self.min = min
        self.max = max
        self.is_treasure = is_treasure


    def to_function_call(self, arg: str, version: str):
        return f"create_enchant_with_levels({arg}, MC_{version}, \"{self.item_name}\", get_item_type(\"{self.item_name}\"), {self.min}, {self.max}, {self.is_treasure})"


class PoolEntry:
    def __init__(self, name: str, weight: int, functions: list[LootFunction]):
        self.name = name
        self.weight = weight
        self.functions = functions


class LootPool:
    def __init__(self, min_rolls: int, max_rolls: int, roll_count_function: str, entries: list[PoolEntry]):
        self.min_rolls = min_rolls
        self.max_rolls = max_rolls
        self.roll_count_function = roll_count_function
        self.entries = entries


class LootTableContext:
    def __init__(self, version: str, item_names: list[str], loot_pools: list[LootPool]):
        self.version = version
        self.item_names = item_names
        self.loot_pools = loot_pools


def main():
    for file_name in glob.glob("ruined_portal.1_21.json"):
        with open(file_name, 'r', encoding='UTF-8') as f:
            data = json.load(f)
            print(data)

        loot_table_name, version, _ = file_name.split('.')
        context = parse_loot_table(version, data["pools"])
        c_file_name = loot_table_name + '_' + version
        content = gen_c_loot_table(c_file_name, context)
        print(content)


def parse_loot_table(version: str, json_pools) -> LootTableContext:
    item_names: list[str] = []
    loot_pools: list[LootPool] = []
    for json_pool in json_pools:
        min_rolls, max_rolls, roll_count_function = parse_pool_rolls(json_pool["rolls"])

        pool_entries: list[PoolEntry] = []
        for entry_idx, json_entry in enumerate(json_pool["entries"]):
            entry_name = json_entry.get("name", None)
            if entry_name is not None:
                item_names.append(entry_name)
            entry_weight = json_entry.get("weight", 1)

            entry_type = json_entry["type"]

            loot_functions: list[LootFunction] = []
            for json_function_entry in json_entry.get("functions", []):
                loot_function = parse_loot_function(json_function_entry, entry_name)
                loot_functions.append(loot_function)

            pool_entries.append(PoolEntry(entry_name, entry_weight, loot_functions))

        loot_pools.append(LootPool(min_rolls, max_rolls, roll_count_function, pool_entries))

    return LootTableContext(version, item_names, loot_pools)


def parse_pool_rolls(json_rolls) -> (int, int, str):
    if type(json_rolls) == float:
        min_rolls, max_rolls = json_rolls, json_rolls
        roll_count_function = "roll_count_constant"
    else:
        min_rolls = json_rolls["min"]
        max_rolls = json_rolls["max"]
        roll_count_function = "roll_count_uniform"
    return int(min_rolls), int(max_rolls), roll_count_function


def parse_loot_function(json_function_entry, entry_name: str) -> LootFunction:
    json_function = json_function_entry["function"]
    if json_function == 'minecraft:set_count':
        json_count = json_function_entry["count"]
        if type(json_count) == float:
            min_rolls, max_rolls = json_count, json_count
        else:
            min_rolls = json_count["min"]
            max_rolls = json_count["max"]
        return SetCountFunction(int(min_rolls), int(max_rolls))
    if json_function == 'minecraft:set_stew_effect':
        return SetEffectFunction()
    if json_function == 'minecraft:set_damage':
        return SetDamageFunction()
    if json_function == 'minecraft:set_ominous_bottle_amplifier':
        return SkipCallsFunction(1)
    if json_function == 'minecraft:no_op':
        return NoOpFunction()
    if json_function == 'minecraft:enchant_randomly':
        enchantments = json_function_entry.get("enchantments", None)
        return EnchantRandomlyFunction(enchantments, entry_name)
    if json_function == 'minecraft:enchant_with_levels':
        levels = json_function_entry.get("levels", None)
        if type(levels) == float:
            min_level, max_level = levels, levels
        elif levels is not None:
            min_level = levels["min"]
            max_level = levels["max"]
        else:
            min_level = 0
            max_level = 0
        is_treasure = json_function_entry.get("is_treasure", True)
        return EnchantWithLevelsFunction(entry_name, int(min_level), int(max_level), int(is_treasure))

    raise RuntimeError("unsupported loot function")


def gen_c_loot_table(c_file_name: str, context: LootTableContext) -> str:
    file_content = dedent(f"""\
        #include "{c_file_name}.h"

        #include "../../biomes.h"

        #include "../loot_table_context.h"
        #include "../loot_table_parser.h"

        char* item_names[{len(context.item_names)}] = {{\"{"\", \"".join(context.item_names)}\"}};
        """)

    loot_functions_size: list[int] = []
    for pool_idx, loot_pool in enumerate(context.loot_pools):
        entries_len = len(loot_pool.entries)
        total_weight = sum(entry.weight for entry in loot_pool.entries)

        precomputed_loot: list[str] = []
        entry_to_item: list[str] = []
        entry_functions_count: list[str] = []
        entry_functions_index: list[str] = []
        function_index = 0
        for entry_idx, entry in enumerate(loot_pool.entries):
            weight = entry.weight
            for _ in range(weight):
                precomputed_loot.append(str(entry_idx))

            if entry.name is None:
                entry_to_item.append(str(-1))
            else:
                entry_to_item.append(str(context.item_names.index(entry.name)))

            functions = entry.functions
            entry_functions_count.append(str(len(functions)))

            entry_functions_index.append(str(function_index))
            function_index += len(functions)
        loot_functions_size.append(function_index)

        file_content += f"int precomputed_loot__{pool_idx}[{total_weight}] = {{{", ".join(precomputed_loot)}}};\n"
        file_content += f"int entry_to_item__{pool_idx}[{entries_len}] = {{{", ".join(entry_to_item)}}};\n"
        file_content += f"int entry_functions_count__{pool_idx}[{entries_len}] = {{{", ".join(entry_functions_count)}}};\n"
        file_content += f"int entry_functions_index__{pool_idx}[{entries_len}] = {{{", ".join(entry_functions_index)}}};\n"

        file_content += dedent(f"""\
            static LootPool {c_file_name}__{pool_idx} = {{
                .min_rolls = {loot_pool.min_rolls},
                .max_rolls = {loot_pool.max_rolls},
                .roll_count_function = {loot_pool.roll_count_function},
                .entry_count = {entries_len},
                .total_weight = {total_weight},
                .precomputed_loot = precomputed_loot__{pool_idx},
                .entry_to_item = entry_to_item__{pool_idx},
                .entry_functions_count = entry_functions_count__{pool_idx},
                .entry_functions_index = entry_functions_index__{pool_idx},
                .loot_functions = NULL, // initialised later
            }};
            """)

    file_content += dedent(f"""\
        int init_{c_file_name}(LootTableContext* context) {{
            context->version = MC_{context.version};
            context->item_count = {len(context.item_names)};
            context->item_names = item_names;

            context->unresolved_subtable_count = 0;
            context->subtable_count = 0;
            context->subtable_pool_offset = NULL;
            context->subtable_pool_count = NULL;

            context->pool_count = {len(context.loot_pools)};
            context->loot_pools = malloc(context->pool_count * sizeof(LootPool));
            if (context->loot_pools == NULL) {{
                return 0;
            }}
        """)
    for i in range(len(context.loot_pools)):
        file_content += f"    context->loot_pools[{i}] = {c_file_name}__{i};\n"

    for pool_idx, loot_pool in enumerate(context.loot_pools):
        file_content += f"    LootPool* loot_pool__{pool_idx} = &(context->loot_pools[{pool_idx}]);\n"
        file_content += f"    loot_pool__{pool_idx}->loot_functions = malloc({loot_functions_size[pool_idx]} * sizeof(LootFunction));\n"
        for entry_idx, entry in enumerate(loot_pool.entries):
            for function in entry.functions:
                findex = f"loot_pool__{pool_idx}->entry_functions_index[{entry_idx}]"
                i = f"loot_pool__{pool_idx}->entry_functions_count[{entry_idx}]"
                function_str = f"&(loot_pool__{pool_idx}->loot_functions[{findex} + {i}])"
                file_content += f"    {function.to_function_call(function_str, context.version)};\n"

    file_content += dedent(f"""\
            return 1;
        }}
        """)

    return file_content


if __name__ == '__main__':
    main()

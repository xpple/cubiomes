#include <stdio.h>

#include "finders.h"
#include "generator.h"

#include "loot/mc_loot.h"

Pos findStructure(StructureConfig sconf, Generator g);
uint64_t getDesertPyramidLootSeed(uint64_t ws, int x, int z, int index, int step, int chestIdx, int mc);
void print_loot(LootTableContext* ctx);

int main() {
	uint64_t seed = 8052710360952744907ULL;
	int version = MC_1_21;

	StructureConfig sconf;
	if (!getStructureConfig(Desert_Pyramid, version, &sconf)) {
		printf("Structure config failed\n");
		return 1;
	}
	Generator generator;
	setupGenerator(&generator, version, 0);
	applySeed(&generator, DIM_OVERWORLD, seed);

	Pos pos = findStructure(sconf, generator);
	printf("Structure found at %d %d\n", pos.x, pos.z);

	LootTableContext ctx;
	FILE* fptr = fopen("loot/loot_tables/desert_pyramid.json", "rb");
	if (fptr == NULL) {
		printf("Failed to open file\n");
		return 1;
	}
	if (init_loot_table_file(fptr, &ctx, MC_1_21) == -1) {
		printf("Failed to parse loot table\n");
	}

	uint64_t lootSeed = getDesertPyramidLootSeed(seed, pos.x >> 4, pos.z >> 4, 1, 4, 0, version);

	set_loot_seed(&ctx, lootSeed);
	generate_loot(&ctx);
	print_loot(&ctx);
	free_loot_table(&ctx);

	return 0;
}

Pos findStructure(StructureConfig sconf, Generator g) {
	int x = 1, dx = 0, z = 0, dz = -1;
	const int radius = 30000000 / (sconf.regionSize << 4);
	const uint64_t max = (2ULL * radius + 1) * (2ULL * radius + 1);
	for (uint64_t i = 0; i < max; i++) {
		if (-radius <= x && x <= radius && -radius <= z && z <= radius) {
			Pos p;
			if (getStructurePos(sconf.structType, g.mc, g.seed, x, z, &p)) {
				if (isViableStructurePos(sconf.structType, &g, p.x, p.z, 0)) {
					return (Pos) {p.x, p.z};
				}
			}
		}
		if (x == z || (x < 0 && x == -z) || (x > 0 && x == 1 - z)) {
			const int temp = dx;
			dx = -dz;
			dz = temp;
		}
		x += dx;
		z += dz;
	}
	return (Pos) {0, 0};
}

uint64_t getDesertPyramidLootSeed(uint64_t ws, int chunkX, int chunkZ, int index, int step, int chestIdx, int mc) {
	uint64_t populationSeed = getPopulationSeed(mc, ws, chunkX << 4, chunkZ << 4);
	Xoroshiro chunkRand;
	// set decorator seed
	xSetSeed(&chunkRand, populationSeed + index + 10000 * step);
	xNextIntJ(&chunkRand, 3);
	for (int i = 0; i < chestIdx; i++) {
		xNextLongJ(&chunkRand);
	}
	return xNextLongJ(&chunkRand);
}

void print_loot(LootTableContext* ctx) {
	for (int i = 0; i < ctx->generated_item_count; i++) {
		ItemStack* item_stack = &(ctx->generated_items[i]);
		printf("%s x %d\n", get_item_name(ctx, item_stack->item), item_stack->count);

		for (int j = 0; j < item_stack->enchantment_count; j++) {
			EnchantInstance* ench = &(item_stack->enchantments[j]);
			printf("    %s %d\n", get_enchantment_name(ench->enchantment), ench->level);
		}
	}
}

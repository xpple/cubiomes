#include <stdio.h>

#include "finders.h"
#include "generator.h"
#include "util.h"

#include "loot/mc_loot.h"

Pos findStructure(StructureConfig sconf, Generator g, SurfaceNoise sn);
void print_loot(LootTableContext* ctx);

int main() {
	uint64_t seed = 8052710360952744907ULL;
	int version = MC_1_21;
	int stype = Ruined_Portal;

	StructureConfig sconf;
	if (!getStructureConfig(stype, version, &sconf)) {
		printf("Structure config failed\n");
		return 1;
	}
	Generator generator;
	setupGenerator(&generator, version, 0);
	applySeed(&generator, sconf.dim, seed);
	SurfaceNoise sn;
	initSurfaceNoise(&sn, sconf.dim, seed);

	Pos pos = findStructure(sconf, generator, sn);
	printf("Structure %s found at %d %d\n", struct2str(sconf.structType), pos.x, pos.z);

	int biome = getBiomeAt(&generator, 4, pos.x >> 2, 320 >> 2, pos.z >> 2);
	StructureVariant sv;
	getVariant(&sv, sconf.structType, version, seed, pos.x, pos.z, biome);
	StructureSaltConfig ssconf;
	if (!getStructureSaltConfig(stype, version, sv.biome != -1 ? sv.biome : biome, &ssconf)) {
		printf("Structure salt config failed\n");
		return 1;
	}

	int n = END_CITY_PIECES_MAX;
	Piece* pieces = malloc(n * sizeof(Piece));
	int pieceCount = getStructurePieces(pieces, n, sconf.structType, ssconf, sv, version, seed, pos.x >> 4, pos.z >> 4);
	if (pieceCount < 0) {
		printf("Pieces not supported for structure\n");
		free(pieces);
		return 1;
	}
	printf("Piece count: %d\n", pieceCount);
	for (int i = 0; i < pieceCount; ++i) {
		Piece p = pieces[i];
		printf("Piece name: %s\n", p.name);
		if (p.chestCount == 0) {
			continue;
		}
		int stringSize = snprintf(NULL, 0, "loot/loot_tables/%s.json", p.lootTable);
		char* lootTableFile = malloc(stringSize + 1);
		snprintf(lootTableFile, stringSize + 1, "loot/loot_tables/%s.json", p.lootTable);
		printf("Loot table file: %s\n", lootTableFile);
		FILE* fptr = fopen(lootTableFile, "rb");
		free(lootTableFile);
		if (fptr == NULL) {
			printf("Failed to open file\n");
			return 1;
		}
		LootTableContext ctx;
		if (init_loot_table_file(fptr, &ctx, version) == -1) {
			printf("Failed to parse loot table\n");
		}
		for (int j = 0; j < p.chestCount; ++j) {
			printf("Chest %d (loot seed %" PRId64 "):\n", j, p.lootSeeds[j]);
			set_loot_seed(&ctx, p.lootSeeds[j]);
			generate_loot(&ctx);
			print_loot(&ctx);
		}
		free_loot_table(&ctx);
	}
	freeStructurePieces(pieces, pieceCount);

	return 0;
}

Pos findStructure(StructureConfig sconf, Generator g, SurfaceNoise sn) {
	int centerX = 0;
	int centerZ = 0;

	int regionSize = sconf.regionSize << 4;
	const int radius = 30000000 / regionSize;
	int x = centerX / regionSize, dx = 0, z = centerZ / regionSize, dz = -1;
	const int leftBoundX = centerX - radius, rightBoundX = centerX + radius;
	const int bottomBoundZ = centerZ - radius, topBoundZ = centerZ + radius;
	const uint64_t max = (2ULL * radius + 1) * (2ULL * radius + 1);
	for (uint64_t i = 0; i < max; i++) {
		if (leftBoundX <= x && x <= rightBoundX && bottomBoundZ <= z && z <= topBoundZ) {
			Pos p;
			if (getStructurePos(sconf.structType, g.mc, g.seed, x, z, &p)) {
				if (isViableStructurePos(sconf.structType, &g, p.x, p.z, 0)) {
					if (sconf.structType == End_City ? isViableEndCityTerrain(&g, &sn, p.x, p.z) : isViableStructureTerrain(sconf.structType, &g, p.x, p.z)) {
						return (Pos) {p.x, p.z};
					}
				}
			}
		}
		if (x - centerX == z - centerZ || (x < centerX && x - centerX == centerZ - z) || (x > centerX && x - centerX == centerZ + 1 - z)) {
			const int temp = dx;
			dx = -dz;
			dz = temp;
		}
		x += dx;
		z += dz;
	}
	return (Pos) {0, 0};
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

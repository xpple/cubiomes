#ifndef JIGSAW_H
#define JIGSAW_H

#include <stdint.h>
#include "jigsaw_data.h"
#include "../finders.h"

/* Piece assembly for jigsaw structures, ported from JigsawPlacement
 * (net.minecraft.world.level.levelgen.structure.pools.JigsawPlacement).
 *
 * All jigsaw structures run through the same engine; what differs per
 * structure is a JigsawConfig, mirroring the vanilla JigsawStructure
 * settings (the worldgen/structure JSONs in modern versions):
 *   start_pool, size, start_height, use_expansion_hack,
 *   project_start_to_heightmap, max_distance_from_center, start_jigsaw_name
 *
 * Currently supported: the fixed-Y RIGID path (bastions). Heightmap
 * projection, the expansion hack, start jigsaw anchors, pool aliases and
 * feature/list pool elements (villages, outposts, ancient cities, trial
 * chambers) are not implemented yet; generation returns -1 if the config
 * or data requires them.
 */

enum { JIGSAW_ROT_NONE, JIGSAW_ROT_CW90, JIGSAW_ROT_CW180, JIGSAW_ROT_CCW90 };

typedef struct JigsawPiece {
    int16_t templateIdx;
    uint8_t rotation;
    uint8_t depth;
    int16_t groundLevelDelta;
    int x, y, z;
    int bb[6];
} JigsawPiece;

typedef struct JigsawConfig {
    const char *startPools[4];
    int8_t nStartPools;
    int8_t maxDepth;
    int16_t startHeight;
    int16_t maxDistanceFromCenter;
    uint8_t useExpansionHack;
    uint8_t projectStartToHeightmap;
    const char *startJigsawName;
} JigsawConfig;

/* Fills jc for the structure type (finders.h enum) and version
 * For villages, biome selects the variant (plains, desert,
 * savanna, taiga, snowy_tundra), pass -1 otherwise
 * Returns nonzero on success 
*/
int getJigsawConfig(int structureType, int mc, int biome, JigsawConfig *jc);

/* Piece list for a jigsaw structure at chunk (chunkX, chunkZ)
 * Returns the piece count, or -1 on error/unsupported
*/
int getJigsawStructurePieces(int structureType, int mc, int biome,
        uint64_t seed, int chunkX, int chunkZ, JigsawPiece *out, int maxOut);

/* Runs the assembly itself. Use setLargeFeatureSeed first
*/
int getJigsawPieces(const JigsawData *jd, const JigsawConfig *jc,
        const char *startPool, uint64_t *rng, int blockX, int blockZ,
        JigsawPiece *out, int maxOut);

/* Piece/pool tables for the given MC version (the nearest table <= mc)
 * Only support for 1.16-1.21 for now
*/
const JigsawData *getJigsawData(int mc);

/* World position of container c of piece p */
void getJigsawContainerPos(const JigsawData *jd, const JigsawPiece *p, int c,
        int *x, int *y, int *z);

typedef struct JigsawChest {
    int piece;
    int x, y, z;
    uint64_t lootSeed;
    const char *lootTable;
} JigsawChest;

/* Chest positions and loot seeds for an already generated piece list
 * Chests appear in piece-list order, template block order within a piece
 * Returns the chest count, or -1 if out doesn't fit them all
*/
int getJigsawLoot(const JigsawData *jd, StructureSaltConfig ssconf, int mc,
        uint64_t seed, const JigsawPiece *pieces, int nPieces,
        JigsawChest *out, int maxOut);

/** 
 * Generates the desired jigsaw structure as well as the chest loot seeds
 * @param structureType which jigsaw structure
 * @param the Minecraft version
 * @param biome the biome for biome-dependent structures (like villages). Pass -1 otherwise
 * @param seed the world seed
 * @param chunkX the chunk X-coordinate
 * @param chunkZ the chunk Z-coordinate
 * @param pieces array for pieces of the jigsaw structure
 * @param maxPieces max number of jigsaw pieces (1024 is more than enough, bastions cant get over 200)
 * @param nPieces optional to find how many pieces were generated (pass int pointer, otherwise NULL)
 * @param chests chest array (where the chests/loot seeds will go)
 * @param maxChests size of chests (128 should be plenty for any structure, bastions/villages have <16)
 * @return number of chests generated (or -1 if failed/arrays were too small)
 * 
*/
int getJigsawStructureLoot(int structureType, int mc, int biome, uint64_t seed,
        int chunkX, int chunkZ, JigsawPiece *pieces, int maxPieces,
        int *nPieces, JigsawChest *chests, int maxChests);

#endif

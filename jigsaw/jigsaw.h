#ifndef JIGSAW_H
#define JIGSAW_H

#include <stdint.h>
#include "jigsaw_data.h"

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

#endif

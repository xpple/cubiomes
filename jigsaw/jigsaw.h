#ifndef JIGSAW_H
#define JIGSAW_H

#include <stdint.h>
#include "jigsaw_data.h"
#include "../finders.h"

/* Jigsaw logic
 * Jigsaw structures are assembled like jigsaw puzzles (hence the name)
 * Each piece can select from a group of pieces (as well as some other things, 
 * such as rotation) before being subtracted from the free space in the structure
 * bounding box and the generator moving to the next piece, breadth-first
 * 
 * All jigsaw structures use the same engine which is very similar to vanilla,
 * the only thing that changes per structure is the JigsawConfig which has things like
 * the pieces/their weights/connections/etc
 * 
 * Villages and outposts require terrain data so make sure to pass 
 * generator and surface noise for those. Otherwise you can pass NULL
 * Terrain sampling is capped at y=151 - village searching will still work, but
 * actual layout generation will break if the village extends anywhere above that
 * Currently bastions/loot is fully supported and village layout (not loot, see 
 * feature elements) is fully supported
 * It should be relatively easy to add other structures as you just need to 
 * create the configs for them. If a structure/version isn't supported yet,
 * the generation returns -1 
 * Start jigsaw anchors (ancient cities) and pool aliases (trial chambers) are not 
 * implemented yet (generation returns -1)
 */

enum { JIGSAW_ROT_NONE, JIGSAW_ROT_CW90, JIGSAW_ROT_CW180, JIGSAW_ROT_CCW90 };

typedef struct JigsawPiece {
    int16_t templateIdx;    // -1 for feature elements, first member for lists
    int16_t entryIdx;       // the pool entry this piece was placed from
    uint8_t rotation;
    uint8_t depth;
    uint8_t projection;
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
 * g and sn are only read by terrain bound structures (villages,
 * outposts), pass NULL for the rest
 * Returns the piece count, or -1 on error/unsupported
*/
int getJigsawStructurePieces(int structureType, int mc, int biome,
        uint64_t seed, int chunkX, int chunkZ, const Generator *g,
        const SurfaceNoise *sn, JigsawPiece *out, int maxOut);

/* Runs the assembly itself. Use setLargeFeatureSeed first
*/
int getJigsawPieces(const JigsawData *jd, const JigsawConfig *jc,
        const char *startPool, uint64_t *rng, int blockX, int blockZ,
        const Generator *g, const SurfaceNoise *sn,
        JigsawPiece *out, int maxOut);

/* Templates that a piece places, in placement order (lists place every member)
 * Returns the count, at most JIGSAW_MAX_PIECE_TEMPLATES
*/
#define JIGSAW_MAX_PIECE_TEMPLATES 8
int getJigsawPieceTemplates(const JigsawData *jd, const JigsawPiece *p,
        int16_t *out);

/* Logic for piece features (hay bales, trees, grass, melons, etc)
 * Isn't complete yet since it requries a lot of other worldgen componenets
 * See the note below struct JigsawChest for more details on its effects
 * This function just determines which feature is where but does not consume RNG
 * So you can test whether a village has hay bales for instance
*/
const char *getJigsawPieceFeature(const JigsawData *jd, const JigsawPiece *p);

/* Counts pieces placing the named feature, and writes their positions to
 * out (may be NULL). Returns the count, capped at maxOut when out is given
*/
int getJigsawFeaturePositions(const JigsawData *jd, const JigsawPiece *pieces,
        int nPieces, const char *feature, Pos3 *out, int maxOut);

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
    uint8_t seedExact;      // 0 = lootSeed is WRONG, see below
} JigsawChest;

/* feature elements like trees/hay bales are a lot more complicated to
 * check for, so for now if it enters the stream, all loot seeds downstream
 * are just garbage. So check if loot seed is 0/check seedExact before using it
 * 0 means it's garbage (assuming seedExact is 0), not that the actual seed is 0
 * 
 * In theory, you could get around this by considering 30 or so loot seeds
 * and checking them to see if they satisfy your conditions (and just manually
 * verify after the fact), but this is out of the scope of cubiomes as its for
 * a case-by-case basis. Just left as a note for myself/anyone who needs this
 * TODO feature elements
*/ 


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
 * @param g generator (only needed for terrain-dependent structures, otherwise NULL)
 * @param sn surface noise (only needed for terrain-dependent structures, otherwise NULL)
 * @param pieces array for pieces of the jigsaw structure
 * @param maxPieces max number of jigsaw pieces (1024 is more than enough, bastions cant get over 200)
 * @param nPieces optional to find how many pieces were generated (pass int pointer, otherwise NULL)
 * @param chests chest array (where the chests/loot seeds will go)
 * @param maxChests size of chests (128 should be plenty for any structure, bastions/villages have <16)
 * @return number of chests generated (or -1 if failed/arrays were too small)
 * 
*/
int getJigsawStructureLoot(int structureType, int mc, int biome, uint64_t seed,
        int chunkX, int chunkZ, const Generator *g, const SurfaceNoise *sn,
        JigsawPiece *pieces, int maxPieces,
        int *nPieces, JigsawChest *chests, int maxChests);

#endif

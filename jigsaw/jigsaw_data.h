#ifndef JIGSAW_DATA_H
#define JIGSAW_DATA_H

#include <stdint.h>

/* Data for jigsaw structure generation (bastions, villages,
 * pillager outposts, ancient cities, trail ruins, trial chambers)
 *
 * Extracted from the vanilla structure templates and template pool JSONs
 * Data kept: template sizes, jigsaw connector blocks and loot containers
 * Block contents and structure processes (e.g. bastion degradation) 
 * were not taken as I didn't think they were necessary 
 * (they do not affect loot or whether the structure generates)
 */

/* directions use vanilla Direction ordinals; opposite(d) == d^1 */
enum {
    JD_DOWN, JD_UP, JD_NORTH, JD_SOUTH, JD_WEST, JD_EAST, JD_NO_DIR = 0xff
};

enum { JIGSAW_JOINT_ROLLABLE, JIGSAW_JOINT_ALIGNED };

enum { JIGSAW_PROJECTION_RIGID, JIGSAW_PROJECTION_TERRAIN_MATCHING };

enum {
    JIGSAW_ELEMENT_SINGLE,
    JIGSAW_ELEMENT_LEGACY_SINGLE,
    JIGSAW_ELEMENT_FEATURE,
    JIGSAW_ELEMENT_LIST,
    JIGSAW_ELEMENT_EMPTY,
};

enum { JIGSAW_CONTAINER_CHEST, JIGSAW_CONTAINER_TRAPPED_CHEST, JIGSAW_CONTAINER_BARREL };

typedef struct JigsawBlockDef {
    uint8_t x, y, z;            // position inside the unrotated template
    uint8_t front, top;         // connector orientation
    uint8_t joint;
    int16_t selectionPriority;  // 1.20.3+, 0 otherwise
    int16_t placementPriority;  // 1.20.3+, 0 otherwise
    int16_t poolIdx;            // pool to draw the next piece from, -1 = none
    uint16_t name, target;      // connector ids, indices into strings
} JigsawBlockDef;

typedef struct JigsawContainerDef {
    uint8_t x, y, z;
    uint8_t kind;               // container
    uint8_t facing;             // JD_*, JD_NO_DIR if unset
    int16_t lootTable;          // index into strings, -1 if none
} JigsawContainerDef;

typedef struct JigsawTemplateDef {
    const char *name;                           // e.g. "bastion/units/air_base"
    uint8_t sx, sy, sz;
    uint16_t jigsawStart, jigsawCount;          // range in jigsawBlocks
    uint16_t containerStart, containerCount;    // range in containers
} JigsawTemplateDef;

typedef struct JigsawPoolEntryDef {
    int16_t templateIdx;        // single/legacy: index into templates, else -1
    int16_t featureIdx;         // feature: index into strings, else -1
    uint16_t weight;
    uint8_t kind;               // element
    uint8_t projection;
    uint16_t listStart, listCount; // list: member range in poolEntries
} JigsawPoolEntryDef;

typedef struct JigsawPoolDef {
    const char *name;                   // e.g. "bastion/starts"
    int16_t fallbackIdx;                // index into pools, -1 = empty
    uint16_t entryStart, entryCount;    // range in poolEntries, weights unexpanded
    uint16_t totalWeight;               // sum of weights = length of expanded list
} JigsawPoolDef;

typedef struct JigsawData {
    const JigsawTemplateDef *templates;     // sorted by name 
    int templateCount;
    const JigsawBlockDef *jigsawBlocks;
    const JigsawContainerDef *containers;
    const JigsawPoolDef *pools;             // sorted by name
    int poolCount;
    const JigsawPoolEntryDef *poolEntries;  // pool ranges, then list members
    const char *const *strings;
    int stringCount;
} JigsawData;

#endif

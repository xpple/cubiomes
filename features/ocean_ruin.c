#include "ocean_ruin.h"

#include <string.h>

#include "../biomes.h"

typedef struct
{
    const char *name;
    Pos chest;
    int hasChest;
}
OceanRuinTemplateInfo;

typedef struct
{
    int chunkX, chunkZ;
    Xoroshiro rng;
}
OceanRuinLootRng;

static const OceanRuinTemplateInfo warmSmall[8] = {
    {"underwater_ruin/warm_1", {3, 1}, 1},
    {"underwater_ruin/warm_2", {3, 4}, 1},
    {"underwater_ruin/warm_3", {3, 4}, 1},
    {"underwater_ruin/warm_4", {1, 2}, 1},
    {"underwater_ruin/warm_5", {3, 4}, 1},
    {"underwater_ruin/warm_6", {4, 4}, 1},
    {"underwater_ruin/warm_7", {3, 3}, 1},
    {"underwater_ruin/warm_8", {3, 3}, 1},
};

static const OceanRuinTemplateInfo warmBig[4] = {
    {"underwater_ruin/big_warm_4", {11, 8}, 1},
    {"underwater_ruin/big_warm_5", {7, 7}, 1},
    {"underwater_ruin/big_warm_6", {10, 9}, 1},
    {"underwater_ruin/big_warm_7", {11, 7}, 1},
};

static const OceanRuinTemplateInfo coldSmall[8][3] = {
    {{"underwater_ruin/brick_1", {3, 5}, 1}, {"underwater_ruin/cracked_1", {3, 5}, 1}, {"underwater_ruin/mossy_1", {0, 0}, 0}},
    {{"underwater_ruin/brick_2", {2, 1}, 1}, {"underwater_ruin/cracked_2", {2, 1}, 1}, {"underwater_ruin/mossy_2", {2, 1}, 1}},
    {{"underwater_ruin/brick_3", {1, 5}, 1}, {"underwater_ruin/cracked_3", {1, 5}, 1}, {"underwater_ruin/mossy_3", {1, 5}, 1}},
    {{"underwater_ruin/brick_4", {1, 4}, 1}, {"underwater_ruin/cracked_4", {1, 4}, 1}, {"underwater_ruin/mossy_4", {1, 4}, 1}},
    {{"underwater_ruin/brick_5", {4, 4}, 1}, {"underwater_ruin/cracked_5", {4, 4}, 1}, {"underwater_ruin/mossy_5", {4, 4}, 1}},
    {{"underwater_ruin/brick_6", {2, 2}, 1}, {"underwater_ruin/cracked_6", {2, 2}, 1}, {"underwater_ruin/mossy_6", {2, 2}, 1}},
    {{"underwater_ruin/brick_7", {1, 3}, 1}, {"underwater_ruin/cracked_7", {1, 3}, 1}, {"underwater_ruin/mossy_7", {1, 3}, 1}},
    {{"underwater_ruin/brick_8", {3, 4}, 1}, {"underwater_ruin/cracked_8", {3, 4}, 1}, {"underwater_ruin/mossy_8", {3, 4}, 1}},
};

static const OceanRuinTemplateInfo coldBig[4][3] = {
    {{"underwater_ruin/big_brick_1", {5, 4}, 1}, {"underwater_ruin/big_cracked_1", {5, 4}, 1}, {"underwater_ruin/big_mossy_1", {5, 4}, 1}},
    {{"underwater_ruin/big_brick_2", {9, 10}, 1}, {"underwater_ruin/big_cracked_2", {9, 10}, 1}, {"underwater_ruin/big_mossy_2", {9, 10}, 1}},
    {{"underwater_ruin/big_brick_3", {12, 2}, 1}, {"underwater_ruin/big_cracked_3", {12, 2}, 1}, {"underwater_ruin/big_mossy_3", {12, 2}, 1}},
    {{"underwater_ruin/big_brick_8", {5, 4}, 1}, {"underwater_ruin/big_cracked_8", {5, 4}, 1}, {"underwater_ruin/big_mossy_8", {5, 4}, 1}},
};

static int isWarmOceanRuinBiome(int biome)
{
    return biome == warm_ocean || biome == lukewarm_ocean ||
        biome == deep_lukewarm_ocean || biome == deep_warm_ocean;
}

static int isWarmOceanRuin(StructureSaltConfig ssconf, StructureVariant *sv)
{
    int biome = sv ? sv->biome : -1;
    if (biome != -1)
        return isWarmOceanRuinBiome(biome);
    return ssconf.generationStep == 4 && ssconf.decoratorIndex == 8;
}

static void initOceanRuinPiece(Piece *piece, const char *name, Pos origin,
        int rotation, int isLarge)
{
    memset(piece, 0, sizeof(*piece));
    piece->name = name;
    piece->pos = (Pos3) {origin.x, 0, origin.z};
    piece->rot = rotation;

    Pos span = isLarge ? (Pos) {15, 15} : (Pos) {5, 6};
    Pos3 offset = {0, 0, 0};
    Pos3 size = {span.x + 1, 1, span.z + 1};
    int facing;

    switch (rotation)
    {
    case 0:
        facing = 2;
        break;
    case 1:
        facing = 3;
        break;
    case 2:
        facing = 0;
        offset.x = -span.x;
        break;
    case 3:
        facing = 1;
        offset.x = -span.x;
        break;
    default: UNREACHABLE();
    }

    orientBox(piece->pos, offset, size, facing, &piece->bb0, &piece->bb1);
}

static Pos getOceanRuinChestPos(Piece piece, Pos chest)
{
    switch (piece.rot)
    {
    case 0: return (Pos) {piece.pos.x + chest.x, piece.pos.z + chest.z};
    case 1: return (Pos) {piece.pos.x - chest.z, piece.pos.z + chest.x};
    case 2: return (Pos) {piece.pos.x - chest.x, piece.pos.z - chest.z};
    case 3: return (Pos) {piece.pos.x + chest.z, piece.pos.z - chest.x};
    default: UNREACHABLE();
    }
}

static uint64_t nextLootSeed(OceanRuinLootRng *rngs, int *rngCount, int maxRngs,
        StructureSaltConfig ssconf, int mc, uint64_t seed, Pos chestPos)
{
    int chunkX = chestPos.x & ~15;
    int chunkZ = chestPos.z & ~15;
    for (int i = 0; i < *rngCount; i++)
    {
        if (rngs[i].chunkX == chunkX && rngs[i].chunkZ == chunkZ)
            return xNextLongJ(&rngs[i].rng);
    }

    if (*rngCount >= maxRngs)
        return 0;

    OceanRuinLootRng *rng = &rngs[(*rngCount)++];
    rng->chunkX = chunkX;
    rng->chunkZ = chunkZ;
    uint64_t populationSeed = getPopulationSeed(mc, seed, chunkX, chunkZ);
    xSetSeed(&rng->rng, populationSeed + ssconf.decoratorIndex + 10000 * ssconf.generationStep);
    return xNextLongJ(&rng->rng);
}

static int recordChest(Piece *list, int n, int *count,
        Piece piece, Pos chestPos, const char *lootTable, uint64_t lootSeed)
{
    for (int i = 0; i < *count; i++)
    {
        Piece *existing = &list[i];
        if (existing->chestCount == 1 &&
            existing->chestPoses[0].x == chestPos.x &&
            existing->chestPoses[0].z == chestPos.z)
        {
            existing->name = piece.name;
            existing->pos = piece.pos;
            existing->bb0 = piece.bb0;
            existing->bb1 = piece.bb1;
            existing->rot = piece.rot;
            existing->lootTables[0] = lootTable;
            existing->lootSeeds[0] = lootSeed;
            return 1;
        }
    }

    if (*count >= n)
        return 0;

    Piece *out = &list[(*count)++];
    *out = piece;
    out->chestCount = 1;
    out->chestPoses[0] = chestPos;
    out->lootSeeds[0] = lootSeed;
    out->lootTables[0] = lootTable;
    return 1;
}

static int addTemplate(Piece *list, int n, int *count,
        OceanRuinLootRng *rngs, int *rngCount, StructureSaltConfig ssconf,
        int mc, uint64_t seed, Pos origin, int rotation, int isLarge,
        OceanRuinTemplateInfo info)
{
    if (!info.hasChest)
        return 1;

    Piece piece;
    initOceanRuinPiece(&piece, info.name, origin, rotation, isLarge);
    Pos chestPos = getOceanRuinChestPos(piece, info.chest);
    uint64_t lootSeed = nextLootSeed(rngs, rngCount, 32, ssconf, mc, seed, chestPos);
    return recordChest(list, n, count, piece, chestPos,
            isLarge ? "underwater_ruin_big" : "underwater_ruin_small", lootSeed);
}

static int addPiece(Piece *list, int n, int *count,
        OceanRuinLootRng *rngs, int *rngCount, StructureSaltConfig ssconf,
        int mc, uint64_t seed, uint64_t *pieceRng, Pos origin, int rotation, int isWarm, int isLarge)
{
    if (isWarm)
    {
        int idx = nextInt(pieceRng, isLarge ? 4 : 8);
        return addTemplate(list, n, count, rngs, rngCount, ssconf, mc, seed,
                origin, rotation, isLarge, isLarge ? warmBig[idx] : warmSmall[idx]);
    }

    int idx = nextInt(pieceRng, isLarge ? 4 : 8);
    const OceanRuinTemplateInfo *templates = isLarge ? coldBig[idx] : coldSmall[idx];
    for (int i = 0; i < 3; i++)
    {
        if (!addTemplate(list, n, count, rngs, rngCount, ssconf, mc, seed,
                origin, rotation, isLarge, templates[i]))
            return 0;
    }
    return 1;
}

static int getAvailableCandidateIndex(uint8_t removedMask, int idx)
{
    for (int i = 0; i < 8; i++)
    {
        if (removedMask & (1 << i))
            continue;
        if (idx-- == 0)
            return i;
    }
    UNREACHABLE();
}

static int addClusterRuins(Piece *list, int n, int *count,
        OceanRuinLootRng *rngs, int *rngCount, StructureSaltConfig ssconf,
        int mc, uint64_t seed, uint64_t *pieceRng, Pos origin, int mainRotation, int isWarm)
{
    Piece mainPiece;
    initOceanRuinPiece(&mainPiece, NULL, origin, mainRotation, 1);

    Pos minCorner = {mainPiece.bb0.x, mainPiece.bb0.z};
    Pos candidates[8] = {
        {minCorner.x - 16 + nextIntBetween(pieceRng, 1, 8), minCorner.z + 16 + nextIntBetween(pieceRng, 1, 7)},
        {minCorner.x - 16 + nextIntBetween(pieceRng, 1, 8), minCorner.z + nextIntBetween(pieceRng, 1, 7)},
        {minCorner.x - 16 + nextIntBetween(pieceRng, 1, 8), minCorner.z - 16 + nextIntBetween(pieceRng, 4, 8)},
        {minCorner.x + nextIntBetween(pieceRng, 1, 7), minCorner.z + 16 + nextIntBetween(pieceRng, 1, 7)},
        {minCorner.x + nextIntBetween(pieceRng, 1, 7), minCorner.z - 16 + nextIntBetween(pieceRng, 4, 6)},
        {minCorner.x + 16 + nextIntBetween(pieceRng, 1, 7), minCorner.z + 16 + nextIntBetween(pieceRng, 3, 8)},
        {minCorner.x + 16 + nextIntBetween(pieceRng, 1, 7), minCorner.z + nextIntBetween(pieceRng, 1, 7)},
        {minCorner.x + 16 + nextIntBetween(pieceRng, 1, 7), minCorner.z - 16 + nextIntBetween(pieceRng, 4, 8)},
    };
    uint8_t removedMask = 0;
    int candidateCount = 8;
    int clusterCount = nextIntBetween(pieceRng, 4, 8);

    for (int i = 0; i < clusterCount && candidateCount > 0; i++)
    {
        int candidateIdx = getAvailableCandidateIndex(removedMask, nextInt(pieceRng, candidateCount));
        Pos clusterOrigin = candidates[candidateIdx];
        removedMask |= 1 << candidateIdx;
        candidateCount--;

        int clusterRotation = nextInt(pieceRng, 4);
        Piece clusterPiece;
        initOceanRuinPiece(&clusterPiece, NULL, clusterOrigin, clusterRotation, 0);
        if (hasIntersection(clusterPiece.bb0, clusterPiece.bb1, mainPiece.bb0, mainPiece.bb1))
            continue;

        if (!addPiece(list, n, count, rngs, rngCount, ssconf,
                mc, seed, pieceRng, clusterOrigin, clusterRotation, isWarm, 0))
            return 0;
    }
    return 1;
}

int getOceanRuinLoot(Piece *list, int n, StructureSaltConfig ssconf,
        StructureVariant *sv, int mc, uint64_t seed, int posX, int posZ)
{
    if (mc != MC_1_21_11 || n <= 0)
        return -1;

    uint64_t pieceRng = chunkGenerateRnd(seed, posX >> 4, posZ >> 4);
    int mainRotation = nextInt(&pieceRng, 4);
    int isLarge = nextFloat(&pieceRng) <= 0.3f;
    int isWarm = isWarmOceanRuin(ssconf, sv);
    Pos origin = {posX & ~15, posZ & ~15};
    OceanRuinLootRng lootRngs[32];
    int lootRngCount = 0;
    int count = 0;

    if (!addPiece(list, n, &count, lootRngs, &lootRngCount, ssconf,
            mc, seed, &pieceRng, origin, mainRotation, isWarm, isLarge))
        return -1;

    if (isLarge && nextFloat(&pieceRng) <= 0.9f)
    {
        if (!addClusterRuins(list, n, &count, lootRngs, &lootRngCount, ssconf,
                mc, seed, &pieceRng, origin, mainRotation, isWarm))
            return -1;
    }

    return count;
}

#include "ocean_ruin.h"

#include <string.h>

#include "piece.h"

STRUCT(OceanRuinTemplateInfo)
{
    int type;
    const char *name;
    Pos chest;
    int hasChest;
};

static const OceanRuinTemplateInfo warmSmall[8] = {
    {OR_WARM_1, "underwater_ruin/warm_1", {3, 1}, 1},
    {OR_WARM_2, "underwater_ruin/warm_2", {3, 4}, 1},
    {OR_WARM_3, "underwater_ruin/warm_3", {3, 4}, 1},
    {OR_WARM_4, "underwater_ruin/warm_4", {1, 2}, 1},
    {OR_WARM_5, "underwater_ruin/warm_5", {3, 4}, 1},
    {OR_WARM_6, "underwater_ruin/warm_6", {4, 4}, 1},
    {OR_WARM_7, "underwater_ruin/warm_7", {3, 3}, 1},
    {OR_WARM_8, "underwater_ruin/warm_8", {3, 3}, 1},
};

static const OceanRuinTemplateInfo warmBig[4] = {
    {OR_BIG_WARM_4, "underwater_ruin/big_warm_4", {11, 8}, 1},
    {OR_BIG_WARM_5, "underwater_ruin/big_warm_5", {7, 7}, 1},
    {OR_BIG_WARM_6, "underwater_ruin/big_warm_6", {10, 9}, 1},
    {OR_BIG_WARM_7, "underwater_ruin/big_warm_7", {11, 7}, 1},
};

static const OceanRuinTemplateInfo coldSmall[8][3] = {
    {{OR_BRICK_1, "underwater_ruin/brick_1", {3, 5}, 1}, {OR_CRACKED_1, "underwater_ruin/cracked_1", {3, 5}, 1}, {OR_MOSSY_1, "underwater_ruin/mossy_1", {0, 0}, 0}},
    {{OR_BRICK_2, "underwater_ruin/brick_2", {2, 1}, 1}, {OR_CRACKED_2, "underwater_ruin/cracked_2", {2, 1}, 1}, {OR_MOSSY_2, "underwater_ruin/mossy_2", {2, 1}, 1}},
    {{OR_BRICK_3, "underwater_ruin/brick_3", {1, 5}, 1}, {OR_CRACKED_3, "underwater_ruin/cracked_3", {1, 5}, 1}, {OR_MOSSY_3, "underwater_ruin/mossy_3", {1, 5}, 1}},
    {{OR_BRICK_4, "underwater_ruin/brick_4", {1, 4}, 1}, {OR_CRACKED_4, "underwater_ruin/cracked_4", {1, 4}, 1}, {OR_MOSSY_4, "underwater_ruin/mossy_4", {1, 4}, 1}},
    {{OR_BRICK_5, "underwater_ruin/brick_5", {4, 4}, 1}, {OR_CRACKED_5, "underwater_ruin/cracked_5", {4, 4}, 1}, {OR_MOSSY_5, "underwater_ruin/mossy_5", {4, 4}, 1}},
    {{OR_BRICK_6, "underwater_ruin/brick_6", {2, 2}, 1}, {OR_CRACKED_6, "underwater_ruin/cracked_6", {2, 2}, 1}, {OR_MOSSY_6, "underwater_ruin/mossy_6", {2, 2}, 1}},
    {{OR_BRICK_7, "underwater_ruin/brick_7", {1, 3}, 1}, {OR_CRACKED_7, "underwater_ruin/cracked_7", {1, 3}, 1}, {OR_MOSSY_7, "underwater_ruin/mossy_7", {1, 3}, 1}},
    {{OR_BRICK_8, "underwater_ruin/brick_8", {3, 4}, 1}, {OR_CRACKED_8, "underwater_ruin/cracked_8", {3, 4}, 1}, {OR_MOSSY_8, "underwater_ruin/mossy_8", {3, 4}, 1}},
};

static const OceanRuinTemplateInfo coldBig[4][3] = {
    {{OR_BIG_BRICK_1, "underwater_ruin/big_brick_1", {5, 4}, 1}, {OR_BIG_CRACKED_1, "underwater_ruin/big_cracked_1", {5, 4}, 1}, {OR_BIG_MOSSY_1, "underwater_ruin/big_mossy_1", {5, 4}, 1}},
    {{OR_BIG_BRICK_2, "underwater_ruin/big_brick_2", {9, 10}, 1}, {OR_BIG_CRACKED_2, "underwater_ruin/big_cracked_2", {9, 10}, 1}, {OR_BIG_MOSSY_2, "underwater_ruin/big_mossy_2", {9, 10}, 1}},
    {{OR_BIG_BRICK_3, "underwater_ruin/big_brick_3", {12, 2}, 1}, {OR_BIG_CRACKED_3, "underwater_ruin/big_cracked_3", {12, 2}, 1}, {OR_BIG_MOSSY_3, "underwater_ruin/big_mossy_3", {12, 2}, 1}},
    {{OR_BIG_BRICK_8, "underwater_ruin/big_brick_8", {5, 4}, 1}, {OR_BIG_CRACKED_8, "underwater_ruin/big_cracked_8", {5, 4}, 1}, {OR_BIG_MOSSY_8, "underwater_ruin/big_mossy_8", {5, 4}, 1}},
};

static int isWarmOceanRuinBiome(int biome)
{
    const uint64_t warm_bits = (1ULL << warm_ocean)
                             | (1ULL << lukewarm_ocean)
                             | (1ULL << deep_lukewarm_ocean)
                             | (1ULL << deep_warm_ocean);

    return (uint32_t) biome < 64 && ((1ULL << biome) & warm_bits);
}

enum {
    OCEAN_RUIN_MAX_PIECE_ATTEMPTS = 32,
};

static Pos getOceanRuinSpan(int isLarge)
{
    return isLarge ? (Pos) {15, 15} : (Pos) {5, 6};
}

static void initOceanRuinPiece(Piece *piece, const OceanRuinTemplateInfo *info, Pos origin,
        int rotation, int isLarge)
{
    memset(piece, 0, sizeof(*piece));
    piece->name = info ? info->name : NULL;
    piece->pos = (Pos3) {origin.x, 0, origin.z};
    piece->rot = rotation;
    piece->type = info ? info->type : -1;

    Pos span = getOceanRuinSpan(isLarge);
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

static Pos getOceanRuinMarkerPos(Piece piece, Pos marker, int isLarge)
{
    Pos span = getOceanRuinSpan(isLarge);
    int x = marker.x;
    int z = span.z - marker.z;
    if (piece.rot >= 2)
        x = span.x - marker.x;
    rotPos(piece.bb0, piece.bb1, &x, &z, piece.rot);
    return (Pos) {x, z};
}

static int recordPiece(Piece *list, int n, int *count, Piece piece)
{
    for (int i = 0; i < *count; i++)
    {
        Piece *existing = &list[i];
        if (existing->chestCount == 1 && piece.chestCount == 1 &&
            existing->chestPoses[0].x == piece.chestPoses[0].x &&
            existing->chestPoses[0].z == piece.chestPoses[0].z)
        {
            *existing = piece;
            return 1;
        }
    }

    if (*count >= n)
        return 0;

    list[(*count)++] = piece;
    return 1;
}

static int addTemplateAttempt(Piece *list, int n, int *count,
        Pos origin, int rotation, int isLarge, const OceanRuinTemplateInfo *info)
{
    if (*count >= n)
        return 0;

    Piece piece;
    initOceanRuinPiece(&piece, info, origin, rotation, isLarge);
    if (info->hasChest)
    {
        piece.chestCount = 1;
        piece.chestPoses[0] = getOceanRuinMarkerPos(piece, info->chest, isLarge);
        piece.lootTables[0] = isLarge ? "underwater_ruin_big" : "underwater_ruin_small";
    }
    list[(*count)++] = piece;
    return 1;
}

static int addPieceAttempts(Piece *list, int n, int *count,
        uint64_t *pieceRng, Pos origin, int rotation, int isWarm, int isLarge)
{
    if (isWarm)
    {
        int idx = nextInt(pieceRng, isLarge ? 4 : 8);
        return addTemplateAttempt(list, n, count,
                origin, rotation, isLarge, isLarge ? &warmBig[idx] : &warmSmall[idx]);
    }

    int idx = nextInt(pieceRng, isLarge ? 4 : 8);
    const OceanRuinTemplateInfo *templates = isLarge ? coldBig[idx] : coldSmall[idx];
    for (int i = 0; i < 3; i++)
    {
        if (!addTemplateAttempt(list, n, count,
                origin, rotation, isLarge, &templates[i]))
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

static int addClusterRuinAttempts(Piece *list, int n, int *count,
        uint64_t *pieceRng, Pos origin, int mainRotation, int isWarm)
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

        if (!addPieceAttempts(list, n, count, pieceRng, clusterOrigin, clusterRotation, isWarm, 0))
            return 0;
    }
    return 1;
}

static int getOceanRuinPieceAttempts(Piece *list, int n, StructureVariant *sv,
        int mc, uint64_t seed, int posX, int posZ)
{
    if (mc < MC_1_13 || n <= 0 || !sv)
        return -1;

    uint64_t pieceRng = chunkGenerateRnd(seed, posX >> 4, posZ >> 4);
    int mainRotation = nextInt(&pieceRng, 4);
    int isLarge = nextFloat(&pieceRng) <= 0.3f;
    int isWarm = isWarmOceanRuinBiome(sv->biome);
    Pos origin = {posX & ~15, posZ & ~15};
    int count = 0;

    if (!addPieceAttempts(list, n, &count, &pieceRng, origin, mainRotation, isWarm, isLarge))
        return -1;

    if (isLarge && nextFloat(&pieceRng) <= 0.9f)
    {
        if (!addClusterRuinAttempts(list, n, &count, &pieceRng, origin, mainRotation, isWarm))
            return -1;
    }

    return count;
}

int getOceanRuinPieces(Piece *list, int n, StructureVariant *sv,
        int mc, uint64_t seed, int posX, int posZ)
{
    Piece attempts[OCEAN_RUIN_MAX_PIECE_ATTEMPTS];
    int attemptCount = getOceanRuinPieceAttempts(attempts, OCEAN_RUIN_MAX_PIECE_ATTEMPTS,
            sv, mc, seed, posX, posZ);
    if (attemptCount < 0)
        return -1;

    int count = 0;
    for (int i = 0; i < attemptCount; i++)
    {
        if (!recordPiece(list, n, &count, attempts[i]))
            return -1;
    }
    return count;
}

static uint64_t getLootSeedForAttempt(Piece *attempts, int attemptIdx,
        StructureSaltConfig ssconf, int mc, uint64_t seed)
{
    Pos chestPos = attempts[attemptIdx].chestPoses[0];
    int chunkX = chestPos.x & ~15;
    int chunkZ = chestPos.z & ~15;
    int callIdx = 0;
    for (int i = 0; i < attemptIdx; i++)
    {
        Pos prevChestPos = attempts[i].chestPoses[0];
        if (attempts[i].chestCount == 1 &&
            (prevChestPos.x & ~15) == chunkX && (prevChestPos.z & ~15) == chunkZ)
            callIdx++;
    }

    Xoroshiro rng;
    uint64_t populationSeed = getPopulationSeed(mc, seed, chunkX, chunkZ);
    xSetSeed(&rng, populationSeed + ssconf.decoratorIndex + 10000 * ssconf.generationStep);
    uint64_t lootSeed = 0;
    for (int i = 0; i <= callIdx; i++)
        lootSeed = xNextLongJ(&rng);
    return lootSeed;
}

int getOceanRuinLoot(Piece *list, int n, StructureSaltConfig ssconf,
        StructureVariant *sv, int mc, uint64_t seed, int posX, int posZ)
{
    Piece attempts[OCEAN_RUIN_MAX_PIECE_ATTEMPTS];
    int attemptCount = getOceanRuinPieceAttempts(attempts, OCEAN_RUIN_MAX_PIECE_ATTEMPTS,
            sv, mc, seed, posX, posZ);
    if (attemptCount < 0)
        return -1;

    int count = 0;
    for (int i = 0; i < attemptCount; i++)
    {
        Piece piece = attempts[i];
        if (piece.chestCount == 1)
            piece.lootSeeds[0] = getLootSeedForAttempt(attempts, i, ssconf, mc, seed);
        if (!recordPiece(list, n, &count, piece))
            return -1;
    }
    return count;
}

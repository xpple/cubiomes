#include "testing.h"
#include "../finders.h"
#include "../features/end_city.h"
#include "../features/stronghold.h"

static int test_stronghold_pieces_1_16() {
    int ret = 0;

    // famous double 12-eye seed
    uint64_t seed = 3973602863870306476ULL;
    int version = MC_1_16_5;

    StructureSaltConfig ssconf;
    ASSERT_NONZERO(ret, getStructureSaltConfig(Stronghold, version, -1, &ssconf));

    const struct { const Pos chunk; const int numPieces, portalIndex, portalData; } test_data[2] = {
        {{-5512 >> 4, -10424 >> 4}, 137, 16, 0b1000111111111},
        {{-20008 >> 4, -6424 >> 4}, 139, 21, 0b1111111111111},
    };

    for (int i = 0; i < 2; ++i) {
        Pos chunk = test_data[i].chunk;

        int n = 1000;
        Piece *pieces = calloc(n, sizeof(Piece));
        int numPieces = getStrongholdLoot(pieces, n, ssconf, version, seed, chunk.x, chunk.z);
        ASSERT_EQ(ret, numPieces, test_data[i].numPieces);
        Piece *piece = &pieces[test_data[i].portalIndex];
        ASSERT_EQ(ret, piece->type, SH_PORTAL_ROOM);
        ASSERT_EQ(ret, piece->additionalData, test_data[i].portalData);
        free(pieces);
    }
    return ret;
}

static int test_end_city_pieces_1_21_11() {
    int ret = 0;

    uint64_t seed = 12345;
    int version = MC_1_21_11;
    Generator generator;
    setupGenerator(&generator, version, 0);
    applySeed(&generator, DIM_END, seed);

    int x = -1600;
    int z = 128;
    int maxPieces = END_CITY_PIECES_MAX;
    Piece *pieces = malloc(maxPieces * sizeof(Piece));
    int numPieces = getEndCityPieces(pieces, seed, x, z);

    int hasShip = 0;

    for (int i = 0; i < numPieces; i++) {
        Piece *piece = &pieces[i];
        if (piece->type == END_SHIP) {
            hasShip = 1;
            break;
        }
    }
    free(pieces);

    ASSERT_TRUE(ret, hasShip);
    return ret;
}

static int test_igloo_pieces_1_21_11() {
    int ret = 0;

    uint64_t seed = 12345;
    int version = MC_1_21_11;
    int structure = Igloo;
    Generator generator;
    setupGenerator(&generator, version, 0);
    applySeed(&generator, DIM_OVERWORLD, seed);

    int x = 2736;
    int z = 96;

    StructureVariant sv;
    // biome is unused for igloos
    ASSERT_NONZERO(ret, getVariant(&sv, structure, version, seed, x, z, -1));

    // igloos can have at most 7 + 4 middle pieces
    ASSERT_EQ(ret, 11, sv.size);
    // add two for the top and bottom piece
    int maxPieces = 11 + 2;
    Piece *pieces = malloc(maxPieces * sizeof(Piece));
    StructureSaltConfig saltConfig = {0}; // keep uninitialised
    int numPieces = getStructurePieces(pieces, maxPieces, structure, saltConfig, &sv, version, seed, x, z);

    int hasBasement = 0;
    ASSERT_EQ(ret, maxPieces, numPieces);

    for (int i = 0; i < numPieces; i++) {
        Piece *piece = &pieces[i];
        if (strcmp(piece->name, "igloo/bottom") == 0) {
            hasBasement = 1;
            break;
        }
    }
    free(pieces);

    ASSERT_TRUE(ret, hasBasement);
    return ret;
}

int main() {
    int ret = 0;

    ret += test_stronghold_pieces_1_16();
    ret += test_end_city_pieces_1_21_11();
    ret += test_igloo_pieces_1_21_11();

    return ret;
}

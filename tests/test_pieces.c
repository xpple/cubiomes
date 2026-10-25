#include "testing.h"
#include "../finders.h"
#include "../features/end_city.h"
#include "../features/village.h"

static int test_end_city_pieces_1_21_11() {
    int ret = 0;

    uint64_t seed = 12345;
    int version = MC_1_21_11;
    Generator generator;;
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

static int test_village_pieces_1_12_2() {
    int ret = 0;
    uint64_t seed = 6983438112510666596ULL;
    int mc = MC_1_12_2;

    int x = 624, z = 832;

    StructureConfig sconf;
    ASSERT_NONZERO(ret, getStructureConfig(Village, mc, &sconf));
    Pos pos;
    ASSERT_NONZERO(ret, getStructurePos(Village, mc, seed, floordiv(x >> 4, sconf.regionSize), floordiv(z >> 4, sconf.regionSize), &pos));
    ASSERT_EQ(ret, pos.x, x);
    ASSERT_EQ(ret, pos.z, z);

    Generator g;
    setupGenerator(&g, mc, 0);
    applySeed(&g, DIM_OVERWORLD, seed);

    ASSERT_NONZERO(ret, isViableStructurePos(Village, &g, x, z, plains));

    Piece *pieces = malloc((VILLAGE_PIECES_MAX + 1) * sizeof(Piece));
    int count = getVillagePieces(pieces, &g, x >> 4, z >> 4);

    int hasBlacksmith = 0;
    Pos blacksmithPos = {};
    for (int i = 0; i < count; ++i) {
        Piece *piece = &pieces[i];
        // blacksmith
        if (piece->type == VILLAGE_HOUSE_2) {
            hasBlacksmith = 1;
            blacksmithPos.x = piece->pos.x;
            blacksmithPos.z = piece->pos.z;
            break;
        }
    }
    ASSERT_TRUE(ret, hasBlacksmith);
    ASSERT_EQ(ret, blacksmithPos.x, 633);
    ASSERT_EQ(ret, blacksmithPos.z, 834);

    return ret;
}

int main() {
    int ret = 0;

    ret += test_village_pieces_1_12_2();
    ret += test_end_city_pieces_1_21_11();
    ret += test_igloo_pieces_1_21_11();

    return ret;
}

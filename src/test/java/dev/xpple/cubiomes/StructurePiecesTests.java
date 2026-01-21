package dev.xpple.cubiomes;

import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;

import java.lang.foreign.Arena;
import java.lang.foreign.MemorySegment;
import java.util.stream.IntStream;

import static org.junit.jupiter.api.Assertions.*;

public class StructurePiecesTests {
    private final Arena arena = Arena.global();

    @BeforeAll
    public static void initCubiomes() {
        CubiomesInit.load();
    }

    @Test
    public void testEndCityPieces1_21_11() {
        long seed = 12345;
        int version = Cubiomes.MC_1_21_11();
        MemorySegment generator = Generator.allocate(this.arena);
        Cubiomes.setupGenerator(generator, version, 0);
        Cubiomes.applySeed(generator, Cubiomes.DIM_END(), seed);

        int x = -1600;
        int z = 128;
        int maxPieces = Cubiomes.END_CITY_PIECES_MAX();
        MemorySegment pieces = Piece.allocateArray(maxPieces, this.arena);
        int numPieces = Cubiomes.getEndCityPieces(pieces, seed, x, z);

        assertTrue(IntStream.range(0, numPieces)
            .mapToObj(i -> Piece.asSlice(pieces, i))
            .anyMatch(p -> Piece.type(p) == Cubiomes.END_SHIP()));
    }

    @Test
    public void testIglooPieces1_21_11() {
        long seed = 12345;
        int version = Cubiomes.MC_1_21_11();
        int structure = Cubiomes.Igloo();
        MemorySegment generator = Generator.allocate(this.arena);
        Cubiomes.setupGenerator(generator, version, 0);
        Cubiomes.applySeed(generator, Cubiomes.DIM_OVERWORLD(), seed);

        int x = 2736;
        int z = 96;

        MemorySegment structureVariant = StructureVariant.allocate(this.arena);
        // biome is unused for igloos
        assertNotEquals(0, Cubiomes.getVariant(structureVariant, structure, version, seed, x, z, -1));

        // igloos can have at most 7 + 4 middle pieces
        assertEquals(11, StructureVariant.size(structureVariant));
        // add two for the top and bottom piece
        int maxPieces = 11 + 2;
        MemorySegment pieces = Piece.allocateArray(maxPieces, this.arena);
        MemorySegment saltConfig = StructureSaltConfig.allocate(this.arena); // keep uninitialised
        int numPieces = Cubiomes.getStructurePieces(pieces, maxPieces, structure, saltConfig, structureVariant, version, seed, x, z);
        assertEquals(maxPieces, numPieces);

        assertTrue(IntStream.range(0, numPieces)
            .mapToObj(i -> Piece.asSlice(pieces, i))
            .anyMatch(p -> Piece.name(p).getString(0).equals("igloo/bottom")));
    }
}

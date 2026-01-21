package dev.xpple.cubiomes;

import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;

import java.lang.foreign.Arena;
import java.lang.foreign.MemorySegment;

import static org.junit.jupiter.api.Assertions.*;

public class GeneratorTests {

    private final Arena arena = Arena.global();

    @BeforeAll
    public static void initCubiomes() {
        CubiomesInit.load();
    }

    @Test
    public void testBiome1_16_1Overworld() {
        MemorySegment generator = Generator.allocate(this.arena);
        Cubiomes.setupGenerator(generator, Cubiomes.MC_1_16_1(), 0);
        Cubiomes.applySeed(generator, Cubiomes.DIM_OVERWORLD(), 12345);
        int biome = Cubiomes.getBiomeAt(generator, 1, 0, 0, 0);
        assertEquals(Cubiomes.taiga_mountains(), biome);
    }

    @Test
    public void testBiome1_18_2Nether() {
        MemorySegment generator = Generator.allocate(this.arena);
        Cubiomes.setupGenerator(generator, Cubiomes.MC_1_18_2(), 0);
        Cubiomes.applySeed(generator, Cubiomes.DIM_NETHER(), 12345);
        int biome = Cubiomes.getBiomeAt(generator, 1, 0, 0, 0);
        assertEquals(Cubiomes.nether_wastes(), biome);
    }

    @Test
    public void testScaledBiome1_21_11LB() {
        MemorySegment generator = Generator.allocate(this.arena);
        Cubiomes.setupGenerator(generator, Cubiomes.MC_1_21_11(), Cubiomes.LARGE_BIOMES());
        Cubiomes.applySeed(generator, Cubiomes.DIM_OVERWORLD(), 12345);
        // scale 4 is biome scale, the scale at which biomes are initially calculated
        int biome = Cubiomes.getBiomeAt(generator, 4, -341 >> 2, -33 >> 2, -288 >> 2);
        assertEquals(Cubiomes.lush_caves(), biome);
    }

    @Test
    public void testBiome1_9_4End() {
        MemorySegment generator = Generator.allocate(this.arena);
        Cubiomes.setupGenerator(generator, Cubiomes.MC_1_9_4(), 0);
        Cubiomes.applySeed(generator, Cubiomes.DIM_END(), 12345);
        int biome = Cubiomes.getBiomeAt(generator, 1, 700, 0, -2411);
        assertEquals(Cubiomes.small_end_islands(), biome);
    }
}

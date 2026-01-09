package com.github.cubiomes;

import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;

import java.lang.foreign.Arena;
import java.lang.foreign.MemorySegment;

import static org.junit.jupiter.api.Assertions.*;

public class CarverTests {
    private final Arena arena = Arena.global();

    @BeforeAll
    public static void initCubiomes() {
        CubiomesInit.load();
    }

    @Test
    public void testCanyonCarver1_21_11() {
        long seed = 12345;
        int version = Cubiomes.MC_1_21_11();
        MemorySegment canyonCarverConfig = CanyonCarverConfig.allocate(this.arena);
        Cubiomes.getCanyonCarverConfig(Cubiomes.CANYON_CARVER(), version, canyonCarverConfig);

        int cx = 2720 >> 4;
        int cz = 96 >> 4;
        MemorySegment rnd = this.arena.allocate(Cubiomes.C_LONG_LONG); // unused
        assertNotEquals(0, Cubiomes.checkCanyonStart(seed, cx, cz, canyonCarverConfig, rnd));
    }
}

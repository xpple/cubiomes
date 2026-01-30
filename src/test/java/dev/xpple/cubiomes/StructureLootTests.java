package dev.xpple.cubiomes;

import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;

import java.lang.foreign.Arena;
import java.lang.foreign.MemorySegment;
import java.lang.foreign.ValueLayout;

import static org.junit.jupiter.api.Assertions.*;

public class StructureLootTests {
    private final Arena arena = Arena.global();

    @BeforeAll
    public static void initCubiomes() {
        CubiomesInit.load();
    }

    @Test
    public void testOutpostLoot1_21_9() {
        long seed = -7594343816690189614L;
        int structure = Cubiomes.Outpost();
        int version = Cubiomes.MC_1_21_9();
        MemorySegment generator = Generator.allocate(this.arena);
        Cubiomes.setupGenerator(generator, version, 0);
        Cubiomes.applySeed(generator, Cubiomes.DIM_OVERWORLD(), seed);

        MemorySegment saltConfig = StructureSaltConfig.allocate(this.arena);
        // in 1.18+ the salts do not depend on the biome
        assertNotEquals(0, Cubiomes.getStructureSaltConfig(structure, version, -1, saltConfig));

        int x = 1344;
        int z = -9872;
        MemorySegment structureVariant = StructureVariant.allocate(this.arena);
        // the biome does not matter for the outpost variant
        assertNotEquals(0, Cubiomes.getVariant(structureVariant, structure, version, seed, x, z, -1));

        // outposts consist of only one piece
        MemorySegment piece = Piece.allocate(this.arena);
        int numPieces = Cubiomes.getStructurePieces(piece, 1, structure, saltConfig, structureVariant, version, seed, x, z);
        assertEquals(1, numPieces);

        // in C, strings are char*, so we use the layout `ValueLayout.ADDRESS`
        // since strings are null terminated, we reinterpret to `Long.MAX_VALUE` to create space
        MemorySegment lootTable = Piece.lootTables(piece).getAtIndex(ValueLayout.ADDRESS, 0).reinterpret(Long.MAX_VALUE);
        // to get a Java string, call `getString(0)`
        assertEquals("pillager_outpost", lootTable.getString(0));
        long lootSeed = Piece.lootSeeds(piece).getAtIndex(Cubiomes.C_LONG_LONG, 0);

        // `init_loot_table_name` expects a LootTableContext**
        MemorySegment ltcPtr = this.arena.allocate(Cubiomes.C_POINTER);
        assertNotEquals(0, Cubiomes.init_loot_table_name(ltcPtr, lootTable, version));
        // this does `*ltcPtr`
        MemorySegment ltc = ltcPtr.get(ValueLayout.ADDRESS, 0).reinterpret(LootTableContext.sizeof());
        Cubiomes.set_loot_seed(ltc, lootSeed);
        Cubiomes.generate_loot(ltc);

        boolean hasFortune1Book = false;

        int lootCount = LootTableContext.generated_item_count(ltc);
        for (int lootIdx = 0; lootIdx < lootCount; lootIdx++) {
            MemorySegment itemStack = ItemStack.asSlice(LootTableContext.generated_items(ltc), lootIdx);
            int itemId = Cubiomes.get_global_item_id(ltc, ItemStack.item(itemStack));
            if (itemId != Cubiomes.ITEM_BOOK()) {
                continue;
            }
            MemorySegment enchantments = ItemStack.enchantments(itemStack);
            int enchantmentCount = ItemStack.enchantment_count(itemStack);
            for (int enchantmentIdx = 0; enchantmentIdx < enchantmentCount; enchantmentIdx++) {
                MemorySegment enchantInstance = EnchantInstance.asSlice(enchantments, enchantmentIdx);
                int itemEnchantment = EnchantInstance.enchantment(enchantInstance);
                if (itemEnchantment != Cubiomes.FORTUNE()) {
                    continue;
                }
                int level = EnchantInstance.level(enchantInstance);
                if (level != 1) {
                    continue;
                }
                hasFortune1Book = true;
            }
        }

        assertTrue(hasFortune1Book);
    }
}

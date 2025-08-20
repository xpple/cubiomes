#CC      = gcc
#AR      = ar
ARFLAGS = cr
override LDFLAGS = -lm
override CFLAGS += -Wall -Wextra -fwrapv

ifeq ($(OS),Windows_NT)
	override CFLAGS += -D_WIN32
	CC = gcc
	RM = del
else
	override LDFLAGS += -pthread
	#RM = rm
endif

.PHONY : all debug release native libcubiomes clean

all: release

debug: CFLAGS += -DDEBUG -O0 -ggdb3
debug: libcubiomes
release: CFLAGS += -O3
release: libcubiomes
native: CFLAGS += -O3 -march=native -ffast-math
native: libcubiomes

ifneq ($(OS),Windows_NT)
release: CFLAGS += -fPIC
#debug: CFLAGS += -fsanitize=undefined
endif


libcubiomes: noise.o biomes.o layers.o biomenoise.o generator.o finders.o util.o quadbase.o items.o loot_functions.o loot_table_context.o loot_table_parser.o loot_tables.o cjson.o bastion_bridge_1_16_1.o bastion_bridge_1_16_5.o bastion_bridge_1_20.o bastion_other_1_16_1.o bastion_other_1_16_5.o bastion_other_1_20.o bastion_other_1_21_1.o buried_treasure_1_13.o buried_treasure_1_18.o desert_pyramid_1_13.o desert_pyramid_1_20.o end_city_treasure_1_13.o end_city_treasure_1_20.o igloo_chest_1_13.o jungle_temple_1_13.o jungle_temple_1_14.o jungle_temple_1_20.o jungle_temple_dispenser_1_13.o nether_bridge_1_13.o nether_bridge_1_20.o pillager_outpost_1_14.o pillager_outpost_1_19_2.o pillager_outpost_1_20.o ruined_portal_1_16_1.o ruined_portal_1_21_5.o
	$(AR) $(ARFLAGS) libcubiomes.a $^

finders.o: finders.c finders.h
	$(CC) -c $(CFLAGS) $<

generator.o: generator.c generator.h
	$(CC) -c $(CFLAGS) $<

biomenoise.o: biomenoise.c
	$(CC) -c $(CFLAGS) $<

biometree.o: biometree.c
	$(CC) -c $(CFLAGS) $<

layers.o: layers.c layers.h
	$(CC) -c $(CFLAGS) $<

biomes.o: biomes.c biomes.h
	$(CC) -c $(CFLAGS) $<

noise.o: noise.c noise.h
	$(CC) -c $(CFLAGS) $<

util.o: util.c util.h
	$(CC) -c $(CFLAGS) $<

quadbase.o: quadbase.c quadbase.h
	$(CC) -c $(CFLAGS) $<

items.o: loot/items.c loot/items.h
	$(CC) -c $(CFLAGS) $<

loot_functions.o: loot/loot_functions.c loot/loot_functions.h
	$(CC) -c $(CFLAGS) $<

loot_table_context.o: loot/loot_table_context.c loot/loot_table_context.h
	$(CC) -c $(CFLAGS) $<

loot_table_parser.o: loot/loot_table_parser.c
	$(CC) -c $(CFLAGS) $<

loot_tables.o: loot/loot_tables.c loot/loot_tables.h
	$(CC) -c $(CFLAGS) $<

bastion_bridge_1_16_1.o: loot/loot_tables/bastion_bridge_1_16_1.c loot/loot_tables/bastion_bridge_1_16_1.h
	$(CC) -c $(CFLAGS) $<

bastion_bridge_1_16_5.o: loot/loot_tables/bastion_bridge_1_16_5.c loot/loot_tables/bastion_bridge_1_16_5.h
	$(CC) -c $(CFLAGS) $<

bastion_bridge_1_20.o: loot/loot_tables/bastion_bridge_1_20.c loot/loot_tables/bastion_bridge_1_20.h
	$(CC) -c $(CFLAGS) $<

bastion_other_1_16_1.o: loot/loot_tables/bastion_other_1_16_1.c loot/loot_tables/bastion_other_1_16_1.h
	$(CC) -c $(CFLAGS) $<

bastion_other_1_16_5.o: loot/loot_tables/bastion_other_1_16_5.c loot/loot_tables/bastion_other_1_16_5.h
	$(CC) -c $(CFLAGS) $<

bastion_other_1_20.o: loot/loot_tables/bastion_other_1_20.c loot/loot_tables/bastion_other_1_20.h
	$(CC) -c $(CFLAGS) $<

bastion_other_1_21_1.o: loot/loot_tables/bastion_other_1_21_1.c loot/loot_tables/bastion_other_1_21_1.h
	$(CC) -c $(CFLAGS) $<

buried_treasure_1_13.o: loot/loot_tables/buried_treasure_1_13.c loot/loot_tables/buried_treasure_1_13.h
	$(CC) -c $(CFLAGS) $<

buried_treasure_1_18.o: loot/loot_tables/buried_treasure_1_18.c loot/loot_tables/buried_treasure_1_18.h
	$(CC) -c $(CFLAGS) $<

desert_pyramid_1_13.o: loot/loot_tables/desert_pyramid_1_13.c loot/loot_tables/desert_pyramid_1_13.h
	$(CC) -c $(CFLAGS) $<

desert_pyramid_1_20.o: loot/loot_tables/desert_pyramid_1_20.c loot/loot_tables/desert_pyramid_1_20.h
	$(CC) -c $(CFLAGS) $<

end_city_treasure_1_13.o: loot/loot_tables/end_city_treasure_1_13.c loot/loot_tables/end_city_treasure_1_13.h
	$(CC) -c $(CFLAGS) $<

end_city_treasure_1_20.o: loot/loot_tables/end_city_treasure_1_20.c loot/loot_tables/end_city_treasure_1_20.h
	$(CC) -c $(CFLAGS) $<

igloo_chest_1_13.o: loot/loot_tables/igloo_chest_1_13.c loot/loot_tables/igloo_chest_1_13.h
	$(CC) -c $(CFLAGS) $<

jungle_temple_1_13.o: loot/loot_tables/jungle_temple_1_13.c loot/loot_tables/jungle_temple_1_13.h
	$(CC) -c $(CFLAGS) $<

jungle_temple_1_14.o: loot/loot_tables/jungle_temple_1_14.c loot/loot_tables/jungle_temple_1_14.h
	$(CC) -c $(CFLAGS) $<

jungle_temple_1_20.o: loot/loot_tables/jungle_temple_1_20.c loot/loot_tables/jungle_temple_1_20.h
	$(CC) -c $(CFLAGS) $<

jungle_temple_dispenser_1_13.o: loot/loot_tables/jungle_temple_dispenser_1_13.c loot/loot_tables/jungle_temple_dispenser_1_13.h
	$(CC) -c $(CFLAGS) $<

nether_bridge_1_13.o: loot/loot_tables/nether_bridge_1_13.c loot/loot_tables/nether_bridge_1_13.h
	$(CC) -c $(CFLAGS) $<

nether_bridge_1_20.o: loot/loot_tables/nether_bridge_1_20.c loot/loot_tables/nether_bridge_1_20.h
	$(CC) -c $(CFLAGS) $<

pillager_outpost_1_14.o: loot/loot_tables/pillager_outpost_1_14.c loot/loot_tables/pillager_outpost_1_14.h
	$(CC) -c $(CFLAGS) $<

pillager_outpost_1_19_2.o: loot/loot_tables/pillager_outpost_1_19_2.c loot/loot_tables/pillager_outpost_1_19_2.h
	$(CC) -c $(CFLAGS) $<

pillager_outpost_1_20.o: loot/loot_tables/pillager_outpost_1_20.c loot/loot_tables/pillager_outpost_1_20.h
	$(CC) -c $(CFLAGS) $<

ruined_portal_1_16_1.o: loot/loot_tables/ruined_portal_1_16_1.c loot/loot_tables/ruined_portal_1_16_1.h
	$(CC) -c $(CFLAGS) $<

ruined_portal_1_21_5.o: loot/loot_tables/ruined_portal_1_21_5.c loot/loot_tables/ruined_portal_1_21_5.h
	$(CC) -c $(CFLAGS) $<

cjson.o: loot/cjson/cJSON.c loot/cjson/cJSON.h
	$(CC) -c $(CFLAGS) $<

clean:
	$(RM) *.o *.a


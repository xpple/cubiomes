cmake_minimum_required(VERSION 2.8)
project(cubiomes)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()
if (WIN32)
	set(CMAKE_C_FLAGS "-D_WIN32")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -DDEBUG")

set(HEADERS
	finders.h
	generator.h
	layers.h
	biomenoise.h
	biomes.h
	noise.h
	rng.h
	util.h
	quadbase.h
	loot/desert_temple.h
	loot/logging.h
	loot/loot_functions.h
	loot/loot_tables.h
	loot/loot_table_context.h
	loot/loot_table_parser.h
	loot/mc_loot.h
	loot/ruined_portal.h
	loot/loot_tables/desert_pyramid_1_13.h
	loot/loot_tables/ruined_portal_1_21.h
	loot/cjson/cJSON.h
)
set(SOURCES
	finders.c
	generator.c
	layers.c
	biomenoise.c
	biomes.c
	noise.c
	util.c
	quadbase.c
	test_loot.c
	loot/loot_functions.c
	loot/loot_tables.c
	loot/loot_table_context.c
	loot/loot_table_parser.c
	loot/loot_tables/desert_pyramid_1_13.c
	loot/loot_tables/ruined_portal_1_21.c
	loot/cjson/cJSON.c
)

add_library(objects OBJECT ${SOURCES})
set_property(TARGET objects PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(cubiomes SHARED $<TARGET_OBJECTS:objects>)
add_library(cubiomes_static STATIC $<TARGET_OBJECTS:objects>)

install(TARGETS cubiomes cubiomes_static DESTINATION lib)
install(FILES ${HEADERS} DESTINATION include)



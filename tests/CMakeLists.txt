function(add_cubiomes_test name source)
    add_executable(${name} ${source})

    target_link_libraries(${name} PRIVATE
        cubiomes
        m
    )

    if (WIN32)
        set_property(TEST ${name} PROPERTY
            ENVIRONMENT_MODIFICATION
            PATH=path_list_append:$<TARGET_RUNTIME_DLL_DIRS:cubiomes>
        )
    endif()

    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_cubiomes_test(test_biomes test_biomes.c)
add_cubiomes_test(test_structures test_structures.c)
add_cubiomes_test(test_loot test_loot.c)
add_cubiomes_test(test_pieces test_pieces.c)
add_cubiomes_test(test_carvers test_carvers.c)

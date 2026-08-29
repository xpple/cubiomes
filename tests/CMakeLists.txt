function(add_cubiomes_test name source)
    add_executable(${name} ${source})

    target_link_libraries(${name} PRIVATE
        cubiomes
        m
    )

    add_test(NAME ${name} COMMAND ${name})

    if (WIN32)
        set_property(TEST ${name} PROPERTY
            ENVIRONMENT_MODIFICATION
            PATH=path_list_prepend:$<TARGET_FILE_DIR:cubiomes>
        )
    endif()
endfunction()

add_cubiomes_test(test_biomes test_biomes.c)
add_cubiomes_test(test_structures test_structures.c)
add_cubiomes_test(test_loot test_loot.c)
add_cubiomes_test(test_pieces test_pieces.c)
add_cubiomes_test(test_carvers test_carvers.c)

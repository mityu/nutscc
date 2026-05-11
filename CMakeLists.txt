cmake_minimum_required(VERSION 4.0)

project(nutscc C)

option(NUTSCC_DISABLE_TEST_ALL "Disable all of the test targets.")
option(NUTSCC_DISABLE_TEST_GC "Diable tests of GC module.")

# Generate compile_commands.json at the root of the project when compiler is either of GCC or Clang.
if (CMAKE_EXPORT_COMPILE_COMMANDS AND (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang"))
    add_custom_target(copy_compile_commands ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_BINARY_DIR}/compile_commands.json"
                "${CMAKE_SOURCE_DIR}/compile_commands.json"
        DEPENDS "${CMAKE_BINARY_DIR}/compile_commands.json"
        COMMENT "Copying compile_commands.json to project root"
        VERBATIM
    )
endif()

add_library(test_framework INTERFACE)
block()
    set(TEST_DISABLER_LIST
        NUTSCC_DISABLE_TEST_GC
    )

    if (NUTSCC_DISABLE_TEST_ALL)
        # Make flag on to disable each test suite.
        foreach (VARNAME ${TEST_DISABLER_LIST})
            set(${VARNAME} ON)
        endforeach()
    endif()

    set(ENABLE_TESTS FALSE)
    foreach(VARNAME ${TEST_DISABLER_LIST})
        if(NOT ${${VARNAME}})
            set(ENABLE_TESTS TRUE)
        else()
            string(REGEX REPLACE "^NUTSCC_DISABLE_TEST_" "" MODNAME ${VARNAME})
            string(TOLOWER ${MODNAME} MODNAME)
            message(WARNING "Tests for ${MODNAME} module are disabled.")
        endif()
    endforeach()

    if (ENABLE_TESTS)
        add_subdirectory(nutest)
        target_link_libraries(test_framework INTERFACE nutest)
        enable_testing()
    else()
        message(WARNING "All tests are disabled.")
    endif()
endblock()

add_compile_options(-Wall)
add_subdirectory(gc)
add_subdirectory(gen_embeddable)

cmake_minimum_required(VERSION 4.0)

project(nutscc C)

option(NUTSCC_DISABLE_TEST "Disable test targets.")

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

if (NOT NUTSCC_DISABLE_TEST)
    set(MUNIT_DIR submodule/munit)
    add_library(munit STATIC)
    target_sources(munit
        PUBLIC FILE_SET HEADERS
        BASE_DIRS ${MUNIT_DIR}
        FILES ${MUNIT_DIR}/munit.h
    )
    target_sources(munit PRIVATE ${MUNIT_DIR}/munit.c)
    enable_testing()
endif()

add_compile_options(-Wall)
add_subdirectory(gc)
add_subdirectory(gen_embeddable)

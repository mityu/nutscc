add_library(gc STATIC)
target_sources(gc
    PUBLIC FILE_SET HEADERS
    BASE_DIRS include
    FILES include/gc/gc.h
)
target_sources(gc
    PRIVATE
    gc.c hashmap.c hashmap.h
)
if (NOT NUTSCC_GC_DONT_USE_DATA_SEGMENT_SYMBOL AND (CMAKE_EXPORT_COMPILE_COMMANDS AND (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")))
    target_compile_options(gc PRIVATE -DNUTSCC_GC_USE_DATA_SEGMENT_SYMBOL)
endif()

add_subdirectory(perf)
if (NOT NUTSCC_DISABLE_TEST_GC)
    add_subdirectory(test)
endif ()

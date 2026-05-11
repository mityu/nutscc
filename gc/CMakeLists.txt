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

add_subdirectory(perf)
if (NOT NUTSCC_DISABLE_TEST_GC)
    add_subdirectory(test)
endif ()

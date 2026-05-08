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
add_subdirectory(test)

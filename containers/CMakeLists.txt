add_library(containers STATIC)

target_sources(containers
    PUBLIC FILE_SET HEADERS
    BASE_DIRS include
    FILES
    include/containers/vector.h
    include/containers/string.h
)
target_sources(containers
    PRIVATE
    src/vector.c
    src/string.c
)
target_link_libraries(containers PUBLIC gc)

if (NOT NUTSCC_DISABLE_TEST_CONTAINERS)
    add_subdirectory(test)
endif()

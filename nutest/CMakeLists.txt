set(TARGET "nutest")

include(cmake/Unity.cmake)
add_library(${TARGET})

target_sources(${TARGET}
    PUBLIC FILE_SET HEADERS
    BASE_DIRS include
    FILES include/nutest.h
)

target_sources(${TARGET}
    PRIVATE
    src/nutest.c
)

target_link_libraries(${TARGET} unity)

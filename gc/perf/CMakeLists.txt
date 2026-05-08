add_library(perfexe INTERFACE)
target_sources(perfexe INTERFACE main.c)
target_link_libraries(perfexe INTERFACE gc)

add_executable(nogc)
target_link_libraries(nogc PRIVATE perfexe)
add_custom_target(perf-nogc
    COMMAND pidstat -r 1 -e "${CMAKE_CURRENT_BINARY_DIR}/nogc"
    COMMENT "Run pidstat on the gc/perf/nogc binary"
    VERBATIM
)

add_executable(withgc)
target_compile_options(withgc PRIVATE -DENABLE_GC)
target_link_libraries(withgc PRIVATE perfexe)
add_custom_target(perf-withgc
    COMMAND pidstat -r 1 -e "${CMAKE_CURRENT_BINARY_DIR}/withgc"
    COMMENT "Run pidstat on the gc/perf/withgc binary"
    VERBATIM
)

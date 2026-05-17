set(TARGET "nutest.test.test_nutest")
add_executable(${TARGET})
target_sources(${TARGET}
    PRIVATE
    test_nutest.c
    test_assertions.c
    test_execution_order.c
    test_setup_teardown.c
)
target_link_libraries(${TARGET} test_framework)
add_test(NAME "nutest/test/nutest" COMMAND ${TARGET})

function(add_test_target TESTNAME SRCS)
    set(TARGET "containers.test.test_${TESTNAME}")
    add_executable(${TARGET} ${SRCS})
    target_link_libraries(${TARGET} containers nutest)
    add_test(NAME "containers/test/${TESTNAME}" COMMAND ${TARGET})
endfunction()

add_test_target(string test_string.c)

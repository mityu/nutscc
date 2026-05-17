#include "nutest.h"
#include <string.h>

struct {
    size_t size;
    char names[4][30];
} exec_func = {0, {}};

void record_exec_func(const char *fn) {
    const size_t fnlen = strlen(fn);

    assert_size_lt(exec_func.size, sizeof(exec_func.names) / sizeof(exec_func.names[0]));
    assert_size_lt(fnlen, sizeof(exec_func.names[0]));

    strcpy(exec_func.names[exec_func.size++], fn);
}

void test_exec0(void *data) {
    (void)data;
    record_exec_func("test_exec0");
}

void test_exec1(void *data) {
    (void)data;
    record_exec_func("test_exec1");
}

void test_exec2(void *data) {
    (void)data;
    record_exec_func("test_exec2");
}

void test_exec3(void *data) {
    (void)data;
    record_exec_func("test_exec3");
}

void test_check_called(void *data) {
    assert_size_equal(exec_func.size, 4);
    assert_string_equal(exec_func.names[0], "test_exec0");
    assert_string_equal(exec_func.names[1], "test_exec1");
    assert_string_equal(exec_func.names[2], "test_exec2");
    assert_string_equal(exec_func.names[3], "test_exec3");
}

static NutestTest test_execution_order[] = {
        {"/exec0", test_exec0},
        {"/exec1", test_exec1},
        {"/exec2", test_exec2},
        {"/exec3", test_exec3},
        {"/check called", test_check_called},
        {},
};

NutestSuite suite_execution_order = {"/test execution order", test_execution_order};

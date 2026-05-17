#include "nutest.h"
#include <string.h>

void *setup_data_modification(void *data) {
    static char str[] = "setup_data_modification";
    assert_null(data);
    return str;
}

void test_data_modification(void *data) {
    const char after[] = "modified";
    assert_string_equal("setup_data_modification", (char *)data);
    assert_size_lt(strlen(after), strlen((char *)data));
    strcpy(data, after);
}

void *teardown_data_modification(void *data) {
    assert_string_equal("modified", (char *)data);
    return NULL;
}

void *setup_count_called(void *data) {
    static int count = 0;
    count++;
    return &count;
}

void test_count_called1(void *data) { assert_int_equal(1, *(int *)data); }

void test_count_called2(void *data) { assert_int_equal(2, *(int *)data); }

void *teardown_count_called(void *data) {
    static int count = 0;
    count++;
    assert_int_equal(count, *(int *)data);
    return NULL;
}

void *setup_depth1(void *data) {
    assert_null(data);
    return "setup_depth1";
}

void *setup_depth2(void *data) {
    assert_string_equal((char *)data, "setup_depth1");
    return "setup_depth2";
}

void *setup_depth3(void *data) {
    assert_string_equal((char *)data, "setup_depth2");
    return "setup_depth3";
}

void *teardown_depth3(void *data) {
    assert_string_equal((char *)data, "setup_depth3");
    return "teardown_depth3";
}

void *teardown_depth2(void *data) {
    assert_string_equal((char *)data, "teardown_depth3");
    return "teardown_depth2";
}

void *teardown_depth1(void *data) {
    assert_string_equal((char *)data, "teardown_depth2");
    return "teardown_depth1";
}

void test_depth3(void *data) { assert_string_equal((char *)data, "setup_depth3"); }

static NutestSuite subsuite_depth3[] = {
        {"/depth3", (NutestTest[]){{"/check-data-depth3", test_depth3}, {}}, NULL,
                setup_depth3, teardown_depth3},
        {},
};
static NutestSuite subsuite_depth2[] = {
        {"/depth2", NULL, subsuite_depth3, setup_depth2, teardown_depth2},
        {},
};
static NutestSuite subsuites[] = {
        {"/data-modification", (NutestTest[]){{"/do-modify", test_data_modification}, {}},
                NULL, setup_data_modification, teardown_data_modification},
        {"/count-called-times",
                (NutestTest[]){
                        {"/count1", test_count_called1},
                        {"/count2", test_count_called2},
                        {},
                },
                NULL, setup_count_called, teardown_count_called},
        {"/depth1", NULL, subsuite_depth2, setup_depth1, teardown_depth1},
        {},
};
NutestSuite suite_setup_teardown = {
        "/setup and teardown",
        NULL,
        subsuites,
};

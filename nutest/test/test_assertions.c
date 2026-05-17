#include "nutest.h"
#include <stdbool.h>

// TODO: How to test assertion failure?

void test_equalities(void *data) {
    assert_string_equal("xyz", "xyz");
    assert_size_equal((size_t)3, (size_t)3);
    assert_int_equal(5, 5);
    assert_uint64_equal((uint64_t)7, (uint64_t)7);
    assert_ptr_equal(&data, &data);
    assert_null(NULL);
    assert_not_null(&data);
    assert_true(true);
    assert_true(1 == 1);
    assert_false(false);
    assert_false(1 == 2);
}

void test_non_equalities(void *data) {
    char c1, c2;
    assert_uint64_neq((uint64_t)3, (uint64_t)5);
    assert_ptr_neq(&c1, &c2);
}

void test_lt(void *data) { assert_size_lt((size_t)3, (size_t)5); }

void test_geq(void *data) {
    assert_size_geq((size_t)3, (size_t)3);
    assert_size_geq((size_t)5, (size_t)3);
}

static NutestTest tests[] = {
        {"/eq", test_equalities},
        {"/neq", test_non_equalities},
        {"/lt", test_lt},
        {"/geq", test_geq},
        {},
};
NutestSuite suite_assertions = {"/assertions", tests};

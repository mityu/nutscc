#pragma once

#include "unity.h"

typedef void (*NutestTestFunc)(void *);
typedef void *(*NutestSuiteSetup)(void *);
typedef void *(*NutestSuiteTeardown)(void *);

typedef struct NutestTest {
    char *name;
    NutestTestFunc test;
} NutestTest;

typedef struct NutestSuite NutestSuite;
struct NutestSuite {
    char *prefix;
    NutestTest *tests;
    NutestSuite *suites;
    NutestSuiteSetup setup;
    NutestSuiteTeardown teardown;
};

int nutest_suite_main(const NutestSuite *suite, void *user_data, int argc, char **argv);

#define assert_null(pointer) TEST_ASSERT_NULL(pointer)
#define assert_not_null(pointer) TEST_ASSERT_NOT_NULL(pointer)

#define assert_true(condition) TEST_ASSERT_TRUE(condition)
#define assert_false(condition) TEST_ASSERT_FALSE(condition)

#define assert_string_equal(expected, actual) TEST_ASSERT_EQUAL_STRING(expected, actual)

#define assert_size_equal(expected, actual) TEST_ASSERT_EQUAL_size_t(expected, actual)
#define assert_uint64_equal(expected, actual) TEST_ASSERT_EQUAL_UINT64(expected, actual)

#define assert_size_geq(threshold, actual)                                               \
    TEST_ASSERT_GREATER_OR_EQUAL_size_t(threshold, actual)

#define assert_uint64_neq(threshold, actual)                                             \
    TEST_ASSERT_NOT_EQUAL_UINT64(threshold, actual)

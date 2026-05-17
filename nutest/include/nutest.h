#pragma once

#define UNITY_INCLUDE_PRINT_FORMATTED
#include "unity.h"
#include <stdint.h> // IWYU pragma: keep; for uint64_t, UINTPTR_MAX, etc...

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

#define assert_int_equal(expected, actual) TEST_ASSERT_EQUAL_INT(expected, actual)
#define assert_size_equal(expected, actual) TEST_ASSERT_EQUAL_size_t(expected, actual)
#define assert_uint64_equal(expected, actual) TEST_ASSERT_EQUAL_UINT64(expected, actual)

#define assert_size_geq(threshold, actual)                                               \
    TEST_ASSERT_GREATER_OR_EQUAL_size_t(actual, threshold)

#define assert_size_lt(threshold, actual) TEST_ASSERT_LESS_THAN_size_t(actual, threshold)

#define assert_uint64_neq(threshold, actual)                                             \
    TEST_ASSERT_NOT_EQUAL_UINT64(threshold, actual)

#ifdef __SIZEOF_POINTER__
#define NUTEST_SIZEOF_POINTER __SIZEOF_POINTER__
#else
#if UINTPTR_MAX == 0xffffffff
#define NUTEST_SIZEOF_POINTER 4
#elif UINTPTR_MAX == 0xffffffffffffffffULL
#define NUTEST_SIZEOF_POINTER 8
#else
#error "Failed to determine pointer size."
#endif
#endif

#if NUTEST_SIZEOF_POINTER == 8
#define assert_ptr_equal(expected, actual)                                               \
    TEST_ASSERT_EQUAL_UINT64((uint64_t)(expected), (uint64_t)(actual))
#define assert_ptr_neq(expected, actual)                                                 \
    TEST_ASSERT_NOT_EQUAL_UINT64((uint64_t)(expected), (uint64_t)(actual))
#elif NUTEST_SIZEOF_POINTER == 4
#define assert_ptr_equal(expected, actual)                                               \
    TEST_ASSERT_EQUAL_UINT32((uint32_t)(expected), (uint32_t)(actual))
#define assert_ptr_neq(expected, actual)                                                 \
    TEST_ASSERT_NOT_EQUAL_UINT32((uint32_t)(expected), (uint32_t)(actual))
#endif

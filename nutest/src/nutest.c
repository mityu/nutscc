#include "nutest.h"
#include "unity.h"
#include <stdlib.h>
#include <string.h>

typedef struct NutestSuiteVec {
    size_t size;
    size_t cap;
    const NutestSuite **suites;
} NutestSuiteVec;

static void *calloc_or_die(size_t count, size_t size);

static NutestSuiteVec *nutest_suite_vec_new(void);
static void nutest_suite_vec_clean(NutestSuiteVec *vec);
static size_t nutest_suite_vec_size(const NutestSuiteVec *vec);
static const NutestSuite *nutest_suite_vec_get(const NutestSuiteVec *vec, size_t i);
static void nutest_suite_vec_push(NutestSuiteVec *vec, const NutestSuite *suite);
static void nutest_suite_vec_pop(NutestSuiteVec *vec);

static void run_test_case(const NutestTest *test_case);
static void run_test_suite(const NutestSuite *suite);

static void testFunc(void);

struct TestState {
    void *initialUserData;
    NutestSuiteVec *suiteVec;
    const NutestTest *currentTest;
    void *currentUserData;
} testState;

void *calloc_or_die(size_t count, size_t size) {
    void *p = calloc(count, size);
    if (p == NULL) {
        static const char message[] =
                "calloc_or_die(): Fatal: Memory allocation failed. Abort.\n";
        UNITY_OUTPUT_START();
        for (const char *pc = message; *pc != '\0'; ++pc) {
            UNITY_OUTPUT_CHAR(*pc);
        }
        UNITY_OUTPUT_FLUSH();
        UNITY_OUTPUT_COMPLETE();
        exit(1);
    }
    return p;
}

NutestSuiteVec *nutest_suite_vec_new(void) {
    NutestSuiteVec *vec = (NutestSuiteVec *)calloc_or_die(1, sizeof(NutestSuiteVec));
    vec->size = 0;
    vec->cap = 1;
    vec->suites = NULL;
    return vec;
}

void nutest_suite_vec_clean(NutestSuiteVec *vec) {
    if (vec) {
        if (vec->suites) {
            free(vec->suites);
        }
        free(vec);
    }
}

size_t nutest_suite_vec_size(const NutestSuiteVec *vec) { return vec->size; }

const NutestSuite *nutest_suite_vec_get(const NutestSuiteVec *vec, size_t i) {
    if (i >= vec->size) {
        return NULL;
    }
    return vec->suites[i];
}

void nutest_suite_vec_push(NutestSuiteVec *vec, const NutestSuite *suite) {
    if (vec->suites == NULL || vec->size == vec->cap) {
        void *save = vec->suites;
        vec->cap <<= 1;
        vec->suites = (const NutestSuite **)calloc_or_die(
                vec->cap, sizeof(const NutestSuite *));
        if (save != NULL) {
            memcpy(vec->suites, save, vec->size);
            free(save);
        }
    }
    vec->suites[vec->size++] = suite;
}

void nutest_suite_vec_pop(NutestSuiteVec *vec) { vec->size--; }

void run_test_case(const NutestTest *test_case) {
    testState.currentTest = test_case;
    RUN_TEST(testFunc);
}

void run_test_suite(const NutestSuite *suite) {
    nutest_suite_vec_push(testState.suiteVec, suite);
    if (suite->tests != NULL) {
        for (NutestTest *test = suite->tests; test->name != NULL; ++test) {
            run_test_case(test);
        }
    }
    if (suite->suites != NULL) {
        for (NutestSuite *su = suite->suites; su->prefix != NULL; ++su) {
            run_test_suite(su);
        }
    }
    nutest_suite_vec_pop(testState.suiteVec);
}

void setUp(void) {
    const size_t size = nutest_suite_vec_size(testState.suiteVec);
    void *userData = NULL;

    UNITY_OUTPUT_START();
    for (size_t i = 0; i < size; ++i) {
        const char *prefix = nutest_suite_vec_get(testState.suiteVec, i)->prefix;
        while (*prefix != '\0') {
            UNITY_OUTPUT_CHAR(*prefix++);
        }
    }
    for (const char *name = testState.currentTest->name; *name != '\0'; ++name) {
        UNITY_OUTPUT_CHAR(*name);
    }
    UNITY_OUTPUT_FLUSH();
    UNITY_OUTPUT_COMPLETE();

    userData = testState.initialUserData;
    for (size_t i = 0; i < size; ++i) {
        const NutestSuite *suite = nutest_suite_vec_get(testState.suiteVec, i);
        if (suite->setup) {
            userData = suite->setup(userData);
        }
    }
    testState.currentUserData = userData;
}

void testFunc(void) { testState.currentTest->test(testState.currentUserData); }

void tearDown(void) {
    const size_t size = nutest_suite_vec_size(testState.suiteVec);
    void *userData = testState.currentUserData;

    // Treat `i` as 1-indexed value here since 0-indexed iteration diverges as
    // you know i >= 0 always hold for any i whose type is size_t.
    for (size_t i = size; i > 0; --i) {
        const NutestSuite *suite = nutest_suite_vec_get(testState.suiteVec, i - 1);
        if (suite->teardown) {
            userData = suite->teardown(userData);
        }
    }
}

int nutest_suite_main(const NutestSuite *suite, void *user_data, int argc, char **argv) {
    (void)argc;

    UnityBegin("");
    testState.initialUserData = user_data;
    testState.suiteVec = nutest_suite_vec_new();
    testState.currentTest = testState.currentUserData = NULL;
    run_test_suite(suite);
    nutest_suite_vec_clean(testState.suiteVec);
    return (UnityEnd());
}

#include "gc/gc.h"
#include "munit.h"
#include <string.h>

static void *safe_malloc(size_t size) {
    void *p = gc_malloc(size);
    munit_assert_not_null(p);
    return p;
}

static char *safe_strdup(const char *src) {
    size_t size = strlen(src) + 1;
    char *dst = (char *)safe_malloc(size);
    memcpy(dst, src, size);
    munit_assert_string_equal(dst, src);
    return dst;
}

static MunitResult test_gc_keeps_used_by_local_var(
        const MunitParameter *params, void *user_data) {
    char *ptr = NULL;

    ptr = safe_strdup("hydrogen");
    ptr = safe_strdup("helium");
    ptr = safe_strdup("lithium");

    munit_assert_string_equal(ptr, "lithium");
    gc_collect();
    munit_assert_string_equal(ptr, "lithium");

    return MUNIT_OK;
}

static char *global_ptr = NULL;
static MunitResult test_gc_keeps_used_by_global_var(
        const MunitParameter *params, void *user_data) {
    global_ptr = safe_strdup("berylium");
    global_ptr = safe_strdup("boron");
    global_ptr = safe_strdup("carbon");

    munit_assert_string_equal(global_ptr, "carbon");
    gc_collect();
    munit_assert_string_equal(global_ptr, "carbon");

    return MUNIT_OK;
}

#define TEST_ADD(name, testfunc)                                                         \
    {name, testfunc, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
static MunitTest tests[] = {
        TEST_ADD("/gc keeps memory being used by local vars",
                test_gc_keeps_used_by_local_var),
        TEST_ADD("/gc keeps memory being used by global vars",
                test_gc_keeps_used_by_global_var),
        {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};
#undef TEST_ADD

static const MunitSuite test_suite = {"test gc", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

int main(int argc, char **argv) {
    gc_init();
    return munit_suite_main(&test_suite, NULL, argc, argv);
}

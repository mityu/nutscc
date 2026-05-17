#include "gc/gc.h"
#include "nutest.h"
#include <string.h>

static void *safe_malloc(size_t size) {
    void *p = gc_malloc(size);
    assert_not_null(p);
    return p;
}

static char *safe_strdup(const char *src) {
    size_t size = strlen(src) + 1;
    char *dst = (char *)safe_malloc(size);
    memcpy(dst, src, size);
    assert_string_equal(dst, src);
    return dst;
}

static void test_gc_keeps_used_by_local_var(void *user_data) {
    char *ptr = NULL;

    ptr = safe_strdup("hydrogen");
    ptr = safe_strdup("helium");
    ptr = safe_strdup("lithium");

    assert_string_equal(ptr, "lithium");
    gc_collect();
    assert_string_equal(ptr, "lithium");
}

static char *global_ptr = NULL;
static void test_gc_keeps_used_by_global_var(void *user_data) {
    global_ptr = safe_strdup("berylium");
    global_ptr = safe_strdup("boron");
    global_ptr = safe_strdup("carbon");

    assert_string_equal(global_ptr, "carbon");
    gc_collect();
    assert_string_equal(global_ptr, "carbon");
}

static void test_check_mem_size_changes(void *user_data) {
    const size_t cap_size = 10;
    void *slots[6] = {};
    size_t total[3] = {};
    size_t slot_size = sizeof(slots) / sizeof(slots[0]);

    gc_collect();

    total[0] = gc_get_total_size();

    for (int i = 0; i < slot_size; ++i) {
        slots[i] = safe_malloc(cap_size);
    }

    total[1] = gc_get_total_size();
    assert_size_geq(total[1] - total[0], cap_size * slot_size);

    for (int i = 0; i < slot_size / 2; ++i) {
        slots[i] = NULL;
    }

    gc_collect();
    total[2] = gc_get_total_size();

    // This GC is conservative, so not always garbage memory areas are freed.
    // Therefore, ideally we want to check that the following,
    //      total[1] - total[2] <= cap_size * (slot_size / 2)
    // but only check the memory usage doesn't increase after GC here.
    assert_size_geq(total[1], total[2]);
}

static NutestTest tests[] = {
        {"/gc keeps memory being used by local vars", test_gc_keeps_used_by_local_var},
        {"/gc keeps memory being used by global vars", test_gc_keeps_used_by_global_var},
        {"/check allocated memory size", test_check_mem_size_changes},
        {NULL, NULL},
};

static const NutestSuite test_suite = {"test gc", tests};

int main(int argc, char **argv) {
    gc_init();
    return nutest_suite_main(&test_suite, NULL, argc, argv);
}

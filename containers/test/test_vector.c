#include "containers/vector.h"
#include "gc/gc.h"
#include "nutest.h"

void *create_new_vec(void *data) { return (void *)vec_new(); }

void test_vec_empty(void *data) {
    Vector *v = (Vector *)data;

    assert_true(vec_empty(v));
    vec_push(v, &v);
    assert_false(vec_empty(v));
    vec_pop(v);
    assert_true(vec_empty(v));
}

void test_vec_get_out_of_range(void *data) {
    int placeholder = 0;
    Vector *v = (Vector *)data;

    vec_push(v, &placeholder);
    vec_push(v, &placeholder);
    assert_null(vec_get(data, 2));
    assert_null(vec_get(data, 3));
}

void test_vec_size(void *data) {
    Vector *v = (Vector *)data;

    assert_size_equal(0, vec_size(v));
    vec_push(v, &v);
    assert_size_equal(1, vec_size(v));
    vec_push(v, &v);
    assert_size_equal(2, vec_size(v));
    vec_push(v, &v);
    assert_size_equal(3, vec_size(v));
    vec_pop(v);
    assert_size_equal(2, vec_size(v));
    vec_pop(v);
    assert_size_equal(1, vec_size(v));
    vec_pop(v);
    assert_size_equal(0, vec_size(v));
}

void test_vec_push(void *data) {
    int placeholders[5] = {};
    Vector *v = (Vector *)data;

    vec_push(v, &placeholders[1]);
    vec_push(v, &placeholders[3]);
    vec_push(v, &placeholders[2]);
    vec_push(v, &placeholders[4]);
    vec_push(v, &placeholders[0]);

    assert_ptr_equal(vec_get(v, 0), &placeholders[1]);
    assert_ptr_equal(vec_get(v, 1), &placeholders[3]);
    assert_ptr_equal(vec_get(v, 2), &placeholders[2]);
    assert_ptr_equal(vec_get(v, 3), &placeholders[4]);
    assert_ptr_equal(vec_get(v, 4), &placeholders[0]);
}

void test_vec_pop(void *data) {
    int placeholders[5] = {};
    Vector *v = (Vector *)data;

    vec_push(v, &placeholders[1]);
    vec_push(v, &placeholders[3]);
    vec_push(v, &placeholders[2]);
    vec_push(v, &placeholders[4]);
    vec_push(v, &placeholders[0]);

    assert_ptr_equal(vec_pop(v), &placeholders[0]);
    assert_ptr_equal(vec_pop(v), &placeholders[4]);
    assert_ptr_equal(vec_pop(v), &placeholders[2]);
    assert_ptr_equal(vec_pop(v), &placeholders[3]);
    assert_ptr_equal(vec_pop(v), &placeholders[1]);
}

void test_vec_pop_when_empty(void *data) {
    Vector *v = (Vector *)data;

    assert_null(vec_pop(v));
    assert_size_equal(0, vec_size(v));
}

void test_vec_push_many(void *data) {
    // TODO: How to ensure that this is more than the initial capacity?
    const int count = 200; // Specify enough size to over the initial capacity.
    Vector *v = (Vector *)data;

    for (int i = 1; i <= count; ++i) {
        vec_push(v, (void *)(uintptr_t)(i * 3));
    }

    assert_size_equal(count, vec_size(v));
    assert_int_equal(count * 3, (int)(uintptr_t)(vec_get(v, count - 1)));
    assert_int_equal((count - 1) * 3, (int)(uintptr_t)(vec_get(v, count - 2)));
}

static NutestTest tests[] = {
        {"/vec_empty() checks whether given vector is empty or not.", test_vec_empty},
        {"/vec_get() returns NULL when index is out of range.",
                test_vec_get_out_of_range},
        {"/vec_size() returns current size of vector.", test_vec_size},
        {"/vec_push() adds an item at the end of vector.", test_vec_push},
        {"/vec_pop() removes items from the end of vector.", test_vec_pop},
        {"/vec_pop() does not modify vector and returns NULL.", test_vec_pop_when_empty},
        {"/vec_push() test for adding more items than initial capacity.",
                test_vec_push_many},
        {},
};

static const NutestSuite suite = {"containers/vector", tests, NULL, create_new_vec};

int main(int argc, char **argv) {
    gc_init();
    return nutest_suite_main(&suite, NULL, argc, argv);
}

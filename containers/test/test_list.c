#include "containers/list.h"
#include "gc/gc.h"
#include "nutest.h"

void *create_new_list(void *data) { return (void *)list_new(); }

void *setup_new_list(void *data) {
    List *list = (List *)data;

    list_push_back(list, (void *)1);
    list_push_back(list, (void *)2);
    list_push_back(list, (void *)3);
    list_push_back(list, (void *)4);

    return (void *)list;
}

void test_list_empty(void *data) {
    List *list = (List *)data;

    assert_true(list_empty(list));
    list_push_back(list, NULL);
    assert_false(list_empty(list));
    list_pop_back(list);
    assert_true(list_empty(list));
}

void test_push_back_pop_back(void *data) {
    List *list = (List *)data;

    list_push_back(list, (void *)1);
    list_push_back(list, (void *)2);
    list_push_back(list, (void *)3);
    list_push_back(list, (void *)4);
    assert_int_equal(4, (int)(uintptr_t)list_pop_back(list));
    assert_int_equal(3, (int)(uintptr_t)list_pop_back(list));
    assert_int_equal(2, (int)(uintptr_t)list_pop_back(list));
    assert_int_equal(1, (int)(uintptr_t)list_pop_back(list));
}

void test_push_back_pop_front(void *data) {
    List *list = (List *)data;

    list_push_back(list, (void *)1);
    list_push_back(list, (void *)2);
    list_push_back(list, (void *)3);
    list_push_back(list, (void *)4);
    assert_int_equal(1, (int)(uintptr_t)list_pop_front(list));
    assert_int_equal(2, (int)(uintptr_t)list_pop_front(list));
    assert_int_equal(3, (int)(uintptr_t)list_pop_front(list));
    assert_int_equal(4, (int)(uintptr_t)list_pop_front(list));
}

void test_push_front_pop_front(void *data) {
    List *list = (List *)data;

    list_push_front(list, (void *)1);
    list_push_front(list, (void *)2);
    list_push_front(list, (void *)3);
    list_push_front(list, (void *)4);
    assert_int_equal(4, (int)(uintptr_t)list_pop_front(list));
    assert_int_equal(3, (int)(uintptr_t)list_pop_front(list));
    assert_int_equal(2, (int)(uintptr_t)list_pop_front(list));
    assert_int_equal(1, (int)(uintptr_t)list_pop_front(list));
}

void test_push_front_pop_back(void *data) {
    List *list = (List *)data;

    list_push_front(list, (void *)1);
    list_push_front(list, (void *)2);
    list_push_front(list, (void *)3);
    list_push_front(list, (void *)4);
    assert_int_equal(1, (int)(uintptr_t)list_pop_back(list));
    assert_int_equal(2, (int)(uintptr_t)list_pop_back(list));
    assert_int_equal(3, (int)(uintptr_t)list_pop_back(list));
    assert_int_equal(4, (int)(uintptr_t)list_pop_back(list));
}

void test_iter_forward_iteration(void *data) {
    List *list = (List *)data;
    ListIter *iter = list_get_forward_iterator(list);

    assert_false(listiter_is_end(iter));
    assert_int_equal(1, (int)(uintptr_t)listiter_get_item(iter));

    listiter_prev(iter);
    assert_false(listiter_is_end(iter));
    assert_int_equal(1, (int)(uintptr_t)listiter_get_item(iter));

    listiter_next(iter);
    assert_false(listiter_is_end(iter));
    assert_int_equal(2, (int)(uintptr_t)listiter_get_item(iter));

    listiter_next(iter);
    assert_false(listiter_is_end(iter));
    assert_int_equal(3, (int)(uintptr_t)listiter_get_item(iter));

    listiter_prev(iter);
    assert_false(listiter_is_end(iter));
    assert_int_equal(2, (int)(uintptr_t)listiter_get_item(iter));

    listiter_next(iter);
    listiter_next(iter);
    assert_false(listiter_is_end(iter));
    assert_int_equal(4, (int)(uintptr_t)listiter_get_item(iter));

    listiter_next(iter);
    assert_true(listiter_is_end(iter));
    assert_ptr_equal(NULL, listiter_get_item(iter));

    listiter_prev(iter);
    assert_false(listiter_is_end(iter));
    assert_int_equal(4, (int)(uintptr_t)listiter_get_item(iter));
}

void test_iter_backward_iteration(void *data) {
    List *list = (List *)data;
    ListIter *iter = list_get_backward_iterator(list);

    assert_false(listiter_is_end(iter));
    assert_int_equal(4, (int)(uintptr_t)listiter_get_item(iter));

    listiter_prev(iter);
    assert_false(listiter_is_end(iter));
    assert_int_equal(4, (int)(uintptr_t)listiter_get_item(iter));

    listiter_next(iter);
    assert_false(listiter_is_end(iter));
    assert_int_equal(3, (int)(uintptr_t)listiter_get_item(iter));

    listiter_next(iter);
    assert_false(listiter_is_end(iter));
    assert_int_equal(2, (int)(uintptr_t)listiter_get_item(iter));

    listiter_prev(iter);
    assert_false(listiter_is_end(iter));
    assert_int_equal(3, (int)(uintptr_t)listiter_get_item(iter));

    listiter_next(iter);
    listiter_next(iter);
    assert_false(listiter_is_end(iter));
    assert_int_equal(1, (int)(uintptr_t)listiter_get_item(iter));

    listiter_next(iter);
    assert_true(listiter_is_end(iter));
    assert_ptr_equal(NULL, listiter_get_item(iter));

    listiter_prev(iter);
    assert_false(listiter_is_end(iter));
    assert_int_equal(1, (int)(uintptr_t)listiter_get_item(iter));
}

void test_iter_remove_first(void *data) {
    List *list = (List *)data;
    ListIter *iter = list_get_forward_iterator(list);

    assert_int_equal(1, (int)(uintptr_t)listiter_get_item(iter));

    assert_int_equal(1, (int)(uintptr_t)listiter_remove(iter));
    assert_false(listiter_is_end(iter));
    assert_int_equal(2, (int)(uintptr_t)listiter_get_item(iter));

    iter = list_get_backward_iterator(list);
    assert_int_equal(4, (int)(uintptr_t)listiter_get_item(iter));
    listiter_next(iter);
    assert_int_equal(3, (int)(uintptr_t)listiter_get_item(iter));
    listiter_next(iter);
    assert_int_equal(2, (int)(uintptr_t)listiter_get_item(iter));
    listiter_next(iter);
    assert_true(listiter_is_end(iter));
}

void test_iter_remove_middle(void *data) {
    List *list = (List *)data;
    ListIter *iter = list_get_forward_iterator(list);

    listiter_next(iter);
    listiter_next(iter);
    assert_int_equal(3, (int)(uintptr_t)listiter_get_item(iter));

    assert_int_equal(3, (int)(uintptr_t)listiter_remove(iter));
    assert_false(listiter_is_end(iter));
    assert_int_equal(4, (int)(uintptr_t)listiter_get_item(iter));

    iter = list_get_forward_iterator(list);
    assert_int_equal(1, (int)(uintptr_t)listiter_get_item(iter));
    listiter_next(iter);
    assert_int_equal(2, (int)(uintptr_t)listiter_get_item(iter));
    listiter_next(iter);
    assert_int_equal(4, (int)(uintptr_t)listiter_get_item(iter));
    listiter_next(iter);
    assert_true(listiter_is_end(iter));
}

void test_iter_remove_last(void *data) {
    List *list = (List *)data;
    ListIter *iter = list_get_forward_iterator(list);

    listiter_next(iter);
    listiter_next(iter);
    listiter_next(iter);
    listiter_next(iter);
    assert_true(listiter_is_end(iter));
    listiter_prev(iter);

    assert_int_equal(4, (int)(uintptr_t)listiter_remove(iter));
    assert_true(listiter_is_end(iter));

    iter = list_get_forward_iterator(list);
    assert_int_equal(1, (int)(uintptr_t)listiter_get_item(iter));
    listiter_next(iter);
    assert_int_equal(2, (int)(uintptr_t)listiter_get_item(iter));
    listiter_next(iter);
    assert_int_equal(3, (int)(uintptr_t)listiter_get_item(iter));
    listiter_next(iter);
    assert_true(listiter_is_end(iter));
}

void test_iter_assign_value(void *data) {
    List *list = (List *)data;
    ListIter *iter = list_get_forward_iterator(list);

    listiter_next(iter);
    assert_int_equal(2, (int)(uintptr_t)listiter_get_item(iter));
    listiter_assign_value(iter, (void *)7);
    assert_int_equal(7, (int)(uintptr_t)listiter_get_item(iter));

    iter = list_get_forward_iterator(list);
    assert_int_equal(1, (int)(uintptr_t)listiter_get_item(iter));
    listiter_next(iter);
    assert_int_equal(7, (int)(uintptr_t)listiter_get_item(iter));
    listiter_next(iter);
    assert_int_equal(3, (int)(uintptr_t)listiter_get_item(iter));
    listiter_next(iter);
    assert_int_equal(4, (int)(uintptr_t)listiter_get_item(iter));
    listiter_next(iter);
    assert_true(listiter_is_end(iter));
}

void test_list_find(void *data) {
    List *list = (List *)data;
    ListIter *iter = NULL;

    iter = list_find(list, (void *)3);
    assert_false(listiter_is_end(iter));
    assert_int_equal(3, (int)(uintptr_t)listiter_get_item(iter));

    iter = list_find(list, (void *)9);
    assert_true(listiter_is_end(iter));
}

static NutestTest tests[] = {
        {"/list_empty()", test_list_empty},
        {"/push-back-pop-back", test_push_back_pop_back},
        {"/push-back-pop-front", test_push_back_pop_front},
        {"/push-front-pop-front", test_push_front_pop_front},
        {"/push-front-pop-back", test_push_front_pop_back},
        {},
};

static NutestTest iter_tests[] = {
        {"/forward-iteration", test_iter_forward_iteration},
        {"/backward-iteration", test_iter_backward_iteration},
        {"/remove-first", test_iter_remove_first},
        {"/remove-middle", test_iter_remove_middle},
        {"/remove-last", test_iter_remove_last},
        {"/assign-value", test_iter_assign_value},
        {"/find-value", test_list_find},
        {},
};

static NutestSuite iter_suite[] = {
        {"/iter", iter_tests, NULL, setup_new_list},
        {},
};

static const NutestSuite suite = {"/containers/list", tests, iter_suite, create_new_list};

int main(int argc, char **argv) {
    gc_init();
    return nutest_suite_main(&suite, NULL, argc, argv);
}

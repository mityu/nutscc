#include "../hashmap.h"
#include "munit.h"
#include <string.h>

static void *safe_malloc(size_t size) {
    void *p = malloc(size);
    munit_assert_not_null(p);
    return p;
}

static char *safe_strdup(const char *src) {
    char *dst = strdup(src);
    munit_assert_not_null(dst);
    return dst;
}

static void *setup_hashmap(const MunitParameter *params, void *user_data) {
    HashMap *map = NULL;
    (void)user_data;

    map = hashmap_new();
    munit_assert_not_null(map);
    return (void *)map;
}

static void teardown_hashmap(void *fixture) {
    HashMap *map = (HashMap *)fixture;
    hashmap_destroy(&map);
}

static MunitResult test_destroy(const MunitParameter *params, void *user_data) {
    HashMap *map = NULL;
    (void)params;
    (void)user_data;

    map = hashmap_new();
    munit_assert_not_null(map);
    hashmap_destroy(&map);
    munit_assert_null(map);

    return MUNIT_OK;
}

static MunitResult test_judge_empty(const MunitParameter *params, void *user_data) {
    HashMap *map = (HashMap *)user_data;
    (void)params;
    (void)user_data;

    map = hashmap_new();
    munit_assert_not_null(map);
    munit_assert_true(hashmap_empty(map));

    hashmap_insert(map, (void *)0x1234, safe_malloc(1));
    munit_assert_false(hashmap_empty(map));

    hashmap_insert(map, (void *)0x2345, safe_malloc(1));
    munit_assert_false(hashmap_empty(map));

    hashmap_remove(map, (void *)0x1234);
    munit_assert_false(hashmap_empty(map));

    hashmap_remove(map, (void *)0x2345);
    munit_assert_true(hashmap_empty(map));

    return MUNIT_OK;
}

static MunitResult test_insert_remove_find(
        const MunitParameter *params, void *user_data) {
    HashMap *map = (HashMap *)user_data;

    hashmap_insert(map, (void *)1, safe_strdup("abc"));
    munit_assert_string_equal(hashmap_find(map, (void *)1), "abc");
    munit_assert_null(hashmap_find(map, (void *)2));

    hashmap_insert(map, (void *)2, safe_strdup("xyz"));
    munit_assert_string_equal(hashmap_find(map, (void *)1), "abc");
    munit_assert_string_equal(hashmap_find(map, (void *)2), "xyz");

    hashmap_remove(map, (void *)1);
    munit_assert_null(hashmap_find(map, (void *)1));
    munit_assert_string_equal(hashmap_find(map, (void *)2), "xyz");

    return MUNIT_OK;
}

static MunitResult test_iterator(const MunitParameter *params, void *user_data) {
#define ENTRY_COUNT (4)
    struct KeyVal {
        uintptr_t key;
        char *value;
    };

    HashMap *map = (HashMap *)user_data;
    HashMapIter *iter;
    struct KeyVal entries[ENTRY_COUNT] = {
            {1, "abc"},
            {2, "def"},
            {3, "ghi"},
            {4, "xyz"},
    };
    struct KeyVal seen[ENTRY_COUNT * 2] = {};
    size_t seenCnt = 0;

    // Verify pre-condition.
    munit_assert_size(sizeof(uint64_t), >=, sizeof(uintptr_t));

    for (int i = 0; i < ENTRY_COUNT; ++i) {
        struct KeyVal *entry = &entries[i];
        hashmap_insert(map, (void *)entry->key, safe_strdup(entry->value));
    }

    iter = hashmap_iter(map);
    munit_assert_not_null(iter);

    while (!hashmap_iter_is_end(iter)) {
        struct KeyVal *work = NULL;
        work = &seen[seenCnt++];
        work->key = (uintptr_t)hashmap_iter_get_key(iter);
        work->value = (char *)hashmap_iter_get_value(iter);
        hashmap_iter_next(iter);
    }

    // Check each hashmap entry is in `seen`.
    munit_assert_size(seenCnt, ==, ENTRY_COUNT);
    for (int i = 0; i < ENTRY_COUNT; ++i) {
        struct KeyVal *src = NULL, *found = NULL;
        src = &entries[i];
        for (int j = 0; j < seenCnt; ++j) {
            struct KeyVal *tmp = &seen[j];
            if (tmp->key == src->key) {
                found = tmp;
                break;
            }
        }
        munit_assert_not_null(found);
        munit_assert_uint64(src->key, ==, found->key);
        munit_assert_string_equal(src->value, found->value);
    }

    hashmap_iter_destroy(&iter);

    return MUNIT_OK;
#undef ENTRY_COUNT
}

static MunitResult test_remove_by_iterator(
        const MunitParameter *params, void *user_data) {
    HashMap *map = (HashMap *)user_data;
    HashMapIter *iter;
    void *key1, *key2;

    hashmap_insert(map, (void *)1, safe_strdup("abc"));
    hashmap_insert(map, (void *)2, safe_strdup("ghi"));
    hashmap_insert(map, (void *)3, safe_strdup("xyz"));

    iter = hashmap_iter(map);
    munit_assert_not_null(iter);
    munit_assert_false(hashmap_iter_is_end(iter));

    hashmap_iter_next(iter);
    munit_assert_false(hashmap_iter_is_end(iter));

    key1 = hashmap_iter_get_key(iter);
    munit_assert_not_null(key1);

    munit_assert_not_null(hashmap_find(map, key1));
    hashmap_iter_remove(iter);
    munit_assert_null(hashmap_find(map, key1));

    // hashmap_iter_remove() should make `iter` to point to next iterm.
    munit_assert_false(hashmap_iter_is_end(iter));
    key2 = hashmap_iter_get_key(iter);
    munit_assert_not_null(key2);
    munit_assert_uint64((uint64_t)key1, !=, (uint64_t)key2);

    hashmap_iter_destroy(&iter);

    return MUNIT_OK;
}

static MunitResult test_remove_last_item_by_iterator(
        const MunitParameter *params, void *user_data) {
    HashMap *map = (HashMap *)user_data;
    HashMapIter *iter;

    hashmap_insert(map, (void *)1, safe_strdup("xyz"));

    iter = hashmap_iter(map);
    munit_assert_not_null(iter);

    munit_assert_false(hashmap_iter_is_end(iter));
    hashmap_iter_remove(iter);
    munit_assert_true(hashmap_iter_is_end(iter));

    hashmap_iter_destroy(&iter);

    return MUNIT_OK;
}

static MunitResult test_iterator_with_removal(
        const MunitParameter *params, void *user_data) {
#define ENTRY_COUNT (4)
#define REMOVE_ENTRY_INDEX (0)
    struct KeyVal {
        uintptr_t key;
        char *value;
    };

    HashMap *map = (HashMap *)user_data;
    HashMapIter *iter;
    struct KeyVal entries[ENTRY_COUNT] = {
            {1, "abc"},
            {2, "def"},
            {3, "ghi"},
            {4, "xyz"},
    };
    struct KeyVal seen[ENTRY_COUNT * 2] = {};
    size_t seenCnt = 0;

    // Verify pre-condition.
    munit_assert_size(sizeof(uint64_t), >=, sizeof(uintptr_t));

    for (int i = 0; i < ENTRY_COUNT; ++i) {
        struct KeyVal *entry = &entries[i];
        hashmap_insert(map, (void *)entry->key, safe_strdup(entry->value));
    }

    iter = hashmap_iter(map);
    munit_assert_not_null(iter);

    while (!hashmap_iter_is_end(iter)) {
        void *key, *value;
        struct KeyVal *work = NULL;

        key = hashmap_iter_get_key(iter);
        value = hashmap_iter_get_value(iter);
        if ((uintptr_t)key == entries[REMOVE_ENTRY_INDEX].key) {
            hashmap_iter_remove(iter);
        } else {
            work = &seen[seenCnt++];
            work->key = (uintptr_t)key;
            work->value = (char *)value;
            hashmap_iter_next(iter);
        }
    }

    // Check each hashmap entry is in `seen`.
    munit_assert_size(seenCnt, ==, ENTRY_COUNT - 1);
    for (int i = 0; i < ENTRY_COUNT; ++i) {
        struct KeyVal *src = NULL, *found = NULL;

        if (i == REMOVE_ENTRY_INDEX) {
            continue;
        }

        src = &entries[i];
        for (int j = 0; j < seenCnt; ++j) {
            struct KeyVal *tmp = &seen[j];
            if (tmp->key == src->key) {
                found = tmp;
                break;
            }
        }
        munit_assert_not_null(found);
        munit_assert_uint64(src->key, ==, found->key);
        munit_assert_string_equal(src->value, found->value);
    }

    hashmap_iter_destroy(&iter);

    return MUNIT_OK;
#undef REMOVE_ENTRY_INDEX
#undef ENTRY_COUNT
}

#define TEST_ADD(name, testfunc)                                                         \
    {name, testfunc, setup_hashmap, teardown_hashmap, MUNIT_TEST_OPTION_NONE, NULL}
static MunitTest tests[] = {
        {"/destroy", test_destroy, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
        TEST_ADD("/check empty", test_judge_empty),
        TEST_ADD("/find item", test_insert_remove_find),
        TEST_ADD("/iterator lists all items", test_iterator),
        TEST_ADD("/remove item using iterator", test_remove_by_iterator),
        TEST_ADD("/remove last item using iterator", test_remove_last_item_by_iterator),
        TEST_ADD("/iterator removal", test_iterator_with_removal),
        {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};
#undef TEST_ADD

static const MunitSuite test_suite = {
        "test hashmap", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

int main(int argc, char **argv) {
    return munit_suite_main(&test_suite, NULL, argc, argv);
}

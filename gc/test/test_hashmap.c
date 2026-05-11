#include "../hashmap.h"
#include "nutest.h"
#include <string.h>

typedef struct {
    HashMap *map;
    HashMapIter *iter;
} UserDataIter;

static void *safe_malloc(size_t size) {
    void *p = malloc(size);
    assert_not_null(p);
    return p;
}

static char *safe_strdup(const char *src) {
    char *dst = strdup(src);
    assert_not_null(dst);
    return dst;
}

static void *setup_hashmap(void *user_data) {
    HashMap *map = NULL;
    (void)user_data;

    map = hashmap_new();
    assert_not_null(map);
    return (void *)map;
}

static void *teardown_hashmap(void *fixture) {
    HashMap *map = (HashMap *)fixture;
    hashmap_destroy(&map);
    return NULL;
}

static void *setup_iterator(void *user_data) {
    static UserDataIter user_data_iter;
    user_data_iter.map = (HashMap *)user_data;
    user_data_iter.iter = hashmap_iter(user_data_iter.map);
    return (void *)&user_data_iter;
}

static void *teardown_iterator(void *user_data) {
    UserDataIter *data = (UserDataIter *)user_data;
    hashmap_iter_destroy(&data->iter);
    return data->map;
}

static void test_destroy(void *user_data) {
    HashMap *map = NULL;
    (void)user_data;

    map = hashmap_new();
    assert_not_null(map);
    hashmap_destroy(&map);
    assert_null(map);
}

static void test_judge_empty(void *user_data) {
    HashMap *map = (HashMap *)user_data;

    map = hashmap_new();
    assert_not_null(map);
    assert_true(hashmap_empty(map));

    hashmap_insert(map, (void *)0x1234, safe_malloc(1));
    assert_false(hashmap_empty(map));

    hashmap_insert(map, (void *)0x2345, safe_malloc(1));
    assert_false(hashmap_empty(map));

    hashmap_remove(map, (void *)0x1234);
    assert_false(hashmap_empty(map));

    hashmap_remove(map, (void *)0x2345);
    assert_true(hashmap_empty(map));
}

static void test_insert_remove_find(void *user_data) {
    HashMap *map = (HashMap *)user_data;

    hashmap_insert(map, (void *)1, safe_strdup("abc"));
    assert_string_equal(hashmap_find(map, (void *)1), "abc");
    assert_null(hashmap_find(map, (void *)2));

    hashmap_insert(map, (void *)2, safe_strdup("xyz"));
    assert_string_equal(hashmap_find(map, (void *)1), "abc");
    assert_string_equal(hashmap_find(map, (void *)2), "xyz");

    hashmap_remove(map, (void *)1);
    assert_null(hashmap_find(map, (void *)1));
    assert_string_equal(hashmap_find(map, (void *)2), "xyz");
}

static void test_iterator(void *user_data) {
#define ENTRY_COUNT (4)
    struct KeyVal {
        uintptr_t key;
        char *value;
    };

    UserDataIter *data = (UserDataIter *)user_data;
    HashMap *map = data->map;
    HashMapIter *iter = data->iter;

    struct KeyVal entries[ENTRY_COUNT] = {
            {1, "abc"},
            {2, "def"},
            {3, "ghi"},
            {4, "xyz"},
    };
    struct KeyVal seen[ENTRY_COUNT * 2] = {};
    size_t seenCnt = 0;

    // Verify pre-condition.
    assert_size_geq(sizeof(uint64_t), sizeof(uintptr_t));

    for (int i = 0; i < ENTRY_COUNT; ++i) {
        struct KeyVal *entry = &entries[i];
        hashmap_insert(map, (void *)entry->key, safe_strdup(entry->value));
    }

    iter = hashmap_iter(map);
    assert_not_null(iter);

    while (!hashmap_iter_is_end(iter)) {
        struct KeyVal *work = NULL;
        work = &seen[seenCnt++];
        work->key = (uintptr_t)hashmap_iter_get_key(iter);
        work->value = (char *)hashmap_iter_get_value(iter);
        hashmap_iter_next(iter);
    }

    // Check each hashmap entry is in `seen`.
    assert_size_equal(seenCnt, ENTRY_COUNT);
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
        assert_not_null(found);
        assert_uint64_equal(src->key, found->key);
        assert_string_equal(src->value, found->value);
    }

#undef ENTRY_COUNT
}

static void test_remove_by_iterator(void *user_data) {
    UserDataIter *data = (UserDataIter *)user_data;
    HashMap *map = data->map;
    HashMapIter *iter = data->iter;
    void *key1, *key2;

    hashmap_insert(map, (void *)1, safe_strdup("abc"));
    hashmap_insert(map, (void *)2, safe_strdup("ghi"));
    hashmap_insert(map, (void *)3, safe_strdup("xyz"));

    iter = hashmap_iter(map);
    assert_not_null(iter);
    assert_false(hashmap_iter_is_end(iter));

    hashmap_iter_next(iter);
    assert_false(hashmap_iter_is_end(iter));

    key1 = hashmap_iter_get_key(iter);
    assert_not_null(key1);

    assert_not_null(hashmap_find(map, key1));
    hashmap_iter_remove(iter);
    assert_null(hashmap_find(map, key1));

    // hashmap_iter_remove() should make `iter` to point to next iterm.
    assert_false(hashmap_iter_is_end(iter));
    key2 = hashmap_iter_get_key(iter);
    assert_not_null(key2);
    assert_uint64_neq((uint64_t)key1, (uint64_t)key2);
}

static void test_remove_last_item_by_iterator(void *user_data) {
    UserDataIter *data = (UserDataIter *)user_data;
    HashMap *map = data->map;
    HashMapIter *iter = data->iter;

    hashmap_insert(map, (void *)1, safe_strdup("xyz"));

    iter = hashmap_iter(map);
    assert_not_null(iter);

    assert_false(hashmap_iter_is_end(iter));
    hashmap_iter_remove(iter);
    assert_true(hashmap_iter_is_end(iter));
}

static void test_iterator_with_removal(void *user_data) {
#define ENTRY_COUNT (4)
#define REMOVE_ENTRY_INDEX (0)
    struct KeyVal {
        uintptr_t key;
        char *value;
    };

    UserDataIter *data = (UserDataIter *)user_data;
    HashMap *map = data->map;
    HashMapIter *iter = data->iter;
    struct KeyVal entries[ENTRY_COUNT] = {
            {1, "abc"},
            {2, "def"},
            {3, "ghi"},
            {4, "xyz"},
    };
    struct KeyVal seen[ENTRY_COUNT * 2] = {};
    size_t seenCnt = 0;

    // Verify pre-condition.
    assert_size_geq(sizeof(uint64_t), sizeof(uintptr_t));

    for (int i = 0; i < ENTRY_COUNT; ++i) {
        struct KeyVal *entry = &entries[i];
        hashmap_insert(map, (void *)entry->key, safe_strdup(entry->value));
    }

    iter = hashmap_iter(map);
    assert_not_null(iter);

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
    assert_size_equal(seenCnt, ENTRY_COUNT - 1);
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
        assert_not_null(found);
        assert_uint64_equal(src->key, found->key);
        assert_string_equal(src->value, found->value);
    }

#undef REMOVE_ENTRY_INDEX
#undef ENTRY_COUNT
}

static NutestTest hashmap_tests[] = {
        {"/destroy", test_destroy},
        {"/check empty", test_judge_empty},
        {"/find item", test_insert_remove_find},
        {NULL, NULL},
};

static NutestTest iter_tests[] = {
        {"/lists all items", test_iterator},
        {"/remove an item", test_remove_by_iterator},
        {"/remove last item", test_remove_last_item_by_iterator},
        {"/iterator removal", test_iterator_with_removal},
        {NULL, NULL},
};

static const NutestSuite suite = {
        "hashmap",
        hashmap_tests,
        &(NutestSuite){
                "/iterator",
                iter_tests,
                NULL,
                setup_iterator,
                teardown_iterator,
        },
        setup_hashmap,
        teardown_hashmap,
};

int main(int argc, char **argv) { return nutest_suite_main(&suite, NULL, argc, argv); }

#include "hashmap.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define HASHMAP_SIZE (200)

typedef struct Entry Entry;

struct Entry {
    uintptr_t key;
    void *value;
    Entry *next;
};

struct HashMap {
    Entry **entries;
};

struct HashMapIter {
    HashMap *map;
    Entry *entry; // Pointer to entry that is currently taking look at.
    int hashIdx;  // Current index for `map->entries` that have `entry`.
};

static size_t get_hash(uintptr_t key, size_t size);
static Entry *find_entry(HashMap *map, uintptr_t key);
static void free_entry(Entry *entry);

HashMap *hashmap_new(void) {
    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    if (map == NULL) {
        return NULL;
    }
    map->entries = (Entry **)malloc(sizeof(Entry) * HASHMAP_SIZE);
    memset(map->entries, 0, sizeof(Entry) * HASHMAP_SIZE);
    return map;
}

void hashmap_destroy(HashMap **map) {
    if (map != NULL) {
        hashmap_clear(*map);
        free(*map);
        *map = NULL;
    }
}

bool hashmap_empty(HashMap *map) {
    for (int i = 0; i < HASHMAP_SIZE; ++i) {
        if (map->entries[i] != NULL) {
            return false;
        }
    }
    return true;
}

void hashmap_insert(HashMap *map, void *key, void *value) {
    const size_t hash = get_hash((uintptr_t)key, HASHMAP_SIZE);
    Entry *newEntry;

    if (find_entry(map, (uintptr_t)key)) {
        fprintf(stderr, "%s:%d: Attempt to overwrite existing entry: key = %p\n",
                __FILE__, __LINE__, key);
        exit(1);
    }

    newEntry = (Entry *)malloc(sizeof(Entry));
    dieWhenNULL(newEntry);
    newEntry->key = (uintptr_t)key;
    newEntry->value = value;
    newEntry->next = map->entries[hash];
    map->entries[hash] = newEntry;
}

void *hashmap_find(HashMap *map, void *key) {
    Entry *entry = find_entry(map, (uintptr_t)key);
    if (entry == NULL) {
        return NULL;
    }
    return entry->value;
}

void hashmap_remove(HashMap *map, void *key) {
    const size_t hash = get_hash((uintptr_t)key, HASHMAP_SIZE);
    Entry **walk = &map->entries[hash];

    while (*walk && (*walk)->key != (uintptr_t)key) {
        walk = &(*walk)->next;
    }

    if (*walk) {
        // Found associated item.
        Entry *entry = *walk;
        *walk = entry->next;
        free_entry(entry);
    }
}

void hashmap_clear(HashMap *map) {
    for (int i = 0; i < HASHMAP_SIZE; ++i) {
        Entry *entry = map->entries[i];
        while (entry) {
            Entry *cur = entry;
            if (entry->value) {
                free(entry->value);
            }
            entry = entry->next;
            free(cur);
        }
    }
}

HashMapIter *hashmap_iter(HashMap *map) {
    HashMapIter *iter = (HashMapIter *)malloc(sizeof(HashMapIter));
    if (iter == NULL) {
        return NULL;
    }
    iter->map = map;
    iter->hashIdx = -1;
    iter->entry = NULL;
    hashmap_iter_next(iter); // Make sure that `iter` points to an valid entry.
    return iter;
}

void hashmap_iter_next(HashMapIter *iter) {
    if (hashmap_iter_is_end(iter)) {
        // Nothing to do.
        return;
    }

    if (iter->entry != NULL) {
        iter->entry = iter->entry->next;
    }

    if (iter->entry == NULL) {
        while (++iter->hashIdx < HASHMAP_SIZE) {
            if (iter->map->entries[iter->hashIdx] != NULL) {
                iter->entry = iter->map->entries[iter->hashIdx];
                return;
            }
        }
    }
}

bool hashmap_iter_is_end(HashMapIter *iter) { return iter->hashIdx >= HASHMAP_SIZE; }

void *hashmap_iter_get_key(HashMapIter *iter) {
    return iter->entry ? (void *)iter->entry->key : NULL;
}

void *hashmap_iter_get_value(HashMapIter *iter) {
    return iter->entry ? (void *)iter->entry->value : NULL;
}

void hashmap_iter_remove(HashMapIter *iter) {
    Entry **work;

    if (iter->entry == NULL) {
        return;
    }
    work = &iter->map->entries[iter->hashIdx];

    while ((*work) != iter->entry) {
        work = &(*work)->next;
    }

    *work = iter->entry->next;
    free_entry(iter->entry);
    iter->entry = *work;
    hashmap_iter_next(iter);
}

void hashmap_iter_destroy(HashMapIter **iter) {
    if (*iter != NULL) {
        free(*iter);
        *iter = NULL;
    }
}

size_t get_hash(uintptr_t key, size_t size) {
    // 47 is a random prime number.
    return (key * 47) % size;
}

Entry *find_entry(HashMap *map, uintptr_t key) {
    const size_t hash = get_hash((uintptr_t)key, HASHMAP_SIZE);
    Entry *entry = map->entries[hash];

    for (; entry; entry = entry->next) {
        if (entry->key == (uintptr_t)key) {
            break;
        }
    }
    return entry;
}

void free_entry(Entry *entry) {
    if (entry) {
        if (entry->value) {
            free(entry->value);
        }
        free(entry);
    }
}

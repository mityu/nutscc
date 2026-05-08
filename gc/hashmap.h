#ifndef NUTS_GC_HASHMAP_H
#define NUTS_GC_HASHMAP_H 1

#include <stdbool.h>
#include <stdio.h> // IWYU pragma: keep (used by dieWhenNULL macro)
#include <stdlib.h>

#define dieWhenNULL(ptr)                                                                 \
    do {                                                                                 \
        if ((ptr) == NULL) {                                                             \
            fprintf(stderr, "Memory allocation failure at: %s:%d", __FILE__, __LINE__);  \
            exit(1);                                                                     \
        }                                                                                \
    } while (0)

typedef struct HashMap HashMap;
typedef struct HashMapIter HashMapIter;

// Creates new HashMap instance.  Returns NULL on memory allocation failure.
HashMap *hashmap_new(void);

// Destroies a HashMap.
void hashmap_destroy(HashMap **map);

// Returns TRUE if the given hashmap contains no items.  Otherwise FALSE.
bool hashmap_empty(HashMap *map);

// Insert an entry to `map`.  Bail out when given key is already in `map`.
// Given value must be a memory area that is
// allocated by malloc() or something similar.
void hashmap_insert(HashMap *map, void *key, void *value);

// Lookup value associated to `key` from `map`.  Returns NULL if entry is not
// found.
void *hashmap_find(HashMap *map, void *key);

// Remove entry whose key is `key` from `map`.  The associated value is also
// freed on this operation.  Do nothing when no items found by the given `key`.
void hashmap_remove(HashMap *map, void *key);

// Remove all entry in `map`.
void hashmap_clear(HashMap *map);

// Get an iterator for enumerate all entries in `map`.
// It's behavior is undefined that some items are added to `map` or
// removed from `map` while iterating.
HashMapIter *hashmap_iter(HashMap *map);

// Step `iter` to next entry.  Returns TRUE if succesfully stepped to next.
// Otherwise returns FALSE (i.e. `iter` no more points to valid entry.)
void hashmap_iter_next(HashMapIter *iter);

// Check `iter` is already reached at the end of the HashMap's entry list.
// Returns TRUE if so, and otherwise returns FALSE.
bool hashmap_iter_is_end(HashMapIter *iter);

// Get key of the hash map entry that `iter` points to.
// Returns NULL when `iter` is already at the end.
void *hashmap_iter_get_key(HashMapIter *iter);

// Get value of the hash map entry that `iter` points to.
// Returns NULL when `iter` is already at the end.
void *hashmap_iter_get_value(HashMapIter *iter);

// Remove an item that is pointed by `iter` from hashmap.
// After the removal, `iter` points to the entry that was next to what `iter`
// used to point to.
void hashmap_iter_remove(HashMapIter *iter);

// Postlude for item iteration.  This is for freeing the iterator object.
void hashmap_iter_destroy(HashMapIter **iter);

#endif

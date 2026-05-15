#include "containers/vector.h"
#include "gc/gc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_VECTOR_CAP (1 << 6)
#define VECTOR_CAP_MERGIN (2)

struct Vector {
    size_t size;
    size_t cap;
    void **entries;
};

Vector *vec_new(void) {
    Vector *v = (Vector *)gc_malloc(sizeof(Vector));
    if (v == NULL) {
        return NULL;
    }
    v->cap = INITIAL_VECTOR_CAP - VECTOR_CAP_MERGIN;
    v->size = 0;
    v->entries = gc_malloc(v->cap);
    if (v->entries == NULL) {
        return NULL;
    }
    return v;
}

void *vec_get(Vector *v, size_t index) {
    if (index >= v->size) {
        return NULL;
    }
    return v->entries[index];
}

void vec_push(Vector *v, void *newElem) {
    if (v->size == v->cap) {
        void **newArea = NULL;
        v->cap = ((v->cap + VECTOR_CAP_MERGIN) << 1) - VECTOR_CAP_MERGIN;
        newArea = (void **)gc_malloc_or_die(v->cap);
        memcpy(newArea, v->entries, v->size);
        v->entries = newArea;
    }
    v->entries[v->size++] = newElem;
}

void *vec_pop(Vector *v) {
    if (vec_empty(v)) {
        return NULL;
    }
    return v->entries[--v->size];
}

bool vec_empty(Vector *v) { return v->size == 0; }

size_t vec_size(Vector *v) { return v->size; }

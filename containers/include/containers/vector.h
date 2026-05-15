// An vector implementation that stores array of pointers.
#ifndef NUTSCC_CONTAINER_VECTOR_H
#define NUTSCC_CONTAINER_VECTOR_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Vector Vector;

Vector *vec_new(void);
void *vec_get(Vector *v, size_t index);
void vec_push(Vector *v, void *newElem);
void *vec_pop(Vector *v);
bool vec_empty(Vector *v);
size_t vec_size(Vector *v);

#endif // NUTSCC_CONTAINER_VECTOR_H

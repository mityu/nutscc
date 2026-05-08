#ifndef NUTS_GC_H
#define NUTS_GC_H 1

#include <stddef.h>

// Initialize the gc library.
void gc_init(void);

// Allocate memory and returns the pointer to the area.  The allocated memories are all
// zero-cleared.  Returns NULL on memory allocation failure.
void *gc_malloc(size_t size);

// Similar to gc_malloc(), but aborts program when allocating memory fails.
void *gc_malloc_or_die(size_t size);

// Run garbage collector.
void gc_collect(void);

#endif

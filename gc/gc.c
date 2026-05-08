#include "gc/gc.h"
#include "./hashmap.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define GC_TRIGGER_BYTE_SIZE (1e10)
#define GC_TRIGGER_INTERVAL (120.0)

// TODO: More portable way.
extern void *__libc_stack_end;

typedef struct MemArea {
    uintptr_t top;
    uintptr_t bottom;
} MemArea;

typedef struct GcInfo GcInfo;
struct GcInfo {
    size_t totalSize;    // Sum of currently allocated memory size.
    clock_t lastGcClock; // The time that GC is done last time.
    HashMap *mems;       // List of allocated memory areas.
    MemArea area;        // Memory area that can allocated memory appears.
};

typedef struct MemInfo MemInfo;
struct MemInfo {
    // Size of allocated memory
    size_t size;
    bool alive;
};

static void gc_collect_mark(uintptr_t stackbottom, uintptr_t stacktop);
static void gc_collect_sweep(void);
static bool should_trigger_gc(void);

static GcInfo gcinfo = {};

void gc_init(void) {
    if (gcinfo.mems == NULL) {
        gcinfo.totalSize = 0;
        gcinfo.area.top = 0;
        gcinfo.area.bottom = UINTPTR_MAX;
        gcinfo.mems = hashmap_new();
        gcinfo.lastGcClock = clock();
    }
}

void *gc_malloc(size_t size) {
    void *ptr = NULL;
    MemInfo *meminfo = NULL;

    if (should_trigger_gc()) {
        gc_collect();
    }

    meminfo = (MemInfo *)malloc(sizeof(MemInfo));
    if (meminfo == NULL) {
        return NULL;
    }

    meminfo->size = size;
    meminfo->alive = false;
    ptr = malloc(size);
    if (ptr == NULL) {
        free(meminfo);
        return NULL;
    }

    gcinfo.totalSize += size;
    if ((uintptr_t)ptr > gcinfo.area.top) {
        gcinfo.area.top = (uintptr_t)ptr;
    }
    if ((uintptr_t)ptr < gcinfo.area.bottom) {
        gcinfo.area.bottom = (uintptr_t)ptr;
    }
    hashmap_insert(gcinfo.mems, ptr, (void *)meminfo);

    memset(ptr, 0, size);
    return ptr;
}

void *gc_malloc_or_die(size_t size) {
    void *p = gc_malloc(size);
    if (p == NULL) {
        fputs("gc_malloc(): Memory allocation failure. Abort.\n", stderr);
        exit(1);
    }
    return p;
}

void gc_collect(void) {
    // This is a dummy variable to get the address of the top of the stack.
    // Starting from this address, we're going to scan the stack memory every
    // sizeof(T*) bytes (typically 8 bytes) to find alive pointer to heap.
    // Since this address will be the starting point of scan, this dummy
    // variable should be a pointer (in order to make sure that it is aligned
    // as same as any pointer value) so that we can prevent scanning memory
    // using a slightly off-aligned offset.
    void *stacktop;

    gc_collect_mark((uintptr_t)&stacktop, (uintptr_t)__libc_stack_end);
    gc_collect_sweep();
    gcinfo.lastGcClock = clock();
}

void gc_collect_mark(uintptr_t stacktop, uintptr_t stackbottom) {
    MemInfo *meminfo = NULL;

    if (stackbottom < stacktop) {
        uintptr_t tmp = stackbottom;
        stackbottom = stacktop;
        stacktop = tmp;
    }

    for (uintptr_t work = stacktop; work <= stackbottom; work += sizeof(void *)) {
        uintptr_t p = (uintptr_t)*(void **)work;
        if ((p & 0x3) != 0) {
            continue;
        } else if (p < gcinfo.area.bottom || p > gcinfo.area.top) {
            continue;
        }

        meminfo = (MemInfo *)hashmap_find(gcinfo.mems, (void *)p);
        if (meminfo == NULL) {
            continue;
        }

        // Maybe the value of `work` is an address to a heap pointer that is under
        // control of this GC library.
        if (!meminfo->alive) {
            meminfo->alive = true;
            gc_collect_mark(p, p + meminfo->size);
        }
    }
}

void gc_collect_sweep(void) {
    HashMapIter *iter = hashmap_iter(gcinfo.mems);
    dieWhenNULL(iter);

    gcinfo.area.bottom = UINTPTR_MAX;
    gcinfo.area.top = 0;

    while (!hashmap_iter_is_end(iter)) {
        uintptr_t address = (uintptr_t)hashmap_iter_get_key(iter);
        MemInfo *meminfo = (MemInfo *)hashmap_iter_get_value(iter);
        if (meminfo->alive) {
            meminfo->alive = false;
            if (address < gcinfo.area.bottom) {
                gcinfo.area.bottom = address;
            }
            if ((address + meminfo->size) > gcinfo.area.top) {
                gcinfo.area.top = address = meminfo->size;
            }
            hashmap_iter_next(iter);
        } else {
            gcinfo.totalSize -= meminfo->size;
            hashmap_iter_remove(iter);
            free((void *)address);
        }
    }

    hashmap_iter_destroy(&iter);
}

bool should_trigger_gc(void) {
    double elapsed = (double)(clock() - gcinfo.lastGcClock) / CLOCKS_PER_SEC;
    return (gcinfo.totalSize > GC_TRIGGER_BYTE_SIZE) || (elapsed > GC_TRIGGER_INTERVAL);
}

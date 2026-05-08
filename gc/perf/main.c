#include "gc/gc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(ENABLE_GC)
#define GC_INIT() gc_init()
#define GC_MALLOC(size) gc_malloc(size)
#define GC_COLLECT() gc_collect()
#else
#define GC_INIT()
#define GC_MALLOC(size) malloc(size)
#define GC_COLLECT()
#endif

int main() {
    char buf[100];
    GC_INIT();
    for (;;) {
        void *p = GC_MALLOC(10000000);

        // Dummy operation with effect.  This is for preventing compiler from
        // applying optimization and eliminating malloc() calls.
        sprintf(buf, "%p", p);

        GC_COLLECT();
        usleep(1000);
    }
}

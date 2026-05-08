#include "../gc.h"
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
    GC_INIT();
    for (;;) {
        void *p = GC_MALLOC(10000000);
        GC_COLLECT();
        usleep(1000);
    }
}

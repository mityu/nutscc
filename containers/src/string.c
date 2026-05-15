// Non NUL-terminated string stroage.
#include "containers/string.h"
#include "gc/gc.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAP (32)
#define CAP_MERGIN (8)

struct String {
    size_t len; // The length of string without the ending NUL.
    size_t cap;
    char *value;
};

static String *string_vformat(const char *fmt, va_list ap);
static size_t decide_capacity(const String *s, size_t required);
static void reserve_capacity(String *s, size_t required);

String *string_new(void) { return string_from_literal(""); }

String *string_from_literal(const char *from) {
    String *s = gc_malloc_or_die(sizeof(String));
    s->len = strlen(from);
    s->cap = INITIAL_CAP - CAP_MERGIN;
    s->cap = decide_capacity(s, s->len);
    s->value = (char *)gc_malloc_or_die(sizeof(char) * s->cap);
    strncpy(s->value, from, s->len);
    return s;
}

String *string_clone(const String *from) {
    String *s = (String *)gc_malloc_or_die(sizeof(String));
    *s = *from;
    s->value = (char *)gc_malloc_or_die(sizeof(char) * s->cap);
    strncpy(s->value, from->value, from->len);
    return s;
}

String *string_format(const char *fmt, ...) {
    String *s = NULL;
    va_list ap;

    va_start(ap, fmt);
    s = string_vformat(fmt, ap);
    va_end(ap);

    return s;
}

String *string_vformat(const char *fmt, va_list ap) {
    String *dst = NULL;
    int outlen = 0;
    va_list apCopy;

    va_copy(apCopy, ap);

    outlen = vsnprintf(NULL, 0, fmt, ap);
    if (outlen < 0) {
        va_end(ap); // Special path; finalize the given va_list before exiting program.
        fprintf(stderr, "vsnprintf() error: returned: %d", outlen);
        exit(1);
    }
    dst = string_new();
    reserve_capacity(dst, outlen + 1); // One more space for NUL at the end of string.
    vsnprintf(dst->value, outlen + 1, fmt, apCopy);
    dst->len = outlen; // The dst->len doesn't count the ending NUL character.

    va_end(apCopy);

    return dst;
}

void string_append(String *base, const String *addition) {
    size_t req = base->len + addition->len;
    reserve_capacity(base, req);
    strncpy(base->value + base->len, addition->value, addition->len);
    base->len += addition->len;
}

String *string_concat(const String *s1, const String *s2) {
    String *dst = string_clone(s1);
    string_append(dst, s2);
    return dst;
}

char *string_get_raw(const String *s) {
    char *p = gc_malloc_or_die(sizeof(char) * (s->len + 1));
    strncpy(p, s->value, s->len);
    p[s->len] = '\0';
    return p;
}

void string_fput(const String *s, FILE *stream) {
    fprintf(stream, "%.*s", (int)s->len, s->value);
}

bool string_eq(const String *s1, const String *s2) {
    return s1->len == s2->len && strncmp(s1->value, s2->value, s1->len) == 0;
}

size_t decide_capacity(const String *s, size_t required) {
    size_t cap = 0;
    if (s->cap > required) {
        return s->cap;
    }
    required += CAP_MERGIN;
    cap = s->cap + CAP_MERGIN;
    while (cap <= required) {
        cap <<= 1;
    }
    return cap - CAP_MERGIN;
}

void reserve_capacity(String *s, size_t required) {
    size_t cap = decide_capacity(s, required);
    char *prev = NULL;
    if (cap <= s->cap) {
        return;
    }
    prev = s->value;
    s->cap = cap;
    s->value = (char *)gc_malloc_or_die(sizeof(char) * s->cap);
    strncpy(s->value, prev, s->len);
}

#ifndef NUTSCC_CONTAINER_STRING_H
#define NUTSCC_CONTAINER_STRING_H

#include <stdbool.h>
#include <stdio.h>

typedef struct String String;

String *string_new(void);
String *string_from_literal(const char *from);
String *string_clone(const String *from);
String *string_format(const char *fmt, ...);
void string_append(String *base, const String *addition);
String *string_concat(const String *s1, const String *s2);
char *string_get_raw(const String *s);
void string_fput(const String *s, FILE *stream);
bool string_eq(const String *s1, const String *s2);

#endif // NUTSCC_CONTAINER_STRING_H

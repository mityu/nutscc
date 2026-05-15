#include "containers/string.h"
#include "gc/gc.h"
#include "nutest.h"

void test_string_new_returns_empty_string(void *data) {
    String *s = string_new();

    (void)data;
    assert_string_equal("", string_get_raw(s));
}

void test_string_from_literal(void *data) {
    String *s = string_from_literal("xyzabc");
    assert_string_equal("xyzabc", string_get_raw(s));
}

void test_string_format(void *data) {
    String *s = string_format("%04d:%s", 42, "hjkl");
    assert_string_equal("0042:hjkl", string_get_raw(s));
}

void test_string_clone(void *data) {
    String *s1 = string_from_literal("xyzabc");
    String *s2 = string_clone(s1);
    assert_string_equal("xyzabc", string_get_raw(s2));
}

void test_string_append(void *data) {
    String *s1 = string_from_literal("abc");
    String *s2 = string_from_literal("xyz");
    string_append(s1, s2);
    assert_string_equal("abcxyz", string_get_raw(s1));
    assert_string_equal("xyz", string_get_raw(s2));
}

void test_string_clone_append(void *data) {
    String *original = string_from_literal("hjkl");
    String *s1 = string_clone(original);
    String *s2 = string_from_literal("abc");
    string_append(s1, s2);
    assert_string_equal("hjkl", string_get_raw(original));
    assert_string_equal("hjklabc", string_get_raw(s1));
    assert_string_equal("abc", string_get_raw(s2));
}

void test_string_concat(void *data) {
    String *s1 = string_from_literal("xyz");
    String *s2 = string_from_literal("abc");
    String *result = string_concat(s1, s2);
    assert_string_equal("xyz", string_get_raw(s1));
    assert_string_equal("abc", string_get_raw(s2));
    assert_string_equal("xyzabc", string_get_raw(result));
}

void test_string_eq(void *data) {
    String *s1 = string_from_literal("xyz");
    String *s2 = string_from_literal("xyz");
    String *s3 = string_from_literal("xyzabc");
    assert_true(string_eq(s1, s2));
    assert_false(string_eq(s2, s3));
}

static NutestTest tests[] = {
        {"/string_new() returns empty string", test_string_new_returns_empty_string},
        {"/string_from_literal() returns string whose contents is given string",
                test_string_from_literal},
        {"/string_format() returns formatted string", test_string_format},
        {"/string_clone() makes a copy of the given string", test_string_clone},
        {"/string_append() adds given string at the end", test_string_append},
        {"/string_clone() makes a string independent from original string",
                test_string_clone_append},
        {"/string_concat() returns a new concat string", test_string_concat},
        {"/string_eq() compares two string", test_string_eq},
        {},
};

static const NutestSuite suite = {"containers/string", tests};

int main(int argc, char **argv) {
    gc_init();
    return nutest_suite_main(&suite, NULL, argc, argv);
}

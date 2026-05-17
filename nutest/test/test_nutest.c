#include "nutest.h"

extern NutestSuite suite_assertions;
extern NutestSuite suite_execution_order;
extern NutestSuite suite_setup_teardown;

int main(int argc, char **argv) {
    NutestSuite subsuites[] = {
            suite_assertions,
            suite_execution_order,
            suite_setup_teardown,
            {},
    };
    const NutestSuite test_suite = {"test nutest", NULL, subsuites};

    return nutest_suite_main(&test_suite, NULL, argc, argv);
}

/* test_runner.c - Main test runner for gneural_network unit tests
 *
 * Runs all test suites and reports results
 */

#include "test_framework.h"

/* Define test tracking variables */
int tests_run = 0;
int tests_passed = 0;
int tests_failed = 0;
const char *current_test_name = NULL;

/* External test suite runners */
extern void run_math_utils_tests(void);
extern void run_activation_tests(void);
extern void run_network_tests(void);
extern void run_feedforward_tests(void);

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    printf("\n");
    printf("=====================================\n");
    printf("  gneural_network Unit Test Suite\n");
    printf("=====================================\n");

    /* Run all test suites */
    run_math_utils_tests();
    run_activation_tests();
    run_network_tests();
    run_feedforward_tests();

    /* Print final results */
    PRINT_TEST_RESULTS();

    return TEST_RESULT();
}

/* test_framework.h - Minimal unit testing framework for gneural_network
 *
 * A lightweight, self-contained testing framework inspired by Unity/MinUnit
 */

#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Test result tracking - declared extern, defined in test_runner.c */
extern int tests_run;
extern int tests_passed;
extern int tests_failed;
extern const char *current_test_name;

/* Color codes for terminal output */
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_RESET   "\x1b[0m"

/* Core assertion macros */
#define TEST_ASSERT(condition) do { \
    if (!(condition)) { \
        printf(COLOR_RED "  FAIL: %s:%d: Assertion failed: %s" COLOR_RESET "\n", \
               __FILE__, __LINE__, #condition); \
        return 1; \
    } \
} while(0)

#define TEST_ASSERT_MSG(condition, msg) do { \
    if (!(condition)) { \
        printf(COLOR_RED "  FAIL: %s:%d: %s" COLOR_RESET "\n", \
               __FILE__, __LINE__, msg); \
        return 1; \
    } \
} while(0)

#define TEST_ASSERT_EQUAL(expected, actual) do { \
    if ((expected) != (actual)) { \
        printf(COLOR_RED "  FAIL: %s:%d: Expected %d, got %d" COLOR_RESET "\n", \
               __FILE__, __LINE__, (int)(expected), (int)(actual)); \
        return 1; \
    } \
} while(0)

#define TEST_ASSERT_EQUAL_UINT(expected, actual) do { \
    if ((expected) != (actual)) { \
        printf(COLOR_RED "  FAIL: %s:%d: Expected %u, got %u" COLOR_RESET "\n", \
               __FILE__, __LINE__, (unsigned int)(expected), (unsigned int)(actual)); \
        return 1; \
    } \
} while(0)

#define TEST_ASSERT_EQUAL_UINT64(expected, actual) do { \
    if ((expected) != (actual)) { \
        printf(COLOR_RED "  FAIL: %s:%d: Expected %lu, got %lu" COLOR_RESET "\n", \
               __FILE__, __LINE__, (unsigned long)(expected), (unsigned long)(actual)); \
        return 1; \
    } \
} while(0)

#define TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual) do { \
    double _diff = fabs((double)(expected) - (double)(actual)); \
    if (_diff > (double)(delta)) { \
        printf(COLOR_RED "  FAIL: %s:%d: Expected %.10g (+/- %.10g), got %.10g (diff: %.10g)" COLOR_RESET "\n", \
               __FILE__, __LINE__, (double)(expected), (double)(delta), (double)(actual), _diff); \
        return 1; \
    } \
} while(0)

#define TEST_ASSERT_DOUBLE_WITHIN(delta, expected, actual) \
    TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)

#define TEST_ASSERT_NOT_NULL(ptr) do { \
    if ((ptr) == NULL) { \
        printf(COLOR_RED "  FAIL: %s:%d: Pointer is NULL" COLOR_RESET "\n", \
               __FILE__, __LINE__); \
        return 1; \
    } \
} while(0)

#define TEST_ASSERT_NULL(ptr) do { \
    if ((ptr) != NULL) { \
        printf(COLOR_RED "  FAIL: %s:%d: Pointer is not NULL" COLOR_RESET "\n", \
               __FILE__, __LINE__); \
        return 1; \
    } \
} while(0)

#define TEST_ASSERT_TRUE(condition) TEST_ASSERT(condition)
#define TEST_ASSERT_FALSE(condition) TEST_ASSERT(!(condition))

#define TEST_ASSERT_GREATER_THAN(threshold, actual) do { \
    if (!((actual) > (threshold))) { \
        printf(COLOR_RED "  FAIL: %s:%d: Expected %g > %g" COLOR_RESET "\n", \
               __FILE__, __LINE__, (double)(actual), (double)(threshold)); \
        return 1; \
    } \
} while(0)

#define TEST_ASSERT_LESS_THAN(threshold, actual) do { \
    if (!((actual) < (threshold))) { \
        printf(COLOR_RED "  FAIL: %s:%d: Expected %g < %g" COLOR_RESET "\n", \
               __FILE__, __LINE__, (double)(actual), (double)(threshold)); \
        return 1; \
    } \
} while(0)

#define TEST_ASSERT_GREATER_OR_EQUAL(threshold, actual) do { \
    if (!((actual) >= (threshold))) { \
        printf(COLOR_RED "  FAIL: %s:%d: Expected %g >= %g" COLOR_RESET "\n", \
               __FILE__, __LINE__, (double)(actual), (double)(threshold)); \
        return 1; \
    } \
} while(0)

#define TEST_ASSERT_LESS_OR_EQUAL(threshold, actual) do { \
    if (!((actual) <= (threshold))) { \
        printf(COLOR_RED "  FAIL: %s:%d: Expected %g <= %g" COLOR_RESET "\n", \
               __FILE__, __LINE__, (double)(actual), (double)(threshold)); \
        return 1; \
    } \
} while(0)

/* Run a single test function */
#define RUN_TEST(test_func) do { \
    current_test_name = #test_func; \
    printf("  Running %s...", #test_func); \
    fflush(stdout); \
    tests_run++; \
    int result = test_func(); \
    if (result == 0) { \
        printf(COLOR_GREEN " PASS" COLOR_RESET "\n"); \
        tests_passed++; \
    } else { \
        tests_failed++; \
    } \
} while(0)

/* Test suite macros */
#define TEST_SUITE_BEGIN(name) do { \
    printf("\n" COLOR_YELLOW "=== Test Suite: %s ===" COLOR_RESET "\n", name); \
} while(0)

#define TEST_SUITE_END() do { \
    printf("\n"); \
} while(0)

/* Print final results */
#define PRINT_TEST_RESULTS() do { \
    printf("\n" COLOR_YELLOW "==============================" COLOR_RESET "\n"); \
    printf("Tests run: %d\n", tests_run); \
    printf(COLOR_GREEN "Passed: %d" COLOR_RESET "\n", tests_passed); \
    if (tests_failed > 0) { \
        printf(COLOR_RED "Failed: %d" COLOR_RESET "\n", tests_failed); \
    } else { \
        printf("Failed: 0\n"); \
    } \
    printf(COLOR_YELLOW "==============================" COLOR_RESET "\n"); \
} while(0)

/* Return overall test result (0 = all passed, 1 = some failed) */
#define TEST_RESULT() (tests_failed > 0 ? 1 : 0)

#endif /* TEST_FRAMEWORK_H */

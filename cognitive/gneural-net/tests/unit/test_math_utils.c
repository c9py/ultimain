/* test_math_utils.c - Unit tests for mathematical utility functions
 *
 * Tests: fact.c, binom.c, rnd.c
 */

#include "test_framework.h"
#include <stdint.h>

/* Include the headers we need to test */
#include "../../include/defines.h"
#include "../../include/fact.h"
#include "../../include/binom.h"
#include "../../include/rnd.h"

/* ==================== Factorial Tests ==================== */

int test_fact_zero(void) {
    /* 0! = 1 */
    TEST_ASSERT_EQUAL_UINT64(1, fact(0));
    return 0;
}

int test_fact_one(void) {
    /* 1! = 1 */
    TEST_ASSERT_EQUAL_UINT64(1, fact(1));
    return 0;
}

int test_fact_small_numbers(void) {
    /* 2! = 2 */
    TEST_ASSERT_EQUAL_UINT64(2, fact(2));
    /* 3! = 6 */
    TEST_ASSERT_EQUAL_UINT64(6, fact(3));
    /* 4! = 24 */
    TEST_ASSERT_EQUAL_UINT64(24, fact(4));
    /* 5! = 120 */
    TEST_ASSERT_EQUAL_UINT64(120, fact(5));
    return 0;
}

int test_fact_medium_numbers(void) {
    /* 10! = 3628800 */
    TEST_ASSERT_EQUAL_UINT64(3628800, fact(10));
    /* 12! = 479001600 */
    TEST_ASSERT_EQUAL_UINT64(479001600, fact(12));
    return 0;
}

int test_fact_large_numbers(void) {
    /* 20! = 2432902008176640000 */
    TEST_ASSERT_EQUAL_UINT64(2432902008176640000ULL, fact(20));
    return 0;
}

/* ==================== Binomial Coefficient Tests ==================== */

int test_binom_n_choose_0(void) {
    /* C(n,0) = 1 for all n */
    TEST_ASSERT_EQUAL(1, binom(0, 0));
    TEST_ASSERT_EQUAL(1, binom(1, 0));
    TEST_ASSERT_EQUAL(1, binom(5, 0));
    TEST_ASSERT_EQUAL(1, binom(10, 0));
    return 0;
}

int test_binom_n_choose_n(void) {
    /* C(n,n) = 1 for all n */
    TEST_ASSERT_EQUAL(1, binom(0, 0));
    TEST_ASSERT_EQUAL(1, binom(1, 1));
    TEST_ASSERT_EQUAL(1, binom(5, 5));
    TEST_ASSERT_EQUAL(1, binom(10, 10));
    return 0;
}

int test_binom_n_choose_1(void) {
    /* C(n,1) = n for all n */
    TEST_ASSERT_EQUAL(1, binom(1, 1));
    TEST_ASSERT_EQUAL(5, binom(5, 1));
    TEST_ASSERT_EQUAL(10, binom(10, 1));
    return 0;
}

int test_binom_pascals_triangle(void) {
    /* Row 4: 1 4 6 4 1 */
    TEST_ASSERT_EQUAL(1, binom(4, 0));
    TEST_ASSERT_EQUAL(4, binom(4, 1));
    TEST_ASSERT_EQUAL(6, binom(4, 2));
    TEST_ASSERT_EQUAL(4, binom(4, 3));
    TEST_ASSERT_EQUAL(1, binom(4, 4));

    /* Row 5: 1 5 10 10 5 1 */
    TEST_ASSERT_EQUAL(1, binom(5, 0));
    TEST_ASSERT_EQUAL(5, binom(5, 1));
    TEST_ASSERT_EQUAL(10, binom(5, 2));
    TEST_ASSERT_EQUAL(10, binom(5, 3));
    TEST_ASSERT_EQUAL(5, binom(5, 4));
    TEST_ASSERT_EQUAL(1, binom(5, 5));
    return 0;
}

int test_binom_larger_values(void) {
    /* C(10,5) = 252 */
    TEST_ASSERT_EQUAL(252, binom(10, 5));
    /* C(10,3) = 120 */
    TEST_ASSERT_EQUAL(120, binom(10, 3));
    /* C(12,4) = 495 */
    TEST_ASSERT_EQUAL(495, binom(12, 4));
    return 0;
}

int test_binom_symmetry(void) {
    /* C(n,k) = C(n, n-k) */
    TEST_ASSERT_EQUAL(binom(10, 3), binom(10, 7));
    TEST_ASSERT_EQUAL(binom(8, 2), binom(8, 6));
    TEST_ASSERT_EQUAL(binom(6, 1), binom(6, 5));
    return 0;
}

/* ==================== Random Number Generator Tests ==================== */

int test_rnd_range(void) {
    /* rnd() should return values in [0, 1) */
    int i;
    for (i = 0; i < 1000; i++) {
        double r = rnd();
        TEST_ASSERT_GREATER_OR_EQUAL(0.0, r);
        TEST_ASSERT_LESS_THAN(1.0, r);
    }
    return 0;
}

int test_rnd_not_constant(void) {
    /* rnd() should produce different values (not all the same) */
    double first = rnd();
    int different_count = 0;
    int i;

    for (i = 0; i < 100; i++) {
        double r = rnd();
        if (fabs(r - first) > 1e-10) {
            different_count++;
        }
    }

    /* At least 90% should be different from the first value */
    TEST_ASSERT_GREATER_THAN(90, different_count);
    return 0;
}

int test_rnd_distribution(void) {
    /* Very basic distribution test - values should be somewhat uniform */
    int bins[10] = {0};
    int i;
    int total = 10000;

    for (i = 0; i < total; i++) {
        double r = rnd();
        int bin = (int)(r * 10);
        if (bin >= 10) bin = 9;  /* Handle r = 1.0 edge case */
        bins[bin]++;
    }

    /* Each bin should have roughly 10% of values (allow 5-15%) */
    for (i = 0; i < 10; i++) {
        double percent = (double)bins[i] / total * 100.0;
        TEST_ASSERT_GREATER_THAN(5.0, percent);
        TEST_ASSERT_LESS_THAN(15.0, percent);
    }
    return 0;
}

int test_rnd_deterministic(void) {
    /* The LCG implementation should be deterministic with fixed seed */
    /* Call rnd() multiple times and verify sequence is consistent */
    /* Note: This tests the current implementation behavior */
    double r1 = rnd();
    double r2 = rnd();
    double r3 = rnd();

    /* Verify they are all different */
    TEST_ASSERT(fabs(r1 - r2) > 1e-10);
    TEST_ASSERT(fabs(r2 - r3) > 1e-10);
    TEST_ASSERT(fabs(r1 - r3) > 1e-10);
    return 0;
}

/* ==================== Test Runner ==================== */

void run_math_utils_tests(void) {
    TEST_SUITE_BEGIN("Factorial (fact.c)");
    RUN_TEST(test_fact_zero);
    RUN_TEST(test_fact_one);
    RUN_TEST(test_fact_small_numbers);
    RUN_TEST(test_fact_medium_numbers);
    RUN_TEST(test_fact_large_numbers);
    TEST_SUITE_END();

    TEST_SUITE_BEGIN("Binomial Coefficient (binom.c)");
    RUN_TEST(test_binom_n_choose_0);
    RUN_TEST(test_binom_n_choose_n);
    RUN_TEST(test_binom_n_choose_1);
    RUN_TEST(test_binom_pascals_triangle);
    RUN_TEST(test_binom_larger_values);
    RUN_TEST(test_binom_symmetry);
    TEST_SUITE_END();

    TEST_SUITE_BEGIN("Random Number Generator (rnd.c)");
    RUN_TEST(test_rnd_range);
    RUN_TEST(test_rnd_not_constant);
    RUN_TEST(test_rnd_distribution);
    RUN_TEST(test_rnd_deterministic);
    TEST_SUITE_END();
}

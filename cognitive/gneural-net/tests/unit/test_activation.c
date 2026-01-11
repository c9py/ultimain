/* test_activation.c - Unit tests for activation functions
 *
 * Tests: activation.c
 */

#include "test_framework.h"
#include <math.h>

/* Include the headers we need to test */
#include "../../include/defines.h"
#include "../../include/activation.h"

/* Tolerance for floating point comparisons */
#define FLOAT_TOLERANCE 1e-10

/* ==================== TANH Activation Tests ==================== */

int test_activation_tanh_zero(void) {
    /* tanh(0) = 0 */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 0.0, activation(TANH, 0.0));
    return 0;
}

int test_activation_tanh_positive(void) {
    /* tanh(1) should be close to 0.7616 */
    TEST_ASSERT_FLOAT_WITHIN(1e-4, 0.7616, activation(TANH, 1.0));
    /* tanh(2) should be close to 0.9640 */
    TEST_ASSERT_FLOAT_WITHIN(1e-4, 0.9640, activation(TANH, 2.0));
    return 0;
}

int test_activation_tanh_negative(void) {
    /* tanh(-1) should be close to -0.7616 */
    TEST_ASSERT_FLOAT_WITHIN(1e-4, -0.7616, activation(TANH, -1.0));
    /* tanh is an odd function: tanh(-x) = -tanh(x) */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE,
                              -activation(TANH, 1.0),
                              activation(TANH, -1.0));
    return 0;
}

int test_activation_tanh_large_values(void) {
    /* tanh approaches 1 for large positive values */
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 1.0, activation(TANH, 10.0));
    /* tanh approaches -1 for large negative values */
    TEST_ASSERT_FLOAT_WITHIN(1e-6, -1.0, activation(TANH, -10.0));
    return 0;
}

int test_activation_tanh_bounds(void) {
    /* tanh output is always in (-1, 1) */
    int i;
    for (i = -100; i <= 100; i++) {
        double x = (double)i / 10.0;
        double result = activation(TANH, x);
        TEST_ASSERT_GREATER_THAN(-1.0, result);
        TEST_ASSERT_LESS_THAN(1.0, result);
    }
    return 0;
}

/* ==================== Sigmoid (EXP) Activation Tests ==================== */

int test_activation_exp_zero(void) {
    /* sigmoid(0) = 0.5 */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 0.5, activation(EXP, 0.0));
    return 0;
}

int test_activation_exp_positive(void) {
    /* sigmoid(1) should be close to 0.7311 */
    TEST_ASSERT_FLOAT_WITHIN(1e-4, 0.7311, activation(EXP, 1.0));
    /* sigmoid(2) should be close to 0.8808 */
    TEST_ASSERT_FLOAT_WITHIN(1e-4, 0.8808, activation(EXP, 2.0));
    return 0;
}

int test_activation_exp_negative(void) {
    /* sigmoid(-1) should be close to 0.2689 */
    TEST_ASSERT_FLOAT_WITHIN(1e-4, 0.2689, activation(EXP, -1.0));
    /* sigmoid(-x) = 1 - sigmoid(x) */
    TEST_ASSERT_FLOAT_WITHIN(1e-8,
                              1.0 - activation(EXP, 1.0),
                              activation(EXP, -1.0));
    return 0;
}

int test_activation_exp_large_values(void) {
    /* sigmoid approaches 1 for large positive values */
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 1.0, activation(EXP, 20.0));
    /* sigmoid approaches 0 for large negative values */
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 0.0, activation(EXP, -20.0));
    return 0;
}

int test_activation_exp_bounds(void) {
    /* sigmoid output is always in (0, 1) */
    int i;
    for (i = -100; i <= 100; i++) {
        double x = (double)i / 10.0;
        double result = activation(EXP, x);
        TEST_ASSERT_GREATER_THAN(0.0, result);
        TEST_ASSERT_LESS_THAN(1.0, result);
    }
    return 0;
}

/* ==================== Identity (ID) Activation Tests ==================== */

int test_activation_id_zero(void) {
    /* id(0) = 0 */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 0.0, activation(ID, 0.0));
    return 0;
}

int test_activation_id_positive(void) {
    /* id(x) = x */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 1.0, activation(ID, 1.0));
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 5.5, activation(ID, 5.5));
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 100.0, activation(ID, 100.0));
    return 0;
}

int test_activation_id_negative(void) {
    /* id(x) = x for negative values too */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, -1.0, activation(ID, -1.0));
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, -5.5, activation(ID, -5.5));
    return 0;
}

int test_activation_id_passthrough(void) {
    /* id should be a perfect passthrough */
    double test_values[] = {-100.0, -1.5, -0.001, 0.0, 0.001, 1.5, 100.0, 3.14159};
    int i;
    int n = sizeof(test_values) / sizeof(test_values[0]);

    for (i = 0; i < n; i++) {
        TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, test_values[i], activation(ID, test_values[i]));
    }
    return 0;
}

/* ==================== POL1 Activation Tests ==================== */

int test_activation_pol1_zero(void) {
    /* pol1(0) = 1 + 0 = 1 */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 1.0, activation(POL1, 0.0));
    return 0;
}

int test_activation_pol1_values(void) {
    /* pol1(x) = 1 + x */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 2.0, activation(POL1, 1.0));
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 0.0, activation(POL1, -1.0));
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 6.0, activation(POL1, 5.0));
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, -4.0, activation(POL1, -5.0));
    return 0;
}

int test_activation_pol1_formula(void) {
    /* Verify the formula: 1 + x */
    int i;
    for (i = -50; i <= 50; i++) {
        double x = (double)i / 10.0;
        double expected = 1.0 + x;
        TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, expected, activation(POL1, x));
    }
    return 0;
}

/* ==================== POL2 Activation Tests ==================== */

int test_activation_pol2_zero(void) {
    /* pol2(0) = 1 + 0 + 0 = 1 */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 1.0, activation(POL2, 0.0));
    return 0;
}

int test_activation_pol2_positive(void) {
    /* pol2(1) = 1 + 1 + 1 = 3 */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 3.0, activation(POL2, 1.0));
    /* pol2(2) = 1 + 2 + 4 = 7 */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 7.0, activation(POL2, 2.0));
    return 0;
}

int test_activation_pol2_negative(void) {
    /* pol2(-1) = 1 + (-1) + 1 = 1 */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 1.0, activation(POL2, -1.0));
    /* pol2(-2) = 1 + (-2) + 4 = 3 */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 3.0, activation(POL2, -2.0));
    return 0;
}

int test_activation_pol2_formula(void) {
    /* Verify the formula: 1 + x + x^2 */
    int i;
    for (i = -50; i <= 50; i++) {
        double x = (double)i / 10.0;
        double expected = 1.0 + x + x * x;
        TEST_ASSERT_FLOAT_WITHIN(1e-9, expected, activation(POL2, x));
    }
    return 0;
}

/* ==================== Test Runner ==================== */

void run_activation_tests(void) {
    TEST_SUITE_BEGIN("TANH Activation");
    RUN_TEST(test_activation_tanh_zero);
    RUN_TEST(test_activation_tanh_positive);
    RUN_TEST(test_activation_tanh_negative);
    RUN_TEST(test_activation_tanh_large_values);
    RUN_TEST(test_activation_tanh_bounds);
    TEST_SUITE_END();

    TEST_SUITE_BEGIN("Sigmoid (EXP) Activation");
    RUN_TEST(test_activation_exp_zero);
    RUN_TEST(test_activation_exp_positive);
    RUN_TEST(test_activation_exp_negative);
    RUN_TEST(test_activation_exp_large_values);
    RUN_TEST(test_activation_exp_bounds);
    TEST_SUITE_END();

    TEST_SUITE_BEGIN("Identity (ID) Activation");
    RUN_TEST(test_activation_id_zero);
    RUN_TEST(test_activation_id_positive);
    RUN_TEST(test_activation_id_negative);
    RUN_TEST(test_activation_id_passthrough);
    TEST_SUITE_END();

    TEST_SUITE_BEGIN("POL1 Activation (1+x)");
    RUN_TEST(test_activation_pol1_zero);
    RUN_TEST(test_activation_pol1_values);
    RUN_TEST(test_activation_pol1_formula);
    TEST_SUITE_END();

    TEST_SUITE_BEGIN("POL2 Activation (1+x+x^2)");
    RUN_TEST(test_activation_pol2_zero);
    RUN_TEST(test_activation_pol2_positive);
    RUN_TEST(test_activation_pol2_negative);
    RUN_TEST(test_activation_pol2_formula);
    TEST_SUITE_END();
}

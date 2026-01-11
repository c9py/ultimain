/* test_feedforward.c - Unit tests for feedforward propagation
 *
 * Tests: feedforward.c
 */

#include "test_framework.h"
#include <stdlib.h>
#include <math.h>

/* Include the headers we need to test */
#include "../../include/defines.h"
#include "../../include/network.h"
#include "../../include/feedforward.h"
#include "../../include/activation.h"

/* Tolerance for floating point comparisons */
#define FLOAT_TOLERANCE 1e-10

/* Helper function to create a simple 1-1 network (single input, single output) */
static network* create_simple_1_1_network(void) {
    network *nn = network_alloc();
    network_set_neuron_number(nn, 2);
    network_set_layer_number(nn, 2);

    /* Layer 0 (input): 1 neuron */
    nn->layers[0].num_of_neurons = 1;
    nn->layers[0].neurons = &nn->neurons[0];
    nn->neurons[0].num_input = 1;  /* Input neurons need this set */

    /* Layer 1 (output): 1 neuron connected to input */
    nn->layers[1].num_of_neurons = 1;
    nn->layers[1].neurons = &nn->neurons[1];
    network_neuron_set_connection_number(&nn->neurons[1], 1);
    nn->neurons[1].connection[0] = &nn->neurons[0];
    nn->neurons[1].activation = ID;
    nn->neurons[1].discriminant = LINEAR;

    return nn;
}

/* Helper function to create a 2-1 network */
static network* create_2_1_network(void) {
    network *nn = network_alloc();
    network_set_neuron_number(nn, 3);
    network_set_layer_number(nn, 2);

    /* Layer 0 (input): 2 neurons */
    nn->layers[0].num_of_neurons = 2;
    nn->layers[0].neurons = &nn->neurons[0];
    nn->neurons[0].num_input = 1;
    nn->neurons[1].num_input = 1;

    /* Layer 1 (output): 1 neuron connected to both inputs */
    nn->layers[1].num_of_neurons = 1;
    nn->layers[1].neurons = &nn->neurons[2];
    network_neuron_set_connection_number(&nn->neurons[2], 2);
    nn->neurons[2].connection[0] = &nn->neurons[0];
    nn->neurons[2].connection[1] = &nn->neurons[1];
    nn->neurons[2].activation = ID;
    nn->neurons[2].discriminant = LINEAR;

    return nn;
}

/* Helper function to create a 2-2-1 network */
static network* create_2_2_1_network(void) {
    network *nn = network_alloc();
    network_set_neuron_number(nn, 5);
    network_set_layer_number(nn, 3);

    /* Layer 0 (input): 2 neurons */
    nn->layers[0].num_of_neurons = 2;
    nn->layers[0].neurons = &nn->neurons[0];
    nn->neurons[0].num_input = 1;
    nn->neurons[1].num_input = 1;

    /* Layer 1 (hidden): 2 neurons */
    nn->layers[1].num_of_neurons = 2;
    nn->layers[1].neurons = &nn->neurons[2];

    network_neuron_set_connection_number(&nn->neurons[2], 2);
    nn->neurons[2].connection[0] = &nn->neurons[0];
    nn->neurons[2].connection[1] = &nn->neurons[1];
    nn->neurons[2].activation = ID;
    nn->neurons[2].discriminant = LINEAR;

    network_neuron_set_connection_number(&nn->neurons[3], 2);
    nn->neurons[3].connection[0] = &nn->neurons[0];
    nn->neurons[3].connection[1] = &nn->neurons[1];
    nn->neurons[3].activation = ID;
    nn->neurons[3].discriminant = LINEAR;

    /* Layer 2 (output): 1 neuron */
    nn->layers[2].num_of_neurons = 1;
    nn->layers[2].neurons = &nn->neurons[4];
    network_neuron_set_connection_number(&nn->neurons[4], 2);
    nn->neurons[4].connection[0] = &nn->neurons[2];
    nn->neurons[4].connection[1] = &nn->neurons[3];
    nn->neurons[4].activation = ID;
    nn->neurons[4].discriminant = LINEAR;

    return nn;
}

/* ==================== Basic Feedforward Tests ==================== */

int test_feedforward_single_neuron_identity(void) {
    network *nn = create_simple_1_1_network();

    /* Set input and weight */
    nn->neurons[0].output = 5.0;
    nn->neurons[1].w[0] = 1.0;

    feedforward(nn);

    /* With identity activation and weight=1, output should equal input */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 5.0, nn->neurons[1].output);

    network_free(nn);
    return 0;
}

int test_feedforward_single_neuron_with_weight(void) {
    network *nn = create_simple_1_1_network();

    /* Set input and weight */
    nn->neurons[0].output = 5.0;
    nn->neurons[1].w[0] = 2.0;

    feedforward(nn);

    /* output = activation(input * weight) = 5 * 2 = 10 */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 10.0, nn->neurons[1].output);

    network_free(nn);
    return 0;
}

int test_feedforward_two_inputs_sum(void) {
    network *nn = create_2_1_network();

    /* Set inputs and weights */
    nn->neurons[0].output = 3.0;
    nn->neurons[1].output = 4.0;
    nn->neurons[2].w[0] = 1.0;
    nn->neurons[2].w[1] = 1.0;

    feedforward(nn);

    /* output = activation(3*1 + 4*1) = 7 */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 7.0, nn->neurons[2].output);

    network_free(nn);
    return 0;
}

int test_feedforward_two_inputs_weighted(void) {
    network *nn = create_2_1_network();

    /* Set inputs and weights */
    nn->neurons[0].output = 2.0;
    nn->neurons[1].output = 3.0;
    nn->neurons[2].w[0] = 0.5;
    nn->neurons[2].w[1] = 2.0;

    feedforward(nn);

    /* output = activation(2*0.5 + 3*2) = 1 + 6 = 7 */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 7.0, nn->neurons[2].output);

    network_free(nn);
    return 0;
}

/* ==================== Multi-layer Feedforward Tests ==================== */

int test_feedforward_two_layer_network(void) {
    network *nn = create_2_2_1_network();

    /* Set inputs */
    nn->neurons[0].output = 1.0;
    nn->neurons[1].output = 2.0;

    /* Set weights for hidden layer */
    nn->neurons[2].w[0] = 1.0;
    nn->neurons[2].w[1] = 1.0;
    nn->neurons[3].w[0] = 1.0;
    nn->neurons[3].w[1] = -1.0;

    /* Set weights for output layer */
    nn->neurons[4].w[0] = 1.0;
    nn->neurons[4].w[1] = 1.0;

    feedforward(nn);

    /* Hidden layer outputs:
     * neuron[2] = 1*1 + 2*1 = 3
     * neuron[3] = 1*1 + 2*(-1) = -1
     * Output:
     * neuron[4] = 3*1 + (-1)*1 = 2
     */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 3.0, nn->neurons[2].output);
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, -1.0, nn->neurons[3].output);
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 2.0, nn->neurons[4].output);

    network_free(nn);
    return 0;
}

/* ==================== Activation Function Tests in Feedforward ==================== */

int test_feedforward_with_tanh_activation(void) {
    network *nn = create_simple_1_1_network();
    nn->neurons[1].activation = TANH;

    nn->neurons[0].output = 0.0;
    nn->neurons[1].w[0] = 1.0;

    feedforward(nn);

    /* tanh(0) = 0 */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 0.0, nn->neurons[1].output);

    network_free(nn);
    return 0;
}

int test_feedforward_with_sigmoid_activation(void) {
    network *nn = create_simple_1_1_network();
    nn->neurons[1].activation = EXP;

    nn->neurons[0].output = 0.0;
    nn->neurons[1].w[0] = 1.0;

    feedforward(nn);

    /* sigmoid(0) = 0.5 */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 0.5, nn->neurons[1].output);

    network_free(nn);
    return 0;
}

int test_feedforward_with_pol1_activation(void) {
    network *nn = create_simple_1_1_network();
    nn->neurons[1].activation = POL1;

    nn->neurons[0].output = 5.0;
    nn->neurons[1].w[0] = 1.0;

    feedforward(nn);

    /* pol1(5) = 1 + 5 = 6 */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 6.0, nn->neurons[1].output);

    network_free(nn);
    return 0;
}

int test_feedforward_with_pol2_activation(void) {
    network *nn = create_simple_1_1_network();
    nn->neurons[1].activation = POL2;

    nn->neurons[0].output = 2.0;
    nn->neurons[1].w[0] = 1.0;

    feedforward(nn);

    /* pol2(2) = 1 + 2 + 4 = 7 */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 7.0, nn->neurons[1].output);

    network_free(nn);
    return 0;
}

/* ==================== Zero and Negative Weight Tests ==================== */

int test_feedforward_zero_weights(void) {
    network *nn = create_2_1_network();

    nn->neurons[0].output = 100.0;
    nn->neurons[1].output = 200.0;
    nn->neurons[2].w[0] = 0.0;
    nn->neurons[2].w[1] = 0.0;

    feedforward(nn);

    /* output = 100*0 + 200*0 = 0 */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, 0.0, nn->neurons[2].output);

    network_free(nn);
    return 0;
}

int test_feedforward_negative_weights(void) {
    network *nn = create_2_1_network();

    nn->neurons[0].output = 5.0;
    nn->neurons[1].output = 3.0;
    nn->neurons[2].w[0] = -2.0;
    nn->neurons[2].w[1] = 1.0;

    feedforward(nn);

    /* output = 5*(-2) + 3*1 = -10 + 3 = -7 */
    TEST_ASSERT_FLOAT_WITHIN(FLOAT_TOLERANCE, -7.0, nn->neurons[2].output);

    network_free(nn);
    return 0;
}

/* ==================== Test Runner ==================== */

void run_feedforward_tests(void) {
    TEST_SUITE_BEGIN("Basic Feedforward");
    RUN_TEST(test_feedforward_single_neuron_identity);
    RUN_TEST(test_feedforward_single_neuron_with_weight);
    RUN_TEST(test_feedforward_two_inputs_sum);
    RUN_TEST(test_feedforward_two_inputs_weighted);
    TEST_SUITE_END();

    TEST_SUITE_BEGIN("Multi-layer Feedforward");
    RUN_TEST(test_feedforward_two_layer_network);
    TEST_SUITE_END();

    TEST_SUITE_BEGIN("Feedforward with Activation Functions");
    RUN_TEST(test_feedforward_with_tanh_activation);
    RUN_TEST(test_feedforward_with_sigmoid_activation);
    RUN_TEST(test_feedforward_with_pol1_activation);
    RUN_TEST(test_feedforward_with_pol2_activation);
    TEST_SUITE_END();

    TEST_SUITE_BEGIN("Feedforward Edge Cases");
    RUN_TEST(test_feedforward_zero_weights);
    RUN_TEST(test_feedforward_negative_weights);
    TEST_SUITE_END();
}

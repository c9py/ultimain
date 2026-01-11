/* test_network.c - Unit tests for network operations
 *
 * Tests: network.c
 */

#include "test_framework.h"
#include <stdlib.h>

/* Include the headers we need to test */
#include "../../include/defines.h"
#include "../../include/network.h"

/* ==================== Network Allocation Tests ==================== */

int test_network_alloc_returns_non_null(void) {
    network *nn = network_alloc();
    TEST_ASSERT_NOT_NULL(nn);
    network_free(nn);
    return 0;
}

int test_network_alloc_initializes_to_zero(void) {
    network *nn = network_alloc();
    TEST_ASSERT_NOT_NULL(nn);
    TEST_ASSERT_EQUAL(0, nn->num_of_neurons);
    TEST_ASSERT_EQUAL(0, nn->num_of_layers);
    TEST_ASSERT_NULL(nn->neurons);
    TEST_ASSERT_NULL(nn->layers);
    network_free(nn);
    return 0;
}

int test_network_free_null_safe(void) {
    /* Should not crash on NULL */
    network_free(NULL);
    return 0;
}

/* ==================== Network Neuron Number Tests ==================== */

int test_network_set_neuron_number_basic(void) {
    network *nn = network_alloc();
    int result = network_set_neuron_number(nn, 5);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(5, nn->num_of_neurons);
    TEST_ASSERT_NOT_NULL(nn->neurons);
    network_free(nn);
    return 0;
}

int test_network_set_neuron_number_initializes_neurons(void) {
    network *nn = network_alloc();
    network_set_neuron_number(nn, 3);

    /* Check that neurons have global_id set correctly */
    int i;
    for (i = 0; i < 3; i++) {
        TEST_ASSERT_EQUAL(i, nn->neurons[i].global_id);
        TEST_ASSERT_FLOAT_WITHIN(1e-10, 0.0, nn->neurons[i].output);
    }
    network_free(nn);
    return 0;
}

int test_network_set_neuron_number_null_network(void) {
    int result = network_set_neuron_number(NULL, 5);
    TEST_ASSERT_EQUAL(-1, result);
    return 0;
}

int test_network_set_neuron_number_zero(void) {
    network *nn = network_alloc();
    int result = network_set_neuron_number(nn, 0);
    TEST_ASSERT_EQUAL(-1, result);
    network_free(nn);
    return 0;
}

int test_network_set_neuron_number_same_value_noop(void) {
    network *nn = network_alloc();
    network_set_neuron_number(nn, 5);
    neuron *original_ptr = nn->neurons;

    /* Setting same number should be a no-op */
    int result = network_set_neuron_number(nn, 5);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT(nn->neurons == original_ptr);
    network_free(nn);
    return 0;
}

/* ==================== Network Layer Number Tests ==================== */

int test_network_set_layer_number_basic(void) {
    network *nn = network_alloc();
    int result = network_set_layer_number(nn, 3);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(3, nn->num_of_layers);
    TEST_ASSERT_NOT_NULL(nn->layers);
    network_free(nn);
    return 0;
}

int test_network_set_layer_number_null_network(void) {
    int result = network_set_layer_number(NULL, 3);
    TEST_ASSERT_EQUAL(-1, result);
    return 0;
}

int test_network_set_layer_number_zero(void) {
    network *nn = network_alloc();
    int result = network_set_layer_number(nn, 0);
    TEST_ASSERT_EQUAL(-1, result);
    network_free(nn);
    return 0;
}

int test_network_set_layer_number_same_value_noop(void) {
    network *nn = network_alloc();
    network_set_layer_number(nn, 3);
    layer *original_ptr = nn->layers;

    /* Setting same number should be a no-op */
    int result = network_set_layer_number(nn, 3);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT(nn->layers == original_ptr);
    network_free(nn);
    return 0;
}

/* ==================== Neuron Connection Tests ==================== */

int test_network_neuron_set_connection_number_basic(void) {
    network *nn = network_alloc();
    network_set_neuron_number(nn, 2);

    int result = network_neuron_set_connection_number(&nn->neurons[1], 3);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(3, nn->neurons[1].num_input);
    TEST_ASSERT_NOT_NULL(nn->neurons[1].connection);
    TEST_ASSERT_NOT_NULL(nn->neurons[1].w);
    network_free(nn);
    return 0;
}

int test_network_neuron_set_connection_number_initializes_weights(void) {
    network *nn = network_alloc();
    network_set_neuron_number(nn, 2);
    network_neuron_set_connection_number(&nn->neurons[1], 3);

    /* Weights should be initialized to 0 */
    int i;
    for (i = 0; i < 3; i++) {
        TEST_ASSERT_FLOAT_WITHIN(1e-10, 0.0, nn->neurons[1].w[i]);
    }
    network_free(nn);
    return 0;
}

int test_network_neuron_set_connection_number_null_neuron(void) {
    int result = network_neuron_set_connection_number(NULL, 3);
    TEST_ASSERT_EQUAL(-1, result);
    return 0;
}

int test_network_neuron_set_connection_number_zero(void) {
    network *nn = network_alloc();
    network_set_neuron_number(nn, 2);

    int result = network_neuron_set_connection_number(&nn->neurons[1], 0);
    TEST_ASSERT_EQUAL(-1, result);
    network_free(nn);
    return 0;
}

/* ==================== Network Config Tests ==================== */

int test_network_config_alloc_returns_non_null(void) {
    network_config *config = network_config_alloc();
    TEST_ASSERT_NOT_NULL(config);
    network_config_free(config);
    return 0;
}

int test_network_config_alloc_default_sets_defaults(void) {
    network_config *config = network_config_alloc_default();
    TEST_ASSERT_NOT_NULL(config);
    TEST_ASSERT_EQUAL(OFF, config->save_output);
    TEST_ASSERT_EQUAL(OFF, config->load_neural_network);
    TEST_ASSERT_EQUAL(OFF, config->save_neural_network);
    TEST_ASSERT_EQUAL(ON, config->initial_weights_randomization);
    TEST_ASSERT_EQUAL(L2, config->error_type);
    network_config_free(config);
    return 0;
}

int test_network_config_free_null_safe(void) {
    /* Should not crash on NULL */
    network_config_free(NULL);
    return 0;
}

/* ==================== Complex Network Setup Tests ==================== */

int test_network_simple_topology(void) {
    /* Create a simple 2-2-1 network (XOR-like) */
    network *nn = network_alloc();
    TEST_ASSERT_NOT_NULL(nn);

    /* Set up 5 neurons total */
    network_set_neuron_number(nn, 5);
    TEST_ASSERT_EQUAL(5, nn->num_of_neurons);

    /* Set up 3 layers */
    network_set_layer_number(nn, 3);
    TEST_ASSERT_EQUAL(3, nn->num_of_layers);

    /* Configure layer 0 (input): 2 neurons */
    nn->layers[0].num_of_neurons = 2;
    nn->layers[0].neurons = &nn->neurons[0];

    /* Configure layer 1 (hidden): 2 neurons */
    nn->layers[1].num_of_neurons = 2;
    nn->layers[1].neurons = &nn->neurons[2];

    /* Configure layer 2 (output): 1 neuron */
    nn->layers[2].num_of_neurons = 1;
    nn->layers[2].neurons = &nn->neurons[4];

    /* Set up connections for hidden layer neurons */
    network_neuron_set_connection_number(&nn->neurons[2], 2);
    network_neuron_set_connection_number(&nn->neurons[3], 2);
    nn->neurons[2].connection[0] = &nn->neurons[0];
    nn->neurons[2].connection[1] = &nn->neurons[1];
    nn->neurons[3].connection[0] = &nn->neurons[0];
    nn->neurons[3].connection[1] = &nn->neurons[1];

    /* Set up connections for output layer neuron */
    network_neuron_set_connection_number(&nn->neurons[4], 2);
    nn->neurons[4].connection[0] = &nn->neurons[2];
    nn->neurons[4].connection[1] = &nn->neurons[3];

    /* Verify structure */
    TEST_ASSERT_EQUAL(2, nn->layers[0].num_of_neurons);
    TEST_ASSERT_EQUAL(2, nn->layers[1].num_of_neurons);
    TEST_ASSERT_EQUAL(1, nn->layers[2].num_of_neurons);
    TEST_ASSERT_EQUAL(2, nn->neurons[4].num_input);

    network_free(nn);
    return 0;
}

int test_network_max_neurons(void) {
    /* Test with MAX_NUM_NEURONS */
    network *nn = network_alloc();
    int result = network_set_neuron_number(nn, MAX_NUM_NEURONS);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(MAX_NUM_NEURONS, nn->num_of_neurons);
    network_free(nn);
    return 0;
}

int test_network_max_layers(void) {
    /* Test with MAX_NUM_LAYERS */
    network *nn = network_alloc();
    int result = network_set_layer_number(nn, MAX_NUM_LAYERS);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(MAX_NUM_LAYERS, nn->num_of_layers);
    network_free(nn);
    return 0;
}

/* ==================== Test Runner ==================== */

void run_network_tests(void) {
    TEST_SUITE_BEGIN("Network Allocation");
    RUN_TEST(test_network_alloc_returns_non_null);
    RUN_TEST(test_network_alloc_initializes_to_zero);
    RUN_TEST(test_network_free_null_safe);
    TEST_SUITE_END();

    TEST_SUITE_BEGIN("Network Neuron Number");
    RUN_TEST(test_network_set_neuron_number_basic);
    RUN_TEST(test_network_set_neuron_number_initializes_neurons);
    RUN_TEST(test_network_set_neuron_number_null_network);
    RUN_TEST(test_network_set_neuron_number_zero);
    RUN_TEST(test_network_set_neuron_number_same_value_noop);
    TEST_SUITE_END();

    TEST_SUITE_BEGIN("Network Layer Number");
    RUN_TEST(test_network_set_layer_number_basic);
    RUN_TEST(test_network_set_layer_number_null_network);
    RUN_TEST(test_network_set_layer_number_zero);
    RUN_TEST(test_network_set_layer_number_same_value_noop);
    TEST_SUITE_END();

    TEST_SUITE_BEGIN("Neuron Connections");
    RUN_TEST(test_network_neuron_set_connection_number_basic);
    RUN_TEST(test_network_neuron_set_connection_number_initializes_weights);
    RUN_TEST(test_network_neuron_set_connection_number_null_neuron);
    RUN_TEST(test_network_neuron_set_connection_number_zero);
    TEST_SUITE_END();

    TEST_SUITE_BEGIN("Network Config");
    RUN_TEST(test_network_config_alloc_returns_non_null);
    RUN_TEST(test_network_config_alloc_default_sets_defaults);
    RUN_TEST(test_network_config_free_null_safe);
    TEST_SUITE_END();

    TEST_SUITE_BEGIN("Complex Network Setup");
    RUN_TEST(test_network_simple_topology);
    RUN_TEST(test_network_max_neurons);
    RUN_TEST(test_network_max_layers);
    TEST_SUITE_END();
}

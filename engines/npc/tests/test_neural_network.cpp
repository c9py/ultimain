/*
 * test_neural_network.cpp - Tests for NPC Neural Network module
 */

#include "test_framework.h"
#include "neural/NeuralNetwork.h"
#include <cmath>

using namespace Ultima::NPC::Neural;

// Test layer configuration
bool test_layer_config() {
    LayerConfig config;
    config.numNeurons = 4;
    config.activation = ActivationFunction::Tanh;
    config.discriminant = DiscriminantFunction::Linear;

    TEST_ASSERT_EQUAL(4, config.numNeurons);
    TEST_ASSERT(config.activation == ActivationFunction::Tanh);
    TEST_ASSERT(config.discriminant == DiscriminantFunction::Linear);
    return true;
}

// Test training config defaults
bool test_training_config() {
    TrainingConfig config;

    TEST_ASSERT(config.method == OptimizationMethod::GradientDescent);
    TEST_ASSERT(config.errorFunction == ErrorFunction::L2);
    TEST_ASSERT_FLOAT_NEAR(0.01, config.learningRate, 0.001);
    TEST_ASSERT_EQUAL(1000, config.maxIterations);
    TEST_ASSERT(config.randomizeInitialWeights);
    return true;
}

// Test neural network creation
bool test_network_creation() {
    NeuralNetwork network;

    LayerConfig layer1;
    layer1.numNeurons = 4;
    layer1.activation = ActivationFunction::Tanh;

    LayerConfig layer2;
    layer2.numNeurons = 2;
    layer2.activation = ActivationFunction::Tanh;

    network.addLayer(layer1);
    network.addLayer(layer2);
    network.build();

    TEST_ASSERT_EQUAL(2, network.getNumLayers());
    return true;
}

// Test network prediction dimensions
bool test_network_prediction() {
    NeuralNetwork network;

    LayerConfig layer1;
    layer1.numNeurons = 4;
    network.addLayer(layer1);

    LayerConfig layer2;
    layer2.numNeurons = 2;
    network.addLayer(layer2);

    network.build();

    std::vector<double> input = {0.5, 0.3, 0.7, 0.1};
    std::vector<double> output = network.predict(input);

    // Output should have 2 values (from layer2)
    TEST_ASSERT_EQUAL(2, output.size());

    // Outputs should be in valid range for tanh (-1 to 1)
    for (const auto& val : output) {
        TEST_ASSERT(val >= -1.0 && val <= 1.0);
    }

    return true;
}

// Test training point structure
bool test_training_point() {
    TrainingPoint point;
    point.inputs = {0.1, 0.2, 0.3};
    point.expectedOutputs = {0.5, 0.5};

    TEST_ASSERT_EQUAL(3, point.inputs.size());
    TEST_ASSERT_EQUAL(2, point.expectedOutputs.size());
    return true;
}

// Test network clear
bool test_network_clear() {
    NeuralNetwork network;

    LayerConfig layer;
    layer.numNeurons = 4;
    network.addLayer(layer);
    network.build();

    TEST_ASSERT_EQUAL(1, network.getNumLayers());

    network.clear();
    TEST_ASSERT_EQUAL(0, network.getNumLayers());

    return true;
}

// Test NPC learning network creation
bool test_npc_learning_network() {
    NPCLearningNetwork network(NPCLearningNetwork::LearningContext::DecisionMaking);

    TEST_ASSERT_EQUAL(0, network.getExperienceCount());
    TEST_ASSERT_FLOAT_NEAR(0.0, network.getAverageReward(), 0.001);

    return true;
}

// Test NPC learning network context types
bool test_learning_contexts() {
    // Test all context types can be created
    NPCLearningNetwork decision(NPCLearningNetwork::LearningContext::DecisionMaking);
    NPCLearningNetwork social(NPCLearningNetwork::LearningContext::SocialBehavior);
    NPCLearningNetwork combat(NPCLearningNetwork::LearningContext::CombatTactics);
    NPCLearningNetwork economic(NPCLearningNetwork::LearningContext::EconomicDecisions);
    NPCLearningNetwork emotional(NPCLearningNetwork::LearningContext::EmotionalResponse);

    // All should start with zero experience
    TEST_ASSERT_EQUAL(0, decision.getExperienceCount());
    TEST_ASSERT_EQUAL(0, social.getExperienceCount());
    TEST_ASSERT_EQUAL(0, combat.getExperienceCount());
    TEST_ASSERT_EQUAL(0, economic.getExperienceCount());
    TEST_ASSERT_EQUAL(0, emotional.getExperienceCount());

    return true;
}

// Test NPC learning from experience
bool test_npc_learning() {
    NPCLearningNetwork network(NPCLearningNetwork::LearningContext::DecisionMaking);
    network.setNumActions(4);

    std::vector<double> situation = {0.5, 0.3, 0.8, 0.2};

    // Learn from positive experience
    network.learnFromExperience(situation, 0, 1.0);
    TEST_ASSERT_EQUAL(1, network.getExperienceCount());

    // Learn from negative experience
    network.learnFromExperience(situation, 1, -0.5);
    TEST_ASSERT_EQUAL(2, network.getExperienceCount());

    return true;
}

// Test action prediction
bool test_action_prediction() {
    NPCLearningNetwork network(NPCLearningNetwork::LearningContext::DecisionMaking);
    network.setNumActions(4);

    std::vector<double> situation = {0.5, 0.3, 0.8, 0.2};

    // Get predicted action
    uint32_t action = network.predictBestAction(situation);
    TEST_ASSERT(action < 4);  // Action should be valid

    // Get action probabilities
    std::vector<double> probs = network.getActionProbabilities(situation);
    TEST_ASSERT_EQUAL(4, probs.size());

    // Probabilities should sum to approximately 1
    double sum = 0.0;
    for (const auto& p : probs) {
        sum += p;
        TEST_ASSERT(p >= 0.0);  // No negative probabilities
    }
    TEST_ASSERT_FLOAT_NEAR(1.0, sum, 0.1);

    return true;
}

// Test activation function enumeration
bool test_activation_functions() {
    // Test all activation types are distinct
    TEST_ASSERT(ActivationFunction::Tanh != ActivationFunction::Exp);
    TEST_ASSERT(ActivationFunction::Exp != ActivationFunction::Identity);
    TEST_ASSERT(ActivationFunction::Identity != ActivationFunction::Polynomial1);
    TEST_ASSERT(ActivationFunction::Polynomial1 != ActivationFunction::Polynomial2);
    return true;
}

// Test optimization methods
bool test_optimization_methods() {
    // Test all optimization methods are distinct
    TEST_ASSERT(OptimizationMethod::SimulatedAnnealing != OptimizationMethod::RandomSearch);
    TEST_ASSERT(OptimizationMethod::RandomSearch != OptimizationMethod::GradientDescent);
    TEST_ASSERT(OptimizationMethod::GradientDescent != OptimizationMethod::GeneticAlgorithm);
    TEST_ASSERT(OptimizationMethod::GeneticAlgorithm != OptimizationMethod::MSMCO);
    return true;
}

int main() {
    TEST_SUITE("Neural Network");

    RUN_TEST("Layer configuration", test_layer_config);
    RUN_TEST("Training configuration", test_training_config);
    RUN_TEST("Network creation", test_network_creation);
    RUN_TEST("Network prediction", test_network_prediction);
    RUN_TEST("Training point", test_training_point);
    RUN_TEST("Network clear", test_network_clear);
    RUN_TEST("NPC learning network", test_npc_learning_network);
    RUN_TEST("Learning contexts", test_learning_contexts);
    RUN_TEST("NPC learning", test_npc_learning);
    RUN_TEST("Action prediction", test_action_prediction);
    RUN_TEST("Activation functions", test_activation_functions);
    RUN_TEST("Optimization methods", test_optimization_methods);

    TEST_SUMMARY();
}

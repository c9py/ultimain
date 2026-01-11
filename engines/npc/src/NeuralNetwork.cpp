/**
 * NeuralNetwork.cpp - C++ Wrapper for GNeural-Net Integration
 *
 * Implementation of lightweight neural network for NPC learning.
 */

#include "neural/NeuralNetwork.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace Ultima {
namespace NPC {
namespace Neural {

// Random number generator
static std::mt19937 rng(std::random_device{}());

//=============================================================================
// Layer Implementation
//=============================================================================

Layer::Layer(const LayerConfig& config)
    : numNeurons_(config.numNeurons)
    , numInputs_(0)
    , activation_(config.activation)
{
}

Layer::~Layer() = default;

double Layer::activate(double x) const {
    switch (activation_) {
        case ActivationFunction::Tanh:
            return std::tanh(x);
        case ActivationFunction::Exp:
            return 1.0 / (1.0 + std::exp(-x)); // Sigmoid
        case ActivationFunction::Identity:
            return x;
        case ActivationFunction::Polynomial1:
            return x;
        case ActivationFunction::Polynomial2:
            return x * x;
        default:
            return std::tanh(x);
    }
}

std::vector<double> Layer::forward(const std::vector<double>& inputs) {
    if (numInputs_ == 0) {
        numInputs_ = static_cast<uint32_t>(inputs.size());
        // Initialize weights if not set
        if (weights_.empty()) {
            std::uniform_real_distribution<double> dist(-1.0, 1.0);
            weights_.resize(numNeurons_);
            biases_.resize(numNeurons_);
            for (uint32_t i = 0; i < numNeurons_; ++i) {
                weights_[i].resize(numInputs_);
                for (uint32_t j = 0; j < numInputs_; ++j) {
                    weights_[i][j] = dist(rng);
                }
                biases_[i] = dist(rng) * 0.1;
            }
        }
    }

    std::vector<double> outputs(numNeurons_);
    for (uint32_t i = 0; i < numNeurons_; ++i) {
        double sum = biases_[i];
        for (uint32_t j = 0; j < numInputs_ && j < inputs.size(); ++j) {
            sum += inputs[j] * weights_[i][j];
        }
        outputs[i] = activate(sum);
    }
    return outputs;
}

void Layer::setWeights(const std::vector<std::vector<double>>& weights) {
    weights_ = weights;
    if (!weights_.empty()) {
        numNeurons_ = static_cast<uint32_t>(weights_.size());
        numInputs_ = static_cast<uint32_t>(weights_[0].size());
    }
}

std::vector<std::vector<double>> Layer::getWeights() const {
    return weights_;
}

//=============================================================================
// NeuralNetwork Implementation
//=============================================================================

NeuralNetwork::NeuralNetwork() = default;
NeuralNetwork::~NeuralNetwork() = default;

void NeuralNetwork::addLayer(const LayerConfig& config) {
    if (isBuilt_) {
        throw std::runtime_error("Cannot add layers to built network");
    }
    layerConfigs_.push_back(config);
}

void NeuralNetwork::build() {
    if (layerConfigs_.empty()) {
        throw std::runtime_error("No layers configured");
    }

    layers_.clear();
    for (const auto& config : layerConfigs_) {
        layers_.push_back(std::make_unique<Layer>(config));
    }
    isBuilt_ = true;
}

std::vector<double> NeuralNetwork::predict(const std::vector<double>& inputs) {
    if (!isBuilt_) {
        throw std::runtime_error("Network not built");
    }

    std::vector<double> current = inputs;
    for (auto& layer : layers_) {
        current = layer->forward(current);
    }
    return current;
}

void NeuralNetwork::randomizeWeights(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    for (auto& layer : layers_) {
        auto weights = layer->getWeights();
        for (auto& neuronWeights : weights) {
            for (auto& w : neuronWeights) {
                w = dist(rng);
            }
        }
        layer->setWeights(weights);
    }
}

double NeuralNetwork::computeError(const std::vector<TrainingPoint>& data,
                                   ErrorFunction errorFunc) {
    double totalError = 0.0;
    for (const auto& point : data) {
        auto output = predict(point.inputs);
        for (size_t i = 0; i < output.size() && i < point.expectedOutputs.size(); ++i) {
            double diff = output[i] - point.expectedOutputs[i];
            if (errorFunc == ErrorFunction::L2) {
                totalError += diff * diff;
            } else {
                totalError += std::abs(diff);
            }
        }
    }
    return totalError / static_cast<double>(data.size());
}

void NeuralNetwork::train(const std::vector<TrainingPoint>& data,
                          const TrainingConfig& config) {
    if (!isBuilt_) {
        build();
    }

    if (config.randomizeInitialWeights) {
        randomizeWeights(config.weightMin, config.weightMax);
    }

    switch (config.method) {
        case OptimizationMethod::GradientDescent:
            trainGradientDescent(data, config);
            break;
        case OptimizationMethod::SimulatedAnnealing:
            trainSimulatedAnnealing(data, config);
            break;
        case OptimizationMethod::GeneticAlgorithm:
            trainGeneticAlgorithm(data, config);
            break;
        case OptimizationMethod::RandomSearch:
            trainRandomSearch(data, config);
            break;
        case OptimizationMethod::MSMCO:
            // Fall back to simulated annealing for now
            trainSimulatedAnnealing(data, config);
            break;
    }

    lastError_ = computeError(data, config.errorFunction);
}

void NeuralNetwork::trainGradientDescent(const std::vector<TrainingPoint>& data,
                                         const TrainingConfig& config) {
    const double epsilon = 0.0001;

    for (int iter = 0; iter < config.maxIterations; ++iter) {
        double error = computeError(data, config.errorFunction);
        if (error < config.accuracy) break;

        // Numerical gradient descent
        for (auto& layer : layers_) {
            auto weights = layer->getWeights();
            for (size_t i = 0; i < weights.size(); ++i) {
                for (size_t j = 0; j < weights[i].size(); ++j) {
                    double original = weights[i][j];

                    // Compute gradient numerically
                    weights[i][j] = original + epsilon;
                    layer->setWeights(weights);
                    double errorPlus = computeError(data, config.errorFunction);

                    weights[i][j] = original - epsilon;
                    layer->setWeights(weights);
                    double errorMinus = computeError(data, config.errorFunction);

                    double gradient = (errorPlus - errorMinus) / (2.0 * epsilon);

                    // Update weight
                    weights[i][j] = original - config.learningRate * gradient;
                }
            }
            layer->setWeights(weights);
        }
    }
}

void NeuralNetwork::trainSimulatedAnnealing(const std::vector<TrainingPoint>& data,
                                            const TrainingConfig& config) {
    double temperature = config.initialTemperature;
    double bestError = computeError(data, config.errorFunction);

    std::uniform_real_distribution<double> probDist(0.0, 1.0);
    std::uniform_real_distribution<double> perturbDist(-0.5, 0.5);

    for (int iter = 0; iter < config.maxIterations && temperature > config.minTemperature; ++iter) {
        // Perturb weights
        for (auto& layer : layers_) {
            auto weights = layer->getWeights();
            for (auto& neuronWeights : weights) {
                for (auto& w : neuronWeights) {
                    w += perturbDist(rng) * temperature;
                    w = std::clamp(w, config.weightMin, config.weightMax);
                }
            }
            layer->setWeights(weights);
        }

        double newError = computeError(data, config.errorFunction);
        double delta = newError - bestError;

        // Accept or reject
        if (delta < 0 || probDist(rng) < std::exp(-delta / temperature)) {
            bestError = newError;
        }

        // Cool down
        temperature *= config.coolingRate;

        if (bestError < config.accuracy) break;
    }
}

void NeuralNetwork::trainGeneticAlgorithm(const std::vector<TrainingPoint>& data,
                                          const TrainingConfig& config) {
    // Simplified genetic algorithm
    struct Individual {
        std::vector<std::vector<std::vector<double>>> weights;
        double fitness = 0.0;
    };

    std::vector<Individual> population(config.populationSize);

    // Initialize population
    std::uniform_real_distribution<double> weightDist(config.weightMin, config.weightMax);
    for (auto& ind : population) {
        ind.weights.resize(layers_.size());
        for (size_t l = 0; l < layers_.size(); ++l) {
            auto layerWeights = layers_[l]->getWeights();
            ind.weights[l] = layerWeights;
            for (auto& neuronWeights : ind.weights[l]) {
                for (auto& w : neuronWeights) {
                    w = weightDist(rng);
                }
            }
        }
    }

    std::uniform_real_distribution<double> probDist(0.0, 1.0);

    for (int gen = 0; gen < config.maxIterations; ++gen) {
        // Evaluate fitness
        for (auto& ind : population) {
            for (size_t l = 0; l < layers_.size(); ++l) {
                layers_[l]->setWeights(ind.weights[l]);
            }
            double error = computeError(data, config.errorFunction);
            ind.fitness = 1.0 / (1.0 + error);
        }

        // Sort by fitness
        std::sort(population.begin(), population.end(),
                  [](const Individual& a, const Individual& b) {
                      return a.fitness > b.fitness;
                  });

        if (1.0 / population[0].fitness - 1.0 < config.accuracy) break;

        // Selection and reproduction
        std::vector<Individual> newPop;
        newPop.push_back(population[0]); // Elitism

        while (newPop.size() < population.size()) {
            // Tournament selection
            size_t p1 = std::min(rng() % 5, population.size() - 1);
            size_t p2 = std::min(rng() % 5, population.size() - 1);

            Individual child;
            child.weights.resize(layers_.size());

            // Crossover
            for (size_t l = 0; l < layers_.size(); ++l) {
                child.weights[l] = (probDist(rng) < 0.5) ?
                    population[p1].weights[l] : population[p2].weights[l];

                // Mutation
                for (auto& neuronWeights : child.weights[l]) {
                    for (auto& w : neuronWeights) {
                        if (probDist(rng) < config.mutationRate) {
                            w += (probDist(rng) - 0.5) * 0.2;
                            w = std::clamp(w, config.weightMin, config.weightMax);
                        }
                    }
                }
            }
            newPop.push_back(child);
        }
        population = std::move(newPop);
    }

    // Apply best weights
    for (size_t l = 0; l < layers_.size(); ++l) {
        layers_[l]->setWeights(population[0].weights[l]);
    }
}

void NeuralNetwork::trainRandomSearch(const std::vector<TrainingPoint>& data,
                                      const TrainingConfig& config) {
    double bestError = computeError(data, config.errorFunction);
    std::vector<std::vector<std::vector<double>>> bestWeights;

    for (auto& layer : layers_) {
        bestWeights.push_back(layer->getWeights());
    }

    std::uniform_real_distribution<double> dist(config.weightMin, config.weightMax);

    for (int iter = 0; iter < config.maxIterations; ++iter) {
        // Random weights
        for (auto& layer : layers_) {
            auto weights = layer->getWeights();
            for (auto& neuronWeights : weights) {
                for (auto& w : neuronWeights) {
                    w = dist(rng);
                }
            }
            layer->setWeights(weights);
        }

        double error = computeError(data, config.errorFunction);
        if (error < bestError) {
            bestError = error;
            bestWeights.clear();
            for (auto& layer : layers_) {
                bestWeights.push_back(layer->getWeights());
            }
        }

        if (bestError < config.accuracy) break;
    }

    // Restore best weights
    for (size_t l = 0; l < layers_.size(); ++l) {
        layers_[l]->setWeights(bestWeights[l]);
    }
}

bool NeuralNetwork::save(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) return false;

    file << layers_.size() << "\n";
    for (const auto& layer : layers_) {
        auto weights = layer->getWeights();
        file << weights.size() << " ";
        if (!weights.empty()) {
            file << weights[0].size() << "\n";
            for (const auto& neuronWeights : weights) {
                for (double w : neuronWeights) {
                    file << w << " ";
                }
                file << "\n";
            }
        } else {
            file << "0\n";
        }
    }
    return true;
}

bool NeuralNetwork::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) return false;

    size_t numLayers;
    file >> numLayers;

    layers_.clear();
    layerConfigs_.clear();

    for (size_t l = 0; l < numLayers; ++l) {
        size_t numNeurons, numInputs;
        file >> numNeurons >> numInputs;

        LayerConfig config;
        config.numNeurons = static_cast<uint32_t>(numNeurons);
        layerConfigs_.push_back(config);

        auto layer = std::make_unique<Layer>(config);
        std::vector<std::vector<double>> weights(numNeurons);

        for (size_t i = 0; i < numNeurons; ++i) {
            weights[i].resize(numInputs);
            for (size_t j = 0; j < numInputs; ++j) {
                file >> weights[i][j];
            }
        }
        layer->setWeights(weights);
        layers_.push_back(std::move(layer));
    }

    isBuilt_ = true;
    return true;
}

uint32_t NeuralNetwork::getNumNeurons() const {
    uint32_t total = 0;
    for (const auto& layer : layers_) {
        total += layer->getNumNeurons();
    }
    return total;
}

void NeuralNetwork::clear() {
    layers_.clear();
    layerConfigs_.clear();
    isBuilt_ = false;
    lastError_ = 0.0;
}

//=============================================================================
// NPCLearningNetwork Implementation
//=============================================================================

NPCLearningNetwork::NPCLearningNetwork(LearningContext context)
    : context_(context)
{
    initializeNetwork();
}

NPCLearningNetwork::~NPCLearningNetwork() = default;

void NPCLearningNetwork::initializeNetwork() {
    network_ = std::make_unique<NeuralNetwork>();

    // Configure based on context
    switch (context_) {
        case LearningContext::DecisionMaking:
            inputSize_ = 16;
            numActions_ = 8;
            break;
        case LearningContext::SocialBehavior:
            inputSize_ = 12;
            numActions_ = 6;
            break;
        case LearningContext::CombatTactics:
            inputSize_ = 20;
            numActions_ = 10;
            break;
        case LearningContext::EconomicDecisions:
            inputSize_ = 14;
            numActions_ = 5;
            break;
        case LearningContext::EmotionalResponse:
            inputSize_ = 10;
            numActions_ = 8;
            break;
    }

    // Build network: input -> hidden -> hidden -> output
    LayerConfig hidden1;
    hidden1.numNeurons = 32;
    hidden1.activation = ActivationFunction::Tanh;
    network_->addLayer(hidden1);

    LayerConfig hidden2;
    hidden2.numNeurons = 16;
    hidden2.activation = ActivationFunction::Tanh;
    network_->addLayer(hidden2);

    LayerConfig output;
    output.numNeurons = numActions_;
    output.activation = ActivationFunction::Tanh;
    network_->addLayer(output);

    network_->build();
}

void NPCLearningNetwork::learnFromExperience(
    const std::vector<double>& situation,
    uint32_t action,
    double outcome)
{
    if (inputSize_ == 0) {
        inputSize_ = static_cast<uint32_t>(situation.size());
    }

    // Create training point
    TrainingPoint point;
    point.inputs = situation;
    point.expectedOutputs = encodeAction(action);

    // Weight by outcome
    for (auto& o : point.expectedOutputs) {
        o *= outcome;
    }

    experienceBuffer_.push_back(point);
    if (experienceBuffer_.size() > MAX_BUFFER_SIZE) {
        experienceBuffer_.erase(experienceBuffer_.begin());
    }

    experienceCount_++;
    totalReward_ += outcome;

    // Periodic training
    if (experienceCount_ % 50 == 0) {
        batchTrain();
    }
}

uint32_t NPCLearningNetwork::predictBestAction(const std::vector<double>& situation) {
    auto probs = getActionProbabilities(situation);
    return decodeAction(probs);
}

std::vector<double> NPCLearningNetwork::getActionProbabilities(
    const std::vector<double>& situation)
{
    std::vector<double> input = situation;
    input.resize(inputSize_, 0.0);
    return network_->predict(input);
}

void NPCLearningNetwork::setNumActions(uint32_t numActions) {
    numActions_ = numActions;
    initializeNetwork();
}

void NPCLearningNetwork::batchTrain() {
    if (experienceBuffer_.empty()) return;

    TrainingConfig config;
    config.method = OptimizationMethod::GradientDescent;
    config.learningRate = 0.01;
    config.maxIterations = 100;
    config.randomizeInitialWeights = false;

    network_->train(experienceBuffer_, config);
}

bool NPCLearningNetwork::save(const std::string& filename) const {
    return network_->save(filename);
}

bool NPCLearningNetwork::load(const std::string& filename) {
    return network_->load(filename);
}

std::vector<double> NPCLearningNetwork::encodeAction(uint32_t action) const {
    std::vector<double> encoded(numActions_, -1.0);
    if (action < numActions_) {
        encoded[action] = 1.0;
    }
    return encoded;
}

uint32_t NPCLearningNetwork::decodeAction(const std::vector<double>& output) const {
    if (output.empty()) return 0;

    auto maxIt = std::max_element(output.begin(), output.end());
    return static_cast<uint32_t>(std::distance(output.begin(), maxIt));
}

} // namespace Neural
} // namespace NPC
} // namespace Ultima

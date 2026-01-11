/**
 * NeuralNetwork.h - C++ Wrapper for GNeural-Net Integration
 *
 * Provides lightweight neural network capabilities for NPC learning.
 * Wraps the GNU Neural Network library for Ultima NPC AI.
 *
 * Part of Phase 1: Foundation
 */

#ifndef ULTIMA_NPC_NEURAL_NETWORK_H
#define ULTIMA_NPC_NEURAL_NETWORK_H

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <cstdint>

namespace Ultima {
namespace NPC {
namespace Neural {

/**
 * Activation function types (mirrors GNeural-Net)
 */
enum class ActivationFunction {
    Tanh,       // Hyperbolic tangent
    Exp,        // Exponential
    Identity,   // Linear
    Polynomial1,// First order polynomial
    Polynomial2 // Second order polynomial
};

/**
 * Discriminant function types
 */
enum class DiscriminantFunction {
    Linear,
    Legendre,
    Laguerre,
    Fourier
};

/**
 * Training optimization methods
 */
enum class OptimizationMethod {
    SimulatedAnnealing,
    RandomSearch,
    GradientDescent,
    GeneticAlgorithm,
    MSMCO   // Multi-scale Monte Carlo optimization
};

/**
 * Error/loss function types
 */
enum class ErrorFunction {
    L2,     // Mean squared error
    L1      // Mean absolute error
};

/**
 * Neuron configuration
 */
struct NeuronConfig {
    uint32_t numInputs = 1;
    ActivationFunction activation = ActivationFunction::Tanh;
    DiscriminantFunction discriminant = DiscriminantFunction::Linear;
    std::vector<double> initialWeights;
};

/**
 * Layer configuration
 */
struct LayerConfig {
    uint32_t numNeurons = 1;
    ActivationFunction activation = ActivationFunction::Tanh;
    DiscriminantFunction discriminant = DiscriminantFunction::Linear;
};

/**
 * Network training configuration
 */
struct TrainingConfig {
    OptimizationMethod method = OptimizationMethod::GradientDescent;
    ErrorFunction errorFunction = ErrorFunction::L2;

    double learningRate = 0.01;
    double accuracy = 0.001;
    int maxIterations = 1000;

    // Genetic algorithm parameters
    int populationSize = 100;
    double mutationRate = 0.1;

    // Simulated annealing parameters
    double initialTemperature = 1.0;
    double minTemperature = 0.001;
    double coolingRate = 0.95;

    // Weight bounds
    double weightMin = -1.0;
    double weightMax = 1.0;

    bool randomizeInitialWeights = true;
    bool verbose = false;
};

/**
 * Training data point
 */
struct TrainingPoint {
    std::vector<double> inputs;
    std::vector<double> expectedOutputs;
};

/**
 * Neural network layer
 */
class Layer {
public:
    Layer(const LayerConfig& config);
    ~Layer();

    std::vector<double> forward(const std::vector<double>& inputs);
    void setWeights(const std::vector<std::vector<double>>& weights);
    std::vector<std::vector<double>> getWeights() const;

    uint32_t getNumNeurons() const { return numNeurons_; }

private:
    uint32_t numNeurons_;
    uint32_t numInputs_;
    ActivationFunction activation_;
    std::vector<std::vector<double>> weights_;
    std::vector<double> biases_;

    double activate(double x) const;
};

/**
 * Main Neural Network class - C++ wrapper for GNeural-Net
 */
class NeuralNetwork {
public:
    NeuralNetwork();
    ~NeuralNetwork();

    // Network topology
    void addLayer(const LayerConfig& config);
    void build();

    // Forward pass
    std::vector<double> predict(const std::vector<double>& inputs);

    // Training
    void train(const std::vector<TrainingPoint>& data, const TrainingConfig& config);
    double getTrainingError() const { return lastError_; }

    // Persistence
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);

    // Network info
    uint32_t getNumLayers() const { return static_cast<uint32_t>(layers_.size()); }
    uint32_t getNumNeurons() const;

    // Reset
    void clear();

private:
    std::vector<std::unique_ptr<Layer>> layers_;
    std::vector<LayerConfig> layerConfigs_;
    bool isBuilt_ = false;
    double lastError_ = 0.0;

    void randomizeWeights(double min, double max);
    double computeError(const std::vector<TrainingPoint>& data, ErrorFunction errorFunc);

    // Training algorithms
    void trainGradientDescent(const std::vector<TrainingPoint>& data, const TrainingConfig& config);
    void trainSimulatedAnnealing(const std::vector<TrainingPoint>& data, const TrainingConfig& config);
    void trainGeneticAlgorithm(const std::vector<TrainingPoint>& data, const TrainingConfig& config);
    void trainRandomSearch(const std::vector<TrainingPoint>& data, const TrainingConfig& config);
};

/**
 * NPC Learning Network - Specialized network for NPC behavior learning
 *
 * Pre-configured network topology for common NPC learning tasks:
 * - Decision making
 * - Preference learning
 * - Behavior adaptation
 */
class NPCLearningNetwork {
public:
    /**
     * Learning context types
     */
    enum class LearningContext {
        DecisionMaking,     // Learn decision preferences
        SocialBehavior,     // Learn social interaction patterns
        CombatTactics,      // Learn combat strategies
        EconomicDecisions,  // Learn trade/economic behavior
        EmotionalResponse   // Learn emotional reactions
    };

    NPCLearningNetwork(LearningContext context);
    ~NPCLearningNetwork();

    /**
     * Learn from experience
     * @param situation Input features describing the situation
     * @param action Action taken
     * @param outcome Reward/penalty (-1.0 to 1.0)
     */
    void learnFromExperience(
        const std::vector<double>& situation,
        uint32_t action,
        double outcome
    );

    /**
     * Predict best action for a situation
     * @param situation Input features
     * @return Action index with highest predicted reward
     */
    uint32_t predictBestAction(const std::vector<double>& situation);

    /**
     * Get action probabilities
     * @param situation Input features
     * @return Probability distribution over actions
     */
    std::vector<double> getActionProbabilities(const std::vector<double>& situation);

    /**
     * Set number of possible actions
     */
    void setNumActions(uint32_t numActions);

    /**
     * Batch training
     */
    void batchTrain();

    /**
     * Save/load learned weights
     */
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);

    /**
     * Get learning statistics
     */
    uint32_t getExperienceCount() const { return experienceCount_; }
    double getAverageReward() const { return totalReward_ / std::max(1u, experienceCount_); }

private:
    std::unique_ptr<NeuralNetwork> network_;
    LearningContext context_;
    uint32_t numActions_ = 4;
    uint32_t inputSize_ = 0;

    // Experience replay buffer
    std::vector<TrainingPoint> experienceBuffer_;
    static constexpr size_t MAX_BUFFER_SIZE = 1000;

    // Statistics
    uint32_t experienceCount_ = 0;
    double totalReward_ = 0.0;

    void initializeNetwork();
    std::vector<double> encodeAction(uint32_t action) const;
    uint32_t decodeAction(const std::vector<double>& output) const;
};

} // namespace Neural
} // namespace NPC
} // namespace Ultima

#endif // ULTIMA_NPC_NEURAL_NETWORK_H

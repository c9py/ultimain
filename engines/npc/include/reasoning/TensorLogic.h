/**
 * TensorLogic.h - Neuro-Symbolic Tensor Logic Engine
 *
 * Implements tensor-based logical reasoning for NPC AI.
 * Combines neural network inference with symbolic logic rules.
 *
 * Part of Phase 2: Reasoning
 */

#ifndef ULTIMA_NPC_TENSOR_LOGIC_H
#define ULTIMA_NPC_TENSOR_LOGIC_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <variant>
#include <optional>

namespace Ultima {
namespace NPC {
namespace Reasoning {

/**
 * Logical value - supports fuzzy/probabilistic logic
 */
struct LogicalValue {
    double truth = 0.0;         // Truth value [0, 1]
    double confidence = 1.0;    // Confidence in the value
    double relevance = 1.0;     // Contextual relevance

    LogicalValue() = default;
    LogicalValue(double t) : truth(t) {}
    LogicalValue(double t, double c) : truth(t), confidence(c) {}

    // Logical operations
    LogicalValue operator&&(const LogicalValue& other) const;
    LogicalValue operator||(const LogicalValue& other) const;
    LogicalValue operator!() const;

    // Comparison
    bool isTrue(double threshold = 0.5) const { return truth >= threshold; }
    bool isFalse(double threshold = 0.5) const { return truth < threshold; }
};

/**
 * Term in logical expressions
 */
struct Term {
    enum class Type {
        Variable,       // ?x, ?y - bound variables
        Constant,       // "John", 5.0 - literal values
        Function,       // f(?x) - function application
        Predicate       // hasItem(?x, ?y) - predicate reference
    };

    Type type;
    std::string name;
    std::vector<Term> arguments;
    std::variant<std::string, double, bool> value;

    // Convenience constructors
    static Term variable(const std::string& name);
    static Term constant(const std::string& value);
    static Term constant(double value);
    static Term predicate(const std::string& name, std::vector<Term> args);
};

/**
 * Logical formula
 */
struct Formula {
    enum class Type {
        Atomic,         // P(x, y)
        Negation,       // NOT phi
        Conjunction,    // phi AND psi
        Disjunction,    // phi OR psi
        Implication,    // phi -> psi
        Universal,      // FORALL x. phi
        Existential,    // EXISTS x. phi
        Modal           // BELIEVES(agent, phi), KNOWS(agent, phi)
    };

    Type type;
    Term predicate;                     // For atomic formulas
    std::vector<Formula> subformulas;   // For compound formulas
    std::string boundVariable;          // For quantified formulas
    std::string modalOperator;          // For modal formulas
    std::string agent;                  // For modal formulas

    // Constructors
    static Formula atomic(const Term& pred);
    static Formula negation(const Formula& f);
    static Formula conjunction(const Formula& f1, const Formula& f2);
    static Formula disjunction(const Formula& f1, const Formula& f2);
    static Formula implication(const Formula& antecedent, const Formula& consequent);
    static Formula forall(const std::string& var, const Formula& body);
    static Formula exists(const std::string& var, const Formula& body);
    static Formula believes(const std::string& agent, const Formula& f);
    static Formula knows(const std::string& agent, const Formula& f);
};

/**
 * Inference rule
 */
struct InferenceRule {
    std::string name;
    std::vector<Formula> premises;
    Formula conclusion;
    double confidence = 1.0;
    int priority = 0;

    // Metadata
    std::string category;       // e.g., "social", "combat", "economic"
    std::string description;
};

/**
 * Binding - variable assignments
 */
using Binding = std::map<std::string, Term>;

/**
 * Fact in the knowledge base
 */
struct Fact {
    Term predicate;
    LogicalValue value;
    uint32_t timestamp = 0;
    std::string source;         // Where the fact came from
    bool isDerived = false;     // Inferred vs observed
};

/**
 * Tensor representation for embedding
 */
class Tensor {
public:
    Tensor();
    Tensor(size_t dim);
    Tensor(const std::vector<double>& data);
    ~Tensor();

    // Element access
    double& operator[](size_t i);
    double operator[](size_t i) const;

    // Operations
    Tensor operator+(const Tensor& other) const;
    Tensor operator-(const Tensor& other) const;
    Tensor operator*(double scalar) const;
    double dot(const Tensor& other) const;
    double norm() const;
    Tensor normalized() const;

    // Similarity
    double cosineSimilarity(const Tensor& other) const;

    size_t size() const { return data_.size(); }
    const std::vector<double>& data() const { return data_; }

private:
    std::vector<double> data_;
};

/**
 * Entity embedding - maps entities to tensor space
 */
class EntityEmbedding {
public:
    EntityEmbedding(size_t dimension = 64);
    ~EntityEmbedding();

    /**
     * Get or create embedding for entity
     */
    Tensor getEmbedding(const std::string& entity);

    /**
     * Update embedding based on context
     */
    void updateEmbedding(const std::string& entity, const Tensor& context, double learningRate = 0.1);

    /**
     * Find similar entities
     */
    std::vector<std::pair<std::string, double>> findSimilar(const std::string& entity, int topK = 5);

    /**
     * Relation embedding: E1 + R = E2
     */
    void learnRelation(const std::string& subject, const std::string& relation, const std::string& object);

    /**
     * Predict object given subject and relation
     */
    std::string predictObject(const std::string& subject, const std::string& relation);

    /**
     * Save/load embeddings
     */
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);

private:
    size_t dimension_;
    std::map<std::string, Tensor> entityEmbeddings_;
    std::map<std::string, Tensor> relationEmbeddings_;

    Tensor randomEmbedding();
};

/**
 * Reasoner - Main inference engine
 */
class TensorLogicReasoner {
public:
    TensorLogicReasoner();
    ~TensorLogicReasoner();

    /**
     * Add fact to knowledge base
     */
    void addFact(const std::string& predicate,
                 const std::vector<std::string>& args,
                 double truthValue = 1.0);

    /**
     * Query fact
     */
    LogicalValue queryFact(const std::string& predicate,
                          const std::vector<std::string>& args) const;

    /**
     * Add inference rule
     */
    void addRule(const InferenceRule& rule);

    /**
     * Forward chaining - derive new facts from existing
     */
    std::vector<Fact> forwardChain(int maxIterations = 10);

    /**
     * Backward chaining - prove a goal
     */
    std::optional<LogicalValue> backwardChain(const Formula& goal,
                                              int maxDepth = 10);

    /**
     * Abductive reasoning - find best explanation
     */
    std::vector<Formula> abductiveReason(const Formula& observation);

    /**
     * Counterfactual reasoning
     */
    LogicalValue counterfactual(const Formula& condition,
                               const Formula& consequence);

    /**
     * Neural-enhanced inference
     * Uses embeddings to find relevant rules and facts
     */
    LogicalValue neuralInfer(const std::string& query);

    /**
     * Access embedding system
     */
    EntityEmbedding& getEmbeddings() { return embeddings_; }

    /**
     * Get all facts about entity
     */
    std::vector<Fact> getFactsAbout(const std::string& entity) const;

    /**
     * Clear knowledge base
     */
    void clear();

    bool matchFormula(const Formula& formula, const Binding& binding,
                     LogicalValue& result) const;

private:
    std::vector<Fact> facts_;
    std::vector<InferenceRule> rules_;
    EntityEmbedding embeddings_;

    // Indexes for fast lookup
    std::multimap<std::string, size_t> predicateIndex_;
    std::multimap<std::string, size_t> entityIndex_;
    std::vector<Binding> findBindings(const Term& pattern) const;
    LogicalValue evaluateFormula(const Formula& f, const Binding& binding) const;
};

/**
 * Modal Logic Reasoner - For beliefs, knowledge, intentions
 */
class ModalReasoner {
public:
    ModalReasoner();
    ~ModalReasoner();

    /**
     * Set agent's belief
     */
    void setBelief(const std::string& agent, const Formula& belief, double strength = 1.0);

    /**
     * Query agent's belief
     */
    LogicalValue queryBelief(const std::string& agent, const Formula& query) const;

    /**
     * Set agent's knowledge (stronger than belief)
     */
    void setKnowledge(const std::string& agent, const Formula& knowledge);

    /**
     * Set agent's intention/goal
     */
    void setIntention(const std::string& agent, const Formula& intention, double priority = 0.5);

    /**
     * Reason about what agent believes about another's beliefs
     */
    LogicalValue queryNestedBelief(const std::string& agent1,
                                   const std::string& agent2,
                                   const Formula& belief) const;

    /**
     * Update beliefs based on observation
     */
    void observe(const std::string& agent, const Formula& observation);

    /**
     * Belief revision - update beliefs consistently
     */
    void reviseBeliefs(const std::string& agent, const Formula& newInfo);

private:
    struct AgentMentalState {
        std::vector<std::pair<Formula, double>> beliefs;
        std::vector<Formula> knowledge;
        std::vector<std::pair<Formula, double>> intentions;
    };

    std::map<std::string, AgentMentalState> agentStates_;
};

/**
 * Temporal Reasoner - Reasoning about time and change
 */
class TemporalReasoner {
public:
    TemporalReasoner();
    ~TemporalReasoner();

    /**
     * Add fact at specific time
     */
    void addTemporalFact(const Fact& fact, uint32_t startTime, uint32_t endTime = UINT32_MAX);

    /**
     * Query fact at specific time
     */
    LogicalValue queryAtTime(const std::string& predicate,
                            const std::vector<std::string>& args,
                            uint32_t time) const;

    /**
     * Query if fact was ever true
     */
    LogicalValue queryEver(const std::string& predicate,
                          const std::vector<std::string>& args) const;

    /**
     * Query if fact was always true in interval
     */
    LogicalValue queryAlways(const std::string& predicate,
                            const std::vector<std::string>& args,
                            uint32_t startTime,
                            uint32_t endTime) const;

    /**
     * Project future state based on rules
     */
    std::vector<Fact> project(uint32_t futureTime) const;

private:
    struct TemporalFact {
        Fact fact;
        uint32_t startTime;
        uint32_t endTime;
    };

    std::vector<TemporalFact> temporalFacts_;
    uint32_t currentTime_ = 0;
};

/**
 * Integrated Reasoning System
 */
class ReasoningSystem {
public:
    ReasoningSystem();
    ~ReasoningSystem();

    /**
     * Initialize with rules file
     */
    bool loadRules(const std::string& filename);

    /**
     * Main query interface
     */
    LogicalValue query(const std::string& queryString);

    /**
     * Assert new fact
     */
    void assertFact(const std::string& factString);

    /**
     * Get explanation for conclusion
     */
    std::string explain(const std::string& conclusion);

    /**
     * Access sub-systems
     */
    TensorLogicReasoner& getLogicReasoner() { return logicReasoner_; }
    ModalReasoner& getModalReasoner() { return modalReasoner_; }
    TemporalReasoner& getTemporalReasoner() { return temporalReasoner_; }

    /**
     * Process natural language query (simplified)
     */
    std::string processNLQuery(const std::string& query);

private:
    TensorLogicReasoner logicReasoner_;
    ModalReasoner modalReasoner_;
    TemporalReasoner temporalReasoner_;

    Formula parseQuery(const std::string& queryString);
    std::string formatResult(const LogicalValue& result);
};

} // namespace Reasoning
} // namespace NPC
} // namespace Ultima

#endif // ULTIMA_NPC_TENSOR_LOGIC_H

/**
 * TensorLogic.cpp - Neuro-Symbolic Tensor Logic Engine Implementation
 */

#include "reasoning/TensorLogic.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <queue>

namespace Ultima {
namespace NPC {
namespace Reasoning {

static std::mt19937 rng(std::random_device{}());

//=============================================================================
// LogicalValue Implementation
//=============================================================================

LogicalValue LogicalValue::operator&&(const LogicalValue& other) const {
    LogicalValue result;
    result.truth = std::min(truth, other.truth);
    result.confidence = confidence * other.confidence;
    result.relevance = (relevance + other.relevance) / 2.0;
    return result;
}

LogicalValue LogicalValue::operator||(const LogicalValue& other) const {
    LogicalValue result;
    result.truth = std::max(truth, other.truth);
    result.confidence = std::max(confidence, other.confidence);
    result.relevance = (relevance + other.relevance) / 2.0;
    return result;
}

LogicalValue LogicalValue::operator!() const {
    LogicalValue result;
    result.truth = 1.0 - truth;
    result.confidence = confidence;
    result.relevance = relevance;
    return result;
}

//=============================================================================
// Term Implementation
//=============================================================================

Term Term::variable(const std::string& name) {
    Term t;
    t.type = Type::Variable;
    t.name = name;
    return t;
}

Term Term::constant(const std::string& value) {
    Term t;
    t.type = Type::Constant;
    t.value = value;
    return t;
}

Term Term::constant(double value) {
    Term t;
    t.type = Type::Constant;
    t.value = value;
    return t;
}

Term Term::predicate(const std::string& name, std::vector<Term> args) {
    Term t;
    t.type = Type::Predicate;
    t.name = name;
    t.arguments = std::move(args);
    return t;
}

//=============================================================================
// Formula Implementation
//=============================================================================

Formula Formula::atomic(const Term& pred) {
    Formula f;
    f.type = Type::Atomic;
    f.predicate = pred;
    return f;
}

Formula Formula::negation(const Formula& formula) {
    Formula f;
    f.type = Type::Negation;
    f.subformulas.push_back(formula);
    return f;
}

Formula Formula::conjunction(const Formula& f1, const Formula& f2) {
    Formula f;
    f.type = Type::Conjunction;
    f.subformulas.push_back(f1);
    f.subformulas.push_back(f2);
    return f;
}

Formula Formula::disjunction(const Formula& f1, const Formula& f2) {
    Formula f;
    f.type = Type::Disjunction;
    f.subformulas.push_back(f1);
    f.subformulas.push_back(f2);
    return f;
}

Formula Formula::implication(const Formula& antecedent, const Formula& consequent) {
    Formula f;
    f.type = Type::Implication;
    f.subformulas.push_back(antecedent);
    f.subformulas.push_back(consequent);
    return f;
}

Formula Formula::forall(const std::string& var, const Formula& body) {
    Formula f;
    f.type = Type::Universal;
    f.boundVariable = var;
    f.subformulas.push_back(body);
    return f;
}

Formula Formula::exists(const std::string& var, const Formula& body) {
    Formula f;
    f.type = Type::Existential;
    f.boundVariable = var;
    f.subformulas.push_back(body);
    return f;
}

Formula Formula::believes(const std::string& agent, const Formula& formula) {
    Formula f;
    f.type = Type::Modal;
    f.modalOperator = "BELIEVES";
    f.agent = agent;
    f.subformulas.push_back(formula);
    return f;
}

Formula Formula::knows(const std::string& agent, const Formula& formula) {
    Formula f;
    f.type = Type::Modal;
    f.modalOperator = "KNOWS";
    f.agent = agent;
    f.subformulas.push_back(formula);
    return f;
}

//=============================================================================
// Tensor Implementation
//=============================================================================

Tensor::Tensor() = default;

Tensor::Tensor(size_t dim) : data_(dim, 0.0) {}

Tensor::Tensor(const std::vector<double>& data) : data_(data) {}

Tensor::~Tensor() = default;

double& Tensor::operator[](size_t i) {
    return data_[i];
}

double Tensor::operator[](size_t i) const {
    return data_[i];
}

Tensor Tensor::operator+(const Tensor& other) const {
    Tensor result(data_.size());
    for (size_t i = 0; i < data_.size() && i < other.data_.size(); ++i) {
        result.data_[i] = data_[i] + other.data_[i];
    }
    return result;
}

Tensor Tensor::operator-(const Tensor& other) const {
    Tensor result(data_.size());
    for (size_t i = 0; i < data_.size() && i < other.data_.size(); ++i) {
        result.data_[i] = data_[i] - other.data_[i];
    }
    return result;
}

Tensor Tensor::operator*(double scalar) const {
    Tensor result(data_.size());
    for (size_t i = 0; i < data_.size(); ++i) {
        result.data_[i] = data_[i] * scalar;
    }
    return result;
}

double Tensor::dot(const Tensor& other) const {
    double sum = 0.0;
    for (size_t i = 0; i < data_.size() && i < other.data_.size(); ++i) {
        sum += data_[i] * other.data_[i];
    }
    return sum;
}

double Tensor::norm() const {
    return std::sqrt(dot(*this));
}

Tensor Tensor::normalized() const {
    double n = norm();
    if (n < 1e-10) return *this;
    return *this * (1.0 / n);
}

double Tensor::cosineSimilarity(const Tensor& other) const {
    double n1 = norm();
    double n2 = other.norm();
    if (n1 < 1e-10 || n2 < 1e-10) return 0.0;
    return dot(other) / (n1 * n2);
}

//=============================================================================
// EntityEmbedding Implementation
//=============================================================================

EntityEmbedding::EntityEmbedding(size_t dimension)
    : dimension_(dimension)
{
}

EntityEmbedding::~EntityEmbedding() = default;

Tensor EntityEmbedding::randomEmbedding() {
    Tensor t(dimension_);
    std::uniform_real_distribution<double> dist(-1.0, 1.0);
    for (size_t i = 0; i < dimension_; ++i) {
        t[i] = dist(rng);
    }
    return t.normalized();
}

Tensor EntityEmbedding::getEmbedding(const std::string& entity) {
    if (!entityEmbeddings_.count(entity)) {
        entityEmbeddings_[entity] = randomEmbedding();
    }
    return entityEmbeddings_[entity];
}

void EntityEmbedding::updateEmbedding(const std::string& entity,
                                      const Tensor& context,
                                      double learningRate) {
    auto& emb = entityEmbeddings_[entity];
    if (emb.size() == 0) {
        emb = randomEmbedding();
    }
    for (size_t i = 0; i < dimension_ && i < context.size(); ++i) {
        emb[i] += learningRate * (context[i] - emb[i]);
    }
    emb = emb.normalized();
}

std::vector<std::pair<std::string, double>> EntityEmbedding::findSimilar(
    const std::string& entity, int topK)
{
    std::vector<std::pair<std::string, double>> results;
    Tensor target = getEmbedding(entity);

    for (const auto& [name, emb] : entityEmbeddings_) {
        if (name == entity) continue;
        double sim = target.cosineSimilarity(emb);
        results.push_back({name, sim});
    }

    std::sort(results.begin(), results.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    if (results.size() > static_cast<size_t>(topK)) {
        results.resize(topK);
    }

    return results;
}

void EntityEmbedding::learnRelation(const std::string& subject,
                                    const std::string& relation,
                                    const std::string& object) {
    Tensor subj = getEmbedding(subject);
    Tensor obj = getEmbedding(object);

    if (!relationEmbeddings_.count(relation)) {
        relationEmbeddings_[relation] = randomEmbedding();
    }

    // Learn: subject + relation ≈ object
    Tensor& rel = relationEmbeddings_[relation];
    Tensor target = obj - subj;

    for (size_t i = 0; i < dimension_; ++i) {
        rel[i] += 0.1 * (target[i] - rel[i]);
    }
}

std::string EntityEmbedding::predictObject(const std::string& subject,
                                            const std::string& relation) {
    if (!relationEmbeddings_.count(relation)) {
        return "";
    }

    Tensor subj = getEmbedding(subject);
    Tensor rel = relationEmbeddings_[relation];
    Tensor predicted = subj + rel;

    double bestSim = -1.0;
    std::string bestEntity;

    for (const auto& [name, emb] : entityEmbeddings_) {
        if (name == subject) continue;
        double sim = predicted.cosineSimilarity(emb);
        if (sim > bestSim) {
            bestSim = sim;
            bestEntity = name;
        }
    }

    return bestEntity;
}

bool EntityEmbedding::save(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) return false;

    file << dimension_ << "\n";
    file << entityEmbeddings_.size() << "\n";

    for (const auto& [name, emb] : entityEmbeddings_) {
        file << name << "\n";
        for (size_t i = 0; i < dimension_; ++i) {
            file << emb[i] << " ";
        }
        file << "\n";
    }

    return true;
}

bool EntityEmbedding::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) return false;

    size_t dim, count;
    file >> dim >> count;
    dimension_ = dim;

    entityEmbeddings_.clear();

    for (size_t c = 0; c < count; ++c) {
        std::string name;
        file >> name;

        Tensor emb(dim);
        for (size_t i = 0; i < dim; ++i) {
            file >> emb[i];
        }
        entityEmbeddings_[name] = emb;
    }

    return true;
}

//=============================================================================
// TensorLogicReasoner Implementation
//=============================================================================

TensorLogicReasoner::TensorLogicReasoner() = default;
TensorLogicReasoner::~TensorLogicReasoner() = default;

void TensorLogicReasoner::addFact(const std::string& predicate,
                                  const std::vector<std::string>& args,
                                  double truthValue) {
    Fact fact;
    fact.predicate = Term::predicate(predicate, {});
    for (const auto& arg : args) {
        fact.predicate.arguments.push_back(Term::constant(arg));
    }
    fact.value = LogicalValue(truthValue);

    size_t idx = facts_.size();
    facts_.push_back(fact);

    predicateIndex_.insert({predicate, idx});
    for (const auto& arg : args) {
        entityIndex_.insert({arg, idx});
    }
}

LogicalValue TensorLogicReasoner::queryFact(const std::string& predicate,
                                            const std::vector<std::string>& args) const {
    auto range = predicateIndex_.equal_range(predicate);
    for (auto it = range.first; it != range.second; ++it) {
        const auto& fact = facts_[it->second];

        bool match = true;
        if (fact.predicate.arguments.size() != args.size()) {
            continue;
        }

        for (size_t i = 0; i < args.size(); ++i) {
            const auto& term = fact.predicate.arguments[i];
            if (term.type == Term::Type::Constant) {
                if (std::holds_alternative<std::string>(term.value)) {
                    if (std::get<std::string>(term.value) != args[i]) {
                        match = false;
                        break;
                    }
                }
            }
        }

        if (match) {
            return fact.value;
        }
    }

    return LogicalValue(0.0, 0.0);  // Unknown
}

void TensorLogicReasoner::addRule(const InferenceRule& rule) {
    rules_.push_back(rule);
}

std::vector<Fact> TensorLogicReasoner::forwardChain(int maxIterations) {
    std::vector<Fact> newFacts;

    for (int iter = 0; iter < maxIterations; ++iter) {
        bool changed = false;

        for (const auto& rule : rules_) {
            // Try to match premises
            bool allPremisesMatch = true;
            Binding binding;

            for (const auto& premise : rule.premises) {
                LogicalValue result;
                if (!matchFormula(premise, binding, result)) {
                    allPremisesMatch = false;
                    break;
                }
                if (!result.isTrue(0.5)) {
                    allPremisesMatch = false;
                    break;
                }
            }

            if (allPremisesMatch) {
                // Derive conclusion
                Fact newFact;
                newFact.predicate = rule.conclusion.predicate;
                newFact.value = LogicalValue(rule.confidence);
                newFact.isDerived = true;

                // Check if already known
                bool alreadyKnown = false;
                // ... (check logic)

                if (!alreadyKnown) {
                    facts_.push_back(newFact);
                    newFacts.push_back(newFact);
                    changed = true;
                }
            }
        }

        if (!changed) break;
    }

    return newFacts;
}

std::optional<LogicalValue> TensorLogicReasoner::backwardChain(const Formula& goal,
                                                               int maxDepth) {
    if (maxDepth <= 0) return std::nullopt;

    Binding binding;
    LogicalValue result;

    // Try direct match
    if (matchFormula(goal, binding, result)) {
        return result;
    }

    // Try rules
    for (const auto& rule : rules_) {
        // Check if conclusion matches goal pattern
        // ... (unification logic)

        // Recursively prove premises
        bool allProved = true;
        for (const auto& premise : rule.premises) {
            auto premiseResult = backwardChain(premise, maxDepth - 1);
            if (!premiseResult || !premiseResult->isTrue()) {
                allProved = false;
                break;
            }
        }

        if (allProved) {
            return LogicalValue(rule.confidence);
        }
    }

    return std::nullopt;
}

std::vector<Formula> TensorLogicReasoner::abductiveReason(const Formula& observation) {
    std::vector<Formula> explanations;

    // Find rules where observation could be the conclusion
    for (const auto& rule : rules_) {
        // Check if rule conclusion matches observation
        // If so, premises are potential explanations
        // ... (matching logic)
    }

    return explanations;
}

LogicalValue TensorLogicReasoner::counterfactual(const Formula& condition,
                                                 const Formula& consequence) {
    // Create hypothetical world with condition true
    // Check if consequence would hold
    return LogicalValue(0.5);  // Uncertain by default
}

LogicalValue TensorLogicReasoner::neuralInfer(const std::string& query) {
    // Use embeddings to find relevant facts
    auto similar = embeddings_.findSimilar(query, 5);

    double totalEvidence = 0.0;
    double weightSum = 0.0;

    for (const auto& [entity, similarity] : similar) {
        auto range = entityIndex_.equal_range(entity);
        for (auto it = range.first; it != range.second; ++it) {
            const auto& fact = facts_[it->second];
            totalEvidence += fact.value.truth * similarity;
            weightSum += similarity;
        }
    }

    if (weightSum > 0) {
        return LogicalValue(totalEvidence / weightSum);
    }

    return LogicalValue(0.5, 0.1);  // Low confidence default
}

std::vector<Fact> TensorLogicReasoner::getFactsAbout(const std::string& entity) const {
    std::vector<Fact> results;
    auto range = entityIndex_.equal_range(entity);
    for (auto it = range.first; it != range.second; ++it) {
        results.push_back(facts_[it->second]);
    }
    return results;
}

void TensorLogicReasoner::clear() {
    facts_.clear();
    rules_.clear();
    predicateIndex_.clear();
    entityIndex_.clear();
}

bool TensorLogicReasoner::matchFormula(const Formula& formula,
                                       const Binding& binding,
                                       LogicalValue& result) const {
    switch (formula.type) {
        case Formula::Type::Atomic: {
            // Match predicate against facts
            std::vector<std::string> args;
            for (const auto& arg : formula.predicate.arguments) {
                if (arg.type == Term::Type::Variable) {
                    if (binding.count(arg.name)) {
                        args.push_back(std::get<std::string>(binding.at(arg.name).value));
                    } else {
                        return false;  // Unbound variable
                    }
                } else if (arg.type == Term::Type::Constant) {
                    args.push_back(std::get<std::string>(arg.value));
                }
            }
            result = queryFact(formula.predicate.name, args);
            return result.confidence > 0;
        }

        case Formula::Type::Negation:
            if (matchFormula(formula.subformulas[0], binding, result)) {
                result = !result;
                return true;
            }
            return false;

        case Formula::Type::Conjunction: {
            LogicalValue r1, r2;
            if (matchFormula(formula.subformulas[0], binding, r1) &&
                matchFormula(formula.subformulas[1], binding, r2)) {
                result = r1 && r2;
                return true;
            }
            return false;
        }

        case Formula::Type::Disjunction: {
            LogicalValue r1, r2;
            bool m1 = matchFormula(formula.subformulas[0], binding, r1);
            bool m2 = matchFormula(formula.subformulas[1], binding, r2);
            if (m1 || m2) {
                result = r1 || r2;
                return true;
            }
            return false;
        }

        default:
            return false;
    }
}

std::vector<Binding> TensorLogicReasoner::findBindings(const Term& pattern) const {
    std::vector<Binding> bindings;
    // Find all possible variable bindings for pattern
    // ... (implementation)
    return bindings;
}

LogicalValue TensorLogicReasoner::evaluateFormula(const Formula& f,
                                                  const Binding& binding) const {
    LogicalValue result;
    matchFormula(f, binding, result);
    return result;
}

//=============================================================================
// ModalReasoner Implementation
//=============================================================================

ModalReasoner::ModalReasoner() = default;
ModalReasoner::~ModalReasoner() = default;

void ModalReasoner::setBelief(const std::string& agent,
                              const Formula& belief,
                              double strength) {
    agentStates_[agent].beliefs.push_back({belief, strength});
}

LogicalValue ModalReasoner::queryBelief(const std::string& agent,
                                        const Formula& query) const {
    auto it = agentStates_.find(agent);
    if (it == agentStates_.end()) {
        return LogicalValue(0.0, 0.0);
    }

    // Simple matching - would need proper formula comparison
    for (const auto& [belief, strength] : it->second.beliefs) {
        // Compare formulas
        // ... (comparison logic)
    }

    return LogicalValue(0.5, 0.5);
}

void ModalReasoner::setKnowledge(const std::string& agent, const Formula& knowledge) {
    agentStates_[agent].knowledge.push_back(knowledge);
}

void ModalReasoner::setIntention(const std::string& agent,
                                 const Formula& intention,
                                 double priority) {
    agentStates_[agent].intentions.push_back({intention, priority});
}

LogicalValue ModalReasoner::queryNestedBelief(const std::string& agent1,
                                               const std::string& agent2,
                                               const Formula& belief) const {
    // What does agent1 believe about agent2's beliefs?
    // This is a simplification
    return LogicalValue(0.5, 0.3);
}

void ModalReasoner::observe(const std::string& agent, const Formula& observation) {
    setBelief(agent, observation, 0.9);
}

void ModalReasoner::reviseBeliefs(const std::string& agent, const Formula& newInfo) {
    // AGM belief revision
    // ... (implementation)
}

//=============================================================================
// TemporalReasoner Implementation
//=============================================================================

TemporalReasoner::TemporalReasoner() = default;
TemporalReasoner::~TemporalReasoner() = default;

void TemporalReasoner::addTemporalFact(const Fact& fact,
                                       uint32_t startTime,
                                       uint32_t endTime) {
    temporalFacts_.push_back({fact, startTime, endTime});
}

LogicalValue TemporalReasoner::queryAtTime(const std::string& predicate,
                                           const std::vector<std::string>& args,
                                           uint32_t time) const {
    for (const auto& tf : temporalFacts_) {
        if (time >= tf.startTime && time <= tf.endTime) {
            if (tf.fact.predicate.name == predicate) {
                // Check args match
                return tf.fact.value;
            }
        }
    }
    return LogicalValue(0.0, 0.0);
}

LogicalValue TemporalReasoner::queryEver(const std::string& predicate,
                                         const std::vector<std::string>& args) const {
    for (const auto& tf : temporalFacts_) {
        if (tf.fact.predicate.name == predicate) {
            return tf.fact.value;
        }
    }
    return LogicalValue(0.0, 0.0);
}

LogicalValue TemporalReasoner::queryAlways(const std::string& predicate,
                                           const std::vector<std::string>& args,
                                           uint32_t startTime,
                                           uint32_t endTime) const {
    // Check if fact held throughout interval
    for (const auto& tf : temporalFacts_) {
        if (tf.fact.predicate.name == predicate &&
            tf.startTime <= startTime &&
            tf.endTime >= endTime) {
            return tf.fact.value;
        }
    }
    return LogicalValue(0.0, 0.5);
}

std::vector<Fact> TemporalReasoner::project(uint32_t futureTime) const {
    std::vector<Fact> projected;
    for (const auto& tf : temporalFacts_) {
        if (tf.endTime >= futureTime) {
            projected.push_back(tf.fact);
        }
    }
    return projected;
}

//=============================================================================
// ReasoningSystem Implementation
//=============================================================================

ReasoningSystem::ReasoningSystem() = default;
ReasoningSystem::~ReasoningSystem() = default;

bool ReasoningSystem::loadRules(const std::string& filename) {
    // Load rules from file (JSON/XML format)
    return true;
}

LogicalValue ReasoningSystem::query(const std::string& queryString) {
    Formula f = parseQuery(queryString);
    Binding binding;
    LogicalValue result;
    logicReasoner_.matchFormula(f, binding, result);
    return result;
}

void ReasoningSystem::assertFact(const std::string& factString) {
    // Parse and add fact
}

std::string ReasoningSystem::explain(const std::string& conclusion) {
    return "Explanation: derived from known facts";
}

std::string ReasoningSystem::processNLQuery(const std::string& query) {
    // Simple pattern matching for natural language
    return formatResult(this->query(query));
}

Formula ReasoningSystem::parseQuery(const std::string& queryString) {
    // Simple parser
    return Formula::atomic(Term::predicate(queryString, {}));
}

std::string ReasoningSystem::formatResult(const LogicalValue& result) {
    std::stringstream ss;
    ss << "Truth: " << result.truth
       << ", Confidence: " << result.confidence;
    return ss.str();
}

} // namespace Reasoning
} // namespace NPC
} // namespace Ultima

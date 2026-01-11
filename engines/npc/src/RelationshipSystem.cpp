/**
 * RelationshipSystem.cpp - Social Dynamics Implementation
 * Stub implementation for Phase 3
 */

#include "social/RelationshipSystem.h"
#include <algorithm>
#include <random>
#include <cmath>

namespace Ultima {
namespace NPC {
namespace Social {

// Relationship Metrics
double Relationship::RelationshipMetrics::getOverallScore() const {
    return (trust * 0.3 + affection * 0.25 + respect * 0.25 + familiarity * 0.2) - fear * 0.1;
}

// Relationship Implementation
Relationship::Relationship() = default;
Relationship::Relationship(const std::string& a, const std::string& b)
    : entityA(a), entityB(b) {}
Relationship::~Relationship() = default;

void Relationship::processInteraction(InteractionType type,
                                      const std::string& initiator,
                                      double intensity) {
    RelationshipMetrics& metrics = (initiator == entityA) ? metricsAtoB : metricsBtoA;
    applyInteractionEffects(type, metrics, intensity);
    interactionCount++;
}

void Relationship::applyInteractionEffects(InteractionType type,
                                           RelationshipMetrics& metrics,
                                           double intensity) {
    switch (type) {
        case InteractionType::Greeting:
            metrics.familiarity += 0.01 * intensity;
            metrics.affection += 0.005 * intensity;
            break;
        case InteractionType::Conversation:
            metrics.familiarity += 0.02 * intensity;
            metrics.trust += 0.01 * intensity;
            break;
        case InteractionType::Help:
            metrics.trust += 0.05 * intensity;
            metrics.affection += 0.03 * intensity;
            metrics.respect += 0.02 * intensity;
            break;
        case InteractionType::Gift:
            metrics.affection += 0.05 * intensity;
            metrics.trust += 0.02 * intensity;
            break;
        case InteractionType::Insult:
            metrics.trust -= 0.05 * intensity;
            metrics.affection -= 0.08 * intensity;
            metrics.respect -= 0.03 * intensity;
            break;
        case InteractionType::Attack:
            metrics.trust -= 0.2 * intensity;
            metrics.affection -= 0.3 * intensity;
            metrics.fear += 0.1 * intensity;
            break;
        case InteractionType::Betray:
            metrics.trust -= 0.5 * intensity;
            metrics.affection -= 0.3 * intensity;
            metrics.respect -= 0.2 * intensity;
            break;
        default:
            break;
    }

    // Clamp values
    metrics.trust = std::clamp(metrics.trust, -1.0, 1.0);
    metrics.affection = std::clamp(metrics.affection, -1.0, 1.0);
    metrics.respect = std::clamp(metrics.respect, -1.0, 1.0);
    metrics.familiarity = std::clamp(metrics.familiarity, 0.0, 1.0);
    metrics.fear = std::clamp(metrics.fear, 0.0, 1.0);
}

void Relationship::updateType() {
    double score = (metricsAtoB.getOverallScore() + metricsBtoA.getOverallScore()) / 2.0;

    if (score < -0.5) {
        type = RelationshipType::Enemy;
    } else if (score < -0.2) {
        type = RelationshipType::Rival;
    } else if (score < 0.1) {
        type = RelationshipType::Acquaintance;
    } else if (score < 0.4) {
        type = RelationshipType::Friend;
    } else if (score < 0.7) {
        type = RelationshipType::CloseFriend;
    } else {
        type = RelationshipType::BestFriend;
    }
}

void Relationship::decay(double deltaTime) {
    double rate = 0.001 * deltaTime;
    metricsAtoB.familiarity *= (1.0 - rate * 0.5);
    metricsBtoA.familiarity *= (1.0 - rate * 0.5);
}

double Relationship::getStrength() const {
    return (std::abs(metricsAtoB.getOverallScore()) +
            std::abs(metricsBtoA.getOverallScore())) / 2.0;
}

bool Relationship::isPositive() const {
    return metricsAtoB.getOverallScore() > 0 && metricsBtoA.getOverallScore() > 0;
}

bool Relationship::isHostile() const {
    return metricsAtoB.getOverallScore() < -0.3 || metricsBtoA.getOverallScore() < -0.3;
}

// SocialNetwork Implementation
SocialNetwork::SocialNetwork() = default;
SocialNetwork::~SocialNetwork() = default;

void SocialNetwork::addEntity(const std::string& entityId, const std::string& name,
                              const std::string& faction) {
    SocialNode node;
    node.entityId = entityId;
    node.name = name;
    node.faction = faction;
    nodes_[entityId] = node;
}

void SocialNetwork::removeEntity(const std::string& entityId) {
    nodes_.erase(entityId);
    edges_.erase(
        std::remove_if(edges_.begin(), edges_.end(),
                      [&](const SocialEdge& e) {
                          return e.from == entityId || e.to == entityId;
                      }),
        edges_.end()
    );
}

std::vector<std::string> SocialNetwork::getConnections(const std::string& entityId,
                                                       double minStrength) const {
    std::vector<std::string> result;
    for (const auto& edge : edges_) {
        if (edge.from == entityId && edge.strength >= minStrength) {
            result.push_back(edge.to);
        } else if (edge.to == entityId && edge.isBidirectional && edge.strength >= minStrength) {
            result.push_back(edge.from);
        }
    }
    return result;
}

// Faction Implementation
Faction::Faction() = default;
Faction::Faction(const std::string& i, const std::string& n) : id(i), name(n) {}
Faction::~Faction() = default;

void Faction::addMember(const std::string& entityId, const std::string& rank) {
    Member m;
    m.entityId = entityId;
    m.rank = rank;
    members.push_back(m);
}

void Faction::removeMember(const std::string& entityId) {
    members.erase(
        std::remove_if(members.begin(), members.end(),
                      [&](const Member& m) { return m.entityId == entityId; }),
        members.end()
    );
}

bool Faction::hasMember(const std::string& entityId) const {
    return std::any_of(members.begin(), members.end(),
                      [&](const Member& m) { return m.entityId == entityId; });
}

// ReputationSystem Implementation
ReputationSystem::ReputationSystem() = default;
ReputationSystem::~ReputationSystem() = default;

double ReputationSystem::getReputation(const std::string& entityId, Category category) const {
    auto it = reputations_.find(entityId);
    if (it == reputations_.end()) return 0.5;
    auto catIt = it->second.categories.find(category);
    return catIt != it->second.categories.end() ? catIt->second : 0.5;
}

void ReputationSystem::modifyReputation(const std::string& entityId, Category category,
                                        double delta, const std::string& location) {
    reputations_[entityId].categories[category] =
        std::clamp(reputations_[entityId].categories[category] + delta, -1.0, 1.0);
}

// EmotionalStateMachine Implementation
EmotionalStateMachine::EmotionalStateMachine() {
    initializeTransitions();
}

EmotionalStateMachine::~EmotionalStateMachine() = default;

void EmotionalStateMachine::initializeTransitions() {
    // Define state transitions based on interactions
    transitions_[State::Neutral][InteractionType::Compliment] = State::Happy;
    transitions_[State::Neutral][InteractionType::Insult] = State::Angry;
    transitions_[State::Happy][InteractionType::Insult] = State::Sad;
    transitions_[State::Angry][InteractionType::Help] = State::Grateful;
}

void EmotionalStateMachine::processEvent(InteractionType interaction, double intensity,
                                         const std::string& otherEntity) {
    auto stateIt = transitions_.find(currentState_);
    if (stateIt != transitions_.end()) {
        auto transIt = stateIt->second.find(interaction);
        if (transIt != stateIt->second.end()) {
            currentState_ = transIt->second;
            intensity_ = intensity;
        }
    }
}

void EmotionalStateMachine::update(double deltaTime) {
    intensity_ *= (1.0 - 0.01 * deltaTime);
    if (intensity_ < 0.1) {
        currentState_ = State::Neutral;
        intensity_ = 0.0;
    }
}

// RelationshipSystem Implementation
RelationshipSystem::RelationshipSystem() = default;
RelationshipSystem::~RelationshipSystem() = default;

void RelationshipSystem::initialize() {}

void RelationshipSystem::registerEntity(const std::string& entityId,
                                        const std::string& name,
                                        const std::string& faction) {
    network_.addEntity(entityId, name, faction);
}

Relationship& RelationshipSystem::getRelationship(const std::string& entityA,
                                                  const std::string& entityB) {
    auto key = std::make_pair(std::min(entityA, entityB), std::max(entityA, entityB));
    if (!relationships_.count(key)) {
        relationships_[key] = Relationship(entityA, entityB);
    }
    return relationships_[key];
}

void RelationshipSystem::processInteraction(const std::string& entityA,
                                           const std::string& entityB,
                                           InteractionType type,
                                           double intensity) {
    auto& rel = getRelationship(entityA, entityB);
    rel.processInteraction(type, entityA, intensity);
    rel.updateType();
}

void RelationshipSystem::update(double deltaTime) {
    for (auto& [key, rel] : relationships_) {
        rel.decay(deltaTime);
    }
}

Faction* RelationshipSystem::getFaction(const std::string& factionId) {
    auto it = factions_.find(factionId);
    return it != factions_.end() ? &it->second : nullptr;
}

void RelationshipSystem::createFaction(const Faction& faction) {
    factions_[faction.id] = faction;
}

} // namespace Social
} // namespace NPC
} // namespace Ultima

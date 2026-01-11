/**
 * MemorySystem.cpp - NPC Memory System Implementation
 * Stub implementation for Phase 2
 */

#include "memory/MemorySystem.h"
#include <algorithm>
#include <random>
#include <cmath>

namespace Ultima {
namespace NPC {
namespace Memory {

// Implementation stubs - full implementation would follow header definitions

double MemoryItem::getRetrievalProbability(uint32_t currentTime) const {
    double timeFactor = 1.0 / (1.0 + 0.001 * (currentTime - lastAccess));
    return strength * clarity * timeFactor;
}

double MemoryItem::getRelevance(const std::vector<std::string>& contextTags,
                                const std::string& contextEntity) const {
    double relevance = 0.0;
    for (const auto& tag : tags) {
        for (const auto& ctx : contextTags) {
            if (tag == ctx) relevance += 0.2;
        }
    }
    if (!contextEntity.empty() && associations.count(contextEntity)) {
        relevance += associations.at(contextEntity);
    }
    return std::min(1.0, relevance);
}

WorkingMemory::WorkingMemory(size_t capacity) : capacity_(capacity) {}
WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::add(const std::string& item, double salience) {
    // Remove if exists
    remove(item);

    // Add new item
    items_.push_back({item, salience, 0});

    // Remove oldest if over capacity
    if (items_.size() > capacity_) {
        items_.erase(items_.begin());
    }
}

void WorkingMemory::remove(const std::string& item) {
    items_.erase(
        std::remove_if(items_.begin(), items_.end(),
                      [&](const WorkingMemoryItem& i) { return i.content == item; }),
        items_.end()
    );
}

bool WorkingMemory::contains(const std::string& item) const {
    return std::any_of(items_.begin(), items_.end(),
                      [&](const WorkingMemoryItem& i) { return i.content == item; });
}

std::string WorkingMemory::getFocus() const {
    return currentFocus_;
}

void WorkingMemory::setFocus(const std::string& item) {
    if (contains(item)) {
        currentFocus_ = item;
    }
}

std::vector<std::string> WorkingMemory::getItems() const {
    std::vector<std::string> result;
    for (const auto& item : items_) {
        result.push_back(item.content);
    }
    return result;
}

void WorkingMemory::decay(double deltaTime, double decayRate) {
    for (auto& item : items_) {
        item.activation *= (1.0 - decayRate * deltaTime);
    }
    items_.erase(
        std::remove_if(items_.begin(), items_.end(),
                      [](const WorkingMemoryItem& i) { return i.activation < 0.1; }),
        items_.end()
    );
}

void WorkingMemory::clear() {
    items_.clear();
    currentFocus_.clear();
}

double WorkingMemory::getCognitiveLoad() const {
    return static_cast<double>(items_.size()) / static_cast<double>(capacity_);
}

LongTermMemory::LongTermMemory() = default;
LongTermMemory::~LongTermMemory() = default;

std::string LongTermMemory::generateId() {
    static int counter = 0;
    return "mem_" + std::to_string(++counter);
}

std::string LongTermMemory::store(const MemoryItem& memory) {
    std::string id = memory.id.empty() ? generateId() : memory.id;
    memories_[id] = std::make_unique<MemoryItem>(memory);
    indexMemory(id, memory);
    return id;
}

void LongTermMemory::indexMemory(const std::string& id, const MemoryItem& memory) {
    for (const auto& tag : memory.tags) {
        tagIndex_.insert({tag, id});
    }
    if (!memory.location.empty()) {
        locationIndex_.insert({memory.location, id});
    }
}

MemorySystem::MemorySystem() = default;
MemorySystem::~MemorySystem() = default;

void MemorySystem::initialize(const std::string& npcId) {
    npcId_ = npcId;
}

void MemorySystem::experienceEvent(const std::string& eventType,
                                   const std::vector<std::string>& participants,
                                   const std::string& outcome,
                                   double emotionalImpact,
                                   const std::string& location) {
    EpisodicMemory mem;
    mem.eventType = eventType;
    mem.participants = participants;
    mem.outcome = outcome;
    mem.location = location;
    mem.emotionalValence = emotionalImpact;
    mem.emotionalIntensity = std::abs(emotionalImpact);
    mem.strength = 0.5 + std::abs(emotionalImpact) * 0.5;

    longTermMemory_.store(mem);
}

void MemorySystem::learnFact(const std::string& subject,
                            const std::string& predicate,
                            const std::string& object,
                            const std::string& source) {
    SemanticMemory mem;
    mem.subject = subject;
    mem.predicate = predicate;
    mem.object = object;
    mem.source = source;

    longTermMemory_.store(mem);
}

void MemorySystem::update(uint32_t currentTime, double deltaTime) {
    currentTime_ = currentTime;
    workingMemory_.decay(deltaTime);
}

MemoryNetwork::MemoryNetwork() = default;
MemoryNetwork::~MemoryNetwork() = default;

void MemoryNetwork::addNode(const std::string& nodeId, double baseActivation) {
    nodes_[nodeId] = {nodeId, baseActivation, baseActivation, {}};
}

void MemoryNetwork::addAssociation(const std::string& from, const std::string& to,
                                   double strength) {
    if (nodes_.count(from)) {
        nodes_[from].connections[to] = strength;
    }
}

void MemoryNetwork::activate(const std::string& nodeId, double amount) {
    if (nodes_.count(nodeId)) {
        nodes_[nodeId].currentActivation += amount;
    }
}

void MemoryNetwork::spreadActivation(int iterations, double decayFactor) {
    for (int i = 0; i < iterations; ++i) {
        std::map<std::string, double> newActivations;

        for (const auto& [id, node] : nodes_) {
            double incoming = 0.0;
            for (const auto& [otherId, otherNode] : nodes_) {
                if (otherNode.connections.count(id)) {
                    incoming += otherNode.currentActivation *
                               otherNode.connections.at(id) * decayFactor;
                }
            }
            newActivations[id] = node.currentActivation * decayFactor + incoming;
        }

        for (auto& [id, node] : nodes_) {
            node.currentActivation = newActivations[id];
        }
    }
}

std::vector<std::pair<std::string, double>> MemoryNetwork::getMostActive(int count) const {
    std::vector<std::pair<std::string, double>> results;
    for (const auto& [id, node] : nodes_) {
        results.push_back({id, node.currentActivation});
    }
    std::sort(results.begin(), results.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    if (results.size() > static_cast<size_t>(count)) {
        results.resize(count);
    }
    return results;
}

void MemoryNetwork::reset() {
    for (auto& [id, node] : nodes_) {
        node.currentActivation = node.baseActivation;
    }
}

} // namespace Memory
} // namespace NPC
} // namespace Ultima

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

std::string LongTermMemory::storeEpisodic(const EpisodicMemory& memory) {
    return store(memory);
}

std::optional<MemoryItem> LongTermMemory::retrieve(const std::string& id) {
    auto it = memories_.find(id);
    if (it != memories_.end()) {
        return *it->second;
    }
    return std::nullopt;
}

double LongTermMemory::getSkillLevel(const std::string& skillName) const {
    // Search for procedural memory with this skill name
    for (const auto& [id, mem] : memories_) {
        if (mem->type == MemoryType::Procedural) {
            // Check if skill name is in tags
            for (const auto& tag : mem->tags) {
                if (tag == skillName) {
                    return mem->strength;
                }
            }
        }
    }
    return 0.0;  // Skill not found
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

void MemorySystem::practiceSkill(const std::string& skillName, double outcome) {
    // Create or update procedural memory for this skill
    ProceduralMemory mem;
    mem.skillName = skillName;
    mem.type = MemoryType::Procedural;
    mem.tags.push_back(skillName);
    mem.strength = std::min(1.0, mem.strength + outcome * 0.1);
    
    longTermMemory_.storeProcedural(mem);
}

std::vector<MemoryItem> MemorySystem::remember(const RetrievalCue& cue) {
    return longTermMemory_.search(cue);
}

std::vector<MemoryItem> MemorySystem::rememberAbout(const std::string& entity, int maxResults) {
    RetrievalCue cue;
    cue.tags.push_back(entity);
    cue.entityContext = entity;
    cue.maxResults = maxResults;
    return remember(cue);
}

bool MemorySystem::knowsFact(const std::string& subject, 
                             const std::string& predicate,
                             const std::string& object) const {
    auto facts = longTermMemory_.queryFacts(subject, predicate, object);
    return !facts.empty();
}

double MemorySystem::getSkillLevel(const std::string& skillName) const {
    return longTermMemory_.getSkillLevel(skillName);
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

// Missing stub implementations
AutobiographicalMemory::AutobiographicalMemory() = default;
AutobiographicalMemory::~AutobiographicalMemory() = default;

// Additional stub implementations for LongTermMemory
std::string LongTermMemory::storeSemantic(const SemanticMemory& memory) {
    return store(memory);
}

std::string LongTermMemory::storeProcedural(const ProceduralMemory& memory) {
    return store(memory);
}

std::vector<MemoryItem> LongTermMemory::search(const RetrievalCue& cue) {
    std::vector<MemoryItem> results;
    
    for (const auto& [id, mem] : memories_) {
        // Filter by type if specified
        if (cue.filterByType && mem->type != cue.typeFilter) {
            continue;
        }
        
        // Check strength threshold
        if (mem->strength < cue.minStrength) {
            continue;
        }
        
        // Check relevance to context
        double relevance = mem->getRelevance(cue.tags, cue.entityContext);
        if (relevance < cue.minRelevance) {
            continue;
        }
        
        results.push_back(*mem);
        
        if (results.size() >= static_cast<size_t>(cue.maxResults)) {
            break;
        }
    }
    
    return results;
}

std::vector<MemoryItem> LongTermMemory::associate(const std::string& memoryId, int depth) {
    std::vector<MemoryItem> results;
    auto mem = retrieve(memoryId);
    if (!mem) {
        return results;
    }
    
    // Find associated memories
    for (const auto& relatedId : mem->relatedMemories) {
        auto related = retrieve(relatedId);
        if (related) {
            results.push_back(*related);
        }
    }
    
    return results;
}

std::vector<SemanticMemory> LongTermMemory::queryFacts(const std::string& subject,
                                                        const std::string& predicate,
                                                        const std::string& object) const {
    std::vector<SemanticMemory> results;
    
    for (const auto& [id, mem] : memories_) {
        if (mem->type != MemoryType::Semantic) {
            continue;
        }
        
        // Cast to SemanticMemory
        const SemanticMemory* semMem = static_cast<const SemanticMemory*>(mem.get());
        
        // Match subject, predicate, object (wildcard "*" matches all)
        bool match = true;
        if (subject != "*" && semMem->subject != subject) match = false;
        if (predicate != "*" && semMem->predicate != predicate) match = false;
        if (object != "*" && semMem->object != object) match = false;
        
        if (match) {
            results.push_back(*semMem);
        }
    }
    
    return results;
}

void LongTermMemory::recordAccess(const std::string& memoryId, uint32_t currentTime) {
    auto it = memories_.find(memoryId);
    if (it != memories_.end()) {
        it->second->lastAccess = currentTime;
        it->second->encoding.rehearsals += 1;
    }
}

void LongTermMemory::consolidate(uint32_t currentTime) {
    // Strengthen recently accessed or emotionally significant memories
    for (auto& [id, mem] : memories_) {
        double timeSinceAccess = currentTime - mem->lastAccess;
        if (timeSinceAccess < 1000) {  // Recently accessed
            mem->strength = std::min(1.0, mem->strength * 1.05);
        }
        if (mem->emotionalIntensity > 0.5) {  // Emotionally significant
            mem->strength = std::min(1.0, mem->strength * 1.03);
        }
    }
}

} // namespace Memory
} // namespace NPC
} // namespace Ultima

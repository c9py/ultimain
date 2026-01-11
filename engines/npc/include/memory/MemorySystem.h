/**
 * MemorySystem.h - NPC Memory and Knowledge System
 *
 * Implements cognitive memory architecture for NPCs including:
 * - Episodic memory (events, experiences)
 * - Semantic memory (facts, knowledge)
 * - Procedural memory (skills, habits)
 * - Working memory (current context)
 *
 * Part of Phase 2: Reasoning
 */

#ifndef ULTIMA_NPC_MEMORY_SYSTEM_H
#define ULTIMA_NPC_MEMORY_SYSTEM_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <queue>
#include <chrono>
#include <optional>
#include <functional>

namespace Ultima {
namespace NPC {
namespace Memory {

/**
 * Memory types
 */
enum class MemoryType {
    Episodic,       // Specific events/experiences
    Semantic,       // Facts and knowledge
    Procedural,     // Skills and how-to knowledge
    Emotional,      // Emotional associations
    Autobiographic  // Self-related memories
};

/**
 * Memory encoding strength factors
 */
struct EncodingFactors {
    double attention = 1.0;         // How much attention was paid
    double emotionalArousal = 0.0;  // Emotional intensity at encoding
    double novelty = 0.5;           // How novel/surprising
    double selfRelevance = 0.5;     // Relevance to self
    double rehearsals = 0;          // Times rehearsed/recalled
};

/**
 * Base memory item
 */
struct MemoryItem {
    std::string id;
    MemoryType type;
    std::string content;            // Description of memory
    uint32_t encodingTime;          // When memory was formed
    uint32_t lastAccess;            // Last retrieval time

    // Memory strength
    double strength = 1.0;          // Overall memory strength
    double clarity = 1.0;           // How clear/detailed
    double confidence = 1.0;        // Confidence in accuracy

    // Associations
    std::vector<std::string> tags;
    std::map<std::string, double> associations;  // entity -> association strength
    std::string location;           // Where it happened
    std::vector<std::string> relatedMemories;

    // Emotional coloring
    double emotionalValence = 0.0;  // -1 (negative) to 1 (positive)
    double emotionalIntensity = 0.0;

    // Encoding factors
    EncodingFactors encoding;

    /**
     * Calculate retrieval probability
     */
    double getRetrievalProbability(uint32_t currentTime) const;

    /**
     * Calculate relevance to context
     */
    double getRelevance(const std::vector<std::string>& contextTags,
                       const std::string& contextEntity = "") const;
};

/**
 * Episodic Memory - Stores specific events/experiences
 */
struct EpisodicMemory : public MemoryItem {
    // Event details
    std::string eventType;          // "conversation", "combat", "trade", etc.
    std::vector<std::string> participants;
    std::string outcome;            // What resulted
    uint32_t duration;              // How long event lasted

    // Perspective
    bool firstPerson = true;        // Own experience vs observed
    std::string perspective;        // POV if observed

    EpisodicMemory() { type = MemoryType::Episodic; }
};

/**
 * Semantic Memory - Stores facts and knowledge
 */
struct SemanticMemory : public MemoryItem {
    // Fact structure
    std::string subject;
    std::string predicate;
    std::string object;

    // Knowledge metadata
    std::string source;             // Where learned from
    std::string category;           // "geography", "people", "items", etc.
    bool isInferred = false;        // Derived vs directly learned

    SemanticMemory() { type = MemoryType::Semantic; }
};

/**
 * Procedural Memory - Stores skills and how-to
 */
struct ProceduralMemory : public MemoryItem {
    std::string skillName;
    double proficiency = 0.0;       // 0 (novice) to 1 (expert)
    uint32_t practiceCount = 0;     // Times practiced
    uint32_t lastPractice = 0;

    // Skill components
    std::vector<std::string> steps;
    std::vector<std::string> prerequisites;

    ProceduralMemory() { type = MemoryType::Procedural; }

    /**
     * Update skill after practice
     */
    void practice(double outcome, uint32_t currentTime);
};

/**
 * Working Memory - Current active context
 */
class WorkingMemory {
public:
    static constexpr size_t DEFAULT_CAPACITY = 7;  // Miller's magic number

    WorkingMemory(size_t capacity = DEFAULT_CAPACITY);
    ~WorkingMemory();

    /**
     * Add item to working memory
     * May displace oldest item if at capacity
     */
    void add(const std::string& item, double salience = 1.0);

    /**
     * Remove item
     */
    void remove(const std::string& item);

    /**
     * Check if item is in working memory
     */
    bool contains(const std::string& item) const;

    /**
     * Get current focus
     */
    std::string getFocus() const;

    /**
     * Set focus to specific item
     */
    void setFocus(const std::string& item);

    /**
     * Get all items
     */
    std::vector<std::string> getItems() const;

    /**
     * Decay items over time
     */
    void decay(double deltaTime, double decayRate = 0.1);

    /**
     * Clear working memory
     */
    void clear();

    /**
     * Get cognitive load (0-1)
     */
    double getCognitiveLoad() const;

private:
    struct WorkingMemoryItem {
        std::string content;
        double activation;
        uint32_t addTime;
    };

    size_t capacity_;
    std::vector<WorkingMemoryItem> items_;
    std::string currentFocus_;
};

/**
 * Memory Retrieval Cue
 */
struct RetrievalCue {
    std::vector<std::string> tags;
    std::string entityContext;
    std::string locationContext;
    std::string emotionalContext;
    uint32_t timeContext = 0;
    MemoryType typeFilter = MemoryType::Episodic;
    bool filterByType = false;

    double minStrength = 0.0;
    double minRelevance = 0.0;
    int maxResults = 10;
};

/**
 * Memory consolidation event
 */
struct ConsolidationEvent {
    std::string memoryId;
    double strengthIncrease;
    uint32_t timestamp;
};

/**
 * Long Term Memory Store
 */
class LongTermMemory {
public:
    LongTermMemory();
    ~LongTermMemory();

    /**
     * Store new memory
     */
    std::string store(const MemoryItem& memory);
    std::string storeEpisodic(const EpisodicMemory& memory);
    std::string storeSemantic(const SemanticMemory& memory);
    std::string storeProcedural(const ProceduralMemory& memory);

    /**
     * Retrieve memory by ID
     */
    std::optional<MemoryItem> retrieve(const std::string& id);

    /**
     * Search memories with cue
     */
    std::vector<MemoryItem> search(const RetrievalCue& cue);

    /**
     * Free association - find related memories
     */
    std::vector<MemoryItem> associate(const std::string& memoryId, int depth = 1);

    /**
     * Query semantic knowledge
     */
    std::vector<SemanticMemory> queryFacts(const std::string& subject = "*",
                                            const std::string& predicate = "*",
                                            const std::string& object = "*");

    /**
     * Get skill proficiency
     */
    double getSkillLevel(const std::string& skillName) const;

    /**
     * Update memory after access
     */
    void recordAccess(const std::string& memoryId, uint32_t currentTime);

    /**
     * Consolidation - strengthen important memories
     */
    void consolidate(uint32_t currentTime);

    /**
     * Forgetting - decay and remove weak memories
     */
    void forget(uint32_t currentTime, double forgetThreshold = 0.1);

    /**
     * Memory reconstruction - may introduce errors
     */
    MemoryItem reconstruct(const std::string& memoryId);

    /**
     * Save/load memory store
     */
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);

    /**
     * Statistics
     */
    size_t getMemoryCount() const { return memories_.size(); }
    size_t getEpisodicCount() const;
    size_t getSemanticCount() const;
    size_t getProceduralCount() const;

private:
    std::map<std::string, std::unique_ptr<MemoryItem>> memories_;

    // Indexes for efficient retrieval
    std::multimap<std::string, std::string> tagIndex_;
    std::multimap<std::string, std::string> entityIndex_;
    std::multimap<std::string, std::string> locationIndex_;
    std::map<std::string, std::string> skillIndex_;  // skill name -> memory id

    std::string generateId();
    void indexMemory(const std::string& id, const MemoryItem& memory);
    void removeFromIndex(const std::string& id, const MemoryItem& memory);
};

/**
 * Autobiographical Memory - Self-narrative
 */
class AutobiographicalMemory {
public:
    AutobiographicalMemory();
    ~AutobiographicalMemory();

    /**
     * Add life event
     */
    void addLifeEvent(const EpisodicMemory& event, double significance);

    /**
     * Get life story summary
     */
    std::string getLifeSummary() const;

    /**
     * Get significant memories
     */
    std::vector<EpisodicMemory> getSignificantMemories(int count = 10) const;

    /**
     * Get memories by life period
     */
    std::vector<EpisodicMemory> getMemoriesByPeriod(uint32_t startTime,
                                                     uint32_t endTime) const;

    /**
     * Self-concept - beliefs about self
     */
    void addSelfBelief(const std::string& belief, double confidence);
    std::vector<std::pair<std::string, double>> getSelfBeliefs() const;

private:
    struct LifeEvent {
        EpisodicMemory memory;
        double significance;
        std::string lifePeriod;
    };

    std::vector<LifeEvent> lifeEvents_;
    std::map<std::string, double> selfBeliefs_;
};

/**
 * Memory System - Integrates all memory components
 */
class MemorySystem {
public:
    MemorySystem();
    ~MemorySystem();

    /**
     * Initialize for NPC
     */
    void initialize(const std::string& npcId);

    /**
     * Experience event (creates episodic memory)
     */
    void experienceEvent(const std::string& eventType,
                        const std::vector<std::string>& participants,
                        const std::string& outcome,
                        double emotionalImpact,
                        const std::string& location);

    /**
     * Learn fact (creates semantic memory)
     */
    void learnFact(const std::string& subject,
                  const std::string& predicate,
                  const std::string& object,
                  const std::string& source = "observation");

    /**
     * Practice skill (creates/updates procedural memory)
     */
    void practiceSkill(const std::string& skillName,
                      double outcome);

    /**
     * Remember - retrieve relevant memories
     */
    std::vector<MemoryItem> remember(const RetrievalCue& cue);

    /**
     * Remember about entity
     */
    std::vector<MemoryItem> rememberAbout(const std::string& entity,
                                          int maxResults = 5);

    /**
     * Check if knows fact
     */
    bool knowsFact(const std::string& subject,
                  const std::string& predicate,
                  const std::string& object) const;

    /**
     * Get skill level
     */
    double getSkillLevel(const std::string& skillName) const;

    /**
     * Working memory access
     */
    WorkingMemory& getWorkingMemory() { return workingMemory_; }

    /**
     * Update system (call each tick)
     */
    void update(uint32_t currentTime, double deltaTime);

    /**
     * Get emotional memory of entity
     */
    double getEmotionalMemory(const std::string& entity) const;

    /**
     * Save/load
     */
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);

private:
    std::string npcId_;
    uint32_t currentTime_ = 0;

    WorkingMemory workingMemory_;
    LongTermMemory longTermMemory_;
    AutobiographicalMemory autobiographicalMemory_;

    // Emotional associations with entities
    std::map<std::string, double> entityEmotions_;

    // Memory formation parameters
    double encodingThreshold_ = 0.3;    // Min strength to form memory
    double consolidationRate_ = 0.01;   // How fast memories consolidate
    double forgettingRate_ = 0.001;     // Base forgetting rate

    void processWorkingMemory();
    EncodingFactors calculateEncodingFactors(double emotionalImpact,
                                             double novelty) const;
};

/**
 * Memory Network - Spreading activation model
 */
class MemoryNetwork {
public:
    MemoryNetwork();
    ~MemoryNetwork();

    /**
     * Add node
     */
    void addNode(const std::string& nodeId, double baseActivation = 0.5);

    /**
     * Add association between nodes
     */
    void addAssociation(const std::string& from, const std::string& to,
                       double strength = 0.5);

    /**
     * Activate node
     */
    void activate(const std::string& nodeId, double amount = 1.0);

    /**
     * Spread activation through network
     */
    void spreadActivation(int iterations = 3, double decayFactor = 0.7);

    /**
     * Get most activated nodes
     */
    std::vector<std::pair<std::string, double>> getMostActive(int count = 5) const;

    /**
     * Reset activations
     */
    void reset();

private:
    struct Node {
        std::string id;
        double baseActivation;
        double currentActivation;
        std::map<std::string, double> connections;
    };

    std::map<std::string, Node> nodes_;
};

} // namespace Memory
} // namespace NPC
} // namespace Ultima

#endif // ULTIMA_NPC_MEMORY_SYSTEM_H

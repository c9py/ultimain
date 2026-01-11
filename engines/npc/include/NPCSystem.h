/**
 * NPCSystem.h - Unified NPC AI System Header
 *
 * Main integration point for the Ultima NPC AI system.
 * Combines all subsystems into a cohesive NPC management framework.
 *
 * Part of Phase 6: Integration
 */

#ifndef ULTIMA_NPC_SYSTEM_H
#define ULTIMA_NPC_SYSTEM_H

// Phase 1: Foundation
#include "neural/NeuralNetwork.h"
#include "aiml/AIMLEngine.h"
#include "persona/Persona.h"

// Phase 2: Reasoning
#include "reasoning/TensorLogic.h"
#include "reasoning/RuleSet.h"
#include "memory/MemorySystem.h"

// Phase 3: Social Dynamics
#include "social/RelationshipSystem.h"

// Phase 4: Economy
#include "economy/EconomicSystem.h"

// Phase 5: Urban Dynamics
#include "urban/UrbanDynamics.h"

#include <memory>
#include <string>
#include <vector>
#include <map>

namespace Ultima {
namespace NPC {

/**
 * NPC Entity - Complete NPC with all subsystems
 */
class NPCEntity {
public:
    NPCEntity(const std::string& id);
    ~NPCEntity();

    std::string getId() const { return id_; }

    // Subsystem access
    Persona::NPCPersona& getPersona() { return *persona_; }
    Memory::MemorySystem& getMemory() { return *memory_; }
    Neural::NPCLearningNetwork& getLearning() { return *learning_; }
    Economy::EconomicAgent& getEconomicAgent() { return *economicAgent_; }

    // State
    bool isAlive() const { return isAlive_; }
    bool isConscious() const { return isConscious_; }
    std::string getCurrentLocation() const { return currentLocation_; }
    std::string getCurrentActivity() const { return currentActivity_; }

    void setLocation(const std::string& location) { currentLocation_ = location; }
    void setActivity(const std::string& activity) { currentActivity_ = activity; }

    // Update
    void update(double deltaTime);

    // Dialogue
    std::string respondToDialogue(const std::string& input);

    // Decision making
    struct Decision {
        std::string action;
        std::string target;
        double confidence;
        std::string reasoning;
    };
    Decision makeDecision(const std::vector<std::string>& options);

    // Serialization
    std::string serialize() const;
    bool deserialize(const std::string& data);

private:
    std::string id_;
    bool isAlive_ = true;
    bool isConscious_ = true;
    std::string currentLocation_;
    std::string currentActivity_;

    std::unique_ptr<Persona::NPCPersona> persona_;
    std::unique_ptr<Memory::MemorySystem> memory_;
    std::unique_ptr<Neural::NPCLearningNetwork> learning_;
    std::unique_ptr<Economy::EconomicAgent> economicAgent_;
    std::unique_ptr<AIML::SessionContext> dialogueContext_;
};

/**
 * NPC Manager - Manages all NPCs in the world
 */
class NPCManager {
public:
    NPCManager();
    ~NPCManager();

    /**
     * Initialize the NPC system
     */
    bool initialize(const std::string& dataDirectory);

    /**
     * Create new NPC
     */
    NPCEntity* createNPC(const std::string& id, const std::string& templateId = "");

    /**
     * Get NPC by ID
     */
    NPCEntity* getNPC(const std::string& id);

    /**
     * Remove NPC
     */
    void removeNPC(const std::string& id);

    /**
     * Get all NPC IDs
     */
    std::vector<std::string> getAllNPCIds() const;

    /**
     * Get NPCs at location
     */
    std::vector<NPCEntity*> getNPCsAtLocation(const std::string& location);

    /**
     * Update all NPCs
     */
    void update(double deltaTime);

    /**
     * Process NPC interaction
     */
    void processInteraction(const std::string& npcA, const std::string& npcB,
                           Social::InteractionType type, double intensity = 1.0);

    /**
     * Access subsystems
     */
    AIML::NPCDialogueManager& getDialogueManager() { return *dialogueManager_; }
    Social::RelationshipSystem& getRelationshipSystem() { return *relationshipSystem_; }
    Economy::EconomicSystem& getEconomicSystem() { return *economicSystem_; }
    Reasoning::RuleEngine& getRuleEngine() { return *ruleEngine_; }
    Urban::UrbanSimulation& getUrbanSimulation() { return *urbanSimulation_; }

    /**
     * Save/load world state
     */
    bool saveWorld(const std::string& filename) const;
    bool loadWorld(const std::string& filename);

    /**
     * Statistics
     */
    size_t getNPCCount() const { return npcs_.size(); }
    double getAverageHappiness() const;
    double getAverageWealth() const;

private:
    std::map<std::string, std::unique_ptr<NPCEntity>> npcs_;

    // Shared systems
    std::unique_ptr<AIML::AIMLEngine> aimlEngine_;
    std::unique_ptr<AIML::NPCDialogueManager> dialogueManager_;
    std::unique_ptr<Social::RelationshipSystem> relationshipSystem_;
    std::unique_ptr<Economy::EconomicSystem> economicSystem_;
    std::unique_ptr<Reasoning::RuleSet> ruleSet_;
    std::unique_ptr<Reasoning::RuleContext> ruleContext_;
    std::unique_ptr<Reasoning::RuleEngine> ruleEngine_;
    std::unique_ptr<Reasoning::ReasoningSystem> reasoningSystem_;
    std::unique_ptr<Urban::UrbanSimulation> urbanSimulation_;
    std::unique_ptr<Persona::PersonaFactory> personaFactory_;

    void initializeSubsystems(const std::string& dataDirectory);
    void setupRuleContext();
};

/**
 * Configuration for NPC system
 */
struct NPCSystemConfig {
    // Data paths
    std::string aimlDirectory = "data/aiml";
    std::string rulesFile = "data/rules.json";
    std::string templatesFile = "data/templates.json";

    // Simulation
    double updateInterval = 1.0;        // Seconds between updates
    int maxActiveNPCs = 100;            // Max NPCs to simulate simultaneously
    double relationshipDecayRate = 0.001;
    double memoryDecayRate = 0.0001;

    // AI
    bool enableLearning = true;
    bool enableEmotions = true;
    bool enableGossip = true;
    bool enableEconomy = true;

    // Performance
    bool asyncUpdates = false;
    int workerThreads = 1;
};

/**
 * World Events - Events that affect NPCs
 */
namespace Events {

    enum class EventType {
        // Natural
        DayStart,
        DayEnd,
        SeasonChange,
        Weather,
        Disaster,

        // Social
        Festival,
        Wedding,
        Funeral,
        Battle,
        Crime,

        // Economic
        MarketOpen,
        MarketClose,
        PriceChange,
        Shortage,
        Surplus,

        // Political
        NewLaw,
        TaxChange,
        WarDeclared,
        PeaceTreaty,

        // Personal
        Birth,
        Death,
        Marriage,
        Divorce,
        Achievement
    };

    struct WorldEvent {
        EventType type;
        std::string description;
        std::string location;
        std::vector<std::string> involvedEntities;
        uint32_t timestamp;
        double magnitude = 1.0;
    };

    /**
     * Broadcast event to all NPCs in range
     */
    void broadcastEvent(NPCManager& manager, const WorldEvent& event);

    /**
     * Notify specific NPC of event
     */
    void notifyNPC(NPCEntity& npc, const WorldEvent& event);

} // namespace Events

/**
 * Utility functions
 */
namespace Utils {

    /**
     * Generate random name
     */
    std::string generateName(bool male = true);

    /**
     * Generate random personality
     */
    Persona::BigFiveTraits generatePersonality();

    /**
     * Calculate distance between entities
     */
    double calculateDistance(const std::string& locationA, const std::string& locationB);

    /**
     * Check line of sight
     */
    bool hasLineOfSight(const std::string& locationA, const std::string& locationB);

    /**
     * Get time of day
     */
    std::string getTimeOfDay(uint32_t gameTime);

    /**
     * Convert game time to string
     */
    std::string formatGameTime(uint32_t gameTime);

} // namespace Utils

} // namespace NPC
} // namespace Ultima

#endif // ULTIMA_NPC_SYSTEM_H

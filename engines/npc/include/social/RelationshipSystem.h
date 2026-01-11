/**
 * RelationshipSystem.h - NPC Relationship and Social Network System
 *
 * Implements the Dream Vortex relationship system for Ultima NPCs including:
 * - Individual relationships with dynamics
 * - Social network graph
 * - Group/faction systems
 * - Reputation and social standing
 *
 * Part of Phase 3: Social Dynamics
 */

#ifndef ULTIMA_NPC_RELATIONSHIP_SYSTEM_H
#define ULTIMA_NPC_RELATIONSHIP_SYSTEM_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <optional>

namespace Ultima {
namespace NPC {
namespace Social {

/**
 * Relationship type
 */
enum class RelationshipType {
    Stranger,
    Acquaintance,
    Friend,
    CloseFriend,
    BestFriend,
    Romantic,
    Spouse,
    Family,
    Rival,
    Enemy,
    NemeSIS,
    Mentor,
    Student,
    Employer,
    Employee,
    Business,
    Ally,
    Neutral
};

/**
 * Interaction type
 */
enum class InteractionType {
    // Positive
    Greeting,
    Conversation,
    DeepConversation,
    Help,
    Gift,
    Compliment,
    Share,
    Defend,
    Heal,
    Trade,
    Celebrate,

    // Neutral
    Observe,
    Ignore,
    Negotiate,
    Inquire,

    // Negative
    Insult,
    Threaten,
    Attack,
    Steal,
    Betray,
    Lie,
    Compete,
    Humiliate,

    // Special
    Romance,
    Propose,
    Reject,
    Reconcile
};

/**
 * Relationship event
 */
struct RelationshipEvent {
    std::string id;
    InteractionType type;
    uint32_t timestamp;
    std::string description;

    // Impact
    double trustImpact;
    double affectionImpact;
    double respectImpact;

    // Context
    std::string location;
    std::vector<std::string> witnesses;
    bool isSecret = false;
};

/**
 * Individual relationship between two entities
 */
class Relationship {
public:
    Relationship();
    Relationship(const std::string& entityA, const std::string& entityB);
    ~Relationship();

    // Identity
    std::string entityA;
    std::string entityB;

    // Core metrics (asymmetric - A's view of B)
    struct RelationshipMetrics {
        double trust = 0.5;         // -1 to 1
        double affection = 0.0;     // -1 to 1
        double respect = 0.5;       // -1 to 1
        double familiarity = 0.0;   // 0 to 1
        double fear = 0.0;          // 0 to 1
        double attraction = 0.0;    // 0 to 1 (for romance)
        double debt = 0.0;          // Positive = owes them

        double getOverallScore() const;
    };

    RelationshipMetrics metricsAtoB;
    RelationshipMetrics metricsBtoA;

    // Type and status
    RelationshipType type = RelationshipType::Stranger;
    bool isActive = true;
    bool isMutual = true;

    // History
    std::vector<RelationshipEvent> history;
    uint32_t firstMeeting = 0;
    uint32_t lastInteraction = 0;
    uint32_t interactionCount = 0;

    /**
     * Process an interaction
     */
    void processInteraction(InteractionType type,
                           const std::string& initiator,
                           double intensity = 1.0);

    /**
     * Update relationship type based on metrics
     */
    void updateType();

    /**
     * Decay relationship over time
     */
    void decay(double deltaTime);

    /**
     * Get relationship strength (0-1)
     */
    double getStrength() const;

    /**
     * Check if relationship is positive
     */
    bool isPositive() const;

    /**
     * Check if relationship is hostile
     */
    bool isHostile() const;

private:
    void applyInteractionEffects(InteractionType type,
                                RelationshipMetrics& metrics,
                                double intensity);
};

/**
 * Social network node
 */
struct SocialNode {
    std::string entityId;
    std::string name;
    std::string faction;
    int socialRank = 5;             // 1-10

    // Centrality metrics (updated by network analysis)
    double degreeCentrality = 0.0;
    double betweennessCentrality = 0.0;
    double closenessCentrality = 0.0;

    // Influence
    double influence = 0.5;
    double reputation = 0.5;
};

/**
 * Social network edge
 */
struct SocialEdge {
    std::string from;
    std::string to;
    double strength;
    RelationshipType type;
    bool isBidirectional = true;
};

/**
 * Social Network Graph
 */
class SocialNetwork {
public:
    SocialNetwork();
    ~SocialNetwork();

    /**
     * Add entity to network
     */
    void addEntity(const std::string& entityId, const std::string& name = "",
                  const std::string& faction = "");

    /**
     * Remove entity from network
     */
    void removeEntity(const std::string& entityId);

    /**
     * Add/update relationship edge
     */
    void updateRelationship(const Relationship& rel);

    /**
     * Get entity's connections
     */
    std::vector<std::string> getConnections(const std::string& entityId,
                                            double minStrength = 0.1) const;

    /**
     * Get friends of entity
     */
    std::vector<std::string> getFriends(const std::string& entityId) const;

    /**
     * Get enemies of entity
     */
    std::vector<std::string> getEnemies(const std::string& entityId) const;

    /**
     * Find path between entities (for "friend of a friend")
     */
    std::vector<std::string> findPath(const std::string& from,
                                      const std::string& to,
                                      int maxLength = 6) const;

    /**
     * Get mutual connections
     */
    std::vector<std::string> getMutualConnections(const std::string& entityA,
                                                   const std::string& entityB) const;

    /**
     * Compute centrality metrics
     */
    void computeCentrality();

    /**
     * Get most influential entities
     */
    std::vector<std::string> getMostInfluential(int count = 10) const;

    /**
     * Detect communities/cliques
     */
    std::vector<std::set<std::string>> detectCommunities() const;

    /**
     * Get node info
     */
    const SocialNode* getNode(const std::string& entityId) const;

    /**
     * Get all entities
     */
    std::vector<std::string> getAllEntities() const;

    /**
     * Statistics
     */
    size_t getNodeCount() const { return nodes_.size(); }
    size_t getEdgeCount() const { return edges_.size(); }
    double getAverageConnections() const;

private:
    std::map<std::string, SocialNode> nodes_;
    std::vector<SocialEdge> edges_;

    // Adjacency list for fast lookup
    std::map<std::string, std::vector<std::pair<std::string, double>>> adjacency_;

    void updateAdjacency();
};

/**
 * Faction/Group
 */
class Faction {
public:
    Faction();
    Faction(const std::string& id, const std::string& name);
    ~Faction();

    std::string id;
    std::string name;
    std::string description;

    // Members
    struct Member {
        std::string entityId;
        std::string rank;
        double standing = 0.5;      // 0-1 within faction
        double loyalty = 0.5;
        uint32_t joinDate;
    };

    std::vector<Member> members;
    std::string leaderId;

    // Faction stats
    double power = 0.5;             // Overall power
    double wealth = 0.5;            // Economic strength
    double influence = 0.5;         // Political influence
    double cohesion = 0.5;          // Internal unity
    double reputation = 0.5;        // Public perception

    // Relations with other factions
    std::map<std::string, double> factionRelations;  // -1 to 1

    /**
     * Add member
     */
    void addMember(const std::string& entityId, const std::string& rank = "member");

    /**
     * Remove member
     */
    void removeMember(const std::string& entityId);

    /**
     * Check membership
     */
    bool hasMember(const std::string& entityId) const;

    /**
     * Get member standing
     */
    double getMemberStanding(const std::string& entityId) const;

    /**
     * Update member standing
     */
    void updateStanding(const std::string& entityId, double delta);

    /**
     * Get faction relation
     */
    double getRelation(const std::string& otherFactionId) const;

    /**
     * Update faction relation
     */
    void updateRelation(const std::string& otherFactionId, double delta);
};

/**
 * Reputation System
 */
class ReputationSystem {
public:
    ReputationSystem();
    ~ReputationSystem();

    /**
     * Reputation categories
     */
    enum class Category {
        General,        // Overall reputation
        Combat,         // Fighting prowess
        Trade,          // Business dealings
        Honor,          // Keeping word
        Wisdom,         // Knowledge
        Kindness,       // Helping others
        Crime           // Criminal acts
    };

    /**
     * Get reputation
     */
    double getReputation(const std::string& entityId, Category category = Category::General) const;

    /**
     * Get reputation in location
     */
    double getLocalReputation(const std::string& entityId, const std::string& location) const;

    /**
     * Modify reputation
     */
    void modifyReputation(const std::string& entityId, Category category, double delta,
                         const std::string& location = "");

    /**
     * Spread reputation (gossip)
     */
    void spreadReputation(const std::string& entityId, const std::string& fromLocation,
                         const std::string& toLocation, double spreadFactor = 0.5);

    /**
     * Decay reputation toward neutral
     */
    void decay(double deltaTime);

    /**
     * Get title based on reputation
     */
    std::string getTitle(const std::string& entityId, Category category) const;

private:
    struct EntityReputation {
        std::map<Category, double> categories;
        std::map<std::string, double> localReputation;  // location -> rep
    };

    std::map<std::string, EntityReputation> reputations_;
};

/**
 * Gossip/Information Propagation
 */
class GossipSystem {
public:
    struct Gossip {
        std::string id;
        std::string subject;        // Who it's about
        std::string content;
        double importance = 0.5;
        double truthfulness = 1.0;  // 0 = complete lie
        bool isSecret = false;
        uint32_t originTime;
        std::string originEntity;
        std::set<std::string> knownBy;
    };

    GossipSystem();
    ~GossipSystem();

    /**
     * Create gossip
     */
    std::string createGossip(const std::string& subject,
                            const std::string& content,
                            const std::string& originator,
                            double truthfulness = 1.0);

    /**
     * Spread gossip between entities
     */
    void spreadGossip(const std::string& gossipId,
                     const std::string& from,
                     const std::string& to);

    /**
     * Simulate gossip spread in network
     */
    void simulateSpread(SocialNetwork& network, double spreadProbability = 0.3);

    /**
     * Get gossip known by entity
     */
    std::vector<Gossip> getKnownGossip(const std::string& entityId) const;

    /**
     * Get gossip about entity
     */
    std::vector<Gossip> getGossipAbout(const std::string& subjectId) const;

    /**
     * Decay old gossip
     */
    void decay(uint32_t currentTime, uint32_t maxAge);

private:
    std::map<std::string, Gossip> gossips_;
};

/**
 * Emotional State Machine for social interactions
 */
class EmotionalStateMachine {
public:
    enum class State {
        Neutral,
        Happy,
        Sad,
        Angry,
        Fearful,
        Disgusted,
        Surprised,
        Loving,
        Trusting,
        Suspicious,
        Jealous,
        Grateful,
        Resentful
    };

    EmotionalStateMachine();
    ~EmotionalStateMachine();

    /**
     * Get current state
     */
    State getCurrentState() const { return currentState_; }

    /**
     * Process social event
     */
    void processEvent(InteractionType interaction, double intensity,
                     const std::string& otherEntity);

    /**
     * Update state over time
     */
    void update(double deltaTime);

    /**
     * Get emotional response to situation
     */
    double getResponseIntensity(InteractionType interaction) const;

    /**
     * Get emotion intensity
     */
    double getIntensity() const { return intensity_; }

private:
    State currentState_ = State::Neutral;
    double intensity_ = 0.0;
    uint32_t stateStartTime_ = 0;

    std::map<State, std::map<InteractionType, State>> transitions_;

    void initializeTransitions();
};

/**
 * Relationship System Manager
 */
class RelationshipSystem {
public:
    RelationshipSystem();
    ~RelationshipSystem();

    /**
     * Initialize system
     */
    void initialize();

    /**
     * Register entity
     */
    void registerEntity(const std::string& entityId, const std::string& name = "",
                       const std::string& faction = "");

    /**
     * Get or create relationship
     */
    Relationship& getRelationship(const std::string& entityA, const std::string& entityB);

    /**
     * Process interaction
     */
    void processInteraction(const std::string& entityA,
                           const std::string& entityB,
                           InteractionType type,
                           double intensity = 1.0);

    /**
     * Access subsystems
     */
    SocialNetwork& getNetwork() { return network_; }
    ReputationSystem& getReputation() { return reputation_; }
    GossipSystem& getGossip() { return gossip_; }

    /**
     * Get faction
     */
    Faction* getFaction(const std::string& factionId);

    /**
     * Create faction
     */
    void createFaction(const Faction& faction);

    /**
     * Update system
     */
    void update(double deltaTime);

    /**
     * Save/load
     */
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);

private:
    SocialNetwork network_;
    ReputationSystem reputation_;
    GossipSystem gossip_;

    std::map<std::string, Faction> factions_;
    std::map<std::pair<std::string, std::string>, Relationship> relationships_;

    std::string makeRelationshipKey(const std::string& a, const std::string& b) const;
};

} // namespace Social
} // namespace NPC
} // namespace Ultima

#endif // ULTIMA_NPC_RELATIONSHIP_SYSTEM_H

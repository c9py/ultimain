/**
 * AIMLEngine.h - C++ AIML Engine for Ultima NPC Dialogue
 *
 * Ported from PandaMania AIML system for Ultima integration.
 * Implements pattern matching, template processing, and
 * meta-cognitive dialogue capabilities.
 *
 * Part of Phase 1: Foundation
 */

#ifndef ULTIMA_NPC_AIML_ENGINE_H
#define ULTIMA_NPC_AIML_ENGINE_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <regex>
#include <variant>

namespace Ultima {
namespace NPC {
namespace AIML {

/**
 * AIML Pattern element types
 */
enum class PatternElementType {
    Word,           // Literal word
    Wildcard,       // * - matches one or more words
    WildcardZero,   // ^ - matches zero or more words
    WildcardOne,    // _ - matches exactly one word
    Set,            // <set name="..."/> - matches words in a set
    Bot             // <bot name="..."/> - matches bot property
};

/**
 * Pattern element
 */
struct PatternElement {
    PatternElementType type;
    std::string value;      // Word content or set/bot name
};

/**
 * Template element types
 */
enum class TemplateElementType {
    Text,           // Literal text
    Star,           // <star/> - captured wildcard
    Get,            // <get name="..."/> - get variable
    Set,            // <set name="...">...</set> - set variable
    Think,          // <think>...</think> - silent processing
    Random,         // <random>...</random> - random choice
    Condition,      // <condition>...</condition> - conditional
    Srai,           // <srai>...</srai> - symbolic reduction
    Learn,          // <learn>...</learn> - learn new pattern
    Date,           // <date/> - current date/time
    Bot,            // <bot name="..."/> - bot property
    Input,          // <input/> - user input
    That,           // <that/> - previous response
    Topic,          // <topic/> - current topic
    System          // <system>...</system> - system command
};

/**
 * Template element
 */
struct TemplateElement {
    TemplateElementType type;
    std::string value;
    std::map<std::string, std::string> attributes;
    std::vector<TemplateElement> children;
};

/**
 * AIML Category (pattern-template pair)
 */
struct Category {
    std::vector<PatternElement> pattern;
    std::string that;       // Optional <that> pattern
    std::string topic;      // Optional topic
    std::vector<TemplateElement> templateElements;
    int priority = 0;       // Higher priority matched first
    std::string sourceFile;
    int sourceLine = 0;
};

/**
 * Match result
 */
struct MatchResult {
    bool matched = false;
    const Category* category = nullptr;
    std::vector<std::string> stars;     // Captured wildcards
    std::vector<std::string> thatStars; // Captured <that> wildcards
    std::vector<std::string> topicStars;// Captured topic wildcards
    double confidence = 0.0;            // Match confidence score
};

/**
 * Session context for conversation
 */
struct SessionContext {
    std::string sessionId;
    std::string userId;
    std::map<std::string, std::string> predicates;  // User variables
    std::vector<std::string> inputHistory;          // Previous inputs
    std::vector<std::string> responseHistory;       // Previous responses
    std::string topic;

    // Learning-related
    std::map<std::string, std::string> learnedFacts;
    std::vector<std::string> preferences;

    // Meta-cognitive state
    int metacogLevel = 0;
    double cognitiveLoad = 0.0;
    std::string currentReasoning;
};

/**
 * Bot properties
 */
struct BotProperties {
    std::string name = "NPC";
    std::string version = "1.0";
    std::string personality;
    std::map<std::string, std::string> properties;
};

/**
 * AIML Parser - Parses AIML XML files
 */
class AIMLParser {
public:
    AIMLParser();
    ~AIMLParser();

    /**
     * Parse AIML file
     * @return Vector of parsed categories
     */
    std::vector<Category> parseFile(const std::string& filename);

    /**
     * Parse AIML string
     */
    std::vector<Category> parseString(const std::string& aimlContent);

    /**
     * Get parser errors
     */
    const std::vector<std::string>& getErrors() const { return errors_; }

    std::vector<PatternElement> parsePattern(const std::string& patternStr);
    std::vector<TemplateElement> parseTemplate(const std::string& templateStr);

private:
    std::vector<std::string> errors_;
    TemplateElement parseTemplateElement(const std::string& tagName,
                                         const std::map<std::string, std::string>& attrs,
                                         const std::string& content);
};

/**
 * Pattern Matcher - Graphmaster-style pattern matching
 */
class PatternMatcher {
public:
    PatternMatcher();
    ~PatternMatcher();

    /**
     * Add category to pattern graph
     */
    void addCategory(const Category& category);

    /**
     * Find best matching category
     */
    MatchResult match(const std::string& input,
                     const std::string& that = "",
                     const std::string& topic = "") const;

    /**
     * Get all matches above threshold
     */
    std::vector<MatchResult> findAllMatches(const std::string& input,
                                            double minConfidence = 0.5) const;

    /**
     * Clear all patterns
     */
    void clear();

    /**
     * Get pattern count
     */
    size_t getPatternCount() const { return categories_.size(); }

private:
    std::vector<Category> categories_;

    // Graphmaster node structure
    struct GraphNode {
        std::map<std::string, std::unique_ptr<GraphNode>> children;
        std::vector<const Category*> categories;
    };
    std::unique_ptr<GraphNode> root_;

    double calculateMatchScore(const std::vector<PatternElement>& pattern,
                              const std::vector<std::string>& words,
                              std::vector<std::string>& captures) const;
};

/**
 * Template Processor - Evaluates AIML templates
 */
class TemplateProcessor {
public:
    using CustomTagHandler = std::function<std::string(
        const std::string& tagName,
        const std::map<std::string, std::string>& attrs,
        const std::string& content,
        SessionContext& context
    )>;

    TemplateProcessor();
    ~TemplateProcessor();

    /**
     * Process template elements
     */
    std::string process(const std::vector<TemplateElement>& elements,
                       const MatchResult& match,
                       SessionContext& context,
                       class AIMLEngine& engine);

    /**
     * Register custom tag handler
     */
    void registerTagHandler(const std::string& tagName, CustomTagHandler handler);

    /**
     * Set bot properties
     */
    void setBotProperties(const BotProperties& props);

private:
    std::map<std::string, CustomTagHandler> customHandlers_;
    BotProperties botProps_;

    std::string processElement(const TemplateElement& elem,
                              const MatchResult& match,
                              SessionContext& context,
                              class AIMLEngine& engine);
    std::string processRandom(const std::vector<TemplateElement>& children,
                             const MatchResult& match,
                             SessionContext& context,
                             class AIMLEngine& engine);
    std::string processCondition(const TemplateElement& elem,
                                const MatchResult& match,
                                SessionContext& context,
                                class AIMLEngine& engine);
};

/**
 * Knowledge Base - Semantic triple storage
 */
class KnowledgeBase {
public:
    struct Triple {
        std::string subject;
        std::string predicate;
        std::string object;
        double confidence = 1.0;
        std::string source;
    };

    KnowledgeBase();
    ~KnowledgeBase();

    /**
     * Store a fact
     */
    void store(const std::string& subject,
               const std::string& predicate,
               const std::string& object,
               double confidence = 1.0);

    /**
     * Query facts
     */
    std::vector<Triple> query(const std::string& subject = "*",
                              const std::string& predicate = "*",
                              const std::string& object = "*") const;

    /**
     * Infer new facts
     */
    std::vector<Triple> infer(const std::string& subject) const;

    /**
     * Check if fact exists
     */
    bool hasFact(const std::string& subject,
                 const std::string& predicate,
                 const std::string& object) const;

    /**
     * Get all facts about subject
     */
    std::string summarize(const std::string& subject) const;

    /**
     * Save/load knowledge base
     */
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);

    size_t getFactCount() const { return triples_.size(); }

private:
    std::vector<Triple> triples_;

    // Indexes for fast lookup
    std::multimap<std::string, size_t> subjectIndex_;
    std::multimap<std::string, size_t> predicateIndex_;
};

/**
 * Main AIML Engine
 */
class AIMLEngine {
public:
    AIMLEngine();
    ~AIMLEngine();

    /**
     * Load AIML file(s)
     */
    bool loadFile(const std::string& filename);
    bool loadDirectory(const std::string& directory, const std::string& pattern = "*.aiml");

    /**
     * Process input and generate response
     */
    std::string respond(const std::string& input, SessionContext& context);

    /**
     * Process with NPC personality influence
     * @param personalityTraits Map of trait names to values (0.0 - 1.0)
     */
    std::string respondWithPersonality(
        const std::string& input,
        SessionContext& context,
        const std::map<std::string, double>& personalityTraits
    );

    /**
     * Learn from interaction
     */
    void learn(const std::string& pattern,
               const std::string& templateContent,
               SessionContext& context);

    /**
     * Access components
     */
    KnowledgeBase& getKnowledgeBase() { return knowledgeBase_; }
    PatternMatcher& getPatternMatcher() { return matcher_; }

    /**
     * Bot properties
     */
    void setBotProperties(const BotProperties& props);
    const BotProperties& getBotProperties() const { return botProps_; }

    /**
     * Statistics
     */
    size_t getCategoryCount() const { return matcher_.getPatternCount(); }
    size_t getKnowledgeCount() const { return knowledgeBase_.getFactCount(); }

    /**
     * Register custom processing
     */
    void registerTagHandler(const std::string& tagName,
                           TemplateProcessor::CustomTagHandler handler);

private:
    AIMLParser parser_;
    PatternMatcher matcher_;
    TemplateProcessor processor_;
    KnowledgeBase knowledgeBase_;
    BotProperties botProps_;

    std::string normalizeInput(const std::string& input) const;
    std::string applyPersonalityModifiers(
        const std::string& response,
        const std::map<std::string, double>& traits
    ) const;
};

/**
 * NPC Dialogue Manager - Integrates AIML with NPC system
 */
class NPCDialogueManager {
public:
    NPCDialogueManager();
    ~NPCDialogueManager();

    /**
     * Initialize with AIML data directory
     */
    bool initialize(const std::string& aimlDirectory);

    /**
     * Get or create session for NPC
     */
    SessionContext& getSession(const std::string& npcId);

    /**
     * Generate NPC dialogue response
     */
    std::string generateResponse(
        const std::string& npcId,
        const std::string& playerInput,
        const std::map<std::string, double>& npcPersonality
    );

    /**
     * Store fact about NPC
     */
    void storeNPCFact(const std::string& npcId,
                      const std::string& predicate,
                      const std::string& object);

    /**
     * Get NPC knowledge summary
     */
    std::string getNPCKnowledge(const std::string& npcId) const;

    /**
     * Update NPC emotional context
     */
    void updateEmotionalContext(const std::string& npcId,
                               const std::string& emotion,
                               double intensity);

private:
    std::unique_ptr<AIMLEngine> engine_;
    std::map<std::string, SessionContext> sessions_;
    std::map<std::string, std::map<std::string, double>> npcEmotions_;

    void applyEmotionalInfluence(SessionContext& context,
                                const std::map<std::string, double>& emotions);
};

} // namespace AIML
} // namespace NPC
} // namespace Ultima

#endif // ULTIMA_NPC_AIML_ENGINE_H

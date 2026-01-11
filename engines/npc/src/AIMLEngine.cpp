/**
 * AIMLEngine.cpp - C++ AIML Engine Implementation
 *
 * Pattern matching, template processing, and knowledge base for NPC dialogue.
 */

#include "aiml/AIMLEngine.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <regex>
#include <cctype>
#include <filesystem>

namespace Ultima {
namespace NPC {
namespace AIML {

static std::mt19937 rng(std::random_device{}());

//=============================================================================
// AIMLParser Implementation
//=============================================================================

AIMLParser::AIMLParser() = default;
AIMLParser::~AIMLParser() = default;

std::vector<PatternElement> AIMLParser::parsePattern(const std::string& patternStr) {
    std::vector<PatternElement> elements;
    std::istringstream iss(patternStr);
    std::string word;

    while (iss >> word) {
        PatternElement elem;

        // Convert to uppercase for matching
        std::transform(word.begin(), word.end(), word.begin(), ::toupper);

        if (word == "*") {
            elem.type = PatternElementType::Wildcard;
        } else if (word == "^") {
            elem.type = PatternElementType::WildcardZero;
        } else if (word == "_") {
            elem.type = PatternElementType::WildcardOne;
        } else if (word.substr(0, 5) == "<SET ") {
            elem.type = PatternElementType::Set;
            // Extract set name
            size_t start = word.find("NAME=\"") + 6;
            size_t end = word.find("\"", start);
            elem.value = word.substr(start, end - start);
        } else {
            elem.type = PatternElementType::Word;
            elem.value = word;
        }

        elements.push_back(elem);
    }

    return elements;
}

std::vector<TemplateElement> AIMLParser::parseTemplate(const std::string& templateStr) {
    std::vector<TemplateElement> elements;

    size_t pos = 0;
    while (pos < templateStr.length()) {
        // Find next tag
        size_t tagStart = templateStr.find('<', pos);

        if (tagStart == std::string::npos) {
            // Rest is text
            if (pos < templateStr.length()) {
                TemplateElement text;
                text.type = TemplateElementType::Text;
                text.value = templateStr.substr(pos);
                elements.push_back(text);
            }
            break;
        }

        // Add text before tag
        if (tagStart > pos) {
            TemplateElement text;
            text.type = TemplateElementType::Text;
            text.value = templateStr.substr(pos, tagStart - pos);
            elements.push_back(text);
        }

        // Find tag end
        size_t tagEnd = templateStr.find('>', tagStart);
        if (tagEnd == std::string::npos) break;

        std::string tag = templateStr.substr(tagStart + 1, tagEnd - tagStart - 1);

        // Check for self-closing tag
        bool selfClosing = (!tag.empty() && tag.back() == '/');
        if (selfClosing) {
            tag = tag.substr(0, tag.length() - 1);
        }

        // Parse tag name and attributes
        std::istringstream tagStream(tag);
        std::string tagName;
        tagStream >> tagName;

        // Convert to lowercase
        std::transform(tagName.begin(), tagName.end(), tagName.begin(), ::tolower);

        std::map<std::string, std::string> attrs;
        std::string attrPart;
        while (tagStream >> attrPart) {
            size_t eqPos = attrPart.find('=');
            if (eqPos != std::string::npos) {
                std::string name = attrPart.substr(0, eqPos);
                std::string value = attrPart.substr(eqPos + 1);
                // Remove quotes
                if (value.front() == '"') value = value.substr(1);
                if (value.back() == '"') value = value.substr(0, value.length() - 1);
                attrs[name] = value;
            }
        }

        pos = tagEnd + 1;

        // Handle tag
        if (selfClosing || tagName == "star" || tagName == "date" ||
            tagName == "bot" || tagName == "get" || tagName == "input") {
            TemplateElement elem = parseTemplateElement(tagName, attrs, "");
            elements.push_back(elem);
        } else {
            // Find closing tag
            std::string closeTag = "</" + tagName + ">";
            size_t closePos = templateStr.find(closeTag, pos);
            if (closePos != std::string::npos) {
                std::string content = templateStr.substr(pos, closePos - pos);
                TemplateElement elem = parseTemplateElement(tagName, attrs, content);
                elements.push_back(elem);
                pos = closePos + closeTag.length();
            }
        }
    }

    return elements;
}

TemplateElement AIMLParser::parseTemplateElement(
    const std::string& tagName,
    const std::map<std::string, std::string>& attrs,
    const std::string& content)
{
    TemplateElement elem;
    elem.attributes = attrs;
    elem.value = content;

    if (tagName == "star") {
        elem.type = TemplateElementType::Star;
        if (attrs.count("index")) {
            elem.value = attrs.at("index");
        }
    } else if (tagName == "get") {
        elem.type = TemplateElementType::Get;
        if (attrs.count("name")) {
            elem.value = attrs.at("name");
        }
    } else if (tagName == "set") {
        elem.type = TemplateElementType::Set;
        elem.children = parseTemplate(content);
    } else if (tagName == "think") {
        elem.type = TemplateElementType::Think;
        elem.children = parseTemplate(content);
    } else if (tagName == "random") {
        elem.type = TemplateElementType::Random;
        // Parse <li> items
        size_t liPos = 0;
        while ((liPos = content.find("<li>", liPos)) != std::string::npos) {
            size_t liEnd = content.find("</li>", liPos);
            if (liEnd != std::string::npos) {
                std::string liContent = content.substr(liPos + 4, liEnd - liPos - 4);
                TemplateElement child;
                child.type = TemplateElementType::Text;
                child.value = liContent;
                elem.children.push_back(child);
                liPos = liEnd + 5;
            } else {
                break;
            }
        }
    } else if (tagName == "condition") {
        elem.type = TemplateElementType::Condition;
        elem.children = parseTemplate(content);
    } else if (tagName == "srai") {
        elem.type = TemplateElementType::Srai;
    } else if (tagName == "learn") {
        elem.type = TemplateElementType::Learn;
    } else if (tagName == "date") {
        elem.type = TemplateElementType::Date;
    } else if (tagName == "bot") {
        elem.type = TemplateElementType::Bot;
    } else if (tagName == "input") {
        elem.type = TemplateElementType::Input;
    } else if (tagName == "that") {
        elem.type = TemplateElementType::That;
    } else if (tagName == "topic") {
        elem.type = TemplateElementType::Topic;
    } else if (tagName == "system") {
        elem.type = TemplateElementType::System;
    } else {
        elem.type = TemplateElementType::Text;
    }

    return elem;
}

std::vector<Category> AIMLParser::parseFile(const std::string& filename) {
    std::vector<Category> categories;
    std::ifstream file(filename);
    if (!file) {
        errors_.push_back("Cannot open file: " + filename);
        return categories;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return parseString(buffer.str());
}

std::vector<Category> AIMLParser::parseString(const std::string& aimlContent) {
    std::vector<Category> categories;

    // Simple XML parsing for AIML categories
    size_t pos = 0;
    while ((pos = aimlContent.find("<category>", pos)) != std::string::npos) {
        size_t catEnd = aimlContent.find("</category>", pos);
        if (catEnd == std::string::npos) break;

        std::string catContent = aimlContent.substr(pos, catEnd - pos + 11);

        Category cat;

        // Extract pattern
        size_t patStart = catContent.find("<pattern>");
        size_t patEnd = catContent.find("</pattern>");
        if (patStart != std::string::npos && patEnd != std::string::npos) {
            std::string pattern = catContent.substr(patStart + 9, patEnd - patStart - 9);
            cat.pattern = parsePattern(pattern);
        }

        // Extract template
        size_t tempStart = catContent.find("<template>");
        size_t tempEnd = catContent.find("</template>");
        if (tempStart != std::string::npos && tempEnd != std::string::npos) {
            std::string templ = catContent.substr(tempStart + 10, tempEnd - tempStart - 10);
            cat.templateElements = parseTemplate(templ);
        }

        // Extract optional that
        size_t thatStart = catContent.find("<that>");
        size_t thatEnd = catContent.find("</that>");
        if (thatStart != std::string::npos && thatEnd != std::string::npos) {
            cat.that = catContent.substr(thatStart + 6, thatEnd - thatStart - 6);
        }

        if (!cat.pattern.empty()) {
            categories.push_back(cat);
        }

        pos = catEnd + 11;
    }

    return categories;
}

//=============================================================================
// PatternMatcher Implementation
//=============================================================================

PatternMatcher::PatternMatcher()
    : root_(std::make_unique<GraphNode>())
{
}

PatternMatcher::~PatternMatcher() = default;

void PatternMatcher::addCategory(const Category& category) {
    categories_.push_back(category);
}

double PatternMatcher::calculateMatchScore(
    const std::vector<PatternElement>& pattern,
    const std::vector<std::string>& words,
    std::vector<std::string>& captures) const
{
    captures.clear();
    size_t pIdx = 0;
    size_t wIdx = 0;

    while (pIdx < pattern.size() && wIdx <= words.size()) {
        const auto& elem = pattern[pIdx];

        switch (elem.type) {
            case PatternElementType::Word: {
                if (wIdx >= words.size()) return 0.0;
                std::string word = words[wIdx];
                std::transform(word.begin(), word.end(), word.begin(), ::toupper);
                if (word != elem.value) return 0.0;
                wIdx++;
                pIdx++;
                break;
            }

            case PatternElementType::Wildcard: {
                // * matches one or more words
                if (wIdx >= words.size()) return 0.0;

                // Find where this wildcard ends
                if (pIdx + 1 >= pattern.size()) {
                    // Consume rest
                    std::string captured;
                    for (size_t i = wIdx; i < words.size(); ++i) {
                        if (!captured.empty()) captured += " ";
                        captured += words[i];
                    }
                    captures.push_back(captured);
                    wIdx = words.size();
                    pIdx++;
                } else {
                    // Find next word match
                    const auto& nextElem = pattern[pIdx + 1];
                    if (nextElem.type == PatternElementType::Word) {
                        std::string captured;
                        while (wIdx < words.size()) {
                            std::string word = words[wIdx];
                            std::transform(word.begin(), word.end(), word.begin(), ::toupper);
                            if (word == nextElem.value) break;
                            if (!captured.empty()) captured += " ";
                            captured += words[wIdx];
                            wIdx++;
                        }
                        if (captured.empty()) return 0.0;
                        captures.push_back(captured);
                        pIdx++;
                    } else {
                        // Greedy match one word
                        captures.push_back(words[wIdx]);
                        wIdx++;
                        pIdx++;
                    }
                }
                break;
            }

            case PatternElementType::WildcardZero: {
                // ^ matches zero or more
                if (pIdx + 1 >= pattern.size()) {
                    std::string captured;
                    for (size_t i = wIdx; i < words.size(); ++i) {
                        if (!captured.empty()) captured += " ";
                        captured += words[i];
                    }
                    captures.push_back(captured);
                    wIdx = words.size();
                    pIdx++;
                } else {
                    captures.push_back("");
                    pIdx++;
                }
                break;
            }

            case PatternElementType::WildcardOne: {
                // _ matches exactly one
                if (wIdx >= words.size()) return 0.0;
                captures.push_back(words[wIdx]);
                wIdx++;
                pIdx++;
                break;
            }

            default:
                pIdx++;
                break;
        }
    }

    if (pIdx < pattern.size() || wIdx < words.size()) {
        return 0.0;
    }

    // Calculate confidence based on specificity
    double score = 1.0;
    int wildcardCount = 0;
    for (const auto& elem : pattern) {
        if (elem.type == PatternElementType::Word) {
            score += 0.1;  // Exact matches increase score
        } else {
            wildcardCount++;
        }
    }

    // Penalize patterns with many wildcards
    score -= wildcardCount * 0.05;

    return std::max(0.0, std::min(1.0, score));
}

MatchResult PatternMatcher::match(const std::string& input,
                                  const std::string& that,
                                  const std::string& topic) const
{
    MatchResult best;

    // Tokenize input
    std::vector<std::string> words;
    std::istringstream iss(input);
    std::string word;
    while (iss >> word) {
        words.push_back(word);
    }

    for (const auto& cat : categories_) {
        // Check topic filter
        if (!cat.topic.empty() && cat.topic != topic) continue;

        // Check that filter
        if (!cat.that.empty() && cat.that != that) continue;

        std::vector<std::string> captures;
        double score = calculateMatchScore(cat.pattern, words, captures);

        if (score > best.confidence) {
            best.matched = true;
            best.category = &cat;
            best.stars = captures;
            best.confidence = score;
        }
    }

    return best;
}

std::vector<MatchResult> PatternMatcher::findAllMatches(const std::string& input,
                                                        double minConfidence) const
{
    std::vector<MatchResult> results;

    std::vector<std::string> words;
    std::istringstream iss(input);
    std::string word;
    while (iss >> word) {
        words.push_back(word);
    }

    for (const auto& cat : categories_) {
        std::vector<std::string> captures;
        double score = calculateMatchScore(cat.pattern, words, captures);

        if (score >= minConfidence) {
            MatchResult result;
            result.matched = true;
            result.category = &cat;
            result.stars = captures;
            result.confidence = score;
            results.push_back(result);
        }
    }

    std::sort(results.begin(), results.end(),
              [](const MatchResult& a, const MatchResult& b) {
                  return a.confidence > b.confidence;
              });

    return results;
}

void PatternMatcher::clear() {
    categories_.clear();
    root_ = std::make_unique<GraphNode>();
}

//=============================================================================
// TemplateProcessor Implementation
//=============================================================================

TemplateProcessor::TemplateProcessor() = default;
TemplateProcessor::~TemplateProcessor() = default;

std::string TemplateProcessor::process(
    const std::vector<TemplateElement>& elements,
    const MatchResult& match,
    SessionContext& context,
    AIMLEngine& engine)
{
    std::string result;
    for (const auto& elem : elements) {
        result += processElement(elem, match, context, engine);
    }
    return result;
}

std::string TemplateProcessor::processElement(
    const TemplateElement& elem,
    const MatchResult& match,
    SessionContext& context,
    AIMLEngine& engine)
{
    switch (elem.type) {
        case TemplateElementType::Text:
            return elem.value;

        case TemplateElementType::Star: {
            int index = 0;
            if (!elem.value.empty()) {
                index = std::stoi(elem.value) - 1;
            }
            if (index >= 0 && static_cast<size_t>(index) < match.stars.size()) {
                return match.stars[index];
            }
            return "";
        }

        case TemplateElementType::Get: {
            std::string name = elem.value;
            if (!name.empty() && elem.attributes.count("name")) {
                name = elem.attributes.at("name");
            }
            if (context.predicates.count(name)) {
                return context.predicates[name];
            }
            return "";
        }

        case TemplateElementType::Set: {
            std::string name = elem.attributes.count("name") ?
                elem.attributes.at("name") : elem.value;
            std::string value = process(elem.children, match, context, engine);
            context.predicates[name] = value;
            return value;
        }

        case TemplateElementType::Think: {
            // Process silently
            process(elem.children, match, context, engine);
            return "";
        }

        case TemplateElementType::Random:
            return processRandom(elem.children, match, context, engine);

        case TemplateElementType::Condition:
            return processCondition(elem, match, context, engine);

        case TemplateElementType::Srai: {
            // Symbolic reduction - recursive call
            std::string newInput = elem.value;
            return engine.respond(newInput, context);
        }

        case TemplateElementType::Date: {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            std::string timeStr = std::ctime(&time);
            // Remove trailing newline
            if (!timeStr.empty() && timeStr.back() == '\n') {
                timeStr.pop_back();
            }
            return timeStr;
        }

        case TemplateElementType::Bot: {
            std::string name = elem.attributes.count("name") ?
                elem.attributes.at("name") : elem.value;
            if (botProps_.properties.count(name)) {
                return botProps_.properties[name];
            }
            if (name == "name") return botProps_.name;
            return "";
        }

        case TemplateElementType::Input: {
            int index = 0;
            if (!elem.value.empty()) {
                index = std::stoi(elem.value) - 1;
            }
            if (index >= 0 && static_cast<size_t>(index) < context.inputHistory.size()) {
                return context.inputHistory[context.inputHistory.size() - 1 - index];
            }
            return "";
        }

        case TemplateElementType::That: {
            int index = 0;
            if (!elem.value.empty()) {
                index = std::stoi(elem.value) - 1;
            }
            if (index >= 0 && static_cast<size_t>(index) < context.responseHistory.size()) {
                return context.responseHistory[context.responseHistory.size() - 1 - index];
            }
            return "";
        }

        case TemplateElementType::Topic:
            return context.topic;

        default:
            return "";
    }
}

std::string TemplateProcessor::processRandom(
    const std::vector<TemplateElement>& children,
    const MatchResult& match,
    SessionContext& context,
    AIMLEngine& engine)
{
    if (children.empty()) return "";

    std::uniform_int_distribution<size_t> dist(0, children.size() - 1);
    const auto& chosen = children[dist(rng)];

    if (chosen.type == TemplateElementType::Text) {
        return chosen.value;
    }
    return processElement(chosen, match, context, engine);
}

std::string TemplateProcessor::processCondition(
    const TemplateElement& elem,
    const MatchResult& match,
    SessionContext& context,
    AIMLEngine& engine)
{
    std::string varName = elem.attributes.count("name") ?
        elem.attributes.at("name") : "";
    std::string varValue = context.predicates.count(varName) ?
        context.predicates[varName] : "";

    // Find matching <li> child
    for (const auto& child : elem.children) {
        if (child.attributes.count("value")) {
            if (child.attributes.at("value") == varValue) {
                return process(child.children, match, context, engine);
            }
        }
    }

    // Default case (li without value)
    for (const auto& child : elem.children) {
        if (!child.attributes.count("value")) {
            return process(child.children, match, context, engine);
        }
    }

    return "";
}

void TemplateProcessor::registerTagHandler(const std::string& tagName,
                                           CustomTagHandler handler) {
    customHandlers_[tagName] = handler;
}

void TemplateProcessor::setBotProperties(const BotProperties& props) {
    botProps_ = props;
}

//=============================================================================
// KnowledgeBase Implementation
//=============================================================================

KnowledgeBase::KnowledgeBase() = default;
KnowledgeBase::~KnowledgeBase() = default;

void KnowledgeBase::store(const std::string& subject,
                          const std::string& predicate,
                          const std::string& object,
                          double confidence)
{
    Triple triple{subject, predicate, object, confidence, ""};
    size_t idx = triples_.size();
    triples_.push_back(triple);

    subjectIndex_.insert({subject, idx});
    predicateIndex_.insert({predicate, idx});
}

std::vector<KnowledgeBase::Triple> KnowledgeBase::query(
    const std::string& subject,
    const std::string& predicate,
    const std::string& object) const
{
    std::vector<Triple> results;

    for (const auto& triple : triples_) {
        bool matchSubject = (subject == "*" || subject == triple.subject);
        bool matchPredicate = (predicate == "*" || predicate == triple.predicate);
        bool matchObject = (object == "*" || object == triple.object);

        if (matchSubject && matchPredicate && matchObject) {
            results.push_back(triple);
        }
    }

    return results;
}

std::vector<KnowledgeBase::Triple> KnowledgeBase::infer(const std::string& subject) const {
    std::vector<Triple> results;

    // Direct facts
    auto direct = query(subject, "*", "*");
    results.insert(results.end(), direct.begin(), direct.end());

    // Transitive inference (is-a)
    for (const auto& triple : direct) {
        if (triple.predicate == "is-a" || triple.predicate == "is") {
            auto inherited = query(triple.object, "*", "*");
            for (auto& t : inherited) {
                t.subject = subject;
                t.confidence *= 0.8;  // Reduce confidence for inferred facts
                results.push_back(t);
            }
        }
    }

    return results;
}

bool KnowledgeBase::hasFact(const std::string& subject,
                            const std::string& predicate,
                            const std::string& object) const
{
    auto results = query(subject, predicate, object);
    return !results.empty();
}

std::string KnowledgeBase::summarize(const std::string& subject) const {
    auto facts = query(subject, "*", "*");
    if (facts.empty()) {
        return "No information about " + subject;
    }

    std::stringstream ss;
    ss << "About " << subject << ":\n";
    for (const auto& triple : facts) {
        ss << "  - " << triple.predicate << ": " << triple.object << "\n";
    }
    return ss.str();
}

bool KnowledgeBase::save(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) return false;

    for (const auto& triple : triples_) {
        file << triple.subject << "\t"
             << triple.predicate << "\t"
             << triple.object << "\t"
             << triple.confidence << "\n";
    }
    return true;
}

bool KnowledgeBase::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) return false;

    triples_.clear();
    subjectIndex_.clear();
    predicateIndex_.clear();

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string subject, predicate, object;
        double confidence;

        if (std::getline(iss, subject, '\t') &&
            std::getline(iss, predicate, '\t') &&
            std::getline(iss, object, '\t') &&
            (iss >> confidence)) {
            store(subject, predicate, object, confidence);
        }
    }
    return true;
}

//=============================================================================
// AIMLEngine Implementation
//=============================================================================

AIMLEngine::AIMLEngine() {
    botProps_.name = "NPC";
    processor_.setBotProperties(botProps_);
}

AIMLEngine::~AIMLEngine() = default;

bool AIMLEngine::loadFile(const std::string& filename) {
    auto categories = parser_.parseFile(filename);
    for (const auto& cat : categories) {
        matcher_.addCategory(cat);
    }
    return !parser_.getErrors().empty();
}

bool AIMLEngine::loadDirectory(const std::string& directory, const std::string& pattern) {
    namespace fs = std::filesystem;

    try {
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                // Simple pattern matching
                if (pattern == "*.aiml" && filename.size() > 5 &&
                    filename.substr(filename.size() - 5) == ".aiml") {
                    loadFile(entry.path().string());
                }
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}

std::string AIMLEngine::normalizeInput(const std::string& input) const {
    std::string normalized = input;

    // Remove punctuation
    normalized.erase(
        std::remove_if(normalized.begin(), normalized.end(),
                      [](char c) { return std::ispunct(c) && c != '\''; }),
        normalized.end()
    );

    // Collapse whitespace
    auto newEnd = std::unique(normalized.begin(), normalized.end(),
                             [](char a, char b) { return a == ' ' && b == ' '; });
    normalized.erase(newEnd, normalized.end());

    // Trim
    size_t start = normalized.find_first_not_of(' ');
    size_t end = normalized.find_last_not_of(' ');
    if (start != std::string::npos) {
        normalized = normalized.substr(start, end - start + 1);
    }

    return normalized;
}

std::string AIMLEngine::respond(const std::string& input, SessionContext& context) {
    std::string normalized = normalizeInput(input);
    context.inputHistory.push_back(normalized);

    std::string that = context.responseHistory.empty() ? "" :
        context.responseHistory.back();

    auto match = matcher_.match(normalized, that, context.topic);

    std::string response;
    if (match.matched && match.category) {
        response = processor_.process(match.category->templateElements, match,
                                      context, *this);
    } else {
        response = "I don't understand.";
    }

    context.responseHistory.push_back(response);
    return response;
}

std::string AIMLEngine::respondWithPersonality(
    const std::string& input,
    SessionContext& context,
    const std::map<std::string, double>& personalityTraits)
{
    std::string response = respond(input, context);
    return applyPersonalityModifiers(response, personalityTraits);
}

std::string AIMLEngine::applyPersonalityModifiers(
    const std::string& response,
    const std::map<std::string, double>& traits) const
{
    std::string modified = response;

    // Apply personality-based modifications
    double formality = traits.count("formality") ? traits.at("formality") : 0.5;
    double verbosity = traits.count("verbosity") ? traits.at("verbosity") : 0.5;
    double warmth = traits.count("warmth") ? traits.at("warmth") : 0.5;

    // High warmth: add friendly elements
    if (warmth > 0.7 && modified.length() > 10) {
        std::vector<std::string> warmPrefixes = {
            "Indeed, ", "Of course, ", "Certainly, "
        };
        std::uniform_int_distribution<size_t> dist(0, warmPrefixes.size() - 1);
        if (rng() % 3 == 0) {
            modified = warmPrefixes[dist(rng)] + modified;
        }
    }

    // Low formality: use contractions
    if (formality < 0.3) {
        // Simple contraction replacements
        std::vector<std::pair<std::string, std::string>> contractions = {
            {"I am", "I'm"}, {"you are", "you're"}, {"it is", "it's"},
            {"do not", "don't"}, {"cannot", "can't"}, {"will not", "won't"}
        };
        for (const auto& [full, cont] : contractions) {
            size_t pos = 0;
            while ((pos = modified.find(full, pos)) != std::string::npos) {
                modified.replace(pos, full.length(), cont);
                pos += cont.length();
            }
        }
    }

    return modified;
}

void AIMLEngine::learn(const std::string& pattern,
                      const std::string& templateContent,
                      SessionContext& context)
{
    Category cat;
    cat.pattern = parser_.parsePattern(pattern);
    cat.templateElements = parser_.parseTemplate(templateContent);
    cat.priority = 10;  // Learned patterns have higher priority
    matcher_.addCategory(cat);
}

void AIMLEngine::setBotProperties(const BotProperties& props) {
    botProps_ = props;
    processor_.setBotProperties(props);
}

void AIMLEngine::registerTagHandler(const std::string& tagName,
                                    TemplateProcessor::CustomTagHandler handler) {
    processor_.registerTagHandler(tagName, handler);
}

//=============================================================================
// NPCDialogueManager Implementation
//=============================================================================

NPCDialogueManager::NPCDialogueManager()
    : engine_(std::make_unique<AIMLEngine>())
{
}

NPCDialogueManager::~NPCDialogueManager() = default;

bool NPCDialogueManager::initialize(const std::string& aimlDirectory) {
    return engine_->loadDirectory(aimlDirectory);
}

SessionContext& NPCDialogueManager::getSession(const std::string& npcId) {
    if (!sessions_.count(npcId)) {
        sessions_[npcId] = SessionContext();
        sessions_[npcId].sessionId = npcId;
    }
    return sessions_[npcId];
}

std::string NPCDialogueManager::generateResponse(
    const std::string& npcId,
    const std::string& playerInput,
    const std::map<std::string, double>& npcPersonality)
{
    auto& context = getSession(npcId);

    // Apply emotional influence
    if (npcEmotions_.count(npcId)) {
        applyEmotionalInfluence(context, npcEmotions_[npcId]);
    }

    return engine_->respondWithPersonality(playerInput, context, npcPersonality);
}

void NPCDialogueManager::storeNPCFact(const std::string& npcId,
                                      const std::string& predicate,
                                      const std::string& object) {
    engine_->getKnowledgeBase().store(npcId, predicate, object);
}

std::string NPCDialogueManager::getNPCKnowledge(const std::string& npcId) const {
    return engine_->getKnowledgeBase().summarize(npcId);
}

void NPCDialogueManager::updateEmotionalContext(const std::string& npcId,
                                                const std::string& emotion,
                                                double intensity) {
    npcEmotions_[npcId][emotion] = intensity;
}

void NPCDialogueManager::applyEmotionalInfluence(
    SessionContext& context,
    const std::map<std::string, double>& emotions)
{
    // Set emotional predicates for use in templates
    for (const auto& [emotion, intensity] : emotions) {
        context.predicates["emotion_" + emotion] = std::to_string(intensity);
    }

    // Calculate cognitive load from emotional state
    double totalIntensity = 0.0;
    for (const auto& [_, intensity] : emotions) {
        totalIntensity += intensity;
    }
    context.cognitiveLoad = std::min(1.0, totalIntensity / 3.0);
}

} // namespace AIML
} // namespace NPC
} // namespace Ultima

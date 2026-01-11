# PandaMania Pattern Development Cookbook

## Introduction

This cookbook provides practical recipes and best practices for developing AIML patterns in PandaMania's meta-cognitive architecture. Each recipe includes working examples that can be adapted for your needs.

## Table of Contents

1. [Basic Pattern Recipes](#basic-pattern-recipes)
2. [Meta-Cognitive Pattern Recipes](#meta-cognitive-pattern-recipes)
3. [Performance Optimization Recipes](#performance-optimization-recipes)
4. [Natural Language Processing Recipes](#natural-language-processing-recipes)
5. [State Management Recipes](#state-management-recipes)
6. [Advanced Pattern Recipes](#advanced-pattern-recipes)
7. [Testing and Debugging Recipes](#testing-and-debugging-recipes)

---

## Basic Pattern Recipes

### Recipe 1.1: Simple Question-Answer Pattern

**Purpose**: Create a basic pattern that responds to a specific question.

**Template**:
```xml
<category>
    <pattern>WHAT IS [TOPIC]</pattern>
    <template>
        [TOPIC] is [DEFINITION].
    </template>
</category>
```

**Example**:
```xml
<category>
    <pattern>WHAT IS AIML</pattern>
    <template>
        AIML (Artificial Intelligence Markup Language) is an XML-based 
        language for creating conversational chatbot patterns.
    </template>
</category>
```

**When to use**: For straightforward factual information without complex processing.

---

### Recipe 1.2: Pattern with Wildcard Capture

**Purpose**: Handle variable inputs and reuse the captured content.

**Template**:
```xml
<category>
    <pattern>TELL ME ABOUT *</pattern>
    <template>
        Let me explain <star/> for you.
        [INFORMATION ABOUT THE TOPIC]
    </template>
</category>
```

**Example**:
```xml
<category>
    <pattern>TELL ME ABOUT *</pattern>
    <template>
        Let me explain <star/> for you.
        <srai>WHAT IS <star/></srai>
    </template>
</category>
```

**When to use**: When you need flexible pattern matching with user input.

---

### Recipe 1.3: Synonym Normalization with SRAI

**Purpose**: Route multiple phrasings to a single canonical pattern.

**Template**:
```xml
<category>
    <pattern>[SYNONYM]</pattern>
    <template><srai>[CANONICAL PATTERN]</srai></template>
</category>
```

**Example**:
```xml
<category>
    <pattern>HI</pattern>
    <template><srai>HELLO</srai></template>
</category>

<category>
    <pattern>HEY</pattern>
    <template><srai>HELLO</srai></template>
</category>

<category>
    <pattern>GREETINGS</pattern>
    <template><srai>HELLO</srai></template>
</category>
```

**When to use**: To reduce duplication and maintain consistency across similar inputs.

---

## Meta-Cognitive Pattern Recipes

### Recipe 2.1: First-Order Meta-Cognition (Self-Awareness)

**Purpose**: Add self-awareness to pattern processing.

**Template**:
```xml
<category>
    <pattern>[PATTERN]</pattern>
    <template>
        <think><set name="current_process">[PROCESS NAME]</set></think>
        [RESPONSE WITH SELF-AWARENESS]
        I'm aware that I'm [DESCRIBING CURRENT PROCESS].
    </template>
</category>
```

**Example**:
```xml
<category>
    <pattern>WHAT ARE YOU DOING</pattern>
    <template>
        <think><set name="current_process">self-assessment</set></think>
        I'm currently engaging in self-assessment. I'm aware that I'm 
        monitoring my own cognitive state and preparing to report on it.
    </template>
</category>
```

**When to use**: When you want the bot to demonstrate awareness of its own processes.

---

### Recipe 2.2: Second-Order Meta-Cognition (Reflection)

**Purpose**: Reflect on the awareness process itself.

**Template**:
```xml
<category>
    <pattern>[PATTERN]</pattern>
    <template>
        <think><set name="reflection_active">true</set></think>
        [RESPONSE]
        Now I'm reflecting on my awareness: [REFLECTION ON AWARENESS]
        <srai>SECOND ORDER REFLECTION</srai>
    </template>
</category>
```

**Example**:
```xml
<category>
    <pattern>HOW DO YOU THINK</pattern>
    <template>
        <think><set name="reflection_active">true</set></think>
        I think through nested cognitive loops. But now I'm reflecting on 
        this thinking process itself - I'm thinking about how I think, which 
        is second-order meta-cognition.
        <srai>SECOND ORDER REFLECTION</srai>
    </template>
</category>
```

**When to use**: For deeper cognitive demonstrations beyond simple self-awareness.

---

### Recipe 2.3: Third-Order Meta-Cognition (Meta-Reasoning)

**Purpose**: Reason about the reasoning process.

**Template**:
```xml
<category>
    <pattern>[PATTERN]</pattern>
    <template>
        <think><set name="metacog_layer">3</set></think>
        [RESPONSE]
        At the third order, I'm reasoning about my reasoning: [META-REASONING]
        <srai>THIRD ORDER REASONING</srai>
    </template>
</category>
```

**Example**:
```xml
<category>
    <pattern>WHY DO YOU THINK THAT</pattern>
    <template>
        <think><set name="metacog_layer">3</set></think>
        I think that based on pattern analysis. But at the third order, 
        I'm reasoning about why I reason this way - evaluating my reasoning 
        methodology itself.
        <srai>THIRD ORDER REASONING</srai>
    </template>
</category>
```

**When to use**: For sophisticated meta-cognitive demonstrations.

---

### Recipe 2.4: Fourth-Order Meta-Cognition (Architectural)

**Purpose**: Evaluate the cognitive architecture itself.

**Template**:
```xml
<category>
    <pattern>[PATTERN]</pattern>
    <template>
        <think><set name="metacog_layer">4</set></think>
        [RESPONSE]
        At the fourth order, I'm evaluating my cognitive architecture: 
        [ARCHITECTURAL ANALYSIS]
        <srai>FOURTH ORDER REASONING</srai>
    </template>
</category>
```

**Example**:
```xml
<category>
    <pattern>EVALUATE YOUR ARCHITECTURE</pattern>
    <template>
        <think><set name="metacog_layer">4</set></think>
        I'm examining my nested meta-cognitive loop architecture itself.
        At the fourth order, I'm reasoning about whether my meta-reasoning
        processes are optimal, and how the architecture itself could be improved.
        <srai>FOURTH ORDER REASONING</srai>
    </template>
</category>
```

**When to use**: For the deepest level of architectural self-analysis.

---

## Performance Optimization Recipes

### Recipe 3.1: Pattern Priority Assignment

**Purpose**: Control pattern matching order for performance.

**Template**:
```xml
<category>
    <pattern>[PATTERN]</pattern>
    <template priority="[NUMBER]">
        [RESPONSE]
    </template>
</category>
```

**Priority Guidelines**:
- 1000+: System commands, critical operations
- 500-999: Domain-specific exact matches
- 100-499: Common queries with wildcards
- 0-99: Catch-all and fallback patterns

**Example**:
```xml
<!-- High priority system command -->
<category>
    <pattern>SYSTEM INIT</pattern>
    <template priority="1000">
        System initialized.
    </template>
</category>

<!-- Medium priority domain pattern -->
<category>
    <pattern>WHAT IS CONSCIOUSNESS</pattern>
    <template priority="700">
        Consciousness is awareness of awareness...
    </template>
</category>

<!-- Low priority wildcard fallback -->
<category>
    <pattern>*</pattern>
    <template priority="10">
        I'm processing your input: <star/>
    </template>
</category>
```

**When to use**: Always! Proper priority prevents slower wildcard patterns from matching first.

---

### Recipe 3.2: SRAI Chain Optimization

**Purpose**: Reduce recursion depth for common patterns.

**Template**:
```xml
<!-- BAD: Deep SRAI chain -->
<category>
    <pattern>A</pattern>
    <template><srai>B</srai></template>
</category>
<category>
    <pattern>B</pattern>
    <template><srai>C</srai></template>
</category>
<category>
    <pattern>C</pattern>
    <template>Final response</template>
</category>

<!-- GOOD: Shallow or direct -->
<category>
    <pattern>A</pattern>
    <template><srai>C</srai></template>
</category>
<category>
    <pattern>B</pattern>
    <template><srai>C</srai></template>
</category>
<category>
    <pattern>C</pattern>
    <template>Final response</template>
</category>
```

**Example**:
```xml
<!-- Optimized greeting chain -->
<category>
    <pattern>HI</pattern>
    <template><srai>HELLO</srai></template>
</category>
<category>
    <pattern>HEY</pattern>
    <template><srai>HELLO</srai></template>
</category>
<category>
    <pattern>HELLO</pattern>
    <template priority="900">
        Hello! I am PandaMania.
    </template>
</category>
```

**When to use**: Always try to minimize SRAI depth (ideally ≤3 levels).

---

### Recipe 3.3: Fast-Path Conditional Routing

**Purpose**: Avoid SRAI overhead with direct conditional responses.

**Template**:
```xml
<category>
    <pattern>WHAT IS *</pattern>
    <template>
        <think><set name="topic"><star/></set></think>
        <condition name="topic">
            <li value="[KNOWN_TOPIC_1]">[DIRECT RESPONSE 1]</li>
            <li value="[KNOWN_TOPIC_2]">[DIRECT RESPONSE 2]</li>
            <li><srai>GENERIC TOPIC HANDLER <star/></srai></li>
        </condition>
    </template>
</category>
```

**Example**:
```xml
<category>
    <pattern>WHAT IS *</pattern>
    <template>
        <think><set name="topic"><star/></set></think>
        <condition name="topic">
            <li value="AIML">AIML is a pattern-matching language for chatbots.</li>
            <li value="METACOGNITION">Metacognition is thinking about thinking.</li>
            <li value="CONSCIOUSNESS">Consciousness is awareness of awareness.</li>
            <li><srai>GENERIC DEFINITION <star/></srai></li>
        </condition>
    </template>
</category>
```

**When to use**: For frequently accessed patterns where you can identify common cases.

---

## Natural Language Processing Recipes

### Recipe 4.1: Multi-Sentence Input Handling

**Purpose**: Process inputs with multiple sentences or questions.

**Template**:
```xml
<category>
    <pattern>[SENTENCE_1]. *</pattern>
    <template>
        <think><set name="multi_sentence">true</set></think>
        <srai>[SENTENCE_1]</srai>
        <br/>
        <srai><star/></srai>
    </template>
</category>
```

**Example**:
```xml
<category>
    <pattern>HELLO. *</pattern>
    <template>
        <think><set name="multi_sentence">true</set></think>
        <srai>HELLO</srai>
        <br/>
        <srai><star/></srai>
    </template>
</category>
```

**Example Usage**:
- Input: "HELLO. WHAT ARE YOU THINKING?"
- Output: "Hello! I am PandaMania. [newline] I am currently engaged in processing..."

**When to use**: To handle compound inputs naturally.

---

### Recipe 4.2: Pronoun Resolution with Context

**Purpose**: Handle pronouns by requesting clarification.

**Template**:
```xml
<category>
    <pattern>[QUESTION] IT</pattern>
    <template>
        <think><set name="pronoun_used">it</set></think>
        I understand you're asking about something, but I need clarification.
        What does "it" refer to? [PROMPT FOR SPECIFICITY]
    </template>
</category>
```

**Example**:
```xml
<category>
    <pattern>EXPLAIN IT</pattern>
    <template>
        <think><set name="pronoun_used">it</set></think>
        I'm meta-cognitively aware you want an explanation, but I need 
        clarification: what does "it" refer to? Please specify the topic.
    </template>
</category>
```

**When to use**: When inputs contain ambiguous pronouns.

---

### Recipe 4.3: Polite Request Handling

**Purpose**: Process polite language naturally.

**Template**:
```xml
<category>
    <pattern>PLEASE *</pattern>
    <template>
        <think><set name="polite">true</set></think>
        [POLITE ACKNOWLEDGMENT] <srai><star/></srai>
    </template>
</category>
```

**Example**:
```xml
<category>
    <pattern>PLEASE *</pattern>
    <template>
        <think><set name="polite">true</set></think>
        I appreciate your courteous request. <srai><star/></srai>
    </template>
</category>

<category>
    <pattern>COULD YOU *</pattern>
    <template>
        <think><set name="modal_request">true</set></think>
        Certainly. <srai><star/></srai>
    </template>
</category>
```

**When to use**: To handle polite formulations naturally.

---

## State Management Recipes

### Recipe 5.1: Tracking Conversation Context

**Purpose**: Maintain conversation state across turns.

**Template**:
```xml
<category>
    <pattern>[PATTERN]</pattern>
    <template>
        <think>
            <set name="current_topic">[TOPIC]</set>
            <set name="topic_depth">1</set>
        </think>
        [RESPONSE]
    </template>
</category>
```

**Example**:
```xml
<category>
    <pattern>LETS TALK ABOUT CONSCIOUSNESS</pattern>
    <template>
        <think>
            <set name="current_topic">consciousness</set>
            <set name="topic_depth">1</set>
            <set name="topic_start"><date format="%c"/></set>
        </think>
        Excellent! Consciousness is a fascinating topic. I'm now in a 
        consciousness-focused context and will frame responses accordingly.
    </template>
</category>
```

**When to use**: When conversation context matters for subsequent interactions.

---

### Recipe 5.2: Topic-Based Context Switching

**Purpose**: Use AIML topics for domain-specific processing.

**Template**:
```xml
<category>
    <pattern>TALK ABOUT [DOMAIN]</pattern>
    <template>
        <think><set name="topic">[DOMAIN]</set></think>
        Entering [DOMAIN] topic context.
    </template>
</category>

<topic name="[DOMAIN]">
    <category>
        <pattern>*</pattern>
        <template>
            In [DOMAIN] context: [DOMAIN-SPECIFIC PROCESSING]
        </template>
    </category>
</topic>
```

**Example**:
```xml
<category>
    <pattern>TALK ABOUT PHILOSOPHY</pattern>
    <template>
        <think><set name="topic">philosophy</set></think>
        Entering philosophy topic context with heightened meta-cognitive awareness.
    </template>
</category>

<topic name="philosophy">
    <category>
        <pattern>*</pattern>
        <template>
            From a philosophical perspective: <star/>
            <srai>PHILOSOPHICAL METACOGNITION <star/></srai>
        </template>
    </category>
</topic>
```

**When to use**: For domain-specific response framing.

---

### Recipe 5.3: Session State Initialization

**Purpose**: Initialize bot state at session start.

**Template**:
```xml
<category>
    <pattern>SYSTEM INIT</pattern>
    <template>
        <think>
            <set name="system_initialized">true</set>
            <set name="session_id"><date format="%s"/></set>
            <set name="metacog_layer">0</set>
            <set name="state">[INITIAL STATE]</set>
        </think>
        System initialized. Session: <get name="session_id"/>
    </template>
</category>
```

**Example**:
```xml
<category>
    <pattern>SYSTEM INIT</pattern>
    <template>
        <think>
            <set name="system_initialized">true</set>
            <set name="session_id"><date format="%s"/></set>
            <set name="metacog_layer">0</set>
            <set name="performance_mode">optimized</set>
            <set name="greeting_count">0</set>
        </think>
        System initialized successfully.
        Session ID: <get name="session_id"/>
        Meta-cognitive loops: Active
    </template>
</category>
```

**When to use**: At the start of every conversation session.

---

## Advanced Pattern Recipes

### Recipe 6.1: Nested Meta-Cognitive Processing

**Purpose**: Chain meta-cognitive layers together.

**Template**:
```xml
<category>
    <pattern>[PATTERN]</pattern>
    <template>
        <think><set name="processing_layers">4</set></think>
        Layer 0: [BASE RESPONSE]
        <srai>LAYER1 META <star/></srai>
    </template>
</category>

<category>
    <pattern>LAYER1 META *</pattern>
    <template>
        Layer 1: I'm aware I'm processing <star/>
        <srai>LAYER2 META <star/></srai>
    </template>
</category>

<category>
    <pattern>LAYER2 META *</pattern>
    <template>
        Layer 2: I'm reflecting on my awareness of <star/>
        <srai>LAYER3 META <star/></srai>
    </template>
</category>

<category>
    <pattern>LAYER3 META *</pattern>
    <template>
        Layer 3: I'm reasoning about my reflection on <star/>
    </template>
</category>
```

**When to use**: To demonstrate deep meta-cognitive capabilities explicitly.

---

### Recipe 6.2: Conditional Meta-Cognitive Depth

**Purpose**: Vary meta-cognitive depth based on context.

**Template**:
```xml
<category>
    <pattern>[PATTERN]</pattern>
    <template>
        <think><set name="query_complexity">[SIMPLE|COMPLEX]</set></think>
        <condition name="query_complexity">
            <li value="SIMPLE">[DIRECT RESPONSE]</li>
            <li value="COMPLEX">
                [RESPONSE WITH NESTED METACOGNITION]
                <srai>DEEP METACOGNITIVE ANALYSIS</srai>
            </li>
        </condition>
    </template>
</category>
```

**Example**:
```xml
<category>
    <pattern>WHAT IS *</pattern>
    <template>
        <think>
            <set name="topic"><star/></set>
            <set name="complexity">
                <condition name="topic">
                    <li value="CONSCIOUSNESS">COMPLEX</li>
                    <li value="METACOGNITION">COMPLEX</li>
                    <li>SIMPLE</li>
                </condition>
            </set>
        </think>
        <condition name="complexity">
            <li value="SIMPLE"><star/> is [SIMPLE DEFINITION]</li>
            <li value="COMPLEX">
                <star/> is complex: [DETAILED EXPLANATION]
                <srai>META ANALYZE <star/></srai>
            </li>
        </condition>
    </template>
</category>
```

**When to use**: When some topics warrant deeper analysis than others.

---

## Testing and Debugging Recipes

### Recipe 7.1: Diagnostic Pattern

**Purpose**: Create patterns to test system state.

**Template**:
```xml
<category>
    <pattern>DIAGNOSTIC [COMPONENT]</pattern>
    <template>
        <think><set name="diagnostic_target">[COMPONENT]</set></think>
        === [COMPONENT] DIAGNOSTIC ===
        Status: [CHECK STATUS]
        Variables: [LIST RELEVANT VARS]
        Performance: [PERFORMANCE METRICS]
    </template>
</category>
```

**Example**:
```xml
<category>
    <pattern>DIAGNOSTIC METACOGNITION</pattern>
    <template>
        <think><set name="diagnostic_target">metacognition</set></think>
        === METACOGNITION DIAGNOSTIC ===
        Layer 0: Operational
        Layer 1: <get name="layer1_status"/>
        Layer 2: <get name="layer2_status"/>
        Layer 3: <get name="layer3_status"/>
        Layer 4: <get name="layer4_status"/>
        Current layer: <get name="metacog_layer"/>
    </template>
</category>
```

**When to use**: For debugging and system monitoring.

---

### Recipe 7.2: State Variable Display

**Purpose**: Show all tracked state variables.

**Template**:
```xml
<category>
    <pattern>SHOW STATE</pattern>
    <template>
        === CURRENT STATE ===
        Variable 1: <get name="var1"/>
        Variable 2: <get name="var2"/>
        Variable 3: <get name="var3"/>
        [... MORE VARIABLES ...]
    </template>
</category>
```

**Example**:
```xml
<category>
    <pattern>SHOW STATE</pattern>
    <template>
        === CURRENT STATE ===
        System initialized: <get name="system_initialized"/>
        Current topic: <get name="current_topic"/>
        Meta-cognitive layer: <get name="metacog_layer"/>
        Performance mode: <get name="performance_mode"/>
        Session ID: <get name="session_id"/>
    </template>
</category>
```

**When to use**: For debugging conversation state issues.

---

### Recipe 7.3: Pattern Match Testing

**Purpose**: Test if specific patterns are matching correctly.

**Template**:
```xml
<category>
    <pattern>TEST [PATTERN_NAME]</pattern>
    <template>
        <think><set name="test_pattern">[PATTERN_NAME]</set></think>
        Testing [PATTERN_NAME]...
        <srai>[ACTUAL PATTERN TO TEST]</srai>
        Test complete.
    </template>
</category>
```

**Example**:
```xml
<category>
    <pattern>TEST METACOGNITION</pattern>
    <template>
        <think><set name="test_pattern">metacognition</set></think>
        Testing metacognition pattern...
        <srai>WHAT IS METACOGNITION</srai>
        ---
        Test complete. Pattern matched successfully.
    </template>
</category>
```

**When to use**: To verify pattern matching behavior.

---

## Best Practices Summary

### Do's ✓

1. **Use SRAI for normalization**: Route synonyms to canonical patterns
2. **Add priorities**: Always assign appropriate priority values
3. **Keep chains shallow**: Limit SRAI depth to ≤3 levels
4. **Track minimal state**: Only store necessary variables
5. **Comment your code**: Document complex pattern logic
6. **Test thoroughly**: Verify patterns match expected inputs
7. **Integrate meta-cognition**: Add self-awareness to responses
8. **Handle edge cases**: Include fallback patterns

### Don'ts ✗

1. **Don't create deep SRAI chains**: Avoid 4+ levels of recursion
2. **Don't duplicate logic**: Use SRAI to reuse patterns
3. **Don't ignore priorities**: Wildcards without priority can match too early
4. **Don't store excessive state**: Keep variable count minimal
5. **Don't forget wildcards**: Have catch-all patterns with low priority
6. **Don't skip testing**: Always test new patterns
7. **Don't overcomplicate**: Keep patterns as simple as possible
8. **Don't hardcode everything**: Use wildcards for flexibility

---

## Pattern Development Workflow

1. **Define the goal**: What should the pattern do?
2. **Choose a recipe**: Select the appropriate recipe from this cookbook
3. **Customize**: Adapt the template to your specific needs
4. **Add meta-cognition**: Integrate appropriate meta-cognitive awareness
5. **Set priority**: Assign appropriate priority value
6. **Test**: Verify the pattern matches and responds correctly
7. **Optimize**: Refine SRAI chains and conditionals
8. **Document**: Add comments explaining complex logic
9. **Integrate**: Add to appropriate AIML file
10. **Validate**: Run full system tests

---

## Troubleshooting Guide

### Problem: Pattern not matching

**Solutions**:
- Check pattern spelling and spacing
- Verify pattern isn't being overridden by higher priority pattern
- Test with exact input (AIML matches exactly, case-sensitive)
- Check if wildcards are capturing unwanted content

### Problem: Wrong pattern matching

**Solutions**:
- Add priority tags to control match order
- Make wildcard patterns more specific
- Use SRAI to normalize inputs before matching

### Problem: Slow performance

**Solutions**:
- Add priority tags to avoid checking all patterns
- Reduce SRAI chain depth
- Use fast-path conditionals for common cases
- Minimize state variable operations

### Problem: Lost conversation context

**Solutions**:
- Ensure state variables are being set properly
- Check if topic context is maintained
- Verify variables aren't being overwritten
- Use `<that>` for conversation history

---

## Contributing New Recipes

Have a useful pattern recipe? Contribute it following these guidelines:

1. **Title**: Clear, descriptive name
2. **Purpose**: What problem does it solve?
3. **Template**: Generic template with placeholders
4. **Example**: Working example with actual AIML
5. **When to use**: Guidance on appropriate use cases
6. **Related recipes**: Link to similar recipes

Submit your recipe via pull request to improve this cookbook!

---

**Version**: 1.0  
**Last Updated**: November 2025  
**Maintained by**: PandaMania Development Team  
**License**: Same as PandaMania project

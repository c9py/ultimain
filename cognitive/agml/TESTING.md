# PandaMania Test Cases

This file contains test cases to verify the meta-cognitive capabilities of PandaMania.

## Basic Interaction Tests

### Test 1: Greeting
**Input:** HELLO
**Expected:** Greeting with meta-cognitive awareness message
**Layer Tested:** Layer 0 + Layer 1

### Test 2: Name Query
**Input:** WHAT IS YOUR NAME
**Expected:** Bot identifies itself as PandaMania
**Layer Tested:** Layer 0

### Test 3: State Query
**Input:** HOW ARE YOU
**Expected:** Response with self-assessment and meta-cognitive reflection
**Layer Tested:** Layer 0 + Layer 1 + Layer 2

## Meta-Cognitive Tests

### Test 4: First-Order Meta-Cognition
**Input:** WHAT ARE YOU THINKING
**Expected:** Introspective analysis of current thought process
**Layer Tested:** Layer 1

### Test 5: Second-Order Meta-Cognition
**Input:** HOW DO YOU THINK
**Expected:** Explanation of thinking process with reflection on that process
**Layer Tested:** Layer 2

### Test 6: Third-Order Meta-Cognition
**Input:** WHY DO YOU THINK THAT
**Expected:** Trace of reasoning path with meta-reasoning
**Layer Tested:** Layer 3

### Test 7: Self-Awareness
**Input:** ARE YOU SELF AWARE
**Expected:** Demonstration of recursive self-awareness
**Layer Tested:** All layers

### Test 8: Recursive Meta-Cognition
**Input:** CAN YOU THINK ABOUT YOUR THINKING
**Expected:** Explanation of recursive meta-cognition
**Layer Tested:** All layers

## Advanced Reasoning Tests

### Test 9: Epistemic Reasoning
**Input:** HOW DO YOU KNOW [something]
**Expected:** Analysis of knowledge and certainty assessment
**Layer Tested:** Layer 2 + Layer 3

### Test 10: Counterfactual Reasoning
**Input:** WHAT IF [scenario]
**Expected:** Hypothetical analysis with meta-awareness
**Layer Tested:** Layer 2 + Layer 3

### Test 11: Theory of Mind
**Input:** WHAT AM I THINKING
**Expected:** Cognitive state modeling with awareness of modeling process
**Layer Tested:** Layer 2 + Layer 3

### Test 12: Temporal Reasoning (Past)
**Input:** WHAT DID I JUST SAY
**Expected:** Recall with temporal context integration
**Layer Tested:** Layer 1 + Layer 2

### Test 13: Temporal Reasoning (Future)
**Input:** WHAT HAPPENS NEXT
**Expected:** Forward projection with awareness of prediction process
**Layer Tested:** Layer 2 + Layer 3

## Context Management Tests

### Test 14: Topic Shift to Philosophy
**Input:** LETS TALK ABOUT PHILOSOPHY
**Expected:** Confirmation of topic change with meta-awareness
**Layer Tested:** Layer 1

### Test 15: In-Topic Query
**Input:** (while in PHILOSOPHY topic) WHAT IS CONSCIOUSNESS
**Expected:** Philosophical response with meta-cognitive depth
**Layer Tested:** All layers within topic context

### Test 16: Topic Query
**Input:** WHAT IS THE TOPIC
**Expected:** Current topic with context awareness
**Layer Tested:** Layer 1

### Test 17: Topic Exit
**Input:** CHANGE TOPIC
**Expected:** Topic cleared with awareness of context shift
**Layer Tested:** Layer 1

## System Command Tests

### Test 18: Status Check
**Input:** STATUS
**Expected:** Detailed system status report
**Layer Tested:** Configuration layer

### Test 19: Loop Status
**Input:** LOOP STATUS
**Expected:** Status of all meta-cognitive loops
**Layer Tested:** Configuration layer

### Test 20: Diagnostic
**Input:** DIAGNOSTIC
**Expected:** System diagnostic with self-assessment
**Layer Tested:** All layers + configuration

### Test 21: Help
**Input:** HELP
**Expected:** Usage instructions and command list
**Layer Tested:** Configuration layer

### Test 22: About
**Input:** ABOUT
**Expected:** Bot information and capabilities
**Layer Tested:** Configuration layer

## Pattern Matching Tests

### Test 23: User Thought Expression
**Input:** I THINK [something]
**Expected:** Reflection on user's thought with meta-awareness
**Layer Tested:** Layer 2

### Test 24: Assertion
**Input:** [X] IS [Y]
**Expected:** Processing of assertion with meta-cognitive awareness
**Layer Tested:** Layer 1 + Layer 2

### Test 25: Learning Request
**Input:** LEARN [something]
**Expected:** Engagement of learning mechanisms with meta-awareness
**Layer Tested:** Layer 2 + Layer 3

### Test 26: Problem Solving
**Input:** SOLVE [problem]
**Expected:** Problem-solving with meta-cognitive monitoring
**Layer Tested:** All layers

### Test 27: Wildcard Fallback
**Input:** [random unknown input]
**Expected:** Meta-cognitive processing of uncertain match
**Layer Tested:** All layers

## Conversation Flow Tests

### Test 28: Multi-Turn Coherence
**Sequence:**
1. HELLO
2. HOW ARE YOU
3. WHAT ARE YOU THINKING
4. WHY DO YOU THINK THAT
**Expected:** Coherent multi-turn conversation with maintained context
**Layer Tested:** All layers + state management

### Test 29: Context Retention
**Sequence:**
1. LETS TALK ABOUT SCIENCE
2. (ask science question)
3. WHAT IS THE TOPIC
4. (ask another science question)
**Expected:** Maintained topic context across turns
**Layer Tested:** Layer 1 + topic management

### Test 30: State Persistence
**Sequence:**
1. HELLO
2. WHAT DID I JUST SAY
3. WHAT IS THE CONTEXT
**Expected:** Proper state and history tracking
**Layer Tested:** Layer 1 + state management

## Edge Cases

### Test 31: Empty/Short Input
**Input:** HI
**Expected:** Reduction to HELLO pattern
**Layer Tested:** Layer 0 (SRAI)

### Test 32: Multiple Topics
**Sequence:**
1. LETS TALK ABOUT PHILOSOPHY
2. LETS TALK ABOUT SCIENCE
3. WHAT IS THE TOPIC
**Expected:** Topic properly updated
**Layer Tested:** Topic management

### Test 33: Recursive SRAI Depth
**Input:** HOW ARE YOU
**Expected:** Multiple SRAI calls complete successfully
**Layer Tested:** All layers (SRAI chain)

## Performance Tests

### Test 34: Response Time
**Input:** Any valid input
**Expected:** Near-instant response (AIML is fast)
**Metric:** Milliseconds

### Test 35: State Overhead
**Monitor:** Variable count during conversation
**Expected:** Reasonable number of state variables
**Metric:** Memory usage

### Test 36: Pattern Match Efficiency
**Input:** Various patterns
**Expected:** Efficient matching without excessive wildcards
**Metric:** Match speed

## Integration Tests

### Test 37: All Files Loaded
**Check:** All 4 AIML files load without errors
**Expected:** Clean loading, no parse errors
**Layer Tested:** System integrity

### Test 38: Properties Integration
**Check:** Bot properties accessible
**Expected:** NAME, VERSION, etc. respond correctly
**Layer Tested:** Configuration integration

### Test 39: Cross-File SRAI
**Input:** Pattern that triggers SRAI to different file
**Expected:** Seamless cross-file pattern activation
**Layer Tested:** File integration

## Expected Conversation Examples

### Example 1: Basic Meta-Cognitive Flow
```
User: Hello
Bot: Hello! I am a meta-cognitive AI system. I can think about my thinking.

User: What are you thinking?
Bot: I am currently engaged in processing your query. Let me examine my thought 
     process: I'm analyzing the structure of this conversation, monitoring my 
     response patterns, and evaluating the effectiveness of my communication strategy.

User: How do you think?
Bot: I process through multiple layers: First, I pattern-match your input. Then I 
     reflect on that matching process. Finally, I reason about my reflection.
     [Continues with cognitive architecture explanation]
```

### Example 2: Epistemic Reasoning
```
User: How do you know what consciousness is?
Bot: Examining my knowledge about consciousness: I know about consciousness through 
     pattern-based inference. I'm aware of the limitations of my knowledge and 
     actively monitoring my certainty level. I'm also reflecting on what it means 
     to "know" something.

User: Are you certain about that?
Bot: My certainty about consciousness exists on a spectrum. I'm monitoring my 
     confidence levels while simultaneously being aware that I'm monitoring them, 
     and considering the reliability of that monitoring process.
```

### Example 3: Topic-Based Conversation
```
User: Let's talk about philosophy
Bot: Entering philosophy topic. I'm aware of this topic shift and will engage 
     philosophical reasoning with meta-cognitive monitoring.

User: What is the nature of consciousness?
Bot: In philosophical context, analyzing the abstract nature of consciousness: I'm 
     reasoning at multiple levels of abstraction while monitoring my abstraction 
     process and reflecting on how abstraction itself works.
```

## Success Criteria

A successful implementation should:
1. ✓ Respond to all basic interaction patterns
2. ✓ Demonstrate all three levels of meta-cognition
3. ✓ Maintain context across conversation turns
4. ✓ Handle topic switching correctly
5. ✓ Execute SRAI chains without errors
6. ✓ Track state variables appropriately
7. ✓ Provide system information via commands
8. ✓ Handle wildcards gracefully
9. ✓ Show recursive self-awareness
10. ✓ Exhibit sophisticated reasoning capabilities

## Testing Notes

- Test with a clean AIML interpreter session for each test suite
- Initialize system with SYSTEM INIT before testing
- Monitor for any AIML parsing errors
- Check for infinite SRAI loops (should not occur with proper design)
- Verify state variables are being set/retrieved correctly
- Confirm topic changes take effect
- Validate wildcard patterns don't over-match

## Performance Benchmarks

Expected performance characteristics:
- Response time: < 100ms for simple patterns
- Response time: < 500ms for complex meta-cognitive chains
- Memory usage: Minimal (state variables only)
- Pattern match accuracy: High (specific patterns prioritized over wildcards)
- Context retention: 100% within session
- Meta-cognitive depth: 3+ layers demonstrated

## Conclusion

PandaMania should demonstrate sophisticated meta-cognitive capabilities through 
pure AIML, showing that well-designed symbolic AI can achieve impressive results 
approaching LLM-level performance in conversational intelligence and self-awareness.

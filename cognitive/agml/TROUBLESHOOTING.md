# PandaMania Troubleshooting Guide

## Introduction

This guide helps you diagnose and fix common issues when developing, deploying, or using PandaMania. Each problem includes symptoms, causes, and step-by-step solutions.

## Table of Contents

1. [Installation and Setup Issues](#installation-and-setup-issues)
2. [Pattern Matching Issues](#pattern-matching-issues)
3. [Performance Issues](#performance-issues)
4. [Meta-Cognitive Loop Issues](#meta-cognitive-loop-issues)
5. [State Management Issues](#state-management-issues)
6. [XML and Syntax Issues](#xml-and-syntax-issues)
7. [Integration Issues](#integration-issues)
8. [Development and Testing Issues](#development-and-testing-issues)

---

## Installation and Setup Issues

### Issue 1.1: AIML Files Won't Load

**Symptoms**:
- Bot doesn't respond
- "No patterns loaded" error
- Empty response on all inputs

**Possible Causes**:
1. Files not in correct directory
2. XML syntax errors in AIML files
3. Bot properties not loaded
4. Files loaded in wrong order

**Solutions**:

**Step 1**: Verify file locations
```bash
ls -l *.aiml *.properties
# Should show all AIML files and bot.properties
```

**Step 2**: Check XML syntax
```bash
xmllint --noout bot.aiml
xmllint --noout advanced_metacog.aiml
xmllint --noout topics.aiml
xmllint --noout config.aiml
xmllint --noout layer4_metacog.aiml
xmllint --noout math_logic.aiml
xmllint --noout programming_tech.aiml
xmllint --noout psychology_cognition.aiml
xmllint --noout ethics_philosophy.aiml
xmllint --noout natural_language.aiml
xmllint --noout performance_optimized.aiml
```

**Step 3**: Load files in correct order
```
1. bot.properties
2. config.aiml
3. bot.aiml
4. advanced_metacog.aiml
5. topics.aiml
6. layer4_metacog.aiml
7. math_logic.aiml
8. programming_tech.aiml
9. psychology_cognition.aiml
10. ethics_philosophy.aiml
11. natural_language.aiml
12. performance_optimized.aiml
```

**Step 4**: Initialize system
```
Input: SYSTEM INIT
Expected: "System initialized" message
```

---

### Issue 1.2: Bot Properties Not Loading

**Symptoms**:
- Variables not working
- Default values used instead of configured ones
- Bot name or properties not set

**Possible Causes**:
1. bot.properties file missing
2. Incorrect property syntax
3. Properties file not loaded first

**Solutions**:

**Step 1**: Verify bot.properties exists
```bash
cat bot.properties
```

**Step 2**: Check property syntax
```properties
name=PandaMania
age=1
species=AI
gender=neutral
location=Cyberspace
master=cogpy
```

**Step 3**: Load properties before AIML files

**Step 4**: Test property access
```
Input: WHO ARE YOU
Expected: Response containing "PandaMania"
```

---

## Pattern Matching Issues

### Issue 2.1: Pattern Not Matching Expected Input

**Symptoms**:
- Specific input doesn't trigger expected response
- Fallback pattern activating instead
- Generic response when specific one expected

**Possible Causes**:
1. Case sensitivity mismatch
2. Extra spaces in input or pattern
3. Wildcard capturing unexpectedly
4. Higher priority pattern matching first

**Solutions**:

**Step 1**: Check exact pattern matching
```xml
<!-- AIML patterns are case-sensitive and match exactly -->
<pattern>HELLO</pattern>  <!-- Matches: HELLO -->
                          <!-- Doesn't match: hello, Hello, HELLO! -->
```

**Step 2**: Check for extra spaces
```xml
<!-- BAD: Extra space -->
<pattern>WHAT  IS AIML</pattern>

<!-- GOOD: Single spaces -->
<pattern>WHAT IS AIML</pattern>
```

**Step 3**: Check priority values
```xml
<!-- This might match before your specific pattern -->
<category>
    <pattern>WHAT IS *</pattern>
    <template priority="500">Generic response</template>
</category>

<!-- Add higher priority to your specific pattern -->
<category>
    <pattern>WHAT IS AIML</pattern>
    <template priority="700">Specific AIML response</template>
</category>
```

**Step 4**: Test with exact uppercase input
```
Input: WHAT IS METACOGNITION
(not: What is metacognition?)
```

---

### Issue 2.2: Wrong Pattern Matching

**Symptoms**:
- Wildcard pattern matching when specific one should
- Unexpected response to specific input
- Pattern matches too broadly

**Possible Causes**:
1. Wildcard pattern has higher priority
2. Missing specific pattern
3. Pattern order in file
4. Multiple wildcards causing confusion

**Solutions**:

**Step 1**: Use priority tags correctly
```xml
<!-- LOW priority for wildcards -->
<category>
    <pattern>*</pattern>
    <template priority="10">Fallback</template>
</category>

<!-- HIGH priority for specific patterns -->
<category>
    <pattern>HELLO</pattern>
    <template priority="900">Hello!</template>
</category>
```

**Step 2**: Check for overly broad wildcards
```xml
<!-- TOO BROAD -->
<category>
    <pattern>WHAT *</pattern>
    <template>...</template>
</category>

<!-- BETTER: More specific -->
<category>
    <pattern>WHAT IS *</pattern>
    <template>...</template>
</category>

<category>
    <pattern>WHAT ARE *</pattern>
    <template>...</template>
</category>
```

**Step 3**: Test pattern matching order
```
Input: DIAGNOSTIC
Expected: Should show which pattern matched
```

---

### Issue 2.3: SRAI Infinite Loop

**Symptoms**:
- Bot hangs or times out
- Stack overflow error
- No response after long delay

**Possible Causes**:
1. Circular SRAI reference (A→B→A)
2. Pattern that SRAI's to itself
3. Recursive loop without base case

**Solutions**:

**Step 1**: Check for circular references
```xml
<!-- BAD: Circular loop -->
<category>
    <pattern>A</pattern>
    <template><srai>B</srai></template>
</category>
<category>
    <pattern>B</pattern>
    <template><srai>A</srai></template>
</category>

<!-- GOOD: Terminates -->
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

**Step 2**: Add SRAI depth tracking (for debugging)
```xml
<category>
    <pattern>*</pattern>
    <template>
        <think>
            <set name="srai_depth">
                <map>
                    <name>increment</name>
                    <get name="srai_depth"/>
                </map>
            </set>
        </think>
        <condition>
            <li><get name="srai_depth"/> &gt; 5</li>
            <li>SRAI depth exceeded!</li>
        </condition>
    </template>
</category>
```

**Step 3**: Limit SRAI chain depth
- Keep chains ≤3 levels deep
- Always have a terminal pattern (no SRAI)

---

## Performance Issues

### Issue 3.1: Slow Response Times

**Symptoms**:
- Noticeable delay before response
- Response time increases over conversation
- System feels sluggish

**Possible Causes**:
1. No priority tags (checking all patterns)
2. Deep SRAI chains (recursive processing)
3. Too many state variables
4. Complex pattern matching with many wildcards

**Solutions**:

**Step 1**: Add priority tags
```xml
<!-- Add to ALL patterns -->
<category>
    <pattern>SYSTEM COMMAND</pattern>
    <template priority="1000">...</template>
</category>

<category>
    <pattern>COMMON QUERY</pattern>
    <template priority="500">...</template>
</category>

<category>
    <pattern>*</pattern>
    <template priority="10">...</template>
</category>
```

**Step 2**: Optimize SRAI chains
```xml
<!-- Before: 4 levels deep -->
A → B → C → D → Response

<!-- After: 2 levels deep -->
A → D → Response
B → D → Response
C → D → Response
```

**Step 3**: Reduce state variables
```xml
<!-- Only track essential variables -->
<think>
    <set name="metacog_layer">3</set>  <!-- Essential -->
    <set name="topic">philosophy</set>  <!-- Essential -->
    <!-- Remove non-essential variables -->
</think>
```

**Step 4**: Use fast-path conditionals
```xml
<category>
    <pattern>WHAT IS *</pattern>
    <template>
        <condition name="star">
            <li value="AIML">AIML is...</li>  <!-- Fast path -->
            <li><srai>GENERIC WHAT IS <star/></srai></li>  <!-- Slow path -->
        </condition>
    </template>
</category>
```

**Step 5**: Run performance diagnostic
```
Input: PERFORMANCE
Expected: Performance metrics report
```

---

### Issue 3.2: Memory Usage Growing

**Symptoms**:
- System memory increases over time
- Slower performance over long conversations
- Eventually crashes or hangs

**Possible Causes**:
1. Too many variables being stored
2. Variables not being cleared
3. Large strings in variables
4. Conversation history not being limited

**Solutions**:

**Step 1**: Clear unnecessary variables
```xml
<think>
    <set name="temporary_var"></set>  <!-- Clear by setting to empty -->
</think>
```

**Step 2**: Limit conversation history
```xml
<!-- Don't store entire conversation -->
<think>
    <set name="last_user_input"><star/></set>  <!-- Only last input -->
    <!-- Not: conversation_history = history + new_input -->
</think>
```

**Step 3**: Use local scope when possible
```xml
<!-- Variables inside patterns are scoped -->
<category>
    <pattern>*</pattern>
    <template>
        <think><set name="temp"><star/></set></think>
        Process: <get name="temp"/>
        <think><set name="temp"></set></think>  <!-- Clear after use -->
    </template>
</category>
```

---

## Meta-Cognitive Loop Issues

### Issue 4.1: Meta-Cognitive Layers Not Activating

**Symptoms**:
- Responses lack meta-cognitive awareness
- No self-reflection in answers
- "Thinking about thinking" not working

**Possible Causes**:
1. Meta-cognitive patterns not loaded
2. SRAI to meta-cognitive patterns failing
3. Meta-cognitive layer variable not set

**Solutions**:

**Step 1**: Verify meta-cognitive files loaded
```bash
# Check advanced_metacog.aiml loaded
# Check layer4_metacog.aiml loaded
```

**Step 2**: Test meta-cognitive activation
```
Input: WHAT ARE YOU THINKING
Expected: Response with meta-cognitive awareness

Input: SECOND ORDER REFLECTION
Expected: Reflection on awareness process

Input: THIRD ORDER REASONING
Expected: Reasoning about reasoning

Input: FOURTH ORDER REASONING
Expected: Architectural meta-reasoning
```

**Step 3**: Check layer tracking
```xml
<think><set name="metacog_layer">1</set></think>  <!-- Layer 1 -->
<think><set name="metacog_layer">2</set></think>  <!-- Layer 2 -->
<think><set name="metacog_layer">3</set></think>  <!-- Layer 3 -->
<think><set name="metacog_layer">4</set></think>  <!-- Layer 4 -->
```

**Step 4**: Verify SRAI chains work
```
Input: TEST METACOGNITION
Expected: Successful activation of meta-cognitive patterns
```

---

### Issue 4.2: Too Much Meta-Cognition

**Symptoms**:
- Responses too verbose
- Every answer includes meta-cognitive commentary
- User just wants simple answer

**Possible Causes**:
1. Every pattern triggers meta-cognitive SRAI
2. No conditional meta-cognition
3. Meta-cognitive depth not contextual

**Solutions**:

**Step 1**: Add conditional meta-cognition
```xml
<category>
    <pattern>WHAT IS *</pattern>
    <template>
        <think><set name="topic"><star/></set></think>
        <star/> is [DEFINITION].
        
        <!-- Only add meta-cognition for complex topics -->
        <condition name="topic">
            <li value="CONSCIOUSNESS"><srai>META REFLECT CONSCIOUSNESS</srai></li>
            <li value="METACOGNITION"><srai>META REFLECT METACOGNITION</srai></li>
            <!-- Simple topics don't trigger meta-cognition -->
        </condition>
    </template>
</category>
```

**Step 2**: Provide simple mode option
```xml
<category>
    <pattern>SIMPLE MODE</pattern>
    <template>
        <think><set name="response_mode">simple</set></think>
        Simple mode activated. I'll provide concise answers without meta-cognitive commentary.
    </template>
</category>

<category>
    <pattern>VERBOSE MODE</pattern>
    <template>
        <think><set name="response_mode">verbose</set></think>
        Verbose mode activated. I'll include full meta-cognitive analysis.
    </template>
</category>
```

---

## State Management Issues

### Issue 5.1: Variables Not Persisting

**Symptoms**:
- Variables reset between inputs
- Context lost across conversation
- Bot "forgets" previous statements

**Possible Causes**:
1. Session not maintained
2. Variables in wrong scope
3. Variables being overwritten
4. Session timeout

**Solutions**:

**Step 1**: Check session management
```xml
<!-- Initialize session -->
<category>
    <pattern>SYSTEM INIT</pattern>
    <template>
        <think>
            <set name="session_id"><date format="%s"/></set>
            <set name="session_active">true</set>
        </think>
        Session initialized: <get name="session_id"/>
    </template>
</category>
```

**Step 2**: Verify variable scope
```xml
<!-- Session-scoped variables persist -->
<think><set name="user_name">John</set></think>

<!-- Not local-only variables -->
```

**Step 3**: Check for overwrites
```xml
<!-- BAD: Overwrites every time -->
<category>
    <pattern>*</pattern>
    <template>
        <think><set name="last_input"><star/></set></think>
        <!-- This overwrites on every input -->
    </template>
</category>

<!-- GOOD: Intentional tracking -->
<category>
    <pattern>*</pattern>
    <template>
        <think>
            <set name="previous_input"><get name="last_input"/></set>
            <set name="last_input"><star/></set>
        </think>
    </template>
</category>
```

**Step 4**: Test variable persistence
```
Input: SYSTEM INIT
Input: HELLO
Input: SHOW STATE
Expected: Should show variables set in previous inputs
```

---

### Issue 5.2: Topic Context Not Working

**Symptoms**:
- Topic-specific patterns not activating
- Bot doesn't maintain topic context
- <topic> element seems ignored

**Possible Causes**:
1. Topic not set correctly
2. Topic name mismatch
3. Topic file not loaded
4. Topic patterns not properly structured

**Solutions**:

**Step 1**: Set topic explicitly
```xml
<category>
    <pattern>TALK ABOUT PHILOSOPHY</pattern>
    <template>
        <think><set name="topic">philosophy</set></think>
        Entering philosophy topic.
    </template>
</category>
```

**Step 2**: Check topic name matches
```xml
<!-- Setting topic -->
<think><set name="topic">philosophy</set></think>

<!-- Topic block must match exactly (case-sensitive) -->
<topic name="philosophy">
    <category>
        <pattern>*</pattern>
        <template>In philosophy context...</template>
    </category>
</topic>
```

**Step 3**: Exit topic when done
```xml
<category>
    <pattern>STOP TALKING ABOUT *</pattern>
    <template>
        <think><set name="topic"></set></think>
        Exiting topic context.
    </template>
</category>
```

---

## XML and Syntax Issues

### Issue 6.1: XML Validation Errors

**Symptoms**:
- "XML parse error" messages
- File won't load
- Specific line number error

**Possible Causes**:
1. Unclosed tags
2. Missing <?xml?> declaration
3. Special characters not escaped
4. Invalid XML structure

**Solutions**:

**Step 1**: Check XML declaration
```xml
<?xml version="1.0" encoding="UTF-8"?>
<aiml version="2.0">
    <!-- Your patterns here -->
</aiml>
```

**Step 2**: Validate with xmllint
```bash
xmllint --noout your_file.aiml
```

**Step 3**: Check for unclosed tags
```xml
<!-- BAD -->
<category>
    <pattern>HELLO</pattern>
    <template>Hi!</template>
<!-- Missing </category> -->

<!-- GOOD -->
<category>
    <pattern>HELLO</pattern>
    <template>Hi!</template>
</category>
```

**Step 4**: Escape special characters
```xml
<!-- BAD -->
<template>I'm thinking < deeply</template>

<!-- GOOD -->
<template>I&apos;m thinking &lt; deeply</template>

<!-- Character entities: -->
<!-- &lt;   = < -->
<!-- &gt;   = > -->
<!-- &amp;  = & -->
<!-- &apos; = ' -->
<!-- &quot; = " -->
```

---

### Issue 6.2: AIML 2.0 Features Not Working

**Symptoms**:
- Priority attribute ignored
- Advanced features not supported
- Interpreter treats as AIML 1.0

**Possible Causes**:
1. Wrong AIML version in declaration
2. Interpreter doesn't support AIML 2.0
3. Feature not available in interpreter

**Solutions**:

**Step 1**: Check AIML version
```xml
<?xml version="1.0" encoding="UTF-8"?>
<aiml version="2.0">  <!-- Must be 2.0 for priority, etc. -->
```

**Step 2**: Verify interpreter support
```
Check your interpreter documentation for AIML 2.0 support
Program AB: Full AIML 2.0 support ✓
Program Y: Partial AIML 2.0 support
Pandorabots: Full AIML 2.0 support ✓
```

**Step 3**: Use fallback for unsupported features
```xml
<!-- If priority not supported, rely on pattern order in file -->
<!-- More specific patterns first -->
<category>
    <pattern>SPECIFIC PATTERN</pattern>
    <template>Specific response</template>
</category>

<!-- More general patterns last -->
<category>
    <pattern>*</pattern>
    <template>General response</template>
</category>
```

---

## Integration Issues

### Issue 7.1: Can't Connect to AIML Interpreter

**Symptoms**:
- Connection refused errors
- Timeout connecting to bot
- Can't send inputs

**Possible Causes**:
1. Interpreter not running
2. Wrong host/port
3. Firewall blocking connection
4. Authentication required

**Solutions**:

**Step 1**: Verify interpreter running
```bash
# Check if process is running
ps aux | grep -i "program-ab\|pandorabot"

# Check port listening
netstat -an | grep [PORT]
```

**Step 2**: Check connection settings
```python
# Example Python connection
import requests

host = "localhost"  # or IP address
port = 8080  # or your port

url = f"http://{host}:{port}/bot/test/talk"
```

**Step 3**: Test with curl
```bash
curl -X POST http://localhost:8080/bot/test/talk \
  -d "input=HELLO" \
  -d "sessionid=test123"
```

---

### Issue 7.2: Responses Not Formatted Correctly

**Symptoms**:
- HTML tags in response
- Line breaks not working
- Formatting appears broken

**Possible Causes**:
1. Client not handling HTML/formatting
2. <br/> tags not converted
3. Response encoding issues

**Solutions**:

**Step 1**: Check response format
```xml
<!-- AIML uses <br/> for line breaks -->
<template>
    First line<br/>
    Second line<br/>
    Third line
</template>
```

**Step 2**: Convert <br/> in client
```python
# Python example
response = response.replace("<br/>", "\n")
```

**Step 3**: Remove HTML if needed
```python
import re
response = re.sub('<[^<]+?>', '', response)
```

---

## Development and Testing Issues

### Issue 8.1: Pattern Not Behaving as Expected

**Symptoms**:
- Pattern works sometimes but not always
- Inconsistent behavior
- Can't figure out why pattern isn't matching

**Possible Causes**:
1. User input not matching pattern exactly
2. Context/topic affecting match
3. Variable state affecting conditional
4. Pattern being overridden

**Solutions**:

**Step 1**: Add debug output
```xml
<category>
    <pattern>TEST PATTERN</pattern>
    <template>
        DEBUG: Pattern matched!
        Star: <star/>
        Topic: <get name="topic"/>
        Layer: <get name="metacog_layer"/>
    </template>
</category>
```

**Step 2**: Test with exact input
```
# Copy pattern exactly
Pattern: WHAT IS METACOGNITION
Test: WHAT IS METACOGNITION
(exact match, all caps, single spaces)
```

**Step 3**: Check pattern priority
```
Input: DIAGNOSTIC
Look for: Which pattern matched
```

**Step 4**: Isolate the pattern
```xml
<!-- Comment out other patterns temporarily -->
<!-- Test target pattern in isolation -->
```

---

### Issue 8.2: Can't Test Meta-Cognitive Features

**Symptoms**:
- Don't know how to test nested loops
- Can't verify meta-cognition working
- No clear testing methodology

**Possible Causes**:
1. Lack of test patterns
2. Don't know test inputs
3. Expected outputs unclear

**Solutions**:

**Step 1**: Use provided test cases
```
See TESTING.md for comprehensive test cases
```

**Step 2**: Test each layer explicitly
```
Layer 0: HELLO
Expected: Basic greeting

Layer 1: WHAT ARE YOU THINKING
Expected: Self-aware response

Layer 2: HOW DO YOU THINK
Expected: Reflection on thinking

Layer 3: WHY DO YOU THINK THAT
Expected: Meta-reasoning

Layer 4: EVALUATE YOUR ARCHITECTURE
Expected: Architectural analysis
```

**Step 3**: Check meta-cognitive state
```
Input: SHOW STATE
Expected: Current meta-cognitive layer and variables
```

---

## Getting More Help

### Resources

1. **Documentation**
   - README.md - User guide
   - IMPLEMENTATION.md - Technical details
   - TESTING.md - Test cases
   - PATTERN_COOKBOOK.md - Pattern recipes
   - ROADMAP.md - Development plans

2. **Community**
   - GitHub Issues: Report bugs
   - GitHub Discussions: Ask questions
   - Pull Requests: Contribute fixes

3. **Debugging Tools**
   ```
   DIAGNOSTIC - System diagnostic
   STATUS - Current status
   SHOW STATE - Variable state
   PERFORMANCE - Performance metrics
   BENCHMARK - Run benchmark tests
   ```

### Reporting Bugs

When reporting issues, include:

1. **Input**: Exact input that caused problem
2. **Expected**: What you expected to happen
3. **Actual**: What actually happened
4. **Environment**: Interpreter, version, OS
5. **Files**: Which AIML files loaded
6. **Steps**: Steps to reproduce

### Example Bug Report

```
Title: Pattern not matching for WHAT IS CONSCIOUSNESS

Input: WHAT IS CONSCIOUSNESS
Expected: Detailed explanation of consciousness
Actual: Generic "I don't understand" fallback

Environment:
- Interpreter: Program AB 0.0.4.3
- OS: Ubuntu 20.04
- Files loaded: bot.aiml, advanced_metacog.aiml, topics.aiml

Steps to reproduce:
1. Load all AIML files
2. Input: SYSTEM INIT
3. Input: WHAT IS CONSCIOUSNESS
4. Observe generic fallback instead of specific response

Additional info:
- Pattern exists in advanced_metacog.aiml line 234
- Other WHAT IS patterns work fine
- Issue started after adding priority tags
```

---

**Version**: 1.0  
**Last Updated**: November 2025  
**Maintained by**: PandaMania Development Team

For additional help, see CONTRIBUTING.md or open a GitHub issue.

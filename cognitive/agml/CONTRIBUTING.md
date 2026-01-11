# Contributing to PandaMania

Thank you for your interest in contributing to PandaMania! This document provides guidelines and instructions for contributing to this meta-cognitive AIML project.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [How to Contribute](#how-to-contribute)
- [Development Process](#development-process)
- [AIML Pattern Guidelines](#aiml-pattern-guidelines)
- [Testing Requirements](#testing-requirements)
- [Documentation Standards](#documentation-standards)
- [Pull Request Process](#pull-request-process)
- [Community](#community)

---

## Code of Conduct

### Our Pledge

We are committed to providing a welcoming and inclusive environment for all contributors, regardless of experience level, background, or identity.

### Expected Behavior

- **Be Respectful**: Treat all contributors with respect and courtesy
- **Be Collaborative**: Work together constructively
- **Be Patient**: Help newcomers learn and grow
- **Be Professional**: Keep discussions focused and constructive
- **Give Credit**: Acknowledge others' contributions

### Unacceptable Behavior

- Harassment, discrimination, or offensive comments
- Personal attacks or insults
- Trolling or deliberately disruptive behavior
- Sharing private information without consent
- Plagiarism or claiming others' work as your own

### Reporting Issues

If you experience or witness unacceptable behavior, please contact the maintainers at [cogpy@github.com].

---

## Getting Started

### Prerequisites

Before contributing, ensure you have:

1. **AIML Knowledge**: Basic understanding of AIML 2.0 syntax
2. **Git Skills**: Familiarity with Git and GitHub workflows
3. **AIML Interpreter**: Access to Program AB, Program Y, or similar
4. **Python** (optional): For testing and demonstration scripts

### Setting Up Your Environment

1. **Fork the Repository**
   ```bash
   # Visit https://github.com/cogpy/pandamania and click "Fork"
   ```

2. **Clone Your Fork**
   ```bash
   git clone https://github.com/YOUR-USERNAME/pandamania.git
   cd pandamania
   ```

3. **Set Upstream Remote**
   ```bash
   git remote add upstream https://github.com/cogpy/pandamania.git
   ```

4. **Install AIML Interpreter** (if needed)
   ```bash
   # Example for Program Y
   pip install programy
   
   # Or use another AIML 2.0 compatible interpreter
   ```

5. **Verify Setup**
   ```bash
   # Run the demo script
   python3 demo.py
   
   # Check startup guide
   python3 startup.py
   ```

---

## How to Contribute

### Types of Contributions

We welcome various types of contributions:

#### 1. **AIML Patterns** (Highest Priority)
- Add new conversation patterns
- Improve existing patterns
- Expand domain-specific knowledge
- Enhance meta-cognitive capabilities

#### 2. **Bug Fixes**
- Fix XML syntax errors
- Correct SRAI recursion issues
- Resolve pattern matching problems
- Fix documentation errors

#### 3. **Documentation**
- Improve README and guides
- Add examples and tutorials
- Create video demonstrations
- Write blog posts about the project

#### 4. **Testing**
- Add test cases to TESTING.md
- Create automated tests
- Perform user testing
- Report bugs and issues

#### 5. **Tools and Scripts**
- Python utilities
- Pattern validation tools
- Testing frameworks
- Visualization tools

#### 6. **Research**
- Cognitive science validation
- Performance benchmarking
- Comparative studies
- Academic papers

---

## Development Process

### Workflow

1. **Check Existing Issues**
   - Look for existing issues or feature requests
   - Avoid duplicate work
   - Comment if you want to work on something

2. **Create an Issue** (for new features/bugs)
   - Describe the problem or enhancement
   - Explain your proposed solution
   - Wait for maintainer feedback before starting work

3. **Create a Branch**
   ```bash
   git checkout -b feature/your-feature-name
   # or
   git checkout -b fix/bug-description
   ```

4. **Make Changes**
   - Follow coding guidelines (see below)
   - Test thoroughly
   - Document your changes

5. **Commit Changes**
   ```bash
   git add .
   git commit -m "Brief description of changes"
   ```

6. **Keep Your Branch Updated**
   ```bash
   git fetch upstream
   git rebase upstream/main
   ```

7. **Push Changes**
   ```bash
   git push origin your-branch-name
   ```

8. **Create Pull Request**
   - Go to GitHub and create a PR
   - Fill out the PR template
   - Link related issues

---

## AIML Pattern Guidelines

### General Principles

1. **Follow Meta-Cognitive Architecture**
   - Respect the 4-layer structure (Layers 0-3)
   - Use SRAI for recursive activation
   - Maintain awareness tracking with `<think>` and `<set>`

2. **Pattern Quality**
   - Be specific: Avoid overly broad wildcards
   - Be efficient: Minimize SRAI chain length
   - Be clear: Use descriptive variable names
   - Be consistent: Follow existing pattern style

3. **XML Formatting**
   ```xml
   <!-- Good: Well-formatted with clear structure -->
   <category>
       <pattern>PATTERN TEXT HERE</pattern>
       <template>
           <think>
               <set name="variable_name">value</set>
           </think>
           Response text here.
           <srai>RELATED PATTERN</srai>
       </template>
   </category>
   ```

### Pattern Categories by Layer

#### Layer 0: Base Patterns
```xml
<!-- Direct responses, no meta-cognitive processing -->
<category>
    <pattern>SIMPLE QUESTION</pattern>
    <template>Simple answer.</template>
</category>
```

#### Layer 1: Self-Aware Patterns
```xml
<!-- Add state tracking and awareness -->
<category>
    <pattern>QUESTION ABOUT STATE</pattern>
    <template>
        <think><set name="awareness_active">true</set></think>
        Response with awareness.
        <srai>METACOGNITIVE PROCESS</srai>
    </template>
</category>
```

#### Layer 2: Reflective Patterns
```xml
<!-- Reflection on awareness itself -->
<category>
    <pattern>METACOGNITIVE PROCESS</pattern>
    <template>
        <think><set name="reflection_depth">2</set></think>
        Reflecting on my processing.
        <srai>META REASONING</srai>
    </template>
</category>
```

#### Layer 3: Meta-Reasoning Patterns
```xml
<!-- Reasoning about reasoning -->
<category>
    <pattern>META REASONING</pattern>
    <template>
        <think><set name="reasoning_level">3</set></think>
        Reasoning about my reasoning processes.
    </template>
</category>
```

### Best Practices

#### DO:
✅ Use uppercase for patterns: `<pattern>HELLO</pattern>`  
✅ Normalize variations with SRAI: `<pattern>HI</pattern><template><srai>HELLO</srai></template>`  
✅ Track state with meaningful variables: `<set name="greeting_complete">true</set>`  
✅ Add comments explaining complex patterns: `<!-- Layer 2 reflection on awareness -->`  
✅ Test patterns thoroughly before submitting  
✅ Document expected behavior in TESTING.md  

#### DON'T:
❌ Use lowercase in patterns: `<pattern>hello</pattern>`  
❌ Create overly broad wildcards: `<pattern>*</pattern>` in main files  
❌ Make infinite SRAI loops: Pattern A → Pattern B → Pattern A  
❌ Use cryptic variable names: `<set name="x">y</set>`  
❌ Forget to close XML tags properly  
❌ Add patterns without testing  

### Adding New Patterns

1. **Choose the Appropriate File**
   - `bot.aiml`: Core interaction patterns
   - `advanced_metacog.aiml`: Advanced reasoning patterns
   - `topics.aiml`: Topic-specific patterns
   - `config.aiml`: System configuration and diagnostics

2. **Follow the Structure**
   ```xml
   <!-- Add descriptive comment -->
   <category>
       <pattern>YOUR PATTERN</pattern>
       <template>
           <!-- State management if needed -->
           <think>
               <set name="relevant_state">value</set>
           </think>
           
           <!-- Response text -->
           Your response text here.
           
           <!-- Trigger meta-cognitive processing if appropriate -->
           <srai>NEXT LAYER PATTERN</srai>
       </template>
   </category>
   ```

3. **Add Test Cases**
   - Add corresponding test case to TESTING.md
   - Document expected input and output
   - Note which layers are tested

---

## Testing Requirements

### Manual Testing

Before submitting, test your patterns:

1. **Load AIML Files**
   ```python
   # Using Program Y or similar
   bot.load_aiml('bot.aiml')
   bot.load_aiml('advanced_metacog.aiml')
   bot.load_aiml('topics.aiml')
   bot.load_aiml('config.aiml')
   ```

2. **Test Your Pattern**
   ```python
   response = bot.respond("YOUR INPUT")
   print(response)
   ```

3. **Verify**
   - ✓ Pattern matches correctly
   - ✓ Response is appropriate
   - ✓ State variables are set correctly
   - ✓ SRAI chains execute without errors
   - ✓ No infinite loops
   - ✓ Meta-cognitive layers activate as expected

### Test Documentation

Add test case to TESTING.md:

```markdown
### Test XX: Your Test Name
**Input:** YOUR INPUT
**Expected:** Expected bot response
**Layer Tested:** Layer X
**Notes:** Any special considerations
```

### XML Validation

Ensure XML is well-formed:

```bash
# Use xmllint if available
xmllint --noout bot.aiml
xmllint --noout advanced_metacog.aiml
xmllint --noout topics.aiml
xmllint --noout config.aiml
```

---

## Documentation Standards

### Code Comments

```xml
<!-- High-level description of pattern group -->

<category>
    <pattern>PATTERN</pattern>
    <template>
        <!-- Explain complex logic here -->
        <think>
            <!-- Note state changes -->
            <set name="var">value</set>
        </think>
        Response.
    </template>
</category>
```

### Documentation Files

When updating documentation:

1. **README.md**: User-facing, getting started information
2. **IMPLEMENTATION.md**: Technical details for developers
3. **TESTING.md**: Test cases and validation
4. **ROADMAP.md**: Future development plans
5. **CONTRIBUTING.md**: This file (contribution guidelines)

### Writing Style

- **Clear**: Use simple, direct language
- **Concise**: Avoid unnecessary verbosity
- **Complete**: Include all necessary information
- **Correct**: Ensure technical accuracy
- **Consistent**: Match existing style and terminology

---

## Pull Request Process

### Before Submitting

1. ✓ All tests pass
2. ✓ XML is well-formed
3. ✓ Documentation is updated
4. ✓ Code follows guidelines
5. ✓ Commit messages are clear
6. ✓ No merge conflicts

### PR Template

When creating a PR, include:

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Performance improvement
- [ ] Other (specify)

## Changes Made
- List specific changes
- Include file names
- Explain reasoning

## Testing
- How did you test?
- What test cases were added?
- Any edge cases considered?

## Related Issues
Fixes #123
Related to #456

## Checklist
- [ ] XML is valid
- [ ] Tests pass
- [ ] Documentation updated
- [ ] Follows AIML guidelines
- [ ] No infinite loops
- [ ] Commit messages are clear
```

### Review Process

1. **Automated Checks**: CI runs XML validation
2. **Maintainer Review**: Code review by maintainer(s)
3. **Discussion**: Address feedback and questions
4. **Approval**: Maintainer approves PR
5. **Merge**: PR is merged into main branch

### After Merge

- Your contribution is acknowledged in releases
- Close related issues
- Update your fork

---

## Community

### Communication Channels

- **GitHub Issues**: Bug reports and feature requests
- **GitHub Discussions**: General questions and ideas
- **Email**: cogpy@github.com (for sensitive matters)

### Getting Help

If you need help:

1. Check existing documentation
2. Search GitHub issues
3. Ask in GitHub Discussions
4. Contact maintainers

### Recognition

Contributors are recognized:

- In commit history
- In release notes
- In CONTRIBUTORS.md (if we create one)
- On project website (if applicable)

---

## Licensing

By contributing to PandaMania, you agree that your contributions will be licensed under the same license as the project (see LICENSE file).

### Copyright

- You retain copyright to your contributions
- You grant the project a license to use your contributions
- Ensure you have the right to contribute code/content

---

## Questions?

If you have questions about contributing:

1. Check this guide first
2. Search GitHub issues
3. Ask in GitHub Discussions
4. Contact maintainers: cogpy@github.com

---

## Thank You!

Your contributions make PandaMania better for everyone. We appreciate your time, effort, and expertise!

**Key Points to Remember:**
- 🧠 Maintain the meta-cognitive architecture
- ✅ Test thoroughly before submitting
- 📝 Document your changes
- 🤝 Be respectful and collaborative
- 🎯 Follow the guidelines in this document

Happy contributing! 🎉

---

**Document Version**: 1.0  
**Last Updated**: November 2025  
**Maintainer**: cogpy  

For more information, see:
- [README.md](README.md) - Project overview
- [ROADMAP.md](ROADMAP.md) - Development roadmap
- [IMPLEMENTATION.md](IMPLEMENTATION.md) - Technical details
- [TESTING.md](TESTING.md) - Test cases

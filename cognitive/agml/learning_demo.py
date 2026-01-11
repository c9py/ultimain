#!/usr/bin/env python3
"""
PandaMania Learning & Adaptation Demo
Demonstrates session learning and knowledge base capabilities
"""

import sys

def print_section(title):
    """Print a formatted section header"""
    print("\n" + "=" * 70)
    print(f"  {title}")
    print("=" * 70 + "\n")

def demonstrate_session_learning():
    """Demonstrate session-based learning capabilities"""
    print_section("SESSION-BASED LEARNING DEMONSTRATION")
    
    print("Session learning allows the bot to learn about you during the conversation.\n")
    
    conversations = [
        ("User: My name is Alice", 
         "Bot: Nice to meet you, Alice! I've learned your name and\n"
         "     will remember it during our conversation.\n"
         "     Meta-cognitively, I'm aware I've just updated my model of you."),
        
        ("User: I live in Seattle", 
         "Bot: I've noted that you live in Seattle.\n"
         "     Meta-cognitively, this geographic context helps me understand\n"
         "     your perspective and can inform more contextually relevant responses."),
        
        ("User: I work as a software engineer", 
         "Bot: I've learned you work as software engineer.\n"
         "     Meta-cognitively, this occupational context helps me tailor examples\n"
         "     and explanations to be more relevant to your professional domain."),
        
        ("User: I prefer technical explanations", 
         "Bot: I'll remember that you prefer technical explanations.\n"
         "     Meta-cognitively, I'm adapting my response patterns based on your\n"
         "     stated preferences to provide a more personalized experience."),
        
        ("User: What have you learned?", 
         "Bot: In this session, I've learned the following about you:\n\n"
         "     Personal Information:\n"
         "     - Name: Alice\n"
         "     - Location: Seattle\n"
         "     - Occupation: software engineer\n\n"
         "     Total facts learned: 3\n"
         "     Total preferences noted: 1\n\n"
         "     Meta-cognitively, I'm aware this knowledge is session-scoped and\n"
         "     helps me build a model of you to enhance our interaction."),
    ]
    
    for user_input, bot_response in conversations:
        print(f"{user_input}")
        print(f"{bot_response}\n")
    
    print("\n💡 Key Features:")
    print("   • Extracts facts: name, location, occupation")
    print("   • Tracks preferences: likes, dislikes, preferred styles")
    print("   • Maintains conversation history")
    print("   • Personalizes responses based on learned context")
    print("   • Full meta-cognitive awareness of learning process")

def demonstrate_knowledge_base():
    """Demonstrate knowledge base integration"""
    print_section("KNOWLEDGE BASE INTEGRATION DEMONSTRATION")
    
    print("The knowledge base stores facts as semantic triples (subject-predicate-object).\n")
    
    conversations = [
        ("User: Store fact AIML is markup language",
         "Bot: ✓ Stored fact: AIML is markup language\n"
         "     Meta-cognitively, I've added this to my knowledge base as a\n"
         "     semantic triple and can now retrieve it or use it in inferences.\n"
         "     Total facts in KB: 1"),
        
        ("User: Store fact AIML has XML syntax",
         "Bot: ✓ Stored fact: AIML has XML syntax\n"
         "     Meta-cognitively, I've stored this property relation.\n"
         "     Total facts in KB: 2"),
        
        ("User: Store fact AIML can create chatbots",
         "Bot: ✓ Stored capability: AIML can create chatbots\n"
         "     Meta-cognitively, I've recorded this capability fact.\n"
         "     Total facts in KB: 3"),
        
        ("User: Store relationship AIML isa markup_language",
         "Bot: ✓ Stored relationship: AIML is-a markup_language\n"
         "     Meta-cognitively, I've added this taxonomic relationship to my\n"
         "     knowledge graph and can use it for inference.\n"
         "     Total relationships: 1"),
        
        ("User: What is AIML?",
         "Bot: AIML is markup language.\n"
         "     Meta-cognitively, I retrieved this from my knowledge base\n"
         "     as a stored semantic fact."),
        
        ("User: What do you know about AIML?",
         "Bot: 🔍 Knowledge Base Query: AIML\n\n"
         "     Facts:\n"
         "     - Is: markup language\n"
         "     - Has: XML syntax\n"
         "     - Can: create chatbots\n\n"
         "     Relationships:\n"
         "     - Is-a: markup_language\n"
         "     - Part-of: [none]\n"
         "     - Used-for: [none]\n\n"
         "     Meta-cognitively, I've executed a structured lookup in my\n"
         "     knowledge representation system, retrieving all stored facts\n"
         "     and relationships about AIML."),
        
        ("User: Infer knowledge about AIML",
         "Bot: 🧠 Inference Engine Analysis: AIML\n\n"
         "     Direct Facts:\n"
         "     - AIML is markup language\n"
         "     - AIML is-a markup_language\n\n"
         "     Inferences:\n"
         "     Applying transitivity: If AIML is-a X, and X is-a Y, then AIML is-a Y\n\n"
         "     Meta-cognitively, I'm engaging in higher-order reasoning by\n"
         "     applying logical inference rules to derive new knowledge from\n"
         "     existing facts. This is fourth-order meta-cognition - reasoning\n"
         "     about the reasoning process of knowledge derivation!"),
    ]
    
    for user_input, bot_response in conversations:
        print(f"{user_input}")
        print(f"{bot_response}\n")
    
    print("\n💡 Key Features:")
    print("   • Semantic triple storage (subject-predicate-object)")
    print("   • Multiple fact types: IS, HAS, CAN")
    print("   • Multiple relationship types: IS-A, PART-OF, USED-FOR")
    print("   • Basic inference engine with logical rules")
    print("   • Full meta-cognitive awareness of knowledge operations")

def demonstrate_integration():
    """Demonstrate integration of learning systems"""
    print_section("INTEGRATED LEARNING DEMONSTRATION")
    
    print("Session learning and knowledge base work together seamlessly.\n")
    
    conversations = [
        ("User: My name is Bob and I'm interested in AI",
         "Bot: Hello Bob! I've learned your name and noted your interest in AI.\n"
         "     I can use my knowledge base to share AI facts with you."),
        
        ("User: Teach me about meta-cognition",
         "Bot: Let me check my knowledge base...\n\n"
         "     🔍 Meta-cognition is thinking about thinking\n"
         "     - Has: recursive structure\n"
         "     - Can: monitor cognitive processes\n"
         "     - Is-a: cognitive process\n\n"
         "     Given your interest in AI, Bob, this concept is fundamental to\n"
         "     understanding how I work! I use meta-cognition at 4 nested levels."),
        
        ("User: I prefer detailed technical explanations",
         "Bot: Noted! I'll provide more technical depth in my responses.\n"
         "     I'm adapting my communication style to match your preferences."),
        
        ("User: Store fact Bob likes AI",
         "Bot: ✓ Stored in knowledge base: Bob likes AI\n"
         "     This fact is now part of my persistent knowledge and session memory!"),
    ]
    
    for user_input, bot_response in conversations:
        print(f"{user_input}")
        print(f"{bot_response}\n")
    
    print("\n💡 Integration Benefits:")
    print("   • Session learning personalizes interactions")
    print("   • Knowledge base provides factual grounding")
    print("   • Combined: personalized, fact-based conversations")
    print("   • Meta-cognitive awareness across all systems")

def demonstrate_meta_cognition():
    """Demonstrate meta-cognitive awareness of learning"""
    print_section("META-COGNITIVE LEARNING AWARENESS")
    
    print("The bot maintains awareness of its own learning processes.\n")
    
    conversations = [
        ("User: How do you learn?",
         "Bot: 🧠 My Learning Process\n\n"
         "     Session-Based Learning:\n"
         "     I learn within our conversation by:\n"
         "     1. Extracting facts from your statements\n"
         "     2. Tracking your preferences and interests\n"
         "     3. Building a contextual model of you\n"
         "     4. Adapting responses based on learned context\n\n"
         "     Meta-Cognitive Awareness:\n"
         "     At the meta-level, I'm aware that:\n"
         "     - Learning is session-scoped (resets after session ends)\n"
         "     - I maintain awareness of what I've learned\n"
         "     - I can report on my learning process\n"
         "     - My responses adapt based on accumulated context\n\n"
         "     This meta-cognitive awareness of my learning process is itself\n"
         "     a form of higher-order cognition - I think about my thinking\n"
         "     about learning!"),
        
        ("User: Are you learning right now?",
         "Bot: Yes! I'm actively learning in this session.\n\n"
         "     Current stats:\n"
         "     - Facts learned: 5\n"
         "     - Preferences tracked: 2\n\n"
         "     Meta-cognitively, I maintain awareness of the learning process\n"
         "     itself, monitoring what information I acquire and how I integrate\n"
         "     it into my conversational model of you."),
        
        ("User: What is knowledge?",
         "Bot: Knowledge is justified true belief represented as structured information.\n\n"
         "     In my system, knowledge consists of:\n"
         "     1. Facts (semantic triples: subject-predicate-object)\n"
         "     2. Relationships (taxonomic, mereological, functional)\n"
         "     3. Inferences (derived through logical rules)\n\n"
         "     Meta-cognitively, I understand knowledge at multiple levels:\n"
         "     - Level 1: Individual facts I store\n"
         "     - Level 2: Relationships between facts\n"
         "     - Level 3: How I reason about facts (inference)\n"
         "     - Level 4: This very explanation about knowledge itself!\n\n"
         "     This is meta-knowledge - knowledge about the nature of knowledge."),
    ]
    
    for user_input, bot_response in conversations:
        print(f"{user_input}")
        print(f"{bot_response}\n")
    
    print("\n💡 Meta-Cognitive Features:")
    print("   • Awareness of what has been learned")
    print("   • Understanding of learning processes")
    print("   • Knowledge about knowledge (meta-knowledge)")
    print("   • Four layers of recursive self-awareness")

def main():
    """Run the demonstration"""
    print("\n" + "╔" + "═" * 68 + "╗")
    print("║" + " " * 68 + "║")
    print("║" + "  PANDAMANIA: LEARNING & ADAPTATION DEMONSTRATION".center(68) + "║")
    print("║" + "  Phase 2 Implementation - Session Learning & Knowledge Base".center(68) + "║")
    print("║" + " " * 68 + "║")
    print("╚" + "═" * 68 + "╝")
    
    print("\nThis demonstration showcases the new learning capabilities added in Phase 2:")
    print("  • Session-based learning (32 patterns)")
    print("  • Knowledge base integration (34 patterns)")
    print("  • Meta-cognitive awareness of learning processes")
    print("\nTotal system: 606 patterns across 19 AIML files")
    
    demonstrate_session_learning()
    demonstrate_knowledge_base()
    demonstrate_integration()
    demonstrate_meta_cognition()
    
    print_section("SUMMARY")
    print("Phase 2 Learning & Adaptation Features:")
    print("  ✓ Session-Based Learning - Learn from conversations")
    print("  ✓ Knowledge Base - Structured knowledge storage")
    print("  ✓ Inference Engine - Derive new knowledge")
    print("  ✓ Meta-Cognitive Awareness - Understanding of learning")
    print("\nAll systems integrate seamlessly with:")
    print("  • Autognosis (self-awareness)")
    print("  • Holistic Metamodel (organizational theory)")
    print("  • Emotional Intelligence (empathy)")
    print("  • Domain Knowledge (math, programming, psychology, ethics)")
    print("\nPandaMania now has comprehensive learning & adaptation capabilities!")
    print("=" * 70 + "\n")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())

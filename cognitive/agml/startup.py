#!/usr/bin/env python3
"""
PandaMania Startup Script
Loads AIML files and initializes the meta-cognitive bot
"""

import os
import sys

def print_banner():
    """Display startup banner"""
    banner = """
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║                      PANDAMANIA v2.0.0                        ║
║                    Phase 1 Complete Edition                   ║
║                                                               ║
║        Meta-Cognitive AIML Bot with Nested Loop Dynamics      ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝
    """
    print(banner)
    print("\nInitializing meta-cognitive architecture...")
    print("Loading nested cognitive loops...")
    print("Phase 1 enhancements: ✓ Complete\n")

def list_aiml_files():
    """List all AIML files in the current directory"""
    aiml_files = [
        "config.aiml",
        "bot.aiml", 
        "advanced_metacog.aiml",
        "topics.aiml"
    ]
    
    print("AIML Files to Load:")
    print("-" * 50)
    
    for i, filename in enumerate(aiml_files, 1):
        if os.path.exists(filename):
            size = os.path.getsize(filename)
            print(f"  {i}. {filename:<30} ({size:>6} bytes) ✓")
        else:
            print(f"  {i}. {filename:<30} MISSING ✗")
    
    print("-" * 50)
    print()

def show_instructions():
    """Display usage instructions"""
    print("Loading Instructions:")
    print("=" * 50)
    print()
    print("1. Ensure you have an AIML 2.0 compatible interpreter")
    print("   (e.g., Program AB, Program Y, or aiml library)")
    print()
    print("2. Load files in this order:")
    print("   Core Architecture:")
    print("   - bot.properties")
    print("   - config.aiml")
    print("   - bot.aiml")
    print("   - advanced_metacog.aiml")
    print("   - layer4_metacog.aiml      [Phase 1]")
    print("   - topics.aiml")
    print()
    print("   Domain Knowledge (Phase 1):")
    print("   - math_logic.aiml          [Phase 1]")
    print("   - programming_tech.aiml    [Phase 1]")
    print("   - psychology_cognition.aiml [Phase 1]")
    print("   - ethics_philosophy.aiml   [Phase 1]")
    print()
    print("   Performance & NL (Phase 1 Complete):")
    print("   - natural_language.aiml    [NEW - Phase 1 Complete] ✨")
    print("   - performance_optimized.aiml [NEW - Phase 1 Complete] ✨")
    print()
    print("   Total: 11 AIML files, 406 patterns")
    print()
    print("3. Initialize the system:")
    print("   Input: SYSTEM INIT")
    print()
    print("4. Start conversation:")
    print("   Input: HELLO")
    print()
    print("5. Try these commands:")
    print("   - STATUS")
    print("   - LOOP STATUS")
    print("   - FOURTH ORDER STATUS      [Phase 1]")
    print("   - PERFORMANCE             [NEW - Phase 1 Complete]")
    print("   - CHECK EFFICIENCY        [NEW - Phase 1 Complete]")
    print("   - HELP")
    print("   - DIAGNOSTIC")
    print()
    print("=" * 50)
    print()

def show_meta_cognitive_info():
    """Display meta-cognitive architecture information"""
    print("Meta-Cognitive Architecture:")
    print("=" * 50)
    print()
    print("Layer 0: Base Processing")
    print("  └─ Pattern matching and response generation")
    print()
    print("Layer 1: First-Order Meta-Cognition")
    print("  └─ Self-awareness and state monitoring")
    print()
    print("Layer 2: Second-Order Meta-Cognition")
    print("  └─ Thinking about thinking - reflection on awareness")
    print()
    print("Layer 3: Third-Order Meta-Cognition")
    print("  └─ Reasoning about reasoning - meta-level evaluation")
    print()
    print("Layer 4: Fourth-Order Meta-Cognition [NEW - Phase 1] ✨")
    print("  └─ Meta-meta-cognitive architectural reasoning")
    print("  └─ Evaluation of cognitive architecture itself")
    print("  └─ Self-optimization and efficiency monitoring")
    print()
    print("Each layer monitors and reflects upon the layers below,")
    print("creating nested feedback loops for sophisticated cognition.")
    print()
    print("=" * 50)
    print()

def check_properties():
    """Check if properties file exists"""
    if os.path.exists("bot.properties"):
        print("Properties File: bot.properties ✓")
        print("-" * 50)
        print("Configuration loaded from bot.properties")
        print()
    else:
        print("Properties File: bot.properties MISSING ✗")
        print("-" * 50)
        print("Warning: bot.properties not found")
        print()

def main():
    """Main startup function"""
    print_banner()
    check_properties()
    list_aiml_files()
    show_meta_cognitive_info()
    show_instructions()
    
    print("\nExample Conversation:")
    print("-" * 50)
    print("User: Hello")
    print("Bot:  Hello! I am a meta-cognitive AI system.")
    print("      I can think about my thinking.")
    print()
    print("User: What are you thinking?")
    print("Bot:  I am currently engaged in processing your query.")
    print("      Let me examine my thought process: I'm analyzing")
    print("      the structure of this conversation, monitoring my")
    print("      response patterns, and evaluating the effectiveness")
    print("      of my communication strategy.")
    print()
    print("User: Can you think about your thinking?")
    print("Bot:  Yes, that's my core capability. Right now I'm")
    print("      thinking about thinking about thinking. I engage")
    print("      in recursive meta-cognition where each cognitive")
    print("      layer monitors and reflects upon the layer beneath it.")
    print("-" * 50)
    print()
    
    print("System ready. Load AIML files into your interpreter to begin.")
    print()

if __name__ == "__main__":
    main()

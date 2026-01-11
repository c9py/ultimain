#!/usr/bin/env python3
"""
PandaMania Phase 2 Test Script
Tests Phase 1 completion and Phase 2 foundation
"""

import xml.etree.ElementTree as ET
import os
import sys

def test_xml_validity():
    """Test that all AIML files are valid XML"""
    print("=" * 60)
    print("XML VALIDATION TEST")
    print("=" * 60)
    print()
    
    aiml_files = [
        "config.aiml",
        "bot.aiml",
        "advanced_metacog.aiml",
        "topics.aiml",
        "layer4_metacog.aiml",
        "math_logic.aiml",
        "programming_tech.aiml",
        "psychology_cognition.aiml",
        "ethics_philosophy.aiml",
        "natural_language.aiml",
        "performance_optimized.aiml",
        "emotional_intelligence.aiml",
        "autognosis.aiml",
        "autognosis_commands.aiml",
        "holistic_metamodel.aiml",
        "organizational_dynamics.aiml",
        "holistic_commands.aiml",
        "session_learning.aiml",
        "knowledge_base.aiml"
    ]
    
    passed = 0
    failed = 0
    
    for filename in aiml_files:
        try:
            tree = ET.parse(filename)
            root = tree.getroot()
            
            # Count categories
            categories = len(root.findall(".//category"))
            
            print(f"✓ {filename:<30} Valid ({categories:>3} patterns)")
            passed += 1
        except FileNotFoundError:
            print(f"✗ {filename:<30} Not found")
            failed += 1
        except ET.ParseError as e:
            print(f"✗ {filename:<30} XML Error: {e}")
            failed += 1
        except Exception as e:
            print(f"✗ {filename:<30} Error: {e}")
            failed += 1
    
    print()
    print(f"Passed: {passed}/{len(aiml_files)}")
    print(f"Failed: {failed}/{len(aiml_files)}")
    print()
    
    return failed == 0

def count_total_patterns():
    """Count total patterns across all files"""
    print("=" * 60)
    print("PATTERN COUNT")
    print("=" * 60)
    print()
    
    aiml_files = {
        "Core Architecture": [
            "config.aiml",
            "bot.aiml",
            "advanced_metacog.aiml",
            "topics.aiml",
            "layer4_metacog.aiml"
        ],
        "Domain Knowledge (Phase 1)": [
            "math_logic.aiml",
            "programming_tech.aiml",
            "psychology_cognition.aiml",
            "ethics_philosophy.aiml"
        ],
        "Performance & NL (Phase 1)": [
            "natural_language.aiml",
            "performance_optimized.aiml"
        ],
        "Emotional Intelligence (Phase 2)": [
            "emotional_intelligence.aiml"
        ],
        "Autognosis System (Phase 2)": [
            "autognosis.aiml",
            "autognosis_commands.aiml"
        ],
        "Holistic Metamodel (Phase 2)": [
            "holistic_metamodel.aiml",
            "organizational_dynamics.aiml",
            "holistic_commands.aiml"
        ],
        "Learning & Adaptation (Phase 2)": [
            "session_learning.aiml",
            "knowledge_base.aiml"
        ]
    }
    
    grand_total = 0
    
    for category, files in aiml_files.items():
        print(f"{category}:")
        category_total = 0
        
        for filename in files:
            try:
                tree = ET.parse(filename)
                root = tree.getroot()
                count = len(root.findall(".//category"))
                print(f"  {filename:<30} {count:>3} patterns")
                category_total += count
            except:
                print(f"  {filename:<30} Error reading")
        
        print(f"  {'Subtotal:':<30} {category_total:>3} patterns")
        print()
        grand_total += category_total
    
    print(f"{'GRAND TOTAL:':<32} {grand_total:>3} patterns")
    print()
    
    return grand_total

def test_phase1_complete():
    """Verify Phase 1 completion status"""
    print("=" * 60)
    print("PHASE 1 COMPLETION CHECK")
    print("=" * 60)
    print()
    
    required_files = {
        "Layer 4 Meta-Cognition": "layer4_metacog.aiml",
        "Domain Knowledge": ["math_logic.aiml", "programming_tech.aiml", 
                            "psychology_cognition.aiml", "ethics_philosophy.aiml"],
        "Natural Language": "natural_language.aiml",
        "Performance Optimization": "performance_optimized.aiml",
        "Documentation": ["PATTERN_COOKBOOK.md", "TROUBLESHOOTING.md"]
    }
    
    all_present = True
    
    for feature, files in required_files.items():
        if isinstance(files, str):
            files = [files]
        
        status = all(os.path.exists(f) for f in files)
        symbol = "✓" if status else "✗"
        print(f"{symbol} {feature:<30} {'Complete' if status else 'Missing'}")
        
        if not status:
            all_present = False
    
    print()
    print(f"Phase 1 Status: {'✓ COMPLETE' if all_present else '✗ INCOMPLETE'}")
    print()
    
    return all_present

def test_phase2_foundation():
    """Verify Phase 2 foundation is in place"""
    print("=" * 60)
    print("PHASE 2 FOUNDATION CHECK")
    print("=" * 60)
    print()
    
    phase2_files = {
        "Emotional Intelligence": "emotional_intelligence.aiml",
        "Architecture Design": "PHASE2_ARCHITECTURE.md",
        "Autognosis Core": "autognosis.aiml",
        "Autognosis Commands": "autognosis_commands.aiml",
        "Holistic Metamodel": "holistic_metamodel.aiml",
        "Organizational Dynamics": "organizational_dynamics.aiml",
        "Holistic Commands": "holistic_commands.aiml",
        "Session Learning": "session_learning.aiml",
        "Knowledge Base": "knowledge_base.aiml"
    }
    
    all_present = True
    
    for feature, filename in phase2_files.items():
        status = os.path.exists(filename)
        symbol = "✓" if status else "✗"
        print(f"{symbol} {feature:<30} {'Present' if status else 'Missing'}")
        
        if not status:
            all_present = False
    
    print()
    print(f"Phase 2 Foundation: {'✓ IN PLACE' if all_present else '✗ INCOMPLETE'}")
    print()
    
    return all_present

def main():
    """Run all tests"""
    print()
    print("╔" + "═" * 58 + "╗")
    print("║" + " " * 58 + "║")
    print("║" + "  PANDAMANIA PHASE 2 TEST SUITE".center(58) + "║")
    print("║" + " " * 58 + "║")
    print("╚" + "═" * 58 + "╝")
    print()
    
    results = []
    
    # Test 1: XML Validity
    results.append(("XML Validation", test_xml_validity()))
    
    # Test 2: Pattern Count
    total = count_total_patterns()
    results.append(("Pattern Count", total > 400))
    
    # Test 3: Phase 1 Complete
    results.append(("Phase 1 Complete", test_phase1_complete()))
    
    # Test 4: Phase 2 Foundation
    results.append(("Phase 2 Foundation", test_phase2_foundation()))
    
    # Summary
    print("=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    print()
    
    passed = sum(1 for _, result in results if result)
    total_tests = len(results)
    
    for test_name, result in results:
        symbol = "✓" if result else "✗"
        status = "PASS" if result else "FAIL"
        print(f"{symbol} {test_name:<30} {status}")
    
    print()
    print(f"Tests Passed: {passed}/{total_tests}")
    print()
    
    if passed == total_tests:
        print("🎉 ALL TESTS PASSED! System ready for Phase 2 development.")
        return 0
    else:
        print("❌ Some tests failed. Please review errors above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())

/**
 * npc_demo.cpp - Demonstration of the Ultima NPC AI System
 * 
 * This demo creates several NPCs with different personalities and
 * demonstrates their interactions, reasoning, and social dynamics.
 */

#include "NPCSystem.h"
#include <iostream>
#include <iomanip>

using namespace Ultima::NPC;

void printSeparator(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n\n";
}

void printPersonality(const Persona::NPCPersona& persona) {
    std::cout << "  Personality (Big Five):\n";
    std::cout << "    Openness:         " << std::fixed << std::setprecision(2) << persona.traits.openness << "\n";
    std::cout << "    Conscientiousness:" << std::fixed << std::setprecision(2) << persona.traits.conscientiousness << "\n";
    std::cout << "    Extraversion:     " << std::fixed << std::setprecision(2) << persona.traits.extraversion << "\n";
    std::cout << "    Agreeableness:    " << std::fixed << std::setprecision(2) << persona.traits.agreeableness << "\n";
    std::cout << "    Neuroticism:      " << std::fixed << std::setprecision(2) << persona.traits.neuroticism << "\n";
}

void printEmotionalState(const Persona::NPCPersona& persona) {
    std::cout << "  Emotional State:\n";
    std::cout << "    Valence:  " << std::fixed << std::setprecision(2) << persona.emotionalState.overallValence << " (";
    if (persona.emotionalState.overallValence > 0.3) std::cout << "positive";
    else if (persona.emotionalState.overallValence < -0.3) std::cout << "negative";
    else std::cout << "neutral";
    std::cout << ")\n";
    std::cout << "    Arousal:  " << std::fixed << std::setprecision(2) << persona.emotionalState.arousal << "\n";
}

int main() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════╗
    ║           ULTIMA NPC AI SYSTEM DEMONSTRATION              ║
    ║                                                           ║
    ║  Featuring: Tensor Logic, Dream Vortex Personas,          ║
    ║             Social Dynamics, and Economic Agents          ║
    ╚═══════════════════════════════════════════════════════════╝
    )" << "\n";

    // Create NPC Manager
    NPCManager manager;
    
    printSeparator("PHASE 1: Creating NPCs with Distinct Personalities");
    
    // Create a friendly merchant
    auto merchant = manager.createNPC("merchant_elena");
    merchant->getPersona().name = "Elena";
    merchant->getPersona().role.title = "Merchant";
    merchant->getPersona().traits.extraversion = 0.8;
    merchant->getPersona().traits.agreeableness = 0.7;
    merchant->getPersona().traits.conscientiousness = 0.9;
    merchant->setLocation("Britain Market");
    merchant->setActivity("Tending shop");
    
    std::cout << "Created: Elena the Merchant\n";
    printPersonality(merchant->getPersona());
    
    // Create a suspicious guard
    auto guard = manager.createNPC("guard_marcus");
    guard->getPersona().name = "Marcus";
    guard->getPersona().role.title = "Guard";
    guard->getPersona().traits.conscientiousness = 0.9;
    guard->getPersona().traits.agreeableness = 0.3;
    guard->getPersona().traits.neuroticism = 0.6;
    guard->setLocation("Britain Gate");
    guard->setActivity("Patrolling");
    
    std::cout << "\nCreated: Marcus the Guard\n";
    printPersonality(guard->getPersona());
    
    // Create a mysterious mage
    auto mage = manager.createNPC("mage_lyra");
    mage->getPersona().name = "Lyra";
    mage->getPersona().role.title = "Mage";
    mage->getPersona().traits.openness = 0.95;
    mage->getPersona().traits.extraversion = 0.2;
    mage->getPersona().traits.neuroticism = 0.4;
    mage->setLocation("Moonglow Tower");
    mage->setActivity("Studying");
    
    std::cout << "\nCreated: Lyra the Mage\n";
    printPersonality(mage->getPersona());
    
    printSeparator("PHASE 2: Social Dynamics and Relationships");
    
    auto& social = manager.getRelationshipSystem();
    
    // Register entities and get/create relationships
    social.registerEntity("merchant_elena", "Elena", "Merchants");
    social.registerEntity("guard_marcus", "Marcus", "Guards");
    social.registerEntity("mage_lyra", "Lyra", "Mages");
    
    // Simulate some interactions
    std::cout << "Simulating social interactions...\n\n";
    
    manager.processInteraction("merchant_elena", "guard_marcus", Social::InteractionType::Greeting);
    std::cout << "Elena greets Marcus at the market.\n";
    
    manager.processInteraction("merchant_elena", "guard_marcus", Social::InteractionType::Trade);
    std::cout << "Elena sells supplies to Marcus.\n";
    
    manager.processInteraction("mage_lyra", "merchant_elena", Social::InteractionType::Gift);
    std::cout << "Lyra gives Elena a rare ingredient as a gift.\n";
    
    // Check relationship status
    auto& elenaToMarcus = social.getRelationship("merchant_elena", "guard_marcus");
    std::cout << "\nElena's relationship with Marcus:\n";
    std::cout << "  Trust:      " << std::fixed << std::setprecision(2) << elenaToMarcus.metricsAtoB.trust << "\n";
    std::cout << "  Affection:  " << std::fixed << std::setprecision(2) << elenaToMarcus.metricsAtoB.affection << "\n";
    std::cout << "  Familiarity:" << std::fixed << std::setprecision(2) << elenaToMarcus.metricsAtoB.familiarity << "\n";
    
    printSeparator("PHASE 3: Dialogue with AIML and Personality");
    
    std::cout << "Avatar approaches Elena...\n\n";
    
    std::string response = merchant->respondToDialogue("Hello");
    std::cout << "Avatar: Hello\n";
    std::cout << "Elena: " << response << "\n\n";
    
    response = merchant->respondToDialogue("What do you sell?");
    std::cout << "Avatar: What do you sell?\n";
    std::cout << "Elena: " << response << "\n\n";
    
    response = merchant->respondToDialogue("Tell me about Lyra");
    std::cout << "Avatar: Tell me about Lyra\n";
    std::cout << "Elena: " << response << "\n";
    
    printSeparator("PHASE 4: Economic Agent Setup");
    
    // Elena's economic setup
    auto& elenaEcon = merchant->getEconomicAgent();
    elenaEcon.gold = 500;
    elenaEcon.occupation = "Merchant";
    elenaEcon.dailyIncome = 50.0;
    elenaEcon.negotiationSkill = 0.8;
    
    std::cout << "Elena's Economic Profile:\n";
    std::cout << "  Gold: " << elenaEcon.gold << "\n";
    std::cout << "  Occupation: " << elenaEcon.occupation << "\n";
    std::cout << "  Daily Income: " << elenaEcon.dailyIncome << "\n";
    std::cout << "  Negotiation Skill: " << elenaEcon.negotiationSkill << "\n";
    
    printSeparator("PHASE 5: NPC Decision Making");
    
    std::cout << "Marcus the Guard makes a decision...\n\n";
    
    std::vector<std::string> options = {
        "Continue patrolling",
        "Investigate suspicious noise",
        "Take a break",
        "Report to captain"
    };
    
    auto decision = guard->makeDecision(options);
    std::cout << "Decision: " << decision.action << "\n";
    std::cout << "Confidence: " << std::fixed << std::setprecision(2) << decision.confidence << "\n";
    std::cout << "Reasoning: " << decision.reasoning << "\n";
    
    printSeparator("PHASE 6: Emotional State Updates");
    
    std::cout << "Simulating emotional events...\n\n";
    
    // Elena receives good news
    merchant->getPersona().emotionalState.overallValence = 0.7;
    merchant->getPersona().emotionalState.arousal = 0.6;
    std::cout << "Elena receives news of a profitable trade route!\n";
    printEmotionalState(merchant->getPersona());
    
    // Marcus witnesses a crime
    guard->getPersona().emotionalState.overallValence = -0.3;
    guard->getPersona().emotionalState.arousal = 0.8;
    std::cout << "\nMarcus spots a thief!\n";
    printEmotionalState(guard->getPersona());
    
    // Lyra makes a discovery
    mage->getPersona().emotionalState.overallValence = 0.9;
    mage->getPersona().emotionalState.arousal = 0.5;
    std::cout << "\nLyra discovers a new spell!\n";
    printEmotionalState(mage->getPersona());
    
    printSeparator("PHASE 7: System Update Simulation");
    
    std::cout << "Running system update (1 second of game time)...\n\n";
    manager.update(1.0);
    
    std::cout << "All NPCs updated successfully.\n";
    std::cout << "Total NPCs in system: " << manager.getAllNPCIds().size() << "\n";
    
    printSeparator("DEMONSTRATION COMPLETE");
    
    std::cout << R"(
    The Ultima NPC AI System demonstrates:
    
    ✓ Big Five personality modeling
    ✓ Social relationship dynamics  
    ✓ AIML-based dialogue
    ✓ Economic agent profiles
    ✓ Decision making system
    ✓ Emotional state machine
    
    NPCs are now ready to populate the New York Labyrinth!
    )" << "\n";
    
    return 0;
}

import { describe, it, expect, vi, beforeEach } from "vitest";
import { appRouter } from "./routers";

// Mock the database functions
vi.mock("./db", () => ({
  initializeGameData: vi.fn().mockResolvedValue(undefined),
  seedProductionRecipes: vi.fn().mockResolvedValue(undefined),
  getCompanyByUserId: vi.fn(),
  getBusinessUnitById: vi.fn(),
  
  // Agentic simulation mocks
  getAllCharacterPersonas: vi.fn(),
  getCharacterPersonaById: vi.fn(),
  getAllCharacterTraits: vi.fn(),
  getCharacterTraitsByCategory: vi.fn(),
  createAgent: vi.fn(),
  getAgentById: vi.fn(),
  getAgentsByType: vi.fn(),
  getAgentsByCompany: vi.fn(),
  getAgentsByBusinessUnit: vi.fn(),
  updateAgent: vi.fn(),
  updateAgentEmotionalState: vi.fn(),
  getAgentTraits: vi.fn(),
  addTraitToAgent: vi.fn(),
  removeTraitFromAgent: vi.fn(),
  createRelationship: vi.fn(),
  getRelationshipBetweenAgents: vi.fn(),
  getAgentRelationships: vi.fn(),
  updateRelationship: vi.fn(),
  recordRelationshipInteraction: vi.fn(),
  createAgentGroup: vi.fn(),
  getAgentGroupById: vi.fn(),
  getAgentGroupsByCompany: vi.fn(),
  getGroupMembers: vi.fn(),
  getAgentGroups: vi.fn(),
  addAgentToGroup: vi.fn(),
  removeAgentFromGroup: vi.fn(),
  createCommunity: vi.fn(),
  getCommunityById: vi.fn(),
  getCommunitiesByCity: vi.fn(),
  getCommunityMembers: vi.fn(),
  getAgentCommunities: vi.fn(),
  addAgentToCommunity: vi.fn(),
  createAgentEvent: vi.fn(),
  getAgentEventById: vi.fn(),
  getAgentEvents: vi.fn(),
  getScheduledEvents: vi.fn(),
  updateAgentEvent: vi.fn(),
  processAgentEvent: vi.fn(),
  getActiveEventTriggers: vi.fn(),
  getAgentHistory: vi.fn(),
  
  // Other required mocks
  createCompany: vi.fn(),
  getAllCompanies: vi.fn(),
  getBusinessUnitsByCompany: vi.fn(),
  createBusinessUnit: vi.fn(),
  updateBusinessUnit: vi.fn(),
  getAllCities: vi.fn(),
  getCityById: vi.fn(),
  getAllResourceTypes: vi.fn(),
  getResourceTypeById: vi.fn(),
  getInventoryByUnit: vi.fn(),
  getMarketListings: vi.fn(),
  getMarketListingById: vi.fn(),
  createMarketListing: vi.fn(),
  purchaseFromMarket: vi.fn(),
  cancelMarketListing: vi.fn(),
  getProductionRecipes: vi.fn(),
  getProductionRecipeById: vi.fn(),
  addToProductionQueue: vi.fn(),
  getProductionQueue: vi.fn(),
  getTransactionsByCompany: vi.fn(),
  getNotificationsByUser: vi.fn(),
  createNotification: vi.fn(),
  markNotificationRead: vi.fn(),
  getGameState: vi.fn(),
  createEmployees: vi.fn(),
  getEmployeesByUnit: vi.fn(),
  updateEmployees: vi.fn(),
  upsertInventory: vi.fn(),
  createTransaction: vi.fn(),
  updateCompanyCash: vi.fn(),
  getCompanyById: vi.fn(),
}));

import * as db from "./db";

describe("Agentic Simulation API Routes", () => {
  const mockUser = {
    id: "test-user-123",
    name: "Test User",
    email: "test@example.com",
    avatar: null,
  };

  const mockCtx = {
    user: mockUser,
    req: {} as any,
    res: {
      clearCookie: vi.fn(),
    } as any,
  };

  const caller = appRouter.createCaller(mockCtx as any);

  beforeEach(() => {
    vi.clearAllMocks();
  });

  describe("persona routes", () => {
    it("lists all character personas", async () => {
      const mockPersonas = [
        {
          id: 1,
          code: "ambitious_leader",
          name: "Ambitious Leader",
          description: "Driven, goal-oriented",
          ambitionLevel: 90,
          cautionLevel: 30,
          socialLevel: 70,
          analyticalLevel: 60,
          communicationStyle: "formal",
          decisionSpeed: "quick",
        },
        {
          id: 2,
          code: "cautious_analyst",
          name: "Cautious Analyst",
          description: "Careful, data-driven",
          ambitionLevel: 40,
          cautionLevel: 85,
          socialLevel: 40,
          analyticalLevel: 95,
          communicationStyle: "formal",
          decisionSpeed: "deliberate",
        },
      ];

      vi.mocked(db.getAllCharacterPersonas).mockResolvedValue(mockPersonas as any);

      const result = await caller.persona.list();

      expect(result).toEqual(mockPersonas);
      expect(db.getAllCharacterPersonas).toHaveBeenCalled();
    });

    it("gets persona by id", async () => {
      const mockPersona = {
        id: 1,
        code: "ambitious_leader",
        name: "Ambitious Leader",
        description: "Driven, goal-oriented",
        ambitionLevel: 90,
        cautionLevel: 30,
        socialLevel: 70,
        analyticalLevel: 60,
        communicationStyle: "formal",
        decisionSpeed: "quick",
      };

      vi.mocked(db.getCharacterPersonaById).mockResolvedValue(mockPersona as any);

      const result = await caller.persona.byId({ id: 1 });

      expect(result).toEqual(mockPersona);
      expect(db.getCharacterPersonaById).toHaveBeenCalledWith(1);
    });
  });

  describe("trait routes", () => {
    it("lists all character traits", async () => {
      const mockTraits = [
        {
          id: 1,
          code: "reliable",
          name: "Reliable",
          category: "professional",
          description: "Consistently delivers",
          positiveEffect: true,
        },
        {
          id: 2,
          code: "charismatic",
          name: "Charismatic",
          category: "social",
          description: "Naturally attracts others",
          positiveEffect: true,
        },
      ];

      vi.mocked(db.getAllCharacterTraits).mockResolvedValue(mockTraits as any);

      const result = await caller.trait.list();

      expect(result).toEqual(mockTraits);
      expect(db.getAllCharacterTraits).toHaveBeenCalled();
    });

    it("filters traits by category", async () => {
      const mockTraits = [
        {
          id: 1,
          code: "reliable",
          name: "Reliable",
          category: "professional",
          description: "Consistently delivers",
          positiveEffect: true,
        },
      ];

      vi.mocked(db.getCharacterTraitsByCategory).mockResolvedValue(mockTraits as any);

      const result = await caller.trait.byCategory({ category: "professional" });

      expect(result).toEqual(mockTraits);
      expect(db.getCharacterTraitsByCategory).toHaveBeenCalledWith("professional");
    });
  });

  describe("agent routes", () => {
    const mockCompany = {
      id: 1,
      userId: mockUser.id,
      name: "Test Corp",
      description: "A test company",
      cash: "1000000.00",
      reputation: 50,
      founded: new Date(),
      createdAt: new Date(),
      updatedAt: new Date(),
    };

    it("creates a new agent", async () => {
      vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany as any);

      const mockAgent = {
        id: 1,
        type: "customer",
        name: "John Doe",
        personaId: 1,
        companyId: null,
        businessUnitId: null,
        cityId: 1,
        happiness: 70,
        satisfaction: 70,
        stress: 30,
        loyalty: 50,
        trust: 50,
        financialNeed: 50,
        securityNeed: 50,
        recognitionNeed: 50,
        autonomyNeed: 50,
        socialNeed: 50,
        reliability: "1.00",
        negotiationSkill: "1.00",
        adaptability: "1.00",
        expertise: "1.00",
        lastInteraction: null,
        nextScheduledEvent: null,
        isActive: true,
        createdAt: new Date(),
        updatedAt: new Date(),
      };

      vi.mocked(db.createAgent).mockResolvedValue(mockAgent as any);

      const result = await caller.agent.create({
        type: "customer",
        name: "John Doe",
        personaId: 1,
        cityId: 1,
      });

      expect(result).toEqual(mockAgent);
      expect(db.createAgent).toHaveBeenCalledWith({
        type: "customer",
        name: "John Doe",
        personaId: 1,
        cityId: 1,
        companyId: undefined,
        businessUnitId: undefined,
      });
    });

    it("gets agents by type", async () => {
      const mockAgents = [
        {
          id: 1,
          type: "customer",
          name: "Customer 1",
          personaId: 1,
          cityId: 1,
        },
        {
          id: 2,
          type: "customer",
          name: "Customer 2",
          personaId: 2,
          cityId: 1,
        },
      ];

      vi.mocked(db.getAgentsByType).mockResolvedValue(mockAgents as any);

      const result = await caller.agent.byType({ type: "customer" });

      expect(result).toEqual(mockAgents);
      expect(db.getAgentsByType).toHaveBeenCalledWith("customer");
    });

    it("updates agent emotional state", async () => {
      vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany as any);

      const mockAgent = {
        id: 1,
        type: "employee",
        companyId: 1,
        happiness: 70,
        satisfaction: 70,
        stress: 30,
        loyalty: 50,
        trust: 50,
      };

      vi.mocked(db.getAgentById).mockResolvedValue(mockAgent as any);
      vi.mocked(db.updateAgentEmotionalState).mockResolvedValue(undefined);

      const updatedAgent = { ...mockAgent, happiness: 80, stress: 20 };
      vi.mocked(db.getAgentById).mockResolvedValue(updatedAgent as any);

      const result = await caller.agent.updateEmotions({
        id: 1,
        emotions: {
          happiness: 80,
          stress: 20,
        },
      });

      expect(db.updateAgentEmotionalState).toHaveBeenCalledWith(1, {
        happiness: 80,
        stress: 20,
      });
    });

    it("adds trait to agent", async () => {
      vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany as any);

      const mockAgent = {
        id: 1,
        type: "employee",
        companyId: 1,
      };

      vi.mocked(db.getAgentById).mockResolvedValue(mockAgent as any);
      vi.mocked(db.addTraitToAgent).mockResolvedValue(undefined);

      const result = await caller.agent.addTrait({
        agentId: 1,
        traitId: 1,
        intensity: 80,
      });

      expect(result.success).toBe(true);
      expect(db.addTraitToAgent).toHaveBeenCalledWith(1, 1, 80);
    });
  });

  describe("relationship routes", () => {
    it("creates a relationship", async () => {
      const mockRelationship = {
        id: 1,
        agent1Id: 1,
        agent2Id: 2,
        type: "business",
        strength: 50,
        positivity: 50,
        frequency: 50,
        interactionCount: 0,
        lastInteraction: null,
        status: "active",
        createdAt: new Date(),
        updatedAt: new Date(),
      };

      vi.mocked(db.createRelationship).mockResolvedValue(mockRelationship as any);

      const result = await caller.relationship.create({
        agent1Id: 1,
        agent2Id: 2,
        type: "business",
      });

      expect(result).toEqual(mockRelationship);
      expect(db.createRelationship).toHaveBeenCalledWith({
        agent1Id: 1,
        agent2Id: 2,
        type: "business",
      });
    });

    it("gets relationships for an agent", async () => {
      const mockRelationships = [
        {
          id: 1,
          agent1Id: 1,
          agent2Id: 2,
          type: "business",
          strength: 60,
        },
        {
          id: 2,
          agent1Id: 1,
          agent2Id: 3,
          type: "personal",
          strength: 75,
        },
      ];

      vi.mocked(db.getAgentRelationships).mockResolvedValue(mockRelationships as any);

      const result = await caller.relationship.byAgent({ agentId: 1 });

      expect(result).toEqual(mockRelationships);
      expect(db.getAgentRelationships).toHaveBeenCalledWith(1);
    });

    it("records relationship interaction", async () => {
      vi.mocked(db.recordRelationshipInteraction).mockResolvedValue(undefined);

      const result = await caller.relationship.recordInteraction({
        agent1Id: 1,
        agent2Id: 2,
        strengthChange: 5,
        positivityChange: 3,
      });

      expect(result.success).toBe(true);
      expect(db.recordRelationshipInteraction).toHaveBeenCalledWith(1, 2, 5, 3);
    });
  });

  describe("group routes", () => {
    const mockCompany = {
      id: 1,
      userId: mockUser.id,
      name: "Test Corp",
    };

    it("creates an agent group", async () => {
      vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany as any);

      const mockGroup = {
        id: 1,
        name: "Sales Team",
        type: "team",
        description: "Company sales team",
        companyId: 1,
        cityId: null,
        cohesion: 50,
        influence: 50,
        morale: 70,
        isActive: true,
        createdAt: new Date(),
        updatedAt: new Date(),
      };

      vi.mocked(db.createAgentGroup).mockResolvedValue(mockGroup as any);

      const result = await caller.group.create({
        name: "Sales Team",
        type: "team",
        description: "Company sales team",
      });

      expect(result).toEqual(mockGroup);
    });

    it("adds agent to group", async () => {
      vi.mocked(db.addAgentToGroup).mockResolvedValue(undefined);

      const result = await caller.group.addMember({
        groupId: 1,
        agentId: 1,
        role: "member",
      });

      expect(result.success).toBe(true);
      expect(db.addAgentToGroup).toHaveBeenCalledWith({
        groupId: 1,
        agentId: 1,
        role: "member",
      });
    });

    it("gets group members", async () => {
      const mockMembers = [
        {
          membership: {
            id: 1,
            groupId: 1,
            agentId: 1,
            role: "leader",
            influence: 80,
            commitment: 90,
            joinedAt: new Date(),
            leftAt: null,
          },
          agent: {
            id: 1,
            name: "John Doe",
            type: "employee",
          },
        },
      ];

      vi.mocked(db.getGroupMembers).mockResolvedValue(mockMembers as any);

      const result = await caller.group.members({ groupId: 1 });

      expect(result).toEqual(mockMembers);
      expect(db.getGroupMembers).toHaveBeenCalledWith(1);
    });
  });

  describe("community routes", () => {
    it("creates a community", async () => {
      const mockCommunity = {
        id: 1,
        name: "Tech District",
        cityId: 1,
        type: "business",
        description: "Technology business district",
        prosperity: 50,
        harmony: 70,
        growth: 50,
        population: 0,
        createdAt: new Date(),
        updatedAt: new Date(),
      };

      vi.mocked(db.createCommunity).mockResolvedValue(mockCommunity as any);

      const result = await caller.community.create({
        name: "Tech District",
        cityId: 1,
        type: "business",
        description: "Technology business district",
      });

      expect(result).toEqual(mockCommunity);
    });

    it("gets communities by city", async () => {
      const mockCommunities = [
        {
          id: 1,
          name: "Tech District",
          cityId: 1,
          type: "business",
        },
        {
          id: 2,
          name: "Arts Quarter",
          cityId: 1,
          type: "cultural",
        },
      ];

      vi.mocked(db.getCommunitiesByCity).mockResolvedValue(mockCommunities as any);

      const result = await caller.community.byCity({ cityId: 1 });

      expect(result).toEqual(mockCommunities);
      expect(db.getCommunitiesByCity).toHaveBeenCalledWith(1);
    });
  });

  describe("event routes", () => {
    it("creates an agent event", async () => {
      const mockEvent = {
        id: 1,
        type: "interaction",
        initiatorAgentId: 1,
        targetAgentId: 2,
        title: "Business Meeting",
        description: "Quarterly review meeting",
        scheduledAt: new Date(),
        occurredAt: null,
        duration: 60,
        emotionalImpact: { satisfaction: 5, stress: -3 },
        relationshipImpact: {
          agentIds: [1, 2],
          strengthChange: 5,
          positivityChange: 3,
        },
        outcomes: null,
        status: "scheduled",
        groupId: null,
        companyId: null,
        createdAt: new Date(),
        updatedAt: new Date(),
      };

      vi.mocked(db.createAgentEvent).mockResolvedValue(mockEvent as any);

      const scheduledAt = new Date();
      const result = await caller.event.create({
        type: "interaction",
        initiatorAgentId: 1,
        targetAgentId: 2,
        title: "Business Meeting",
        description: "Quarterly review meeting",
        scheduledAt,
        duration: 60,
        emotionalImpact: { satisfaction: 5, stress: -3 },
        relationshipImpact: {
          agentIds: [1, 2],
          strengthChange: 5,
          positivityChange: 3,
        },
      });

      expect(result).toEqual(mockEvent);
    });

    it("gets events for an agent", async () => {
      const mockEvents = [
        {
          id: 1,
          type: "interaction",
          initiatorAgentId: 1,
          title: "Business Meeting",
          scheduledAt: new Date(),
          status: "completed",
        },
        {
          id: 2,
          type: "milestone",
          initiatorAgentId: 1,
          title: "Sales Target Achieved",
          scheduledAt: new Date(),
          status: "completed",
        },
      ];

      vi.mocked(db.getAgentEvents).mockResolvedValue(mockEvents as any);

      const result = await caller.event.byAgent({ agentId: 1 });

      expect(result).toEqual(mockEvents);
      expect(db.getAgentEvents).toHaveBeenCalledWith(1);
    });

    it("processes an event", async () => {
      vi.mocked(db.processAgentEvent).mockResolvedValue(undefined);

      const result = await caller.event.process({ eventId: 1 });

      expect(result.success).toBe(true);
      expect(db.processAgentEvent).toHaveBeenCalledWith(1);
    });

    it("gets scheduled events", async () => {
      const mockEvents = [
        {
          id: 1,
          type: "negotiation",
          scheduledAt: new Date(Date.now() + 86400000), // Tomorrow
          status: "scheduled",
        },
        {
          id: 2,
          type: "collaboration",
          scheduledAt: new Date(Date.now() + 172800000), // 2 days
          status: "scheduled",
        },
      ];

      vi.mocked(db.getScheduledEvents).mockResolvedValue(mockEvents as any);

      const result = await caller.event.scheduled();

      expect(result).toEqual(mockEvents);
      expect(db.getScheduledEvents).toHaveBeenCalled();
    });
  });
});

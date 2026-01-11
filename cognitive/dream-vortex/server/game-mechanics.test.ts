import { describe, it, expect, vi, beforeEach } from "vitest";
import { appRouter } from "./routers";

// Mock the database functions
vi.mock("./db", () => ({
  initializeGameData: vi.fn().mockResolvedValue(undefined),
  seedProductionRecipes: vi.fn().mockResolvedValue(undefined),
  getCompanyByUserId: vi.fn(),
  getCompanyById: vi.fn(),
  getAllCompanies: vi.fn(),
  updateCompanyCash: vi.fn(),
  createTransaction: vi.fn(),
  getBusinessUnitsByCompany: vi.fn(),
  getBusinessUnitById: vi.fn(),
  getEmployeesByUnit: vi.fn(),
  getAgentById: vi.fn(),
  getWorldById: vi.fn(),

  // Technology functions
  getAllTechnologies: vi.fn(),
  getTechnologyById: vi.fn(),
  getCompanyTechnologies: vi.fn(),
  startTechnologyResearch: vi.fn(),
  updateTechnologyResearch: vi.fn(),
  hasCompanyResearchedTech: vi.fn(),

  // Game processing functions
  processTurnAdvancement: vi.fn(),
  processCompanyPayroll: vi.fn(),
  completeProductionItem: vi.fn(),
  getReadyProductionItems: vi.fn(),

  // Event propagation functions
  getEventPropagationHistory: vi.fn(),
  getEventPropagationBySourceType: vi.fn(),

  // API key functions
  getApiKeysByUserId: vi.fn(),
  createApiKey: vi.fn(),
  deleteApiKey: vi.fn(),
  verifyApiKey: vi.fn(),

  // Other required mocks
  getGameState: vi.fn(),
  createNotification: vi.fn(),
  getNotificationsByUser: vi.fn(),
  markNotificationRead: vi.fn(),
  createCompany: vi.fn(),
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
  createEmployees: vi.fn(),
  updateEmployees: vi.fn(),
  upsertInventory: vi.fn(),

  // Agentic simulation mocks
  getAllCharacterPersonas: vi.fn(),
  getCharacterPersonaById: vi.fn(),
  getAllCharacterTraits: vi.fn(),
  getCharacterTraitsByCategory: vi.fn(),
  createAgent: vi.fn(),
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

  // DreamCog integration mocks
  createAgentBigFivePersonality: vi.fn(),
  getAgentBigFivePersonality: vi.fn(),
  updateAgentBigFivePersonality: vi.fn(),
  createAgentMotivation: vi.fn(),
  getAgentMotivations: vi.fn(),
  updateAgentMotivation: vi.fn(),
  createAgentMemory: vi.fn(),
  getAgentMemories: vi.fn(),
  updateAgentMemory: vi.fn(),
  createRelationshipEvent: vi.fn(),
  getRelationshipEvents: vi.fn(),
  createWorld: vi.fn(),
  getWorldsByUserId: vi.fn(),
  updateWorld: vi.fn(),
  createLocation: vi.fn(),
  getLocationById: vi.fn(),
  getLocationsByWorldId: vi.fn(),
  getSubLocations: vi.fn(),
  updateLocation: vi.fn(),
  createLoreEntry: vi.fn(),
  getLoreEntryById: vi.fn(),
  getLoreEntriesByWorldId: vi.fn(),
  updateLoreEntry: vi.fn(),
  createWorldEvent: vi.fn(),
  getWorldEventById: vi.fn(),
  getWorldEventsByWorldId: vi.fn(),
  updateWorldEvent: vi.fn(),
  createScheduledWorldEvent: vi.fn(),
  getScheduledWorldEventById: vi.fn(),
  getPendingScheduledWorldEvents: vi.fn(),
  updateScheduledWorldEvent: vi.fn(),

  // DreamCog storytelling mocks (createApiKey already mocked above)
  getStoryCharactersByUserId: vi.fn(),
  getStoryCharacterById: vi.fn(),
  createStoryCharacter: vi.fn(),
  updateStoryCharacter: vi.fn(),
  deleteStoryCharacter: vi.fn(),
  getScenariosByUserId: vi.fn(),
  getPublicScenarios: vi.fn(),
  getScenarioById: vi.fn(),
  createScenario: vi.fn(),
  updateScenario: vi.fn(),
  deleteScenario: vi.fn(),
  addScenarioCharacter: vi.fn(),
  getScenarioCharacters: vi.fn(),
  updateScenarioCharacter: vi.fn(),
  deleteScenarioCharacter: vi.fn(),
  addScenarioInteraction: vi.fn(),
  getScenarioInteractions: vi.fn(),
  updateScenarioInteraction: vi.fn(),
  deleteScenarioInteraction: vi.fn(),
  createChatSession: vi.fn(),
  getChatSessionsByUserId: vi.fn(),
  getChatSessionById: vi.fn(),
  updateChatSession: vi.fn(),
  deleteChatSession: vi.fn(),
  addChatMessage: vi.fn(),
  getChatMessages: vi.fn(),
  createStory: vi.fn(),
  getStoriesByUserId: vi.fn(),
  getStoryById: vi.fn(),
  updateStory: vi.fn(),
  deleteStory: vi.fn(),
  addStoryCharacterLink: vi.fn(),
  getStoryCharacterLinks: vi.fn(),
  updateStoryCharacterLink: vi.fn(),
  deleteStoryCharacterLink: vi.fn(),
  createGeneratedImage: vi.fn(),
  getGeneratedImagesByUserId: vi.fn(),
  deleteGeneratedImage: vi.fn(),
}));

// Mock the services
vi.mock("./services/agentBrain", () => ({
  agentBrain: {
    makeDecision: vi.fn(),
    processDecisionOutcome: vi.fn(),
    createAgentWithPersonality: vi.fn(),
  },
  DecisionContext: {},
  DecisionOption: {},
}));

vi.mock("./services/eventBridge", () => ({
  eventBridge: {
    propagateBusinessEvent: vi.fn(),
    propagateNarrativeEvent: vi.fn(),
    processScheduledEvents: vi.fn(),
  },
  BusinessEvent: {},
  NarrativeEvent: {},
}));

vi.mock("./services/simulationEngine", () => ({
  default: {
    getSimulationState: vi.fn(),
    getAvailableActions: vi.fn(),
    getRecommendedActions: vi.fn(),
    executeAction: vi.fn(),
    simulateTimePassage: vi.fn(),
    generateAutonomousAction: vi.fn(),
    SIMULATION_ACTIONS: [
      { id: 'eat_meal', name: 'Eat a Meal', category: 'self_care', duration: 30, needEffects: { hunger: 50 } },
      { id: 'sleep', name: 'Sleep', category: 'self_care', duration: 480, needEffects: { energy: 100 } },
    ],
  },
}));

import * as db from "./db";
import { agentBrain } from "./services/agentBrain";
import { eventBridge } from "./services/eventBridge";
import simulationEngine from "./services/simulationEngine";

describe("Game Mechanics API Routes", () => {
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

  // ============================================================================
  // TECHNOLOGY ROUTES TESTS
  // ============================================================================
  describe("technology routes", () => {
    const mockCompany = {
      id: 1,
      userId: mockUser.id,
      name: "Test Corp",
      description: "A test company",
      cash: "1000000.00",
      reputation: "50.00",
      totalAssets: "1000000.00",
      createdAt: new Date(),
    };

    describe("technology.list", () => {
      it("returns all available technologies", async () => {
        const mockTechnologies = [
          {
            id: 1,
            name: "Basic Automation",
            description: "Basic production automation",
            tier: 1,
            researchCost: "50000.00",
            prerequisiteId: null,
            benefits: { productionBonus: 10 },
          },
          {
            id: 2,
            name: "Advanced Robotics",
            description: "Advanced robotic systems",
            tier: 2,
            researchCost: "150000.00",
            prerequisiteId: 1,
            benefits: { productionBonus: 25 },
          },
          {
            id: 3,
            name: "AI Integration",
            description: "AI-powered systems",
            tier: 3,
            researchCost: "500000.00",
            prerequisiteId: 2,
            benefits: { productionBonus: 50 },
          },
        ];

        vi.mocked(db.getAllTechnologies).mockResolvedValue(mockTechnologies);

        const result = await caller.technology.list();

        expect(result).toEqual(mockTechnologies);
        expect(db.getAllTechnologies).toHaveBeenCalled();
        expect(result.length).toBe(3);
      });

      it("returns empty array when no technologies exist", async () => {
        vi.mocked(db.getAllTechnologies).mockResolvedValue([]);

        const result = await caller.technology.list();

        expect(result).toEqual([]);
      });
    });

    describe("technology.byId", () => {
      it("returns specific technology by ID", async () => {
        const mockTech = {
          id: 1,
          name: "Basic Automation",
          description: "Basic production automation",
          tier: 1,
          researchCost: "50000.00",
          prerequisiteId: null,
          benefits: { productionBonus: 10 },
        };

        vi.mocked(db.getTechnologyById).mockResolvedValue(mockTech);

        const result = await caller.technology.byId({ id: 1 });

        expect(result).toEqual(mockTech);
        expect(db.getTechnologyById).toHaveBeenCalledWith(1);
      });

      it("returns null for non-existent technology", async () => {
        vi.mocked(db.getTechnologyById).mockResolvedValue(null);

        const result = await caller.technology.byId({ id: 999 });

        expect(result).toBeNull();
      });
    });

    describe("technology.companyTechnologies", () => {
      it("returns researched technologies for user's company", async () => {
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany);

        const mockCompanyTechs = [
          {
            companyTech: {
              id: 1,
              companyId: 1,
              technologyId: 1,
              researchProgress: "100.00",
              isResearching: false,
              researchStarted: new Date(),
              researchCompleted: new Date(),
            },
            technology: {
              id: 1,
              name: "Basic Automation",
              tier: 1,
            },
          },
          {
            companyTech: {
              id: 2,
              companyId: 1,
              technologyId: 2,
              researchProgress: "45.00",
              isResearching: true,
              researchStarted: new Date(),
              researchCompleted: null,
            },
            technology: {
              id: 2,
              name: "Advanced Robotics",
              tier: 2,
            },
          },
        ];

        vi.mocked(db.getCompanyTechnologies).mockResolvedValue(mockCompanyTechs);

        const result = await caller.technology.companyTechnologies();

        expect(result).toEqual(mockCompanyTechs);
        expect(db.getCompanyTechnologies).toHaveBeenCalledWith(mockCompany.id);
      });

      it("returns empty array when user has no company", async () => {
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(null);

        const result = await caller.technology.companyTechnologies();

        expect(result).toEqual([]);
      });
    });

    describe("technology.startResearch", () => {
      it("starts research on a technology", async () => {
        const mockTech = {
          id: 1,
          code: "basic_automation",
          name: "Basic Automation",
          category: "production" as const,
          description: "Basic production automation",
          researchCost: 50000,
          prerequisites: null,
          effects: null,
        };

        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany);
        vi.mocked(db.getTechnologyById).mockResolvedValue(mockTech);
        vi.mocked(db.hasCompanyResearchedTech).mockResolvedValue(true);
        vi.mocked(db.updateCompanyCash).mockResolvedValue(undefined);
        vi.mocked(db.createTransaction).mockResolvedValue(undefined);
        vi.mocked(db.startTechnologyResearch).mockResolvedValue({
          id: 1,
          companyId: 1,
          technologyId: 1,
          researchProgress: 0,
          isCompleted: false,
          completedAt: null,
        });

        const result = await caller.technology.startResearch({ technologyId: 1 });

        expect(result.isCompleted).toBe(false);
        expect(result.researchProgress).toBe(0);
        expect(db.updateCompanyCash).toHaveBeenCalled();
        expect(db.createTransaction).toHaveBeenCalled();
      });

      it("throws error when company not found", async () => {
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(null);

        await expect(
          caller.technology.startResearch({ technologyId: 1 })
        ).rejects.toThrow("Company not found");
      });

      it("throws error when technology not found", async () => {
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany);
        vi.mocked(db.getTechnologyById).mockResolvedValue(null);

        await expect(
          caller.technology.startResearch({ technologyId: 999 })
        ).rejects.toThrow("Technology not found");
      });

      it("throws error when prerequisite not researched", async () => {
        const mockTech = {
          id: 2,
          code: "advanced_robotics",
          name: "Advanced Robotics",
          category: "production" as const,
          description: "Advanced robotic systems",
          researchCost: 150000,
          prerequisites: [1],
          effects: null,
        };

        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany);
        vi.mocked(db.getTechnologyById).mockResolvedValue(mockTech);
        vi.mocked(db.hasCompanyResearchedTech).mockResolvedValue(false);

        await expect(
          caller.technology.startResearch({ technologyId: 2 })
        ).rejects.toThrow("Prerequisite technology not researched");
      });

      it("throws error when insufficient funds", async () => {
        const poorCompany = { ...mockCompany, cash: "10000.00" };
        const expensiveTech = {
          id: 1,
          code: "basic_automation",
          name: "Basic Automation",
          category: "production" as const,
          description: "Basic production automation",
          researchCost: 50000,
          prerequisites: null,
          effects: null,
        };

        vi.mocked(db.getCompanyByUserId).mockResolvedValue(poorCompany);
        vi.mocked(db.getTechnologyById).mockResolvedValue(expensiveTech);

        await expect(
          caller.technology.startResearch({ technologyId: 1 })
        ).rejects.toThrow("Insufficient funds for research");
      });
    });

    describe("technology.hasResearched", () => {
      it("returns true when technology is researched", async () => {
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany);
        vi.mocked(db.hasCompanyResearchedTech).mockResolvedValue(true);

        const result = await caller.technology.hasResearched({ technologyId: 1 });

        expect(result).toBe(true);
        expect(db.hasCompanyResearchedTech).toHaveBeenCalledWith(mockCompany.id, 1);
      });

      it("returns false when technology is not researched", async () => {
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany);
        vi.mocked(db.hasCompanyResearchedTech).mockResolvedValue(false);

        const result = await caller.technology.hasResearched({ technologyId: 2 });

        expect(result).toBe(false);
      });

      it("returns false when user has no company", async () => {
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(null);

        const result = await caller.technology.hasResearched({ technologyId: 1 });

        expect(result).toBe(false);
      });
    });
  });

  // ============================================================================
  // GAME TURN PROCESSING TESTS
  // ============================================================================
  describe("gameTurn routes", () => {
    const mockCompany = {
      id: 1,
      userId: mockUser.id,
      name: "Test Corp",
      description: "A test company",
      cash: "1000000.00",
      reputation: "50.00",
      totalAssets: "1000000.00",
      createdAt: new Date(),
    };

    describe("gameTurn.advance", () => {
      it("advances the game turn and processes all mechanics", async () => {
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany);
        vi.mocked(db.processTurnAdvancement).mockResolvedValue({
          turn: 5,
          productionCompleted: 3,
          payrollProcessed: [
            { companyId: 1, totalPayroll: 50000, employeeCount: 25 },
            { companyId: 2, totalPayroll: 30000, employeeCount: 15 },
          ],
          researchAdvanced: 2,
        });

        const result = await caller.gameTurn.advance();

        expect(result.turn).toBe(5);
        expect(result.productionCompleted).toBe(3);
        expect(result.payrollProcessed.length).toBe(2);
        expect(result.researchAdvanced).toBe(2);
        expect(db.processTurnAdvancement).toHaveBeenCalled();
      });

      it("throws error when company not found", async () => {
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(null);

        await expect(caller.gameTurn.advance()).rejects.toThrow("Company not found");
      });

      it("processes turn with zero production and research", async () => {
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany);
        vi.mocked(db.processTurnAdvancement).mockResolvedValue({
          turn: 1,
          productionCompleted: 0,
          payrollProcessed: [],
          researchAdvanced: 0,
        });

        const result = await caller.gameTurn.advance();

        expect(result.turn).toBe(1);
        expect(result.productionCompleted).toBe(0);
        expect(result.researchAdvanced).toBe(0);
      });
    });

    describe("gameTurn.processPayroll", () => {
      it("processes payroll for user's company", async () => {
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany);
        vi.mocked(db.processCompanyPayroll).mockResolvedValue({
          totalPayroll: 75000,
          employeeCount: 30,
        });

        const result = await caller.gameTurn.processPayroll();

        expect(result.totalPayroll).toBe(75000);
        expect(result.employeeCount).toBe(30);
        expect(db.processCompanyPayroll).toHaveBeenCalledWith(mockCompany.id);
      });

      it("throws error when company not found", async () => {
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(null);

        await expect(caller.gameTurn.processPayroll()).rejects.toThrow("Company not found");
      });

      it("handles company with no employees", async () => {
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany);
        vi.mocked(db.processCompanyPayroll).mockResolvedValue({
          totalPayroll: 0,
          employeeCount: 0,
        });

        const result = await caller.gameTurn.processPayroll();

        expect(result.totalPayroll).toBe(0);
        expect(result.employeeCount).toBe(0);
      });
    });

    describe("gameTurn.readyProduction", () => {
      it("returns ready production items for user's company", async () => {
        const mockUnits = [
          { id: 1, companyId: 1, name: "Factory 1", type: "factory" },
          { id: 2, companyId: 1, name: "Mine 1", type: "mine" },
        ];

        const mockReadyItems = [
          {
            queue: { id: 1, businessUnitId: 1, recipeId: 1, quantity: "100" },
            recipe: { id: 1, name: "Steel", outputResourceId: 2 },
          },
          {
            queue: { id: 2, businessUnitId: 1, recipeId: 2, quantity: "50" },
            recipe: { id: 2, name: "Iron Ore", outputResourceId: 1 },
          },
          {
            queue: { id: 3, businessUnitId: 3, recipeId: 1, quantity: "200" }, // Different company
            recipe: { id: 1, name: "Steel", outputResourceId: 2 },
          },
        ];

        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany);
        vi.mocked(db.getBusinessUnitsByCompany).mockResolvedValue(mockUnits as any);
        vi.mocked(db.getReadyProductionItems).mockResolvedValue(mockReadyItems);

        const result = await caller.gameTurn.readyProduction();

        expect(result.length).toBe(2); // Only items from user's units
        expect(result.every((item: any) => [1, 2].includes(item.queue.businessUnitId))).toBe(true);
      });

      it("returns empty array when user has no company", async () => {
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(null);

        const result = await caller.gameTurn.readyProduction();

        expect(result).toEqual([]);
      });

      it("returns empty array when no production is ready", async () => {
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany);
        vi.mocked(db.getBusinessUnitsByCompany).mockResolvedValue([
          { id: 1, companyId: 1, name: "Factory 1" } as any,
        ]);
        vi.mocked(db.getReadyProductionItems).mockResolvedValue([]);

        const result = await caller.gameTurn.readyProduction();

        expect(result).toEqual([]);
      });
    });

    describe("gameTurn.completeProduction", () => {
      it("completes a production item", async () => {
        const mockUnits = [
          { id: 1, companyId: 1, name: "Factory 1", type: "factory" },
        ];

        const mockReadyItems = [
          {
            queue: { id: 1, businessUnitId: 1, recipeId: 1, quantity: "100" },
            recipe: { id: 1, name: "Steel", outputResourceId: 2, outputQuantity: "1.00" },
          },
        ];

        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany);
        vi.mocked(db.getReadyProductionItems).mockResolvedValue(mockReadyItems);
        vi.mocked(db.getBusinessUnitsByCompany).mockResolvedValue(mockUnits as any);
        vi.mocked(db.completeProductionItem).mockResolvedValue({
          businessUnitId: 1,
          outputResourceId: 2,
          outputQuantity: 100,
        });

        const result = await caller.gameTurn.completeProduction({ queueItemId: 1 });

        expect(result.outputResourceId).toBe(2);
        expect(result.outputQuantity).toBe(100);
        expect(db.completeProductionItem).toHaveBeenCalledWith(1);
      });

      it("throws error when company not found", async () => {
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(null);

        await expect(
          caller.gameTurn.completeProduction({ queueItemId: 1 })
        ).rejects.toThrow("Company not found");
      });

      it("throws error when production item not found", async () => {
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany);
        vi.mocked(db.getReadyProductionItems).mockResolvedValue([]);

        await expect(
          caller.gameTurn.completeProduction({ queueItemId: 999 })
        ).rejects.toThrow("Production item not found or not ready");
      });

      it("throws error when not authorized for production item", async () => {
        const mockUnits = [
          { id: 1, companyId: 1, name: "Factory 1" },
        ];

        const mockReadyItems = [
          {
            queue: { id: 1, businessUnitId: 999, recipeId: 1, quantity: "100" }, // Different unit
            recipe: { id: 1, name: "Steel" },
          },
        ];

        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany);
        vi.mocked(db.getReadyProductionItems).mockResolvedValue(mockReadyItems);
        vi.mocked(db.getBusinessUnitsByCompany).mockResolvedValue(mockUnits as any);

        await expect(
          caller.gameTurn.completeProduction({ queueItemId: 1 })
        ).rejects.toThrow("Not authorized to complete this production");
      });
    });
  });

  // ============================================================================
  // SIMULATION ENGINE ROUTES TESTS
  // ============================================================================
  describe("simulation routes", () => {
    const mockCompany = {
      id: 1,
      userId: mockUser.id,
      name: "Test Corp",
      cash: "1000000.00",
    };

    const mockAgent = {
      id: 1,
      type: "employee",
      name: "Test Agent",
      companyId: 1,
      happiness: 70,
      satisfaction: 70,
      stress: 30,
    };

    describe("simulation.state", () => {
      it("returns simulation state for an agent", async () => {
        const mockState = {
          agentId: 1,
          needs: {
            hunger: 65,
            energy: 80,
            hygiene: 90,
            bladder: 50,
            social: 40,
            fun: 30,
            comfort: 75,
          },
          skills: {
            cooking: 3.5,
            charisma: 2.0,
            fitness: 1.0,
          },
          mood: "happy",
          moodlets: [
            { id: "well_rested", name: "Well Rested", emotion: "happy", intensity: 20, expiresAt: null },
          ],
          currentAction: null,
          actionProgress: 0,
        };

        vi.mocked(simulationEngine.getSimulationState).mockResolvedValue(mockState);

        const result = await caller.simulation.state({ agentId: 1 });

        expect(result).toEqual(mockState);
        expect(simulationEngine.getSimulationState).toHaveBeenCalledWith(1);
      });

      it("returns null for non-existent agent", async () => {
        vi.mocked(simulationEngine.getSimulationState).mockResolvedValue(null);

        const result = await caller.simulation.state({ agentId: 999 });

        expect(result).toBeNull();
      });
    });

    describe("simulation.availableActions", () => {
      it("returns available actions for an agent", async () => {
        const mockState = {
          agentId: 1,
          needs: { hunger: 30, energy: 80 },
          skills: { cooking: 2.0 },
          mood: "fine",
          moodlets: [],
          currentAction: null,
          actionProgress: 0,
        };

        const mockActions = [
          { id: "eat_meal", name: "Eat a Meal", category: "self_care", duration: 30, needEffects: { hunger: 50 } },
          { id: "chat", name: "Have a Chat", category: "social", duration: 30, needEffects: { social: 25 } },
        ];

        vi.mocked(simulationEngine.getSimulationState).mockResolvedValue(mockState);
        vi.mocked(simulationEngine.getAvailableActions).mockReturnValue(mockActions);

        const result = await caller.simulation.availableActions({ agentId: 1 });

        expect(result).toEqual(mockActions);
        expect(simulationEngine.getAvailableActions).toHaveBeenCalledWith(mockState);
      });

      it("returns empty array when agent has no state", async () => {
        vi.mocked(simulationEngine.getSimulationState).mockResolvedValue(null);

        const result = await caller.simulation.availableActions({ agentId: 999 });

        expect(result).toEqual([]);
      });
    });

    describe("simulation.recommendedActions", () => {
      it("returns recommended actions based on agent needs", async () => {
        const mockState = {
          agentId: 1,
          needs: { hunger: 20, energy: 50, social: 80 },
          skills: {},
          mood: "sad",
          moodlets: [],
          currentAction: null,
          actionProgress: 0,
        };

        const mockRecommended = [
          { id: "eat_meal", name: "Eat a Meal", priority: 1, reason: "Hunger is critical" },
          { id: "nap", name: "Take a Nap", priority: 2, reason: "Energy is low" },
        ];

        vi.mocked(simulationEngine.getSimulationState).mockResolvedValue(mockState);
        vi.mocked(simulationEngine.getRecommendedActions).mockReturnValue(mockRecommended);

        const result = await caller.simulation.recommendedActions({ agentId: 1 });

        expect(result).toEqual(mockRecommended);
      });
    });

    describe("simulation.executeAction", () => {
      it("executes an action for an authorized agent", async () => {
        vi.mocked(db.getAgentById).mockResolvedValue(mockAgent as any);
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany as any);
        vi.mocked(simulationEngine.executeAction).mockResolvedValue({
          success: true,
          newState: {
            agentId: 1,
            needs: { hunger: 80, energy: 75 },
            skills: {},
            mood: "happy",
            moodlets: [],
            currentAction: null,
            actionProgress: 0,
          },
          message: "Finished eating a meal",
        });

        const result = await caller.simulation.executeAction({ agentId: 1, actionId: "eat_meal" });

        expect(result.success).toBe(true);
        expect(result.newState.needs.hunger).toBe(80);
        expect(simulationEngine.executeAction).toHaveBeenCalledWith(1, "eat_meal");
      });

      it("throws error when agent not found", async () => {
        vi.mocked(db.getAgentById).mockResolvedValue(null);

        await expect(
          caller.simulation.executeAction({ agentId: 999, actionId: "eat_meal" })
        ).rejects.toThrow("Agent not found");
      });

      it("throws error when not authorized for agent", async () => {
        const otherCompanyAgent = { ...mockAgent, companyId: 999 };
        vi.mocked(db.getAgentById).mockResolvedValue(otherCompanyAgent as any);
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany as any);

        await expect(
          caller.simulation.executeAction({ agentId: 1, actionId: "eat_meal" })
        ).rejects.toThrow("Not authorized");
      });

      it("allows action for agent without company", async () => {
        const publicAgent = { ...mockAgent, companyId: null };
        vi.mocked(db.getAgentById).mockResolvedValue(publicAgent as any);
        vi.mocked(simulationEngine.executeAction).mockResolvedValue({
          success: true,
          newState: {} as any,
          message: "Action completed",
        });

        const result = await caller.simulation.executeAction({ agentId: 1, actionId: "eat_meal" });

        expect(result.success).toBe(true);
      });
    });

    describe("simulation.simulateTime", () => {
      it("simulates time passage for an agent", async () => {
        vi.mocked(db.getAgentById).mockResolvedValue(mockAgent as any);
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany as any);
        vi.mocked(simulationEngine.simulateTimePassage).mockResolvedValue({
          agentId: 1,
          minutesSimulated: 120,
          newState: {
            agentId: 1,
            needs: { hunger: 40, energy: 70, hygiene: 80 },
            skills: {},
            mood: "fine",
            moodlets: [],
            currentAction: null,
            actionProgress: 0,
          },
          events: [
            { type: "need_decay", message: "Hunger decreased by 25" },
          ],
        });

        const result = await caller.simulation.simulateTime({ agentId: 1, minutes: 120 });

        expect(result.minutesSimulated).toBe(120);
        expect(simulationEngine.simulateTimePassage).toHaveBeenCalledWith(1, 120);
      });

      it("throws error for invalid time range", async () => {
        // This should be validated by zod schema
        vi.mocked(db.getAgentById).mockResolvedValue(mockAgent as any);
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany as any);

        // Try with over 24 hours (1440 minutes max)
        await expect(
          caller.simulation.simulateTime({ agentId: 1, minutes: 2000 })
        ).rejects.toThrow();
      });
    });

    describe("simulation.generateAutonomousAction", () => {
      it("generates an autonomous action recommendation", async () => {
        vi.mocked(simulationEngine.generateAutonomousAction).mockResolvedValue({
          actionId: "eat_meal",
          reason: "Agent is hungry",
          urgency: "high",
        });

        const result = await caller.simulation.generateAutonomousAction({ agentId: 1 });

        expect(result.actionId).toBe("eat_meal");
        expect(result.urgency).toBe("high");
      });
    });

    describe("simulation.allActions", () => {
      it("returns all available simulation actions", async () => {
        const result = await caller.simulation.allActions();

        expect(Array.isArray(result)).toBe(true);
        expect(result.length).toBeGreaterThan(0);
        expect(result[0]).toHaveProperty("id");
        expect(result[0]).toHaveProperty("name");
        expect(result[0]).toHaveProperty("category");
      });
    });
  });

  // ============================================================================
  // EVENT BRIDGE ROUTES TESTS
  // ============================================================================
  describe("eventBridge routes", () => {
    const mockCompany = {
      id: 1,
      userId: mockUser.id,
      name: "Test Corp",
      cash: "1000000.00",
    };

    const mockWorld = {
      id: 1,
      userId: mockUser.id,
      name: "Test World",
    };

    describe("eventBridge.history", () => {
      it("returns event propagation history", async () => {
        const mockHistory = [
          {
            id: 1,
            sourceType: "business",
            sourceEventId: 1,
            targetType: "narrative",
            targetEventId: 2,
            propagatedAt: new Date(),
            status: "completed",
          },
          {
            id: 2,
            sourceType: "narrative",
            sourceEventId: 3,
            targetType: "business",
            targetEventId: 4,
            propagatedAt: new Date(),
            status: "completed",
          },
        ];

        vi.mocked(db.getEventPropagationHistory).mockResolvedValue(mockHistory);

        const result = await caller.eventBridge.history();

        expect(result).toEqual(mockHistory);
        expect(db.getEventPropagationHistory).toHaveBeenCalled();
      });

      it("respects limit parameter", async () => {
        vi.mocked(db.getEventPropagationHistory).mockResolvedValue([]);

        await caller.eventBridge.history({ limit: 10 });

        expect(db.getEventPropagationHistory).toHaveBeenCalledWith(10);
      });
    });

    describe("eventBridge.bySourceType", () => {
      it("returns events filtered by source type", async () => {
        const mockEvents = [
          {
            id: 1,
            sourceType: "business",
            sourceEventId: 1,
            status: "completed",
          },
        ];

        vi.mocked(db.getEventPropagationBySourceType).mockResolvedValue(mockEvents);

        const result = await caller.eventBridge.bySourceType({ sourceType: "business" });

        expect(result).toEqual(mockEvents);
        expect(db.getEventPropagationBySourceType).toHaveBeenCalledWith("business", undefined);
      });

      it("filters narrative source type with limit", async () => {
        vi.mocked(db.getEventPropagationBySourceType).mockResolvedValue([]);

        await caller.eventBridge.bySourceType({ sourceType: "narrative", limit: 5 });

        expect(db.getEventPropagationBySourceType).toHaveBeenCalledWith("narrative", 5);
      });
    });

    describe("eventBridge.propagateBusinessEvent", () => {
      it("propagates a business event to narrative", async () => {
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany as any);
        vi.mocked(eventBridge.propagateBusinessEvent).mockResolvedValue({
          success: true,
          propagationId: 1,
          targetEvents: [
            { id: 1, type: "world_event", worldId: 1 },
          ],
        });

        const result = await caller.eventBridge.propagateBusinessEvent({
          type: "expansion",
          companyId: 1,
          magnitude: 75,
          description: "Company expanded to new city",
        });

        expect(result.success).toBe(true);
        expect(result.propagationId).toBe(1);
        expect(eventBridge.propagateBusinessEvent).toHaveBeenCalled();
      });

      it("throws error when not authorized for company", async () => {
        const otherCompany = { ...mockCompany, id: 999 };
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(otherCompany as any);

        await expect(
          caller.eventBridge.propagateBusinessEvent({
            type: "expansion",
            companyId: 1,
            magnitude: 75,
            description: "Unauthorized event",
          })
        ).rejects.toThrow("Not authorized to propagate events for this company");
      });

      it("supports all business event types", async () => {
        vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany as any);
        vi.mocked(eventBridge.propagateBusinessEvent).mockResolvedValue({
          success: true,
          propagationId: 1,
          targetEvents: [],
        });

        const eventTypes = ["bankruptcy", "merger", "market_crash", "expansion", "layoff", "innovation", "scandal", "success"];

        for (const type of eventTypes) {
          const result = await caller.eventBridge.propagateBusinessEvent({
            type: type as any,
            companyId: 1,
            magnitude: 50,
            description: `Test ${type} event`,
          });
          expect(result.success).toBe(true);
        }
      });
    });

    describe("eventBridge.propagateNarrativeEvent", () => {
      it("propagates a narrative event to business", async () => {
        vi.mocked(db.getWorldById).mockResolvedValue(mockWorld as any);
        vi.mocked(eventBridge.propagateNarrativeEvent).mockResolvedValue({
          success: true,
          propagationId: 2,
          targetEvents: [
            { id: 1, type: "market_shift", companyId: 1 },
          ],
        });

        const result = await caller.eventBridge.propagateNarrativeEvent({
          type: "discovery",
          worldId: 1,
          importance: 80,
          description: "Ancient artifact discovered",
        });

        expect(result.success).toBe(true);
        expect(eventBridge.propagateNarrativeEvent).toHaveBeenCalled();
      });

      it("throws error when world not found", async () => {
        vi.mocked(db.getWorldById).mockResolvedValue(null);

        await expect(
          caller.eventBridge.propagateNarrativeEvent({
            type: "discovery",
            worldId: 999,
            importance: 80,
            description: "Test event",
          })
        ).rejects.toThrow("Not authorized to propagate events for this world");
      });

      it("throws error when not authorized for world", async () => {
        const otherWorld = { ...mockWorld, userId: "other-user" };
        vi.mocked(db.getWorldById).mockResolvedValue(otherWorld as any);

        await expect(
          caller.eventBridge.propagateNarrativeEvent({
            type: "conflict",
            worldId: 1,
            importance: 60,
            description: "Unauthorized event",
          })
        ).rejects.toThrow("Not authorized to propagate events for this world");
      });
    });

    describe("eventBridge.processScheduledEvents", () => {
      it("processes scheduled events", async () => {
        vi.mocked(eventBridge.processScheduledEvents).mockResolvedValue(undefined);

        const result = await caller.eventBridge.processScheduledEvents();

        expect(result.success).toBe(true);
        expect(eventBridge.processScheduledEvents).toHaveBeenCalled();
      });
    });
  });

  // ============================================================================
  // API KEYS VERIFICATION TESTS
  // ============================================================================
  describe("apiKeys routes", () => {
    describe("apiKeys.list", () => {
      it("returns user's API keys", async () => {
        const mockKeys = [
          {
            id: 1,
            userId: mockUser.id,
            keyName: "Production Key",
            createdAt: new Date(),
            lastUsed: null,
          },
          {
            id: 2,
            userId: mockUser.id,
            keyName: "Development Key",
            createdAt: new Date(),
            lastUsed: new Date(),
          },
        ];

        vi.mocked(db.getApiKeysByUserId).mockResolvedValue(mockKeys);

        const result = await caller.apiKeys.list();

        expect(result).toEqual(mockKeys);
        expect(result.length).toBe(2);
      });
    });

    describe("apiKeys.create", () => {
      it("creates a new API key", async () => {
        const mockKey = {
          id: 1,
          userId: mockUser.id,
          keyName: "New Key",
          encryptedKey: "encrypted_value",
          createdAt: new Date(),
        };

        vi.mocked(db.createApiKey).mockResolvedValue(mockKey);

        const result = await caller.apiKeys.create({
          keyName: "New Key",
          encryptedKey: "my-api-key-value",
        });

        expect(result.keyName).toBe("New Key");
        expect(db.createApiKey).toHaveBeenCalledWith({
          userId: mockUser.id,
          keyName: "New Key",
          encryptedKey: "my-api-key-value",
        });
      });
    });

    describe("apiKeys.delete", () => {
      it("deletes an API key", async () => {
        vi.mocked(db.deleteApiKey).mockResolvedValue(undefined);

        const result = await caller.apiKeys.delete({ id: 1 });

        expect(result.success).toBe(true);
        expect(db.deleteApiKey).toHaveBeenCalledWith(1, mockUser.id);
      });
    });

    describe("apiKeys.verify", () => {
      it("verifies a valid API key", async () => {
        vi.mocked(db.verifyApiKey).mockResolvedValue({
          valid: true,
          message: "API key is valid",
        });

        const result = await caller.apiKeys.verify({ id: 1 });

        expect(result.valid).toBe(true);
        expect(db.verifyApiKey).toHaveBeenCalledWith(1, mockUser.id);
      });

      it("returns invalid result for bad key", async () => {
        vi.mocked(db.verifyApiKey).mockResolvedValue({
          valid: false,
          error: "Invalid API key",
        });

        const result = await caller.apiKeys.verify({ id: 1 });

        expect(result.valid).toBe(false);
        expect(result.error).toBe("Invalid API key");
      });

      it("handles verification errors gracefully", async () => {
        vi.mocked(db.verifyApiKey).mockResolvedValue({
          valid: false,
          error: "Failed to connect to DreamGen API",
        });

        const result = await caller.apiKeys.verify({ id: 1 });

        expect(result.valid).toBe(false);
        expect(result.error).toContain("DreamGen");
      });
    });
  });
});

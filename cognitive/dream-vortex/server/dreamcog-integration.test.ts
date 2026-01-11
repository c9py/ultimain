import { describe, it, expect, vi, beforeEach } from "vitest";
import { appRouter } from "./routers";

// Test constants
const TEST_USER_ID = 1;
const TEST_AGENT_ID = 1;
const TEST_WORLD_ID = 1;
const TEST_RELATIONSHIP_ID = 1;

// Mock the database functions
vi.mock("./db", () => ({
  initializeGameData: vi.fn().mockResolvedValue(undefined),
  seedProductionRecipes: vi.fn().mockResolvedValue(undefined),
  getCompanyByUserId: vi.fn(),
  
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
  getWorldById: vi.fn(),
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
  
  // Other required mocks for base functionality
  getAgentById: vi.fn(),
  createAgent: vi.fn(),
  getAllCharacterPersonas: vi.fn(),
  getAllCharacterTraits: vi.fn(),
  getAgentsByType: vi.fn(),
  getAgentsByCompany: vi.fn(),
  createCompany: vi.fn(),
  getAllCompanies: vi.fn(),
  getBusinessUnitsByCompany: vi.fn(),
  getBusinessUnitById: vi.fn(),
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
}));

import * as db from "./db";

// Create a mock context
const createMockContext = (userId: number = 1) => ({
  user: { id: userId, openId: "test-user", role: "user" as const },
  req: {} as any,
  res: {} as any,
});

describe("DreamCog Integration - Big Five Personality", () => {
  beforeEach(() => {
    vi.clearAllMocks();
  });

  it("should create Big Five personality profile", async () => {
    const mockPersonality = {
      id: 1,
      agentId: TEST_AGENT_ID,
      openness: 80,
      conscientiousness: 70,
      extraversion: 60,
      agreeableness: 75,
      neuroticism: 40,
      formalityLevel: 50,
      verbosityLevel: 50,
      emotionalExpression: 50,
      humorLevel: 50,
      directnessLevel: 50,
      impulsiveness: 30,
      riskTaking: 40,
      empathy: 80,
      leadership: 70,
      independence: 60,
      createdAt: new Date(),
      updatedAt: new Date(),
    };

    vi.mocked(db.createAgentBigFivePersonality).mockResolvedValue(mockPersonality);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.personality.create({
      agentId: TEST_AGENT_ID,
      openness: 80,
      conscientiousness: 70,
      extraversion: 60,
      agreeableness: 75,
      neuroticism: 40,
      empathy: 80,
      leadership: 70,
    });

    expect(db.createAgentBigFivePersonality).toHaveBeenCalledWith({
      agentId: TEST_AGENT_ID,
      openness: 80,
      conscientiousness: 70,
      extraversion: 60,
      agreeableness: 75,
      neuroticism: 40,
      empathy: 80,
      leadership: 70,
    });
    expect(result).toEqual(mockPersonality);
  });

  it("should get Big Five personality profile", async () => {
    const mockPersonality = {
      id: 1,
      agentId: TEST_AGENT_ID,
      openness: 80,
      conscientiousness: 70,
      extraversion: 60,
      agreeableness: 75,
      neuroticism: 40,
      formalityLevel: 50,
      verbosityLevel: 50,
      emotionalExpression: 50,
      humorLevel: 50,
      directnessLevel: 50,
      impulsiveness: 30,
      riskTaking: 40,
      empathy: 80,
      leadership: 70,
      independence: 60,
      createdAt: new Date(),
      updatedAt: new Date(),
    };

    vi.mocked(db.getAgentBigFivePersonality).mockResolvedValue(mockPersonality);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.personality.get({ agentId: TEST_AGENT_ID });

    expect(db.getAgentBigFivePersonality).toHaveBeenCalledWith(1);
    expect(result).toEqual(mockPersonality);
  });

  it("should update Big Five personality profile", async () => {
    vi.mocked(db.updateAgentBigFivePersonality).mockResolvedValue(undefined);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.personality.update({
      agentId: TEST_AGENT_ID,
      updates: {
        openness: 85,
        leadership: 75,
      },
    });

    expect(db.updateAgentBigFivePersonality).toHaveBeenCalledWith(1, {
      openness: 85,
      leadership: 75,
    });
    expect(result).toEqual({ success: true });
  });
});

describe("DreamCog Integration - Agent Motivations", () => {
  beforeEach(() => {
    vi.clearAllMocks();
  });

  it("should create agent motivation", async () => {
    const mockMotivation = {
      id: 1,
      agentId: TEST_AGENT_ID,
      motivationType: "long_term" as const,
      description: "Become the CEO",
      priority: 10,
      progress: 0,
      isActive: true,
      createdAt: new Date(),
      updatedAt: new Date(),
    };

    vi.mocked(db.createAgentMotivation).mockResolvedValue(mockMotivation);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.motivation.create({
      agentId: TEST_AGENT_ID,
      motivationType: "long_term",
      description: "Become the CEO",
      priority: 10,
    });

    expect(db.createAgentMotivation).toHaveBeenCalled();
    expect(result).toEqual(mockMotivation);
  });

  it("should get agent motivations with activeOnly filter", async () => {
    const mockMotivations = [
      {
        id: 1,
        agentId: TEST_AGENT_ID,
        motivationType: "long_term" as const,
        description: "Become the CEO",
        priority: 10,
        progress: 50,
        isActive: true,
        createdAt: new Date(),
        updatedAt: new Date(),
      },
    ];

    vi.mocked(db.getAgentMotivations).mockResolvedValue(mockMotivations);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.motivation.byAgent({
      agentId: TEST_AGENT_ID,
      activeOnly: true,
    });

    expect(db.getAgentMotivations).toHaveBeenCalledWith(TEST_AGENT_ID, true);
    expect(result).toEqual(mockMotivations);
  });

  it("should update motivation progress", async () => {
    vi.mocked(db.updateAgentMotivation).mockResolvedValue(undefined);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.motivation.update({
      id: 1,
      updates: {
        progress: 75,
      },
    });

    expect(db.updateAgentMotivation).toHaveBeenCalledWith(1, {
      progress: 75,
    });
    expect(result).toEqual({ success: true });
  });
});

describe("DreamCog Integration - Agent Memories", () => {
  beforeEach(() => {
    vi.clearAllMocks();
  });

  it("should create agent memory", async () => {
    const mockMemory = {
      id: 1,
      agentId: TEST_AGENT_ID,
      memoryType: "achievement" as const,
      content: "Successfully led the team through Q3 launch",
      emotionalImpact: 75,
      importance: 8,
      eventId: null,
      relatedAgentId: null,
      locationId: null,
      memoryDate: new Date(),
      isRepressed: false,
      lastRecalled: null,
      createdAt: new Date(),
    };

    vi.mocked(db.createAgentMemory).mockResolvedValue(mockMemory);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.memory.create({
      agentId: TEST_AGENT_ID,
      memoryType: "achievement",
      content: "Successfully led the team through Q3 launch",
      emotionalImpact: 75,
      importance: 8,
      memoryDate: new Date(),
    });

    expect(db.createAgentMemory).toHaveBeenCalled();
    expect(result).toEqual(mockMemory);
  });

  it("should get agent memories", async () => {
    const mockMemories = [
      {
        id: 1,
        agentId: TEST_AGENT_ID,
        memoryType: "achievement" as const,
        content: "Successfully led the team",
        emotionalImpact: 75,
        importance: 8,
        eventId: null,
        relatedAgentId: null,
        locationId: null,
        memoryDate: new Date(),
        isRepressed: false,
        lastRecalled: null,
        createdAt: new Date(),
      },
    ];

    vi.mocked(db.getAgentMemories).mockResolvedValue(mockMemories);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.memory.byAgent({
      agentId: TEST_AGENT_ID,
      limit: 50,
    });

    expect(db.getAgentMemories).toHaveBeenCalledWith(TEST_AGENT_ID, 50);
    expect(result).toEqual(mockMemories);
  });
});

describe("DreamCog Integration - Worlds", () => {
  beforeEach(() => {
    vi.clearAllMocks();
  });

  it("should create world", async () => {
    const mockWorld = {
      id: 1,
      userId: TEST_USER_ID,
      name: "Technotopia",
      description: "A cyberpunk world",
      genre: "cyberpunk",
      timePeriod: "2150",
      technologyLevel: "advanced",
      magicSystem: null,
      culturalNotes: null,
      rules: null,
      isPublic: false,
      createdAt: new Date(),
      updatedAt: new Date(),
    };

    vi.mocked(db.createWorld).mockResolvedValue(mockWorld);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.world.create({
      name: "Technotopia",
      description: "A cyberpunk world",
      genre: "cyberpunk",
      timePeriod: "2150",
      technologyLevel: "advanced",
    });

    expect(db.createWorld).toHaveBeenCalledWith({
      userId: TEST_USER_ID,
      name: "Technotopia",
      description: "A cyberpunk world",
      genre: "cyberpunk",
      timePeriod: "2150",
      technologyLevel: "advanced",
    });
    expect(result).toEqual(mockWorld);
  });

  it("should get user's worlds", async () => {
    const mockWorlds = [
      {
        id: 1,
        userId: TEST_USER_ID,
        name: "Technotopia",
        description: "A cyberpunk world",
        genre: "cyberpunk",
        timePeriod: "2150",
        technologyLevel: "advanced",
        magicSystem: null,
        culturalNotes: null,
        rules: null,
        isPublic: false,
        createdAt: new Date(),
        updatedAt: new Date(),
      },
    ];

    vi.mocked(db.getWorldsByUserId).mockResolvedValue(mockWorlds);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.world.list();

    expect(db.getWorldsByUserId).toHaveBeenCalledWith(1);
    expect(result).toEqual(mockWorlds);
  });

  it("should update world with authorization", async () => {
    const mockWorld = {
      id: 1,
      userId: TEST_USER_ID,
      name: "Technotopia",
      description: "Updated description",
      genre: "cyberpunk",
      timePeriod: "2150",
      technologyLevel: "advanced",
      magicSystem: null,
      culturalNotes: null,
      rules: null,
      isPublic: false,
      createdAt: new Date(),
      updatedAt: new Date(),
    };

    vi.mocked(db.getWorldById).mockResolvedValue(mockWorld);
    vi.mocked(db.updateWorld).mockResolvedValue(undefined);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.world.update({
      id: 1,
      updates: {
        description: "Updated description",
      },
    });

    expect(db.updateWorld).toHaveBeenCalledWith(1, {
      description: "Updated description",
    });
    expect(result).toEqual({ success: true });
  });

  it("should reject update if user doesn't own world", async () => {
    const mockWorld = {
      id: 1,
      userId: 2, // Different user
      name: "Technotopia",
      description: "A cyberpunk world",
      genre: "cyberpunk",
      timePeriod: "2150",
      technologyLevel: "advanced",
      magicSystem: null,
      culturalNotes: null,
      rules: null,
      isPublic: false,
      createdAt: new Date(),
      updatedAt: new Date(),
    };

    vi.mocked(db.getWorldById).mockResolvedValue(mockWorld);

    const caller = appRouter.createCaller(createMockContext(1));
    
    await expect(
      caller.world.update({
        id: 1,
        updates: { description: "Updated" },
      })
    ).rejects.toThrow("Not authorized");
  });
});

describe("DreamCog Integration - Locations", () => {
  beforeEach(() => {
    vi.clearAllMocks();
  });

  it("should create location", async () => {
    const mockLocation = {
      id: 1,
      worldId: TEST_WORLD_ID,
      name: "Neo-Singapore",
      locationType: "city" as const,
      description: "A sprawling megacity",
      parentLocationId: null,
      attributes: {
        climate: "tropical",
        population: "50 million",
        dangerLevel: 40,
      },
      createdAt: new Date(),
      updatedAt: new Date(),
    };

    vi.mocked(db.createLocation).mockResolvedValue(mockLocation);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.location.create({
      worldId: TEST_WORLD_ID,
      name: "Neo-Singapore",
      locationType: "city",
      description: "A sprawling megacity",
      attributes: {
        climate: "tropical",
        population: "50 million",
        dangerLevel: 40,
      },
    });

    expect(db.createLocation).toHaveBeenCalled();
    expect(result).toEqual(mockLocation);
  });

  it("should get locations by world", async () => {
    const mockLocations = [
      {
        id: 1,
        worldId: TEST_WORLD_ID,
        name: "Neo-Singapore",
        locationType: "city" as const,
        description: "A sprawling megacity",
        parentLocationId: null,
        attributes: null,
        createdAt: new Date(),
        updatedAt: new Date(),
      },
    ];

    vi.mocked(db.getLocationsByWorldId).mockResolvedValue(mockLocations);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.location.byWorld({ worldId: TEST_WORLD_ID });

    expect(db.getLocationsByWorldId).toHaveBeenCalledWith(1);
    expect(result).toEqual(mockLocations);
  });

  it("should get sub-locations", async () => {
    const mockSubLocations = [
      {
        id: 2,
        worldId: TEST_WORLD_ID,
        name: "Tech District",
        locationType: "building" as const,
        description: "High-tech buildings",
        parentLocationId: 1,
        attributes: null,
        createdAt: new Date(),
        updatedAt: new Date(),
      },
    ];

    vi.mocked(db.getSubLocations).mockResolvedValue(mockSubLocations);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.location.subLocations({ parentLocationId: 1 });

    expect(db.getSubLocations).toHaveBeenCalledWith(1);
    expect(result).toEqual(mockSubLocations);
  });
});

describe("DreamCog Integration - Lore Entries", () => {
  beforeEach(() => {
    vi.clearAllMocks();
  });

  it("should create lore entry", async () => {
    const mockLore = {
      id: 1,
      worldId: TEST_WORLD_ID,
      category: "history" as const,
      title: "The Digital Awakening",
      content: "In 2085, the first AI awakened...",
      isPublic: true,
      isSecret: false,
      relatedLocationId: null,
      relatedAgentId: null,
      tags: ["history", "technology"],
      createdAt: new Date(),
      updatedAt: new Date(),
    };

    vi.mocked(db.createLoreEntry).mockResolvedValue(mockLore);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.lore.create({
      worldId: TEST_WORLD_ID,
      category: "history",
      title: "The Digital Awakening",
      content: "In 2085, the first AI awakened...",
      isPublic: true,
      tags: ["history", "technology"],
    });

    expect(db.createLoreEntry).toHaveBeenCalled();
    expect(result).toEqual(mockLore);
  });

  it("should get lore entries by world and category", async () => {
    const mockLoreEntries = [
      {
        id: 1,
        worldId: TEST_WORLD_ID,
        category: "history" as const,
        title: "The Digital Awakening",
        content: "In 2085...",
        isPublic: true,
        isSecret: false,
        relatedLocationId: null,
        relatedAgentId: null,
        tags: null,
        createdAt: new Date(),
        updatedAt: new Date(),
      },
    ];

    vi.mocked(db.getLoreEntriesByWorldId).mockResolvedValue(mockLoreEntries);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.lore.byWorld({
      worldId: TEST_WORLD_ID,
      category: "history",
    });

    expect(db.getLoreEntriesByWorldId).toHaveBeenCalledWith(1, "history");
    expect(result).toEqual(mockLoreEntries);
  });
});

describe("DreamCog Integration - World Events", () => {
  beforeEach(() => {
    vi.clearAllMocks();
  });

  it("should create world event", async () => {
    const mockEvent = {
      id: 1,
      worldId: TEST_WORLD_ID,
      title: "The Corporate Wars",
      description: "Three megacorps battled for control",
      eventType: "battle" as const,
      importance: 9,
      eventDate: "2145-03-15",
      duration: "6 months",
      locationId: null,
      involvedAgentIds: null,
      involvedGroupIds: null,
      consequences: "Established tripartite power structure",
      createdAt: new Date(),
      updatedAt: new Date(),
    };

    vi.mocked(db.createWorldEvent).mockResolvedValue(mockEvent);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.worldEvent.create({
      worldId: TEST_WORLD_ID,
      title: "The Corporate Wars",
      description: "Three megacorps battled for control",
      eventType: "battle",
      importance: 9,
      eventDate: "2145-03-15",
      duration: "6 months",
      consequences: "Established tripartite power structure",
    });

    expect(db.createWorldEvent).toHaveBeenCalled();
    expect(result).toEqual(mockEvent);
  });

  it("should get world events by world", async () => {
    const mockEvents = [
      {
        id: 1,
        worldId: TEST_WORLD_ID,
        title: "The Corporate Wars",
        description: "Three megacorps battled",
        eventType: "battle" as const,
        importance: 9,
        eventDate: "2145-03-15",
        duration: null,
        locationId: null,
        involvedAgentIds: null,
        involvedGroupIds: null,
        consequences: null,
        createdAt: new Date(),
        updatedAt: new Date(),
      },
    ];

    vi.mocked(db.getWorldEventsByWorldId).mockResolvedValue(mockEvents);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.worldEvent.byWorld({ worldId: TEST_WORLD_ID });

    expect(db.getWorldEventsByWorldId).toHaveBeenCalledWith(1);
    expect(result).toEqual(mockEvents);
  });
});

describe("DreamCog Integration - Scheduled World Events", () => {
  beforeEach(() => {
    vi.clearAllMocks();
  });

  it("should create scheduled world event", async () => {
    const scheduledFor = new Date("2025-04-01");
    const mockEvent = {
      id: 1,
      worldId: TEST_WORLD_ID,
      eventName: "Quarterly Business Review",
      description: "Company-wide performance evaluation",
      scheduledFor,
      eventTrigger: {
        triggerType: "time" as const,
      },
      targetAgentIds: [1, 2],
      targetLocationId: null,
      priority: 7,
      isRecurring: true,
      status: "pending" as const,
      createdAt: new Date(),
      updatedAt: new Date(),
    };

    vi.mocked(db.createScheduledWorldEvent).mockResolvedValue(mockEvent);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.scheduledWorldEvent.create({
      worldId: TEST_WORLD_ID,
      eventName: "Quarterly Business Review",
      description: "Company-wide performance evaluation",
      scheduledFor,
      eventTrigger: {
        triggerType: "time",
      },
      targetAgentIds: [1, 2],
      priority: 7,
      isRecurring: true,
    });

    expect(db.createScheduledWorldEvent).toHaveBeenCalled();
    expect(result).toEqual(mockEvent);
  });

  it("should get pending scheduled events", async () => {
    const mockEvents = [
      {
        id: 1,
        worldId: TEST_WORLD_ID,
        eventName: "Quarterly Review",
        description: "Performance evaluation",
        scheduledFor: new Date("2025-04-01"),
        eventTrigger: null,
        targetAgentIds: null,
        targetLocationId: null,
        priority: 7,
        isRecurring: false,
        status: "pending" as const,
        createdAt: new Date(),
        updatedAt: new Date(),
      },
    ];

    vi.mocked(db.getPendingScheduledWorldEvents).mockResolvedValue(mockEvents);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.scheduledWorldEvent.pending({ worldId: TEST_WORLD_ID });

    expect(db.getPendingScheduledWorldEvents).toHaveBeenCalledWith(1);
    expect(result).toEqual(mockEvents);
  });

  it("should update scheduled event status", async () => {
    vi.mocked(db.updateScheduledWorldEvent).mockResolvedValue(undefined);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.scheduledWorldEvent.update({
      id: 1,
      updates: {
        status: "completed",
      },
    });

    expect(db.updateScheduledWorldEvent).toHaveBeenCalledWith(1, {
      status: "completed",
    });
    expect(result).toEqual({ success: true });
  });
});

describe("DreamCog Integration - Relationship Events", () => {
  beforeEach(() => {
    vi.clearAllMocks();
  });

  it("should create relationship event", async () => {
    const eventDate = new Date();
    const mockEvent = {
      id: 1,
      relationshipId: 1,
      eventType: "milestone" as const,
      description: "Received promotion together",
      impactOnTrust: 15,
      impactOnAffection: 10,
      impactOnRespect: 20,
      eventDate,
      createdAt: new Date(),
    };

    vi.mocked(db.createRelationshipEvent).mockResolvedValue(mockEvent);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.relationshipEvent.create({
      relationshipId: 1,
      eventType: "milestone",
      description: "Received promotion together",
      impactOnTrust: 15,
      impactOnAffection: 10,
      impactOnRespect: 20,
      eventDate,
    });

    expect(db.createRelationshipEvent).toHaveBeenCalled();
    expect(result).toEqual(mockEvent);
  });

  it("should get relationship events", async () => {
    const mockEvents = [
      {
        id: 1,
        relationshipId: 1,
        eventType: "milestone" as const,
        description: "Promotion",
        impactOnTrust: 15,
        impactOnAffection: 10,
        impactOnRespect: 20,
        eventDate: new Date(),
        createdAt: new Date(),
      },
    ];

    vi.mocked(db.getRelationshipEvents).mockResolvedValue(mockEvents);

    const caller = appRouter.createCaller(createMockContext());
    const result = await caller.relationshipEvent.byRelationship({
      relationshipId: 1,
    });

    expect(db.getRelationshipEvents).toHaveBeenCalledWith(1);
    expect(result).toEqual(mockEvents);
  });
});

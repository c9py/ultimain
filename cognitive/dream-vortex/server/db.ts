import { and, eq, desc, asc, sql, like } from "drizzle-orm";
import { drizzle } from "drizzle-orm/mysql2";
import {
  InsertUser,
  users,
  companies,
  InsertCompany,
  Company,
  businessUnits,
  InsertBusinessUnit,
  BusinessUnit,
  cities,
  City,
  employees,
  InsertEmployee,
  inventory,
  InsertInventory,
  resourceTypes,
  ResourceType,
  marketListings,
  InsertMarketListing,
  transactions,
  InsertTransaction,
  notifications,
  InsertNotification,
  gameState,
  technologies,
  companyTechnologies,
  productionRecipes,
  productionQueue,
  // Agentic simulation imports
  characterPersonas,
  InsertCharacterPersona,
  CharacterPersona,
  characterTraits,
  InsertCharacterTrait,
  CharacterTrait,
  agents,
  InsertAgent,
  Agent,
  agentTraits,
  InsertAgentTrait,
  relationships,
  InsertRelationship,
  Relationship,
  agentGroups,
  InsertAgentGroup,
  AgentGroup,
  groupMemberships,
  InsertGroupMembership,
  communities,
  InsertCommunity,
  Community,
  communityMemberships,
  InsertCommunityMembership,
  agentEvents,
  InsertAgentEvent,
  AgentEvent,
  eventTriggers,
  InsertEventTrigger,
  agentHistories,
  InsertAgentHistory,
  // DreamCog integration imports
  agentBigFivePersonality,
  InsertAgentBigFivePersonality,
  AgentBigFivePersonality,
  agentMotivations,
  InsertAgentMotivation,
  AgentMotivation,
  agentMemories,
  InsertAgentMemory,
  AgentMemory,
  relationshipEvents,
  InsertRelationshipEvent,
  RelationshipEvent,
  worlds,
  InsertWorld,
  World,
  locations,
  InsertLocation,
  Location,
  loreEntries,
  InsertLoreEntry,
  LoreEntry,
  worldEvents,
  InsertWorldEvent,
  WorldEvent,
  scheduledWorldEvents,
  InsertScheduledWorldEvent,
  ScheduledWorldEvent,
  eventPropagation,
  InsertEventPropagation,
  EventPropagation,
  // DreamCog storytelling imports
  apiKeys,
  InsertApiKey,
  characters,
  InsertCharacter,
  Character,
  characterEmotionalStates,
  InsertCharacterEmotionalState,
  characterMotivations,
  InsertCharacterMotivation,
  characterMemories,
  InsertCharacterMemory,
  scenarios,
  InsertScenario,
  scenarioCharacters,
  InsertScenarioCharacter,
  scenarioInteractions,
  InsertScenarioInteraction,
  chatSessions,
  InsertChatSession,
  chatMessages,
  InsertChatMessage,
  stories,
  InsertStory,
  storyCharacters,
  InsertStoryCharacter,
  generatedImages,
  InsertGeneratedImage,
  groups,
  InsertGroup,
  scheduledEvents,
  InsertScheduledEvent,
} from "../drizzle/schema";
import { ENV } from "./_core/env";

let _db: ReturnType<typeof drizzle> | null = null;

export async function getDb() {
  if (!_db && process.env.DATABASE_URL) {
    try {
      _db = drizzle(process.env.DATABASE_URL);
    } catch (error) {
      console.warn("[Database] Failed to connect:", error);
      _db = null;
    }
  }
  return _db;
}
// ============================================================================
// USER OPERATIONS
// ============================================================================
export async function upsertUser(user: InsertUser): Promise<void> {
  if (!user.openId) {
    throw new Error("User openId is required for upsert");
  }

  const db = await getDb();
  if (!db) {
    console.warn("[Database] Cannot upsert user: database not available");
    return;
  }

  try {
    const values: InsertUser = {
      openId: user.openId,
    };
    const updateSet: Record<string, unknown> = {};

    const textFields = ["name", "email", "loginMethod"] as const;
    type TextField = (typeof textFields)[number];

    const assignNullable = (field: TextField) => {
      const value = user[field];
      if (value === undefined) return;
      const normalized = value ?? null;
      values[field] = normalized;
      updateSet[field] = normalized;
    };

    textFields.forEach(assignNullable);

    if (user.lastSignedIn !== undefined) {
      values.lastSignedIn = user.lastSignedIn;
      updateSet.lastSignedIn = user.lastSignedIn;
    }
    if (user.role !== undefined) {
      values.role = user.role;
      updateSet.role = user.role;
    } else if (user.openId === ENV.ownerOpenId) {
      values.role = "admin";
      updateSet.role = "admin";
    }

    if (!values.lastSignedIn) {
      values.lastSignedIn = new Date();
    }

    if (Object.keys(updateSet).length === 0) {
      updateSet.lastSignedIn = new Date();
    }

    await db.insert(users).values(values).onDuplicateKeyUpdate({
      set: updateSet,
    });
  } catch (error) {
    console.error("[Database] Failed to upsert user:", error);
    throw error;
  }
}

export async function getUserByOpenId(openId: string) {
  const db = await getDb();
  if (!db) {
    console.warn("[Database] Cannot get user: database not available");
    return undefined;
  }

  const result = await db
    .select()
    .from(users)
    .where(eq(users.openId, openId))
    .limit(1);

  return result.length > 0 ? result[0] : undefined;
}

export async function getUserById(id: number) {
  const db = await getDb();
  if (!db) return undefined;

  const result = await db.select().from(users).where(eq(users.id, id)).limit(1);
  return result.length > 0 ? result[0] : undefined;
}
// ============================================================================
// COMPANY OPERATIONS
// ============================================================================
export async function createCompany(data: InsertCompany): Promise<Company | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db.insert(companies).values(data);
  const insertId = result[0].insertId;
  
  const created = await db
    .select()
    .from(companies)
    .where(eq(companies.id, insertId))
    .limit(1);
  
  return created[0] || null;
}

export async function getCompanyByUserId(userId: number): Promise<Company | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db
    .select()
    .from(companies)
    .where(eq(companies.userId, userId))
    .limit(1);

  return result[0] || null;
}

export async function getCompanyById(id: number): Promise<Company | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db
    .select()
    .from(companies)
    .where(eq(companies.id, id))
    .limit(1);

  return result[0] || null;
}

export async function updateCompanyCash(
  companyId: number,
  amount: string
): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db
    .update(companies)
    .set({ cash: amount })
    .where(eq(companies.id, companyId));
}

export async function getAllCompanies(): Promise<Company[]> {
  const db = await getDb();
  if (!db) return [];

  return await db.select().from(companies).orderBy(desc(companies.cash));
}
// ============================================================================
// BUSINESS UNIT OPERATIONS
// ============================================================================
export async function createBusinessUnit(
  data: InsertBusinessUnit
): Promise<BusinessUnit | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db.insert(businessUnits).values(data);
  const insertId = result[0].insertId;

  const created = await db
    .select()
    .from(businessUnits)
    .where(eq(businessUnits.id, insertId))
    .limit(1);

  return created[0] || null;
}

export async function getBusinessUnitsByCompany(
  companyId: number
): Promise<BusinessUnit[]> {
  const db = await getDb();
  if (!db) return [];

  return await db
    .select()
    .from(businessUnits)
    .where(eq(businessUnits.companyId, companyId));
}

export async function getBusinessUnitById(
  id: number
): Promise<BusinessUnit | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db
    .select()
    .from(businessUnits)
    .where(eq(businessUnits.id, id))
    .limit(1);

  return result[0] || null;
}

export async function updateBusinessUnit(
  id: number,
  data: Partial<InsertBusinessUnit>
): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db.update(businessUnits).set(data).where(eq(businessUnits.id, id));
}
// ============================================================================
// CITY OPERATIONS
// ============================================================================
export async function getAllCities(): Promise<City[]> {
  const db = await getDb();
  if (!db) return [];

  return await db.select().from(cities).orderBy(cities.name);
}

export async function getCityById(id: number): Promise<City | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db.select().from(cities).where(eq(cities.id, id)).limit(1);
  return result[0] || null;
}

export async function seedCities(): Promise<void> {
  const db = await getDb();
  if (!db) return;

  const existingCities = await db.select().from(cities).limit(1);
  if (existingCities.length > 0) return;

  const defaultCities = [
    { name: "New York", country: "USA", population: 8336817, wealthIndex: "1.50", taxRate: "0.2500" },
    { name: "Los Angeles", country: "USA", population: 3979576, wealthIndex: "1.40", taxRate: "0.2300" },
    { name: "Chicago", country: "USA", population: 2693976, wealthIndex: "1.30", taxRate: "0.2200" },
    { name: "London", country: "UK", population: 8982000, wealthIndex: "1.60", taxRate: "0.2800" },
    { name: "Paris", country: "France", population: 2161000, wealthIndex: "1.45", taxRate: "0.3000" },
    { name: "Berlin", country: "Germany", population: 3645000, wealthIndex: "1.35", taxRate: "0.2600" },
    { name: "Tokyo", country: "Japan", population: 13960000, wealthIndex: "1.55", taxRate: "0.2400" },
    { name: "Shanghai", country: "China", population: 24870000, wealthIndex: "1.20", taxRate: "0.2000" },
    { name: "Mumbai", country: "India", population: 12442373, wealthIndex: "0.90", taxRate: "0.1800" },
    { name: "São Paulo", country: "Brazil", population: 12325232, wealthIndex: "1.00", taxRate: "0.2100" },
    { name: "Sydney", country: "Australia", population: 5312163, wealthIndex: "1.45", taxRate: "0.2700" },
    { name: "Dubai", country: "UAE", population: 3331420, wealthIndex: "1.70", taxRate: "0.0500" },
  ];

  await db.insert(cities).values(defaultCities);
}
// ============================================================================
// RESOURCE TYPE OPERATIONS
// ============================================================================
export async function getAllResourceTypes(): Promise<ResourceType[]> {
  const db = await getDb();
  if (!db) return [];

  return await db.select().from(resourceTypes).orderBy(resourceTypes.name);
}

export async function getResourceTypeById(id: number): Promise<ResourceType | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db
    .select()
    .from(resourceTypes)
    .where(eq(resourceTypes.id, id))
    .limit(1);

  return result[0] || null;
}

export async function seedResourceTypes(): Promise<void> {
  const db = await getDb();
  if (!db) return;

  const existing = await db.select().from(resourceTypes).limit(1);
  if (existing.length > 0) return;

  const defaultResources = [
    // Raw Materials
    { code: "iron_ore", name: "Iron Ore", category: "raw_material" as const, basePrice: "50.00", unit: "tons", icon: "⛏️" },
    { code: "coal", name: "Coal", category: "raw_material" as const, basePrice: "30.00", unit: "tons", icon: "�ite" },
    { code: "oil", name: "Crude Oil", category: "raw_material" as const, basePrice: "80.00", unit: "barrels", icon: "🛢️" },
    { code: "wood", name: "Timber", category: "raw_material" as const, basePrice: "25.00", unit: "m³", icon: "🪵" },
    { code: "cotton", name: "Cotton", category: "raw_material" as const, basePrice: "40.00", unit: "bales", icon: "🌿" },
    { code: "wheat", name: "Wheat", category: "raw_material" as const, basePrice: "15.00", unit: "tons", icon: "🌾" },
    
    // Intermediate Goods
    { code: "steel", name: "Steel", category: "intermediate" as const, basePrice: "200.00", unit: "tons", icon: "🔩" },
    { code: "plastic", name: "Plastic", category: "intermediate" as const, basePrice: "150.00", unit: "tons", icon: "♻️" },
    { code: "fabric", name: "Fabric", category: "intermediate" as const, basePrice: "100.00", unit: "rolls", icon: "🧵" },
    { code: "flour", name: "Flour", category: "intermediate" as const, basePrice: "35.00", unit: "tons", icon: "🌾" },
    { code: "lumber", name: "Lumber", category: "intermediate" as const, basePrice: "60.00", unit: "m³", icon: "🪓" },
    
    // Finished Goods
    { code: "cars", name: "Automobiles", category: "finished_good" as const, basePrice: "25000.00", unit: "units", icon: "🚗" },
    { code: "electronics", name: "Electronics", category: "finished_good" as const, basePrice: "500.00", unit: "units", icon: "📱" },
    { code: "clothing", name: "Clothing", category: "finished_good" as const, basePrice: "50.00", unit: "units", icon: "👕" },
    { code: "furniture", name: "Furniture", category: "finished_good" as const, basePrice: "300.00", unit: "units", icon: "🪑" },
    { code: "food", name: "Processed Food", category: "finished_good" as const, basePrice: "20.00", unit: "units", icon: "🍞" },
    
    // Equipment
    { code: "machinery", name: "Industrial Machinery", category: "equipment" as const, basePrice: "50000.00", unit: "units", icon: "⚙️" },
    { code: "computers", name: "Computers", category: "equipment" as const, basePrice: "1000.00", unit: "units", icon: "💻" },
  ];

  await db.insert(resourceTypes).values(defaultResources);
}
// ============================================================================
// EMPLOYEE OPERATIONS
// ============================================================================
export async function createEmployees(data: InsertEmployee): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db.insert(employees).values(data);
}

export async function getEmployeesByUnit(businessUnitId: number) {
  const db = await getDb();
  if (!db) return null;

  const result = await db
    .select()
    .from(employees)
    .where(eq(employees.businessUnitId, businessUnitId))
    .limit(1);

  return result[0] || null;
}

export async function updateEmployees(
  businessUnitId: number,
  data: Partial<InsertEmployee>
): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db
    .update(employees)
    .set(data)
    .where(eq(employees.businessUnitId, businessUnitId));
}
// ============================================================================
// INVENTORY OPERATIONS
// ============================================================================
export async function getInventoryByUnit(businessUnitId: number) {
  const db = await getDb();
  if (!db) return [];

  return await db
    .select({
      inventory: inventory,
      resourceType: resourceTypes,
    })
    .from(inventory)
    .leftJoin(resourceTypes, eq(inventory.resourceTypeId, resourceTypes.id))
    .where(eq(inventory.businessUnitId, businessUnitId));
}

export async function upsertInventory(data: InsertInventory): Promise<void> {
  const db = await getDb();
  if (!db) return;

  const existing = await db
    .select()
    .from(inventory)
    .where(
      and(
        eq(inventory.businessUnitId, data.businessUnitId),
        eq(inventory.resourceTypeId, data.resourceTypeId)
      )
    )
    .limit(1);

  if (existing.length > 0) {
    await db
      .update(inventory)
      .set({
        quantity: data.quantity,
        quality: data.quality,
        averageCost: data.averageCost,
      })
      .where(eq(inventory.id, existing[0].id));
  } else {
    await db.insert(inventory).values(data);
  }
}
// ============================================================================
// MARKET OPERATIONS
// ============================================================================
export async function createMarketListing(
  data: InsertMarketListing
): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db.insert(marketListings).values(data);
}

export async function getMarketListings(
  resourceTypeId?: number,
  cityId?: number
) {
  const db = await getDb();
  if (!db) return [];

  let query = db
    .select({
      listing: marketListings,
      company: companies,
      resourceType: resourceTypes,
      city: cities,
    })
    .from(marketListings)
    .leftJoin(companies, eq(marketListings.companyId, companies.id))
    .leftJoin(resourceTypes, eq(marketListings.resourceTypeId, resourceTypes.id))
    .leftJoin(cities, eq(marketListings.cityId, cities.id))
    .where(eq(marketListings.isActive, true));

  return await query;
}
// ============================================================================
// TRANSACTION OPERATIONS
// ============================================================================
export async function createTransaction(data: InsertTransaction): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db.insert(transactions).values(data);
}

export async function getTransactionsByCompany(
  companyId: number,
  limit = 50
) {
  const db = await getDb();
  if (!db) return [];

  return await db
    .select()
    .from(transactions)
    .where(eq(transactions.companyId, companyId))
    .orderBy(desc(transactions.createdAt))
    .limit(limit);
}
// ============================================================================
// NOTIFICATION OPERATIONS
// ============================================================================
export async function createNotification(data: InsertNotification): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db.insert(notifications).values(data);
}

export async function getNotificationsByUser(userId: number, limit = 20) {
  const db = await getDb();
  if (!db) return [];

  return await db
    .select()
    .from(notifications)
    .where(eq(notifications.userId, userId))
    .orderBy(desc(notifications.createdAt))
    .limit(limit);
}

export async function markNotificationRead(id: number): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db
    .update(notifications)
    .set({ isRead: true })
    .where(eq(notifications.id, id));
}
// ============================================================================
// GAME STATE OPERATIONS
// ============================================================================
export async function getGameState() {
  const db = await getDb();
  if (!db) return null;

  const result = await db.select().from(gameState).limit(1);
  
  if (result.length === 0) {
    // Initialize game state
    await db.insert(gameState).values({
      currentTurn: 1,
      turnDuration: 3600,
    });
    const newState = await db.select().from(gameState).limit(1);
    return newState[0] || null;
  }
  
  return result[0];
}

export async function incrementGameTurn(): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db
    .update(gameState)
    .set({
      currentTurn: sql`${gameState.currentTurn} + 1`,
      lastTurnProcessed: new Date(),
    });
}

// ============================================================================
// TECHNOLOGY RESEARCH OPERATIONS
// ============================================================================
export async function getAllTechnologies() {
  const db = await getDb();
  if (!db) return [];

  return await db.select().from(technologies).orderBy(technologies.category, technologies.name);
}

export async function getTechnologyById(id: number) {
  const db = await getDb();
  if (!db) return null;

  const result = await db.select().from(technologies).where(eq(technologies.id, id)).limit(1);
  return result[0] || null;
}

export async function getCompanyTechnologies(companyId: number) {
  const db = await getDb();
  if (!db) return [];

  return await db
    .select({
      companyTech: companyTechnologies,
      technology: technologies,
    })
    .from(companyTechnologies)
    .leftJoin(technologies, eq(companyTechnologies.technologyId, technologies.id))
    .where(eq(companyTechnologies.companyId, companyId));
}

export async function startTechnologyResearch(companyId: number, technologyId: number) {
  const db = await getDb();
  if (!db) return null;

  // Check if already researching
  const existing = await db
    .select()
    .from(companyTechnologies)
    .where(and(
      eq(companyTechnologies.companyId, companyId),
      eq(companyTechnologies.technologyId, technologyId)
    ))
    .limit(1);

  if (existing.length > 0) {
    return existing[0];
  }

  const result = await db.insert(companyTechnologies).values({
    companyId,
    technologyId,
    researchProgress: 0,
    isCompleted: false,
  });

  const created = await db
    .select()
    .from(companyTechnologies)
    .where(eq(companyTechnologies.id, result[0].insertId))
    .limit(1);

  return created[0] || null;
}

export async function updateTechnologyResearch(
  companyId: number,
  technologyId: number,
  progress: number
) {
  const db = await getDb();
  if (!db) return;

  const isComplete = progress >= 100;

  await db
    .update(companyTechnologies)
    .set({
      researchProgress: Math.min(100, progress),
      isCompleted: isComplete,
      completedAt: isComplete ? new Date() : null,
    })
    .where(and(
      eq(companyTechnologies.companyId, companyId),
      eq(companyTechnologies.technologyId, technologyId)
    ));
}

export async function hasCompanyResearchedTech(companyId: number, technologyId: number): Promise<boolean> {
  const db = await getDb();
  if (!db) return false;

  const result = await db
    .select()
    .from(companyTechnologies)
    .where(and(
      eq(companyTechnologies.companyId, companyId),
      eq(companyTechnologies.technologyId, technologyId),
      eq(companyTechnologies.isCompleted, true)
    ))
    .limit(1);

  return result.length > 0;
}

// ============================================================================
// PRODUCTION COMPLETION OPERATIONS
// ============================================================================
export async function completeProductionItem(queueItemId: number) {
  const db = await getDb();
  if (!db) return null;

  // Get the queue item with recipe
  const [queueItem] = await db
    .select({
      queue: productionQueue,
      recipe: productionRecipes,
    })
    .from(productionQueue)
    .leftJoin(productionRecipes, eq(productionQueue.recipeId, productionRecipes.id))
    .where(eq(productionQueue.id, queueItemId))
    .limit(1);

  if (!queueItem || !queueItem.recipe) return null;

  // Add output to inventory
  const outputQuantity = parseFloat(queueItem.queue.quantity) * parseFloat(queueItem.recipe.outputQuantity);
  await upsertInventory({
    businessUnitId: queueItem.queue.businessUnitId,
    resourceTypeId: queueItem.recipe.outputResourceId,
    quantity: outputQuantity.toFixed(4),
  });

  // Remove from queue
  await db.delete(productionQueue).where(eq(productionQueue.id, queueItemId));

  return {
    businessUnitId: queueItem.queue.businessUnitId,
    outputResourceId: queueItem.recipe.outputResourceId,
    outputQuantity: parseFloat(queueItem.queue.quantity) * parseFloat(queueItem.recipe.outputQuantity),
  };
}

export async function getReadyProductionItems() {
  const db = await getDb();
  if (!db) return [];

  // Get items that have been in queue long enough (based on recipe production time)
  // For simplicity, consider items older than 1 hour as ready
  const oneHourAgo = new Date(Date.now() - 60 * 60 * 1000);

  return await db
    .select({
      queue: productionQueue,
      recipe: productionRecipes,
    })
    .from(productionQueue)
    .leftJoin(productionRecipes, eq(productionQueue.recipeId, productionRecipes.id))
    .where(sql`${productionQueue.createdAt} < ${oneHourAgo}`);
}

// ============================================================================
// EMPLOYEE PAYROLL OPERATIONS
// ============================================================================
export async function processCompanyPayroll(companyId: number): Promise<{ totalPayroll: number; employeeCount: number }> {
  const db = await getDb();
  if (!db) return { totalPayroll: 0, employeeCount: 0 };

  // Get company's business units
  const units = await getBusinessUnitsByCompany(companyId);

  let totalPayroll = 0;
  let employeeCount = 0;

  for (const unit of units) {
    const employeeData = await getEmployeesByUnit(unit.id);
    if (employeeData) {
      const salary = parseFloat(employeeData.salary) * employeeData.count;
      totalPayroll += salary;
      employeeCount += employeeData.count;
    }
  }

  // Deduct from company cash
  const company = await getCompanyById(companyId);
  if (company) {
    const newCash = parseFloat(company.cash) - totalPayroll;
    await updateCompanyCash(companyId, newCash.toFixed(2));

    // Create transaction record
    await createTransaction({
      companyId,
      type: "salary",
      amount: totalPayroll.toFixed(2),
      description: `Payroll for ${employeeCount} employees`,
    });
  }

  return { totalPayroll, employeeCount };
}

export async function processAllCompaniesPayroll(): Promise<Array<{ companyId: number; totalPayroll: number; employeeCount: number }>> {
  const db = await getDb();
  if (!db) return [];

  const allCompanies = await getAllCompanies();
  const results: Array<{ companyId: number; totalPayroll: number; employeeCount: number }> = [];

  for (const company of allCompanies) {
    const result = await processCompanyPayroll(company.id);
    results.push({ companyId: company.id, ...result });
  }

  return results;
}

// ============================================================================
// GAME TURN PROCESSING
// ============================================================================
export async function processTurnAdvancement(): Promise<{
  turn: number;
  productionCompleted: number;
  payrollProcessed: Array<{ companyId: number; totalPayroll: number; employeeCount: number }>;
  researchAdvanced: number;
}> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  // 1. Increment turn
  await incrementGameTurn();
  const state = await getGameState();
  const turn = state?.currentTurn || 1;

  // 2. Process completed production
  const readyItems = await getReadyProductionItems();
  let productionCompleted = 0;
  for (const item of readyItems) {
    await completeProductionItem(item.queue.id);
    productionCompleted++;
  }

  // 3. Process payroll
  const payrollProcessed = await processAllCompaniesPayroll();

  // 4. Advance research progress
  let researchAdvanced = 0;
  const allCompanies = await getAllCompanies();
  for (const company of allCompanies) {
    const companyTechs = await getCompanyTechnologies(company.id);
    for (const tech of companyTechs) {
      if (!tech.companyTech.isCompleted) {
        const currentProgress = tech.companyTech.researchProgress;
        // Advance by 10% per turn (can be modified by research speed bonuses)
        const newProgress = currentProgress + 10;
        await updateTechnologyResearch(company.id, tech.companyTech.technologyId, newProgress);
        researchAdvanced++;
      }
    }
  }

  return {
    turn,
    productionCompleted,
    payrollProcessed,
    researchAdvanced,
  };
}
// ============================================================================
// INITIALIZATION
// ============================================================================
export async function initializeGameData(): Promise<void> {
  await seedCities();
  await seedResourceTypes();
  await seedAgenticSimulationData(); // Add agentic simulation seed data
  await getGameState(); // Ensures game state exists
}
// ============================================================================
// PRODUCTION OPERATIONS
// ============================================================================
export async function getProductionRecipes(unitType?: "factory" | "farm" | "mine" | "laboratory") {
  const db = await getDb();
  if (!db) return [];

  if (unitType) {
    return await db
      .select()
      .from(productionRecipes)
      .where(eq(productionRecipes.unitType, unitType));
  }

  return await db.select().from(productionRecipes);
}

export async function getProductionRecipeById(id: number) {
  const db = await getDb();
  if (!db) return null;

  const result = await db
    .select()
    .from(productionRecipes)
    .where(eq(productionRecipes.id, id))
    .limit(1);

  return result[0] || null;
}

export async function addToProductionQueue(data: {
  businessUnitId: number;
  recipeId: number;
  quantity: number;
}) {
  const db = await getDb();
  if (!db) return;

  await db.insert(productionQueue).values({
    businessUnitId: data.businessUnitId,
    recipeId: data.recipeId,
    quantity: data.quantity.toFixed(4),
  });
}

export async function getProductionQueue(businessUnitId: number) {
  const db = await getDb();
  if (!db) return [];

  return await db
    .select({
      queue: productionQueue,
      recipe: productionRecipes,
    })
    .from(productionQueue)
    .leftJoin(productionRecipes, eq(productionQueue.recipeId, productionRecipes.id))
    .where(eq(productionQueue.businessUnitId, businessUnitId))
    .orderBy(productionQueue.createdAt);
}
// ============================================================================
// MARKET TRADING OPERATIONS
// ============================================================================
export async function purchaseFromMarket(
  buyerCompanyId: number,
  listingId: number,
  quantity: number,
  buyerUnitId: number
): Promise<{ success: boolean; message: string }> {
  const db = await getDb();
  if (!db) return { success: false, message: "Database not available" };

  // Get the listing
  const listingResult = await db
    .select()
    .from(marketListings)
    .where(eq(marketListings.id, listingId))
    .limit(1);

  if (listingResult.length === 0) {
    return { success: false, message: "Listing not found" };
  }

  const listing = listingResult[0];

  if (!listing.isActive) {
    return { success: false, message: "Listing is no longer active" };
  }

  const availableQty = parseFloat(listing.quantity);
  if (quantity > availableQty) {
    return { success: false, message: "Insufficient quantity available" };
  }

  const totalCost = quantity * parseFloat(listing.pricePerUnit);

  // Get buyer company
  const buyerResult = await db
    .select()
    .from(companies)
    .where(eq(companies.id, buyerCompanyId))
    .limit(1);

  if (buyerResult.length === 0) {
    return { success: false, message: "Buyer company not found" };
  }

  const buyer = buyerResult[0];

  if (parseFloat(buyer.cash) < totalCost) {
    return { success: false, message: "Insufficient funds" };
  }

  // Get seller company
  const sellerResult = await db
    .select()
    .from(companies)
    .where(eq(companies.id, listing.companyId))
    .limit(1);

  if (sellerResult.length === 0) {
    return { success: false, message: "Seller company not found" };
  }

  const seller = sellerResult[0];

  // Execute the trade
  // 1. Deduct cash from buyer
  await db
    .update(companies)
    .set({ cash: (parseFloat(buyer.cash) - totalCost).toFixed(2) })
    .where(eq(companies.id, buyerCompanyId));

  // 2. Add cash to seller
  await db
    .update(companies)
    .set({ cash: (parseFloat(seller.cash) + totalCost).toFixed(2) })
    .where(eq(companies.id, listing.companyId));

  // 3. Update or remove listing
  const newQty = availableQty - quantity;
  if (newQty <= 0) {
    await db
      .update(marketListings)
      .set({ isActive: false, quantity: "0" })
      .where(eq(marketListings.id, listingId));
  } else {
    await db
      .update(marketListings)
      .set({ quantity: newQty.toFixed(4) })
      .where(eq(marketListings.id, listingId));
  }

  // 4. Add inventory to buyer's unit
  await upsertInventory({
    businessUnitId: buyerUnitId,
    resourceTypeId: listing.resourceTypeId,
    quantity: quantity.toFixed(4),
    quality: listing.quality,
  });

  // 5. Record transactions
  await createTransaction({
    type: "purchase",
    companyId: buyerCompanyId,
    amount: (-totalCost).toFixed(2),
    description: `Purchased ${quantity} units from market`,
    relatedUnitId: buyerUnitId,
  });

  await createTransaction({
    type: "sale",
    companyId: listing.companyId,
    amount: totalCost.toFixed(2),
    description: `Sold ${quantity} units on market`,
    relatedUnitId: listing.businessUnitId,
  });

  return { success: true, message: "Purchase completed successfully" };
}

export async function getMarketListingById(id: number) {
  const db = await getDb();
  if (!db) return null;

  const result = await db
    .select({
      listing: marketListings,
      company: companies,
      resourceType: resourceTypes,
      city: cities,
    })
    .from(marketListings)
    .leftJoin(companies, eq(marketListings.companyId, companies.id))
    .leftJoin(resourceTypes, eq(marketListings.resourceTypeId, resourceTypes.id))
    .leftJoin(cities, eq(marketListings.cityId, cities.id))
    .where(eq(marketListings.id, id))
    .limit(1);

  return result[0] || null;
}

export async function cancelMarketListing(listingId: number, companyId: number): Promise<{ success: boolean; message: string }> {
  const db = await getDb();
  if (!db) return { success: false, message: "Database not available" };

  const listing = await db
    .select()
    .from(marketListings)
    .where(eq(marketListings.id, listingId))
    .limit(1);

  if (listing.length === 0) {
    return { success: false, message: "Listing not found" };
  }

  if (listing[0].companyId !== companyId) {
    return { success: false, message: "Not authorized" };
  }

  // Return inventory to the unit
  await upsertInventory({
    businessUnitId: listing[0].businessUnitId,
    resourceTypeId: listing[0].resourceTypeId,
    quantity: listing[0].quantity,
    quality: listing[0].quality,
  });

  // Deactivate listing
  await db
    .update(marketListings)
    .set({ isActive: false })
    .where(eq(marketListings.id, listingId));

  return { success: true, message: "Listing cancelled" };
}
// ============================================================================
// SEED PRODUCTION RECIPES
// ============================================================================
export async function seedProductionRecipes(): Promise<void> {
  const db = await getDb();
  if (!db) return;

  // Check if recipes already exist
  const existing = await db.select().from(productionRecipes).limit(1);
  if (existing.length > 0) return;

  // Get resource type IDs
  const resources = await db.select().from(resourceTypes);
  const resourceMap = new Map(resources.map((r) => [r.name, r.id]));

  type UnitType = "factory" | "farm" | "mine" | "laboratory";

  const recipes: Array<{
    name: string;
    unitType: UnitType;
    outputResourceId: number | undefined;
    outputQuantity: string;
    inputResources: { resourceId: number | undefined; quantity: number }[];
    productionTime: number;
    requiredEmployees: number;
  }> = [
    // Mining recipes
    {
      name: "Iron Ore Extraction",
      unitType: "mine" as UnitType,
      outputResourceId: resourceMap.get("Iron Ore"),
      outputQuantity: "100.0000",
      inputResources: [],
      productionTime: 3600,
      requiredEmployees: 10,
    },
    {
      name: "Coal Mining",
      unitType: "mine" as UnitType,
      outputResourceId: resourceMap.get("Coal"),
      outputQuantity: "150.0000",
      inputResources: [],
      productionTime: 3600,
      requiredEmployees: 8,
    },
    {
      name: "Bauxite Mining",
      unitType: "mine" as UnitType,
      outputResourceId: resourceMap.get("Bauxite"),
      outputQuantity: "80.0000",
      inputResources: [],
      productionTime: 3600,
      requiredEmployees: 12,
    },
    // Farm recipes
    {
      name: "Wheat Farming",
      unitType: "farm" as UnitType,
      outputResourceId: resourceMap.get("Wheat"),
      outputQuantity: "200.0000",
      inputResources: [],
      productionTime: 7200,
      requiredEmployees: 5,
    },
    {
      name: "Cotton Farming",
      unitType: "farm" as UnitType,
      outputResourceId: resourceMap.get("Cotton"),
      outputQuantity: "100.0000",
      inputResources: [],
      productionTime: 7200,
      requiredEmployees: 6,
    },
    {
      name: "Cattle Ranching",
      unitType: "farm" as UnitType,
      outputResourceId: resourceMap.get("Cattle"),
      outputQuantity: "50.0000",
      inputResources: [{ resourceId: resourceMap.get("Wheat"), quantity: 100 }],
      productionTime: 14400,
      requiredEmployees: 8,
    },
    // Factory recipes
    {
      name: "Steel Production",
      unitType: "factory" as UnitType,
      outputResourceId: resourceMap.get("Steel"),
      outputQuantity: "50.0000",
      inputResources: [
        { resourceId: resourceMap.get("Iron Ore"), quantity: 100 },
        { resourceId: resourceMap.get("Coal"), quantity: 50 },
      ],
      productionTime: 7200,
      requiredEmployees: 15,
    },
    {
      name: "Aluminum Production",
      unitType: "factory" as UnitType,
      outputResourceId: resourceMap.get("Aluminum"),
      outputQuantity: "40.0000",
      inputResources: [
        { resourceId: resourceMap.get("Bauxite"), quantity: 80 },
      ],
      productionTime: 7200,
      requiredEmployees: 12,
    },
    {
      name: "Textile Production",
      unitType: "factory" as UnitType,
      outputResourceId: resourceMap.get("Textiles"),
      outputQuantity: "100.0000",
      inputResources: [
        { resourceId: resourceMap.get("Cotton"), quantity: 50 },
      ],
      productionTime: 3600,
      requiredEmployees: 20,
    },
    {
      name: "Electronics Assembly",
      unitType: "factory" as UnitType,
      outputResourceId: resourceMap.get("Electronics"),
      outputQuantity: "20.0000",
      inputResources: [
        { resourceId: resourceMap.get("Steel"), quantity: 10 },
        { resourceId: resourceMap.get("Aluminum"), quantity: 5 },
      ],
      productionTime: 7200,
      requiredEmployees: 25,
    },
    {
      name: "Automobile Manufacturing",
      unitType: "factory" as UnitType,
      outputResourceId: resourceMap.get("Automobiles"),
      outputQuantity: "5.0000",
      inputResources: [
        { resourceId: resourceMap.get("Steel"), quantity: 50 },
        { resourceId: resourceMap.get("Aluminum"), quantity: 20 },
        { resourceId: resourceMap.get("Electronics"), quantity: 10 },
        { resourceId: resourceMap.get("Textiles"), quantity: 15 },
      ],
      productionTime: 14400,
      requiredEmployees: 50,
    },
    {
      name: "Furniture Production",
      unitType: "factory" as UnitType,
      outputResourceId: resourceMap.get("Furniture"),
      outputQuantity: "30.0000",
      inputResources: [
        { resourceId: resourceMap.get("Timber"), quantity: 40 },
        { resourceId: resourceMap.get("Textiles"), quantity: 10 },
      ],
      productionTime: 5400,
      requiredEmployees: 15,
    },
    {
      name: "Clothing Production",
      unitType: "factory" as UnitType,
      outputResourceId: resourceMap.get("Clothing"),
      outputQuantity: "100.0000",
      inputResources: [
        { resourceId: resourceMap.get("Textiles"), quantity: 30 },
      ],
      productionTime: 3600,
      requiredEmployees: 30,
    },
    {
      name: "Food Processing",
      unitType: "factory" as UnitType,
      outputResourceId: resourceMap.get("Processed Food"),
      outputQuantity: "150.0000",
      inputResources: [
        { resourceId: resourceMap.get("Wheat"), quantity: 100 },
        { resourceId: resourceMap.get("Cattle"), quantity: 20 },
      ],
      productionTime: 3600,
      requiredEmployees: 20,
    },
  ];

  for (const recipe of recipes) {
    if (recipe.outputResourceId) {
      await db.insert(productionRecipes).values({
        unitType: recipe.unitType,
        outputResourceId: recipe.outputResourceId,
        outputQuantity: recipe.outputQuantity,
        inputResources: recipe.inputResources.filter(i => i.resourceId !== undefined) as { resourceId: number; quantity: number }[],
        laborRequired: recipe.requiredEmployees,
        timeRequired: Math.floor(recipe.productionTime / 3600),
        description: recipe.name,
      });
    }
  }
}
// ============================================================================
// AGENTIC SIMULATION OPERATIONS
// ============================================================================

// ============================================================================
// CHARACTER PERSONAS
// ============================================================================
export async function createCharacterPersona(
  persona: InsertCharacterPersona
): Promise<CharacterPersona | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db.insert(characterPersonas).values(persona);
  return await getCharacterPersonaById(Number(result[0].insertId));
}

export async function getCharacterPersonaById(id: number): Promise<CharacterPersona | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db
    .select()
    .from(characterPersonas)
    .where(eq(characterPersonas.id, id))
    .limit(1);
  return result[0] || null;
}

export async function getCharacterPersonaByCode(code: string): Promise<CharacterPersona | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db
    .select()
    .from(characterPersonas)
    .where(eq(characterPersonas.code, code))
    .limit(1);
  return result[0] || null;
}

export async function getAllCharacterPersonas(): Promise<CharacterPersona[]> {
  const db = await getDb();
  if (!db) return [];

  return await db.select().from(characterPersonas);
}
// ============================================================================
// CHARACTER TRAITS
// ============================================================================
export async function createCharacterTrait(
  trait: InsertCharacterTrait
): Promise<CharacterTrait | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db.insert(characterTraits).values(trait);
  return await getCharacterTraitById(Number(result[0].insertId));
}

export async function getCharacterTraitById(id: number): Promise<CharacterTrait | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db
    .select()
    .from(characterTraits)
    .where(eq(characterTraits.id, id))
    .limit(1);
  return result[0] || null;
}

export async function getAllCharacterTraits(): Promise<CharacterTrait[]> {
  const db = await getDb();
  if (!db) return [];

  return await db.select().from(characterTraits);
}

export async function getCharacterTraitsByCategory(
  category: "professional" | "social" | "cognitive" | "emotional"
): Promise<CharacterTrait[]> {
  const db = await getDb();
  if (!db) return [];

  return await db
    .select()
    .from(characterTraits)
    .where(eq(characterTraits.category, category));
}
// ============================================================================
// AGENTS
// ============================================================================
export async function createAgent(agent: InsertAgent): Promise<Agent | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db.insert(agents).values(agent);
  return await getAgentById(Number(result[0].insertId));
}

export async function getAgentById(id: number): Promise<Agent | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db
    .select()
    .from(agents)
    .where(eq(agents.id, id))
    .limit(1);
  return result[0] || null;
}

export async function getAgentsByType(
  type: "customer" | "supplier" | "employee" | "partner" | "investor" | "competitor"
): Promise<Agent[]> {
  const db = await getDb();
  if (!db) return [];

  return await db.select().from(agents).where(eq(agents.type, type));
}

export async function getAgentsByCompany(companyId: number): Promise<Agent[]> {
  const db = await getDb();
  if (!db) return [];

  return await db.select().from(agents).where(eq(agents.companyId, companyId));
}

export async function getAgentsByBusinessUnit(businessUnitId: number): Promise<Agent[]> {
  const db = await getDb();
  if (!db) return [];

  return await db
    .select()
    .from(agents)
    .where(eq(agents.businessUnitId, businessUnitId));
}

export async function getAgentsByCity(cityId: number): Promise<Agent[]> {
  const db = await getDb();
  if (!db) return [];

  return await db.select().from(agents).where(eq(agents.cityId, cityId));
}

export async function updateAgent(
  id: number,
  updates: Partial<InsertAgent>
): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db.update(agents).set(updates).where(eq(agents.id, id));
}

export async function updateAgentEmotionalState(
  id: number,
  emotions: {
    happiness?: number;
    satisfaction?: number;
    stress?: number;
    loyalty?: number;
    trust?: number;
  }
): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db.update(agents).set(emotions).where(eq(agents.id, id));
}
// ============================================================================
// AGENT TRAITS
// ============================================================================
export async function addTraitToAgent(
  agentId: number,
  traitId: number,
  intensity: number = 50
): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db.insert(agentTraits).values({
    agentId,
    traitId,
    intensity,
  });
}

export async function getAgentTraits(agentId: number): Promise<
  Array<{
    agentTrait: typeof agentTraits.$inferSelect;
    trait: CharacterTrait;
  }>
> {
  const db = await getDb();
  if (!db) return [];

  const result = await db
    .select()
    .from(agentTraits)
    .leftJoin(characterTraits, eq(agentTraits.traitId, characterTraits.id))
    .where(eq(agentTraits.agentId, agentId));

  return result.map((r) => ({
    agentTrait: r.agent_traits,
    trait: r.character_traits!,
  }));
}

export async function removeTraitFromAgent(
  agentId: number,
  traitId: number
): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db
    .delete(agentTraits)
    .where(and(eq(agentTraits.agentId, agentId), eq(agentTraits.traitId, traitId)));
}
// ============================================================================
// RELATIONSHIPS
// ============================================================================
export async function createRelationship(
  relationship: InsertRelationship
): Promise<Relationship | null> {
  const db = await getDb();
  if (!db) return null;

  // Ensure agent1Id < agent2Id for consistency
  const agent1Id = Math.min(relationship.agent1Id, relationship.agent2Id);
  const agent2Id = Math.max(relationship.agent1Id, relationship.agent2Id);

  const result = await db.insert(relationships).values({
    ...relationship,
    agent1Id,
    agent2Id,
  });
  return await getRelationshipById(Number(result[0].insertId));
}

export async function getRelationshipById(id: number): Promise<Relationship | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db
    .select()
    .from(relationships)
    .where(eq(relationships.id, id))
    .limit(1);
  return result[0] || null;
}

export async function getRelationshipBetweenAgents(
  agent1Id: number,
  agent2Id: number
): Promise<Relationship | null> {
  const db = await getDb();
  if (!db) return null;

  const minId = Math.min(agent1Id, agent2Id);
  const maxId = Math.max(agent1Id, agent2Id);

  const result = await db
    .select()
    .from(relationships)
    .where(
      and(
        eq(relationships.agent1Id, minId),
        eq(relationships.agent2Id, maxId)
      )
    )
    .limit(1);
  return result[0] || null;
}

export async function getAgentRelationships(agentId: number): Promise<Relationship[]> {
  const db = await getDb();
  if (!db) return [];

  return await db
    .select()
    .from(relationships)
    .where(
      sql`${relationships.agent1Id} = ${agentId} OR ${relationships.agent2Id} = ${agentId}`
    );
}

export async function updateRelationship(
  id: number,
  updates: Partial<InsertRelationship>
): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db.update(relationships).set(updates).where(eq(relationships.id, id));
}

export async function recordRelationshipInteraction(
  agent1Id: number,
  agent2Id: number,
  strengthChange: number = 0,
  positivityChange: number = 0
): Promise<void> {
  const db = await getDb();
  if (!db) return;

  const relationship = await getRelationshipBetweenAgents(agent1Id, agent2Id);
  
  if (relationship) {
    const newStrength = Math.max(0, Math.min(100, relationship.strength + strengthChange));
    const newPositivity = Math.max(0, Math.min(100, relationship.positivity + positivityChange));
    
    await updateRelationship(relationship.id, {
      strength: newStrength,
      positivity: newPositivity,
      interactionCount: relationship.interactionCount + 1,
      lastInteraction: new Date(),
      frequency: Math.min(100, relationship.frequency + 1),
    });
  }
}
// ============================================================================
// AGENT GROUPS
// ============================================================================
export async function createAgentGroup(group: InsertAgentGroup): Promise<AgentGroup | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db.insert(agentGroups).values(group);
  return await getAgentGroupById(Number(result[0].insertId));
}

export async function getAgentGroupById(id: number): Promise<AgentGroup | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db
    .select()
    .from(agentGroups)
    .where(eq(agentGroups.id, id))
    .limit(1);
  return result[0] || null;
}

export async function getAgentGroupsByCompany(companyId: number): Promise<AgentGroup[]> {
  const db = await getDb();
  if (!db) return [];

  return await db
    .select()
    .from(agentGroups)
    .where(eq(agentGroups.companyId, companyId));
}

export async function getAgentGroupsByCity(cityId: number): Promise<AgentGroup[]> {
  const db = await getDb();
  if (!db) return [];

  return await db.select().from(agentGroups).where(eq(agentGroups.cityId, cityId));
}

export async function updateAgentGroup(
  id: number,
  updates: Partial<InsertAgentGroup>
): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db.update(agentGroups).set(updates).where(eq(agentGroups.id, id));
}
// ============================================================================
// GROUP MEMBERSHIPS
// ============================================================================
export async function addAgentToGroup(
  membership: InsertGroupMembership
): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db.insert(groupMemberships).values(membership);
}

export async function getGroupMembers(groupId: number): Promise<
  Array<{
    membership: typeof groupMemberships.$inferSelect;
    agent: Agent;
  }>
> {
  const db = await getDb();
  if (!db) return [];

  const result = await db
    .select()
    .from(groupMemberships)
    .leftJoin(agents, eq(groupMemberships.agentId, agents.id))
    .where(eq(groupMemberships.groupId, groupId));

  return result.map((r) => ({
    membership: r.group_memberships,
    agent: r.agents!,
  }));
}

export async function getAgentGroups(agentId: number): Promise<
  Array<{
    membership: typeof groupMemberships.$inferSelect;
    group: AgentGroup;
  }>
> {
  const db = await getDb();
  if (!db) return [];

  const result = await db
    .select()
    .from(groupMemberships)
    .leftJoin(agentGroups, eq(groupMemberships.groupId, agentGroups.id))
    .where(eq(groupMemberships.agentId, agentId));

  return result.map((r) => ({
    membership: r.group_memberships,
    group: r.agent_groups!,
  }));
}

export async function removeAgentFromGroup(
  groupId: number,
  agentId: number
): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db
    .update(groupMemberships)
    .set({ leftAt: new Date() })
    .where(
      and(
        eq(groupMemberships.groupId, groupId),
        eq(groupMemberships.agentId, agentId)
      )
    );
}
// ============================================================================
// COMMUNITIES
// ============================================================================
export async function createCommunity(
  community: InsertCommunity
): Promise<Community | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db.insert(communities).values(community);
  return await getCommunityById(Number(result[0].insertId));
}

export async function getCommunityById(id: number): Promise<Community | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db
    .select()
    .from(communities)
    .where(eq(communities.id, id))
    .limit(1);
  return result[0] || null;
}

export async function getCommunitiesByCity(cityId: number): Promise<Community[]> {
  const db = await getDb();
  if (!db) return [];

  return await db.select().from(communities).where(eq(communities.cityId, cityId));
}

export async function updateCommunity(
  id: number,
  updates: Partial<InsertCommunity>
): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db.update(communities).set(updates).where(eq(communities.id, id));
}
// ============================================================================
// COMMUNITY MEMBERSHIPS
// ============================================================================
export async function addAgentToCommunity(
  membership: InsertCommunityMembership
): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db.insert(communityMemberships).values(membership);
}

export async function getCommunityMembers(communityId: number): Promise<
  Array<{
    membership: typeof communityMemberships.$inferSelect;
    agent: Agent;
  }>
> {
  const db = await getDb();
  if (!db) return [];

  const result = await db
    .select()
    .from(communityMemberships)
    .leftJoin(agents, eq(communityMemberships.agentId, agents.id))
    .where(eq(communityMemberships.communityId, communityId));

  return result.map((r) => ({
    membership: r.community_memberships,
    agent: r.agents!,
  }));
}

export async function getAgentCommunities(agentId: number): Promise<
  Array<{
    membership: typeof communityMemberships.$inferSelect;
    community: Community;
  }>
> {
  const db = await getDb();
  if (!db) return [];

  const result = await db
    .select()
    .from(communityMemberships)
    .leftJoin(communities, eq(communityMemberships.communityId, communities.id))
    .where(eq(communityMemberships.agentId, agentId));

  return result.map((r) => ({
    membership: r.community_memberships,
    community: r.communities!,
  }));
}
// ============================================================================
// AGENT EVENTS
// ============================================================================
export async function createAgentEvent(
  event: InsertAgentEvent
): Promise<AgentEvent | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db.insert(agentEvents).values(event);
  return await getAgentEventById(Number(result[0].insertId));
}

export async function getAgentEventById(id: number): Promise<AgentEvent | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db
    .select()
    .from(agentEvents)
    .where(eq(agentEvents.id, id))
    .limit(1);
  return result[0] || null;
}

export async function getAgentEvents(agentId: number): Promise<AgentEvent[]> {
  const db = await getDb();
  if (!db) return [];

  return await db
    .select()
    .from(agentEvents)
    .where(
      sql`${agentEvents.initiatorAgentId} = ${agentId} OR ${agentEvents.targetAgentId} = ${agentId}`
    )
    .orderBy(desc(agentEvents.scheduledAt));
}

export async function getScheduledEvents(): Promise<AgentEvent[]> {
  const db = await getDb();
  if (!db) return [];

  return await db
    .select()
    .from(agentEvents)
    .where(eq(agentEvents.status, "scheduled"))
    .orderBy(agentEvents.scheduledAt);
}

export async function updateAgentEvent(
  id: number,
  updates: Partial<InsertAgentEvent>
): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db.update(agentEvents).set(updates).where(eq(agentEvents.id, id));
}

export async function processAgentEvent(eventId: number): Promise<void> {
  const db = await getDb();
  if (!db) return;

  const event = await getAgentEventById(eventId);
  if (!event) return;

  // Update event status
  await updateAgentEvent(eventId, {
    status: "completed",
    occurredAt: new Date(),
  });

  // Apply emotional impacts to initiator
  if (event.emotionalImpact) {
    const initiator = await getAgentById(event.initiatorAgentId);
    if (initiator) {
      const emotionUpdates: Record<string, number> = {};
      
      Object.entries(event.emotionalImpact).forEach(([key, value]) => {
        if (value !== undefined) {
          const currentValue = initiator[key as keyof Agent] as number;
          emotionUpdates[key] = Math.max(0, Math.min(100, currentValue + value));
        }
      });
      
      if (Object.keys(emotionUpdates).length > 0) {
        await updateAgentEmotionalState(event.initiatorAgentId, emotionUpdates);
      }
    }
  }

  // Apply emotional impacts to target if present
  if (event.targetAgentId && event.emotionalImpact) {
    const target = await getAgentById(event.targetAgentId);
    if (target) {
      const emotionUpdates: Record<string, number> = {};
      
      Object.entries(event.emotionalImpact).forEach(([key, value]) => {
        if (value !== undefined) {
          const currentValue = target[key as keyof Agent] as number;
          emotionUpdates[key] = Math.max(0, Math.min(100, currentValue + value));
        }
      });
      
      if (Object.keys(emotionUpdates).length > 0) {
        await updateAgentEmotionalState(event.targetAgentId, emotionUpdates);
      }
    }
  }

  // Apply relationship impacts
  if (event.relationshipImpact && event.targetAgentId) {
    await recordRelationshipInteraction(
      event.initiatorAgentId,
      event.targetAgentId,
      event.relationshipImpact.strengthChange || 0,
      event.relationshipImpact.positivityChange || 0
    );
  }

  // Record history for affected agents
  const initiator = await getAgentById(event.initiatorAgentId);
  if (initiator) {
    await createAgentHistory({
      agentId: event.initiatorAgentId,
      happiness: initiator.happiness,
      satisfaction: initiator.satisfaction,
      stress: initiator.stress,
      loyalty: initiator.loyalty,
      trust: initiator.trust,
      eventId: eventId,
      notes: `Event: ${event.title}`,
    });
  }
}
// ============================================================================
// EVENT TRIGGERS
// ============================================================================
export async function createEventTrigger(
  trigger: InsertEventTrigger
): Promise<typeof eventTriggers.$inferSelect | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db.insert(eventTriggers).values(trigger);
  return await getEventTriggerById(Number(result[0].insertId));
}

export async function getEventTriggerById(
  id: number
): Promise<typeof eventTriggers.$inferSelect | null> {
  const db = await getDb();
  if (!db) return null;

  const result = await db
    .select()
    .from(eventTriggers)
    .where(eq(eventTriggers.id, id))
    .limit(1);
  return result[0] || null;
}

export async function getActiveEventTriggers(): Promise<
  (typeof eventTriggers.$inferSelect)[]
> {
  const db = await getDb();
  if (!db) return [];

  return await db
    .select()
    .from(eventTriggers)
    .where(eq(eventTriggers.isActive, true))
    .orderBy(desc(eventTriggers.priority));
}
// ============================================================================
// AGENT HISTORIES
// ============================================================================
export async function createAgentHistory(
  history: InsertAgentHistory
): Promise<void> {
  const db = await getDb();
  if (!db) return;

  await db.insert(agentHistories).values(history);
}

export async function getAgentHistory(
  agentId: number,
  limit: number = 50
): Promise<(typeof agentHistories.$inferSelect)[]> {
  const db = await getDb();
  if (!db) return [];

  return await db
    .select()
    .from(agentHistories)
    .where(eq(agentHistories.agentId, agentId))
    .orderBy(desc(agentHistories.recordedAt))
    .limit(limit);
}
// ============================================================================
// SEED AGENTIC SIMULATION DATA
// ============================================================================
export async function seedAgenticSimulationData(): Promise<void> {
  const db = await getDb();
  if (!db) return;

  // Check if data already exists
  const existingPersonas = await db.select().from(characterPersonas).limit(1);
  if (existingPersonas.length > 0) return;

  console.log("[Database] Seeding agentic simulation data...");

  // Seed Character Personas
  const personas = [
    {
      code: "ambitious_leader",
      name: "Ambitious Leader",
      description: "Driven, goal-oriented, seeks growth and success",
      ambitionLevel: 90,
      cautionLevel: 30,
      socialLevel: 70,
      analyticalLevel: 60,
      communicationStyle: "formal" as const,
      decisionSpeed: "quick" as const,
    },
    {
      code: "cautious_analyst",
      name: "Cautious Analyst",
      description: "Careful, data-driven, risk-averse",
      ambitionLevel: 40,
      cautionLevel: 85,
      socialLevel: 40,
      analyticalLevel: 95,
      communicationStyle: "formal" as const,
      decisionSpeed: "deliberate" as const,
    },
    {
      code: "social_connector",
      name: "Social Connector",
      description: "People-focused, relationship-builder, collaborative",
      ambitionLevel: 60,
      cautionLevel: 50,
      socialLevel: 95,
      analyticalLevel: 50,
      communicationStyle: "casual" as const,
      decisionSpeed: "moderate" as const,
    },
    {
      code: "aggressive_competitor",
      name: "Aggressive Competitor",
      description: "Assertive, competitive, direct approach",
      ambitionLevel: 85,
      cautionLevel: 25,
      socialLevel: 50,
      analyticalLevel: 55,
      communicationStyle: "aggressive" as const,
      decisionSpeed: "impulsive" as const,
    },
    {
      code: "diplomatic_mediator",
      name: "Diplomatic Mediator",
      description: "Balanced, conflict-resolver, seeks harmony",
      ambitionLevel: 55,
      cautionLevel: 65,
      socialLevel: 80,
      analyticalLevel: 70,
      communicationStyle: "diplomatic" as const,
      decisionSpeed: "moderate" as const,
    },
  ];

  for (const persona of personas) {
    await db.insert(characterPersonas).values(persona);
  }

  // Seed Character Traits
  const traits = [
    // Professional traits
    { code: "reliable", name: "Reliable", category: "professional" as const, description: "Consistently delivers on commitments", positiveEffect: true },
    { code: "innovative", name: "Innovative", category: "professional" as const, description: "Brings creative solutions", positiveEffect: true },
    { code: "detail_oriented", name: "Detail-Oriented", category: "professional" as const, description: "Pays attention to details", positiveEffect: true },
    { code: "procrastinator", name: "Procrastinator", category: "professional" as const, description: "Tends to delay tasks", positiveEffect: false },
    
    // Social traits
    { code: "charismatic", name: "Charismatic", category: "social" as const, description: "Naturally attracts others", positiveEffect: true },
    { code: "empathetic", name: "Empathetic", category: "social" as const, description: "Understands others' feelings", positiveEffect: true },
    { code: "introverted", name: "Introverted", category: "social" as const, description: "Prefers solitary work", positiveEffect: true },
    { code: "conflict_averse", name: "Conflict-Averse", category: "social" as const, description: "Avoids confrontation", positiveEffect: false },
    
    // Cognitive traits
    { code: "strategic_thinker", name: "Strategic Thinker", category: "cognitive" as const, description: "Sees the big picture", positiveEffect: true },
    { code: "analytical", name: "Analytical", category: "cognitive" as const, description: "Strong logical reasoning", positiveEffect: true },
    { code: "creative", name: "Creative", category: "cognitive" as const, description: "Generates novel ideas", positiveEffect: true },
    { code: "impulsive", name: "Impulsive", category: "cognitive" as const, description: "Acts without planning", positiveEffect: false },
    
    // Emotional traits
    { code: "optimistic", name: "Optimistic", category: "emotional" as const, description: "Maintains positive outlook", positiveEffect: true },
    { code: "resilient", name: "Resilient", category: "emotional" as const, description: "Bounces back from setbacks", positiveEffect: true },
    { code: "anxious", name: "Anxious", category: "emotional" as const, description: "Prone to worry", positiveEffect: false },
    { code: "hot_tempered", name: "Hot-Tempered", category: "emotional" as const, description: "Quick to anger", positiveEffect: false },
  ];

  for (const trait of traits) {
    await db.insert(characterTraits).values(trait);
  }

  console.log("[Database] Agentic simulation data seeded successfully");
}
// ============================================================================
// DREAMCOG INTEGRATION - BIG FIVE PERSONALITY FUNCTIONS
// ============================================================================

export async function createAgentBigFivePersonality(
  data: InsertAgentBigFivePersonality
): Promise<AgentBigFivePersonality> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  const [personality] = await db
    .insert(agentBigFivePersonality)
    .values(data)
    .$returningId();
  
  const created = await db
    .select()
    .from(agentBigFivePersonality)
    .where(eq(agentBigFivePersonality.id, personality.id))
    .limit(1);
  
  return created[0];


}
export async function getAgentBigFivePersonality(
  agentId: number
): Promise<AgentBigFivePersonality | null> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  const [personality] = await db
    .select()
    .from(agentBigFivePersonality)
    .where(eq(agentBigFivePersonality.agentId, agentId))
    .limit(1);
  
  return personality || null;


}
export async function updateAgentBigFivePersonality(
  agentId: number,
  data: Partial<InsertAgentBigFivePersonality>
): Promise<void> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  await db
    .update(agentBigFivePersonality)
    .set(data)
    .where(eq(agentBigFivePersonality.agentId, agentId));
}

// ============================================================================
// DREAMCOG INTEGRATION - AGENT MOTIVATIONS FUNCTIONS
// ============================================================================

export async function createAgentMotivation(
  data: InsertAgentMotivation
): Promise<AgentMotivation> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  const [motivation] = await db
    .insert(agentMotivations)
    .values(data)
    .$returningId();
  
  const created = await db
    .select()
    .from(agentMotivations)
    .where(eq(agentMotivations.id, motivation.id))
    .limit(1);
  
  return created[0];


}
export async function getAgentMotivations(
  agentId: number,
  activeOnly: boolean = false
): Promise<AgentMotivation[]> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  const conditions = [eq(agentMotivations.agentId, agentId)];
  if (activeOnly) {
    conditions.push(eq(agentMotivations.isActive, true));
  }
  
  return await db
    .select()
    .from(agentMotivations)
    .where(and(...conditions))
    .orderBy(desc(agentMotivations.priority));


}
export async function updateAgentMotivation(
  id: number,
  data: Partial<InsertAgentMotivation>
): Promise<void> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  await db
    .update(agentMotivations)
    .set(data)
    .where(eq(agentMotivations.id, id));
}

// ============================================================================
// DREAMCOG INTEGRATION - AGENT MEMORIES FUNCTIONS
// ============================================================================

export async function createAgentMemory(
  data: InsertAgentMemory
): Promise<AgentMemory> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  const [memory] = await db
    .insert(agentMemories)
    .values(data)
    .$returningId();
  
  const created = await db
    .select()
    .from(agentMemories)
    .where(eq(agentMemories.id, memory.id))
    .limit(1);
  
  return created[0];


}
export async function getAgentMemories(
  agentId: number,
  limit: number = 50
): Promise<AgentMemory[]> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  return await db
    .select()
    .from(agentMemories)
    .where(
      and(
        eq(agentMemories.agentId, agentId),
        eq(agentMemories.isRepressed, false)
      )
    )
    .orderBy(desc(agentMemories.importance), desc(agentMemories.memoryDate))
    .limit(limit);


}
export async function updateAgentMemory(
  id: number,
  data: Partial<InsertAgentMemory>
): Promise<void> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  await db
    .update(agentMemories)
    .set(data)
    .where(eq(agentMemories.id, id));
}

// ============================================================================
// DREAMCOG INTEGRATION - RELATIONSHIP EVENTS FUNCTIONS
// ============================================================================

export async function createRelationshipEvent(
  data: InsertRelationshipEvent
): Promise<RelationshipEvent> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  const [event] = await db
    .insert(relationshipEvents)
    .values(data)
    .$returningId();
  
  const created = await db
    .select()
    .from(relationshipEvents)
    .where(eq(relationshipEvents.id, event.id))
    .limit(1);
  
  return created[0];


}
export async function getRelationshipEvents(
  relationshipId: number
): Promise<RelationshipEvent[]> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  return await db
    .select()
    .from(relationshipEvents)
    .where(eq(relationshipEvents.relationshipId, relationshipId))
    .orderBy(desc(relationshipEvents.eventDate));
}

// ============================================================================
// DREAMCOG INTEGRATION - WORLDS FUNCTIONS
// ============================================================================

export async function createWorld(
  data: InsertWorld
): Promise<World> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  const [world] = await db
    .insert(worlds)
    .values(data)
    .$returningId();
  
  const created = await db
    .select()
    .from(worlds)
    .where(eq(worlds.id, world.id))
    .limit(1);
  
  return created[0];


}
export async function getWorldById(id: number): Promise<World | null> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  const [world] = await db
    .select()
    .from(worlds)
    .where(eq(worlds.id, id))
    .limit(1);
  
  return world || null;


}
export async function getWorldsByUserId(userId: number): Promise<World[]> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  return await db
    .select()
    .from(worlds)
    .where(eq(worlds.userId, userId))
    .orderBy(desc(worlds.createdAt));


}
export async function updateWorld(
  id: number,
  data: Partial<InsertWorld>
): Promise<void> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  await db
    .update(worlds)
    .set(data)
    .where(eq(worlds.id, id));
}

// ============================================================================
// DREAMCOG INTEGRATION - LOCATIONS FUNCTIONS
// ============================================================================

export async function createLocation(
  data: InsertLocation
): Promise<Location> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  const [location] = await db
    .insert(locations)
    .values(data)
    .$returningId();
  
  const created = await db
    .select()
    .from(locations)
    .where(eq(locations.id, location.id))
    .limit(1);
  
  return created[0];


}
export async function getLocationById(id: number): Promise<Location | null> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  const [location] = await db
    .select()
    .from(locations)
    .where(eq(locations.id, id))
    .limit(1);
  
  return location || null;


}
export async function getLocationsByWorldId(worldId: number): Promise<Location[]> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  return await db
    .select()
    .from(locations)
    .where(eq(locations.worldId, worldId))
    .orderBy(desc(locations.createdAt));


}
export async function getSubLocations(parentLocationId: number): Promise<Location[]> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  return await db
    .select()
    .from(locations)
    .where(eq(locations.parentLocationId, parentLocationId));


}
export async function updateLocation(
  id: number,
  data: Partial<InsertLocation>
): Promise<void> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  await db
    .update(locations)
    .set(data)
    .where(eq(locations.id, id));
}

// ============================================================================
// DREAMCOG INTEGRATION - LORE ENTRIES FUNCTIONS
// ============================================================================

export async function createLoreEntry(
  data: InsertLoreEntry
): Promise<LoreEntry> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  const [lore] = await db
    .insert(loreEntries)
    .values(data)
    .$returningId();
  
  const created = await db
    .select()
    .from(loreEntries)
    .where(eq(loreEntries.id, lore.id))
    .limit(1);
  
  return created[0];


}
export async function getLoreEntryById(id: number): Promise<LoreEntry | null> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  const [lore] = await db
    .select()
    .from(loreEntries)
    .where(eq(loreEntries.id, id))
    .limit(1);
  
  return lore || null;


}
export async function getLoreEntriesByWorldId(
  worldId: number,
  category?: string
): Promise<LoreEntry[]> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  const conditions = [eq(loreEntries.worldId, worldId)];
  if (category) {
    conditions.push(eq(loreEntries.category, category as any));
  }
  
  return await db
    .select()
    .from(loreEntries)
    .where(and(...conditions))
    .orderBy(desc(loreEntries.createdAt));


}
export async function updateLoreEntry(
  id: number,
  data: Partial<InsertLoreEntry>
): Promise<void> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  await db
    .update(loreEntries)
    .set(data)
    .where(eq(loreEntries.id, id));
}

// ============================================================================
// DREAMCOG INTEGRATION - WORLD EVENTS FUNCTIONS
// ============================================================================

export async function createWorldEvent(
  data: InsertWorldEvent
): Promise<WorldEvent> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  const [event] = await db
    .insert(worldEvents)
    .values(data)
    .$returningId();
  
  const created = await db
    .select()
    .from(worldEvents)
    .where(eq(worldEvents.id, event.id))
    .limit(1);
  
  return created[0];


}
export async function getWorldEventById(id: number): Promise<WorldEvent | null> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  const [event] = await db
    .select()
    .from(worldEvents)
    .where(eq(worldEvents.id, id))
    .limit(1);
  
  return event || null;


}
export async function getWorldEventsByWorldId(worldId: number): Promise<WorldEvent[]> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  return await db
    .select()
    .from(worldEvents)
    .where(eq(worldEvents.worldId, worldId))
    .orderBy(desc(worldEvents.importance), desc(worldEvents.eventDate));


}
export async function updateWorldEvent(
  id: number,
  data: Partial<InsertWorldEvent>
): Promise<void> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  await db
    .update(worldEvents)
    .set(data)
    .where(eq(worldEvents.id, id));
}

// ============================================================================
// DREAMCOG INTEGRATION - SCHEDULED WORLD EVENTS FUNCTIONS
// ============================================================================

export async function createScheduledWorldEvent(
  data: InsertScheduledWorldEvent
): Promise<ScheduledWorldEvent> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  const [event] = await db
    .insert(scheduledWorldEvents)
    .values(data)
    .$returningId();
  
  const created = await db
    .select()
    .from(scheduledWorldEvents)
    .where(eq(scheduledWorldEvents.id, event.id))
    .limit(1);
  
  return created[0];


}
export async function getScheduledWorldEventById(
  id: number
): Promise<ScheduledWorldEvent | null> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  const [event] = await db
    .select()
    .from(scheduledWorldEvents)
    .where(eq(scheduledWorldEvents.id, id))
    .limit(1);
  
  return event || null;


}
export async function getPendingScheduledWorldEvents(
  worldId: number
): Promise<ScheduledWorldEvent[]> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  return await db
    .select()
    .from(scheduledWorldEvents)
    .where(
      and(
        eq(scheduledWorldEvents.worldId, worldId),
        eq(scheduledWorldEvents.status, "pending")
      )
    )
    .orderBy(asc(scheduledWorldEvents.scheduledFor), desc(scheduledWorldEvents.priority));


}
export async function updateScheduledWorldEvent(
  id: number,
  data: Partial<InsertScheduledWorldEvent>
): Promise<void> {
  const db = await getDb();
  if (!db) throw new Error("Database not available");

  await db
    .update(scheduledWorldEvents)
    .set(data)
    .where(eq(scheduledWorldEvents.id, id));
}

// ============================================================================
// DREAMCOG INTEGRATION - AI Storytelling Database Helpers
// ============================================================================

// Note: Import these from schema at the top of the file when integrating
// For now, using inline references

// ============ API KEY HELPERS ============

export async function createApiKey(data: { userId: number; keyName: string; encryptedKey: string }) {
  const db = await getDb();
  if (!db) throw new Error("Database not available");
  const result = await db.insert(apiKeys).values(data);
  return result[0].insertId;
}

export async function getApiKeysByUserId(userId: number) {
  const db = await getDb();
  if (!db) return [];
  return db.select({
    id: apiKeys.id,
    keyName: apiKeys.keyName,
    lastUsed: apiKeys.lastUsed,
    createdAt: apiKeys.createdAt,
  }).from(apiKeys).where(eq(apiKeys.userId, userId));
}

export async function getApiKeyById(id: number, userId: number) {
  const db = await getDb();
  if (!db) return undefined;
  const result = await db.select().from(apiKeys)
    .where(and(eq(apiKeys.id, id), eq(apiKeys.userId, userId)))
    .limit(1);
  return result[0];
}

export async function deleteApiKey(id: number, userId: number) {
  const db = await getDb();
  if (!db) return;
  await db.delete(apiKeys).where(and(eq(apiKeys.id, id), eq(apiKeys.userId, userId)));
}

export async function updateApiKeyLastUsed(id: number) {
  const db = await getDb();
  if (!db) return;
  await db.update(apiKeys).set({ lastUsed: new Date() }).where(eq(apiKeys.id, id));
}
// ============ STORY CHARACTER HELPERS ============

export async function createStoryCharacter(data: InsertCharacter) {
  const db = await getDb();
  if (!db) throw new Error("Database not available");
  const result = await db.insert(characters).values(data);
  return result[0].insertId;
}

export async function getStoryCharactersByUserId(userId: number) {
  const db = await getDb();
  if (!db) return [];
  return db.select().from(characters).where(eq(characters.userId, userId)).orderBy(desc(characters.updatedAt));
}

export async function getStoryCharacterById(id: number, userId: number) {
  const db = await getDb();
  if (!db) return undefined;
  const result = await db.select().from(characters)
    .where(and(eq(characters.id, id), eq(characters.userId, userId)))
    .limit(1);
  return result[0];
}

export async function updateStoryCharacter(id: number, userId: number, data: Partial<InsertCharacter>) {
  const db = await getDb();
  if (!db) return;
  await db.update(characters).set(data).where(and(eq(characters.id, id), eq(characters.userId, userId)));
}

export async function deleteStoryCharacter(id: number, userId: number) {
  const db = await getDb();
  if (!db) return;
  await db.delete(characters).where(and(eq(characters.id, id), eq(characters.userId, userId)));
}
// ============ SCENARIO HELPERS ============

export async function createScenario(data: InsertScenario) {
  const db = await getDb();
  if (!db) throw new Error("Database not available");
  const result = await db.insert(scenarios).values(data);
  return result[0].insertId;
}

export async function getScenariosByUserId(userId: number) {
  const db = await getDb();
  if (!db) return [];
  return db.select().from(scenarios).where(eq(scenarios.userId, userId)).orderBy(desc(scenarios.updatedAt));
}

export async function getPublicScenarios(searchQuery?: string) {
  const db = await getDb();
  if (!db) return [];
  if (searchQuery) {
    return db.select().from(scenarios)
      .where(and(eq(scenarios.isPublic, true), like(scenarios.title, `%${searchQuery}%`)))
      .orderBy(desc(scenarios.createdAt));
  }
  return db.select().from(scenarios).where(eq(scenarios.isPublic, true)).orderBy(desc(scenarios.createdAt));
}

export async function getScenarioById(id: number) {
  const db = await getDb();
  if (!db) return undefined;
  const result = await db.select().from(scenarios).where(eq(scenarios.id, id)).limit(1);
  return result[0];
}

export async function updateScenario(id: number, userId: number, data: Partial<InsertScenario>) {
  const db = await getDb();
  if (!db) return;
  await db.update(scenarios).set(data).where(and(eq(scenarios.id, id), eq(scenarios.userId, userId)));
}

export async function deleteScenario(id: number, userId: number) {
  const db = await getDb();
  if (!db) return;
  await db.delete(scenarioInteractions).where(eq(scenarioInteractions.scenarioId, id));
  await db.delete(scenarioCharacters).where(eq(scenarioCharacters.scenarioId, id));
  await db.delete(scenarios).where(and(eq(scenarios.id, id), eq(scenarios.userId, userId)));
}
// ============ SCENARIO CHARACTER HELPERS ============

export async function addScenarioCharacter(data: InsertScenarioCharacter) {
  const db = await getDb();
  if (!db) throw new Error("Database not available");
  const result = await db.insert(scenarioCharacters).values(data);
  return result[0].insertId;
}

export async function getScenarioCharacters(scenarioId: number) {
  const db = await getDb();
  if (!db) return [];
  return db.select().from(scenarioCharacters)
    .where(eq(scenarioCharacters.scenarioId, scenarioId))
    .orderBy(scenarioCharacters.orderIndex);
}

export async function updateScenarioCharacter(id: number, data: Partial<InsertScenarioCharacter>) {
  const db = await getDb();
  if (!db) return;
  await db.update(scenarioCharacters).set(data).where(eq(scenarioCharacters.id, id));
}

export async function deleteScenarioCharacter(id: number) {
  const db = await getDb();
  if (!db) return;
  await db.delete(scenarioCharacters).where(eq(scenarioCharacters.id, id));
}
// ============ SCENARIO INTERACTION HELPERS ============

export async function addScenarioInteraction(data: InsertScenarioInteraction) {
  const db = await getDb();
  if (!db) throw new Error("Database not available");
  const result = await db.insert(scenarioInteractions).values(data);
  return result[0].insertId;
}

export async function getScenarioInteractions(scenarioId: number) {
  const db = await getDb();
  if (!db) return [];
  return db.select().from(scenarioInteractions)
    .where(eq(scenarioInteractions.scenarioId, scenarioId))
    .orderBy(scenarioInteractions.orderIndex);
}

export async function updateScenarioInteraction(id: number, data: Partial<InsertScenarioInteraction>) {
  const db = await getDb();
  if (!db) return;
  await db.update(scenarioInteractions).set(data).where(eq(scenarioInteractions.id, id));
}

export async function deleteScenarioInteraction(id: number) {
  const db = await getDb();
  if (!db) return;
  await db.delete(scenarioInteractions).where(eq(scenarioInteractions.id, id));
}
// ============ CHAT SESSION HELPERS ============

export async function createChatSession(data: InsertChatSession) {
  const db = await getDb();
  if (!db) throw new Error("Database not available");
  const result = await db.insert(chatSessions).values(data);
  return result[0].insertId;
}

export async function getChatSessionsByUserId(userId: number) {
  const db = await getDb();
  if (!db) return [];
  return db.select().from(chatSessions).where(eq(chatSessions.userId, userId)).orderBy(desc(chatSessions.updatedAt));
}

export async function getChatSessionById(id: number, userId: number) {
  const db = await getDb();
  if (!db) return undefined;
  const result = await db.select().from(chatSessions)
    .where(and(eq(chatSessions.id, id), eq(chatSessions.userId, userId)))
    .limit(1);
  return result[0];
}

export async function updateChatSession(id: number, userId: number, data: Partial<InsertChatSession>) {
  const db = await getDb();
  if (!db) return;
  await db.update(chatSessions).set(data).where(and(eq(chatSessions.id, id), eq(chatSessions.userId, userId)));
}

export async function deleteChatSession(id: number, userId: number) {
  const db = await getDb();
  if (!db) return;
  await db.delete(chatMessages).where(eq(chatMessages.sessionId, id));
  await db.delete(chatSessions).where(and(eq(chatSessions.id, id), eq(chatSessions.userId, userId)));
}
// ============ CHAT MESSAGE HELPERS ============

export async function addChatMessage(data: InsertChatMessage) {
  const db = await getDb();
  if (!db) throw new Error("Database not available");
  const result = await db.insert(chatMessages).values(data);
  return result[0].insertId;
}

export async function getChatMessages(sessionId: number) {
  const db = await getDb();
  if (!db) return [];
  return db.select().from(chatMessages)
    .where(eq(chatMessages.sessionId, sessionId))
    .orderBy(chatMessages.createdAt);
}

export async function deleteChatMessage(id: number) {
  const db = await getDb();
  if (!db) return;
  await db.delete(chatMessages).where(eq(chatMessages.id, id));
}
// ============ STORY HELPERS ============

export async function createStory(data: InsertStory) {
  const db = await getDb();
  if (!db) throw new Error("Database not available");
  const result = await db.insert(stories).values(data);
  return result[0].insertId;
}

export async function getStoriesByUserId(userId: number) {
  const db = await getDb();
  if (!db) return [];
  return db.select().from(stories).where(eq(stories.userId, userId)).orderBy(desc(stories.updatedAt));
}

export async function getStoryById(id: number, userId: number) {
  const db = await getDb();
  if (!db) return undefined;
  const result = await db.select().from(stories)
    .where(and(eq(stories.id, id), eq(stories.userId, userId)))
    .limit(1);
  return result[0];
}

export async function updateStory(id: number, userId: number, data: Partial<InsertStory>) {
  const db = await getDb();
  if (!db) return;
  await db.update(stories).set(data).where(and(eq(stories.id, id), eq(stories.userId, userId)));
}

export async function deleteStory(id: number, userId: number) {
  const db = await getDb();
  if (!db) return;
  await db.delete(storyCharacters).where(eq(storyCharacters.storyId, id));
  await db.delete(stories).where(and(eq(stories.id, id), eq(stories.userId, userId)));
}
// ============ STORY CHARACTER LINK HELPERS ============

export async function addStoryCharacterLink(data: InsertStoryCharacter) {
  const db = await getDb();
  if (!db) throw new Error("Database not available");
  const result = await db.insert(storyCharacters).values(data);
  return result[0].insertId;
}

export async function getStoryCharacterLinks(storyId: number) {
  const db = await getDb();
  if (!db) return [];
  return db.select().from(storyCharacters)
    .where(eq(storyCharacters.storyId, storyId))
    .orderBy(storyCharacters.orderIndex);
}

export async function updateStoryCharacterLink(id: number, data: Partial<InsertStoryCharacter>) {
  const db = await getDb();
  if (!db) return;
  await db.update(storyCharacters).set(data).where(eq(storyCharacters.id, id));
}

export async function deleteStoryCharacterLink(id: number) {
  const db = await getDb();
  if (!db) return;
  await db.delete(storyCharacters).where(eq(storyCharacters.id, id));
}
// ============ GENERATED IMAGE HELPERS ============

export async function createGeneratedImage(data: InsertGeneratedImage) {
  const db = await getDb();
  if (!db) throw new Error("Database not available");
  const result = await db.insert(generatedImages).values(data);
  return result[0].insertId;
}

export async function getGeneratedImagesByUserId(userId: number, limit = 50) {
  const db = await getDb();
  if (!db) return [];
  return db.select().from(generatedImages)
    .where(eq(generatedImages.userId, userId))
    .orderBy(desc(generatedImages.createdAt))
    .limit(limit);
}

export async function deleteGeneratedImage(id: number, userId: number) {
  const db = await getDb();
  if (!db) return;
  await db.delete(generatedImages).where(and(eq(generatedImages.id, id), eq(generatedImages.userId, userId)));
}
// ============ CHARACTER EMOTIONAL STATE HELPERS ============

export async function getCharacterEmotionalState(characterId: number) {
  const db = await getDb();
  if (!db) return undefined;
  const result = await db.select().from(characterEmotionalStates)
    .where(eq(characterEmotionalStates.characterId, characterId))
    .limit(1);
  return result[0];
}

export async function upsertCharacterEmotionalState(data: InsertCharacterEmotionalState) {
  const db = await getDb();
  if (!db) throw new Error("Database not available");
  const existing = await getCharacterEmotionalState(data.characterId);
  if (existing) {
    await db.update(characterEmotionalStates)
      .set({ ...data, lastUpdated: new Date() })
      .where(eq(characterEmotionalStates.characterId, data.characterId));
    return existing.id;
  } else {
    const result = await db.insert(characterEmotionalStates).values(data);
    return result[0].insertId;
  }
}
// ============ CHARACTER MOTIVATION HELPERS ============

export async function createCharacterMotivationEntry(data: InsertCharacterMotivation) {
  const db = await getDb();
  if (!db) throw new Error("Database not available");
  const result = await db.insert(characterMotivations).values(data);
  return result[0].insertId;
}

export async function getCharacterMotivationsByCharacterId(characterId: number, activeOnly = false) {
  const db = await getDb();
  if (!db) return [];
  if (activeOnly) {
    return db.select().from(characterMotivations)
      .where(and(eq(characterMotivations.characterId, characterId), eq(characterMotivations.isActive, true)))
      .orderBy(desc(characterMotivations.priority));
  }
  return db.select().from(characterMotivations)
    .where(eq(characterMotivations.characterId, characterId))
    .orderBy(desc(characterMotivations.priority));
}

export async function updateCharacterMotivationEntry(id: number, data: Partial<InsertCharacterMotivation>) {
  const db = await getDb();
  if (!db) return;
  await db.update(characterMotivations).set(data).where(eq(characterMotivations.id, id));
}

export async function deleteCharacterMotivationEntry(id: number) {
  const db = await getDb();
  if (!db) return;
  await db.delete(characterMotivations).where(eq(characterMotivations.id, id));
}
// ============ CHARACTER MEMORY HELPERS ============

export async function createCharacterMemoryEntry(data: InsertCharacterMemory) {
  const db = await getDb();
  if (!db) throw new Error("Database not available");
  const result = await db.insert(characterMemories).values(data);
  return result[0].insertId;
}

export async function getCharacterMemoriesByCharacterId(characterId: number, limit = 50) {
  const db = await getDb();
  if (!db) return [];
  return db.select().from(characterMemories)
    .where(eq(characterMemories.characterId, characterId))
    .orderBy(desc(characterMemories.memoryDate))
    .limit(limit);
}

export async function updateCharacterMemoryEntry(id: number, data: Partial<InsertCharacterMemory>) {
  const db = await getDb();
  if (!db) return;
  await db.update(characterMemories).set(data).where(eq(characterMemories.id, id));
}

export async function deleteCharacterMemoryEntry(id: number) {
  const db = await getDb();
  if (!db) return;
  await db.delete(characterMemories).where(eq(characterMemories.id, id));
}
// ============ STORY GROUP HELPERS ============

export async function createStoryGroup(data: InsertGroup) {
  const db = await getDb();
  if (!db) throw new Error("Database not available");
  const result = await db.insert(groups).values(data);
  return result[0].insertId;
}

export async function getStoryGroupsByUserId(userId: number) {
  const db = await getDb();
  if (!db) return [];
  return db.select().from(groups).where(eq(groups.userId, userId)).orderBy(desc(groups.updatedAt));
}

export async function getStoryGroupById(id: number, userId: number) {
  const db = await getDb();
  if (!db) return undefined;
  const result = await db.select().from(groups)
    .where(and(eq(groups.id, id), eq(groups.userId, userId)))
    .limit(1);
  return result[0];
}

export async function updateStoryGroup(id: number, userId: number, data: Partial<InsertGroup>) {
  const db = await getDb();
  if (!db) return;
  await db.update(groups).set(data).where(and(eq(groups.id, id), eq(groups.userId, userId)));
}

export async function deleteStoryGroup(id: number, userId: number) {
  const db = await getDb();
  if (!db) return;
  await db.delete(groups).where(and(eq(groups.id, id), eq(groups.userId, userId)));
}
// ============ SCHEDULED EVENT HELPERS (DREAMCOG) ============

export async function createScheduledEventEntry(data: InsertScheduledEvent) {
  const db = await getDb();
  if (!db) throw new Error("Database not available");
  const result = await db.insert(scheduledEvents).values(data);
  return result[0].insertId;
}

export async function getScheduledEventsByWorldId(worldId: number) {
  const db = await getDb();
  if (!db) return [];
  return db.select().from(scheduledEvents)
    .where(eq(scheduledEvents.worldId, worldId))
    .orderBy(scheduledEvents.scheduledFor);
}

export async function updateScheduledEventEntry(id: number, data: Partial<InsertScheduledEvent>) {
  const db = await getDb();
  if (!db) return;
  await db.update(scheduledEvents).set(data).where(eq(scheduledEvents.id, id));
}

export async function deleteScheduledEventEntry(id: number) {
  const db = await getDb();
  if (!db) return;
  await db.delete(scheduledEvents).where(eq(scheduledEvents.id, id));
}

// ============ EVENT PROPAGATION HELPERS ============

export async function getEventPropagationHistory(limit = 50): Promise<EventPropagation[]> {
  const db = await getDb();
  if (!db) return [];
  return db.select().from(eventPropagation)
    .orderBy(desc(eventPropagation.createdAt))
    .limit(limit);
}

export async function getEventPropagationBySourceType(sourceType: "business" | "narrative", limit = 50): Promise<EventPropagation[]> {
  const db = await getDb();
  if (!db) return [];
  return db.select().from(eventPropagation)
    .where(eq(eventPropagation.sourceType, sourceType))
    .orderBy(desc(eventPropagation.createdAt))
    .limit(limit);
}

// ============ API KEY VERIFICATION ============

export async function verifyApiKey(id: number, userId: number): Promise<{ valid: boolean; error?: string }> {
  const db = await getDb();
  if (!db) return { valid: false, error: "Database not available" };

  const apiKey = await getApiKeyById(id, userId);
  if (!apiKey) {
    return { valid: false, error: "API key not found" };
  }

  try {
    // Test the API key against DreamGen API
    const response = await fetch("https://dreamgen.com/api/v1/models", {
      method: "GET",
      headers: {
        "Authorization": `Bearer ${apiKey.encryptedKey}`,
        "Content-Type": "application/json",
      },
    });

    if (response.ok) {
      // Update last used timestamp
      await updateApiKeyLastUsed(id);
      return { valid: true };
    } else if (response.status === 401) {
      return { valid: false, error: "Invalid or expired API key" };
    } else {
      return { valid: false, error: `API returned status ${response.status}` };
    }
  } catch (error) {
    return { valid: false, error: error instanceof Error ? error.message : "Unknown error" };
  }
}

import { COOKIE_NAME } from "@shared/const";
import { z } from "zod";
import { getSessionCookieOptions } from "./_core/cookies";
import { systemRouter } from "./_core/systemRouter";
import { publicProcedure, protectedProcedure, router } from "./_core/trpc";
import { agentBrain, DecisionContext, DecisionOption } from "./services/agentBrain";
import { eventBridge, BusinessEvent, NarrativeEvent } from "./services/eventBridge";
import simulationEngine from "./services/simulationEngine";
import {
  createCompany,
  getCompanyByUserId,
  getCompanyById,
  getAllCompanies,
  updateCompanyCash,
  createBusinessUnit,
  getBusinessUnitsByCompany,
  getBusinessUnitById,
  updateBusinessUnit,
  getAllCities,
  getCityById,
  getAllResourceTypes,
  getResourceTypeById,
  createEmployees,
  getEmployeesByUnit,
  updateEmployees,
  getInventoryByUnit,
  upsertInventory,
  createMarketListing,
  getMarketListings,
  createTransaction,
  getTransactionsByCompany,
  createNotification,
  getNotificationsByUser,
  markNotificationRead,
  getGameState,
  initializeGameData,
  getProductionRecipes,
  getProductionRecipeById,
  addToProductionQueue,
  getProductionQueue,
  purchaseFromMarket,
  getMarketListingById,
  cancelMarketListing,
  seedProductionRecipes,
  // Agentic simulation functions
  getAllCharacterPersonas,
  getCharacterPersonaById,
  getAllCharacterTraits,
  getCharacterTraitsByCategory,
  createAgent,
  getAgentById,
  getAgentsByType,
  getAgentsByCompany,
  getAgentsByBusinessUnit,
  updateAgent,
  updateAgentEmotionalState,
  getAgentTraits,
  addTraitToAgent,
  removeTraitFromAgent,
  createRelationship,
  getRelationshipBetweenAgents,
  getAgentRelationships,
  updateRelationship,
  recordRelationshipInteraction,
  createAgentGroup,
  getAgentGroupById,
  getAgentGroupsByCompany,
  getGroupMembers,
  getAgentGroups,
  addAgentToGroup,
  removeAgentFromGroup,
  createCommunity,
  getCommunityById,
  getCommunitiesByCity,
  getCommunityMembers,
  getAgentCommunities,
  addAgentToCommunity,
  createAgentEvent,
  getAgentEventById,
  getAgentEvents,
  getScheduledEvents,
  updateAgentEvent,
  processAgentEvent,
  getActiveEventTriggers,
  getAgentHistory,
  // DreamCog integration functions
  createAgentBigFivePersonality,
  getAgentBigFivePersonality,
  updateAgentBigFivePersonality,
  createAgentMotivation,
  getAgentMotivations,
  updateAgentMotivation,
  createAgentMemory,
  getAgentMemories,
  updateAgentMemory,
  createRelationshipEvent,
  getRelationshipEvents,
  createWorld,
  getWorldById,
  getWorldsByUserId,
  updateWorld,
  createLocation,
  getLocationById,
  getLocationsByWorldId,
  getSubLocations,
  updateLocation,
  createLoreEntry,
  getLoreEntryById,
  getLoreEntriesByWorldId,
  updateLoreEntry,
  createWorldEvent,
  getWorldEventById,
  getWorldEventsByWorldId,
  updateWorldEvent,
  createScheduledWorldEvent,
  getScheduledWorldEventById,
  getPendingScheduledWorldEvents,
  updateScheduledWorldEvent,
  // DreamCog storytelling functions
  createApiKey,
  getApiKeysByUserId,
  getApiKeyById,
  deleteApiKey,
  createStoryCharacter,
  getStoryCharactersByUserId,
  getStoryCharacterById,
  updateStoryCharacter,
  deleteStoryCharacter,
  createScenario,
  getScenariosByUserId,
  getPublicScenarios,
  getScenarioById,
  updateScenario,
  deleteScenario,
  addScenarioCharacter,
  getScenarioCharacters,
  updateScenarioCharacter,
  deleteScenarioCharacter,
  addScenarioInteraction,
  getScenarioInteractions,
  updateScenarioInteraction,
  deleteScenarioInteraction,
  getEventPropagationHistory,
  getEventPropagationBySourceType,
  verifyApiKey,
  // Technology functions
  getAllTechnologies,
  getTechnologyById,
  getCompanyTechnologies,
  startTechnologyResearch,
  updateTechnologyResearch,
  hasCompanyResearchedTech,
  // Game processing functions
  processTurnAdvancement,
  processCompanyPayroll,
  completeProductionItem,
  getReadyProductionItems,
  createChatSession,
  getChatSessionsByUserId,
  getChatSessionById,
  updateChatSession,
  deleteChatSession,
  addChatMessage,
  getChatMessages,
  createStory,
  getStoriesByUserId,
  getStoryById,
  updateStory,
  deleteStory,
  addStoryCharacterLink,
  getStoryCharacterLinks,
  updateStoryCharacterLink,
  deleteStoryCharacterLink,
  createGeneratedImage,
  getGeneratedImagesByUserId,
  deleteGeneratedImage,
} from "./db";

// Initialize game data on server start
initializeGameData().then(() => seedProductionRecipes()).catch(console.error);

export const appRouter = router({
  system: systemRouter,
  
  auth: router({
    me: publicProcedure.query((opts) => opts.ctx.user),
    logout: publicProcedure.mutation(({ ctx }) => {
      const cookieOptions = getSessionCookieOptions(ctx.req);
      ctx.res.clearCookie(COOKIE_NAME, { ...cookieOptions, maxAge: -1 });
      return { success: true } as const;
    }),
  }),

  // ============================================================================
  // COMPANY ROUTES
  // ============================================================================
  company: router({
    // Get current user's company
    mine: protectedProcedure.query(async ({ ctx }) => {
      return await getCompanyByUserId(ctx.user.id);
    }),

    // Create a new company
    create: protectedProcedure
      .input(
        z.object({
          name: z.string().min(2).max(128),
          description: z.string().optional(),
        })
      )
      .mutation(async ({ ctx, input }) => {
        // Check if user already has a company
        const existing = await getCompanyByUserId(ctx.user.id);
        if (existing) {
          throw new Error("You already have a company");
        }

        const company = await createCompany({
          userId: ctx.user.id,
          name: input.name,
          description: input.description,
          cash: "1000000.00", // Starting capital
        });

        if (company) {
          // Create notification
          await createNotification({
            userId: ctx.user.id,
            type: "success",
            title: "Company Founded!",
            message: `Congratulations! ${input.name} has been established with $1,000,000 starting capital.`,
          });
        }

        return company;
      }),

    // Get company by ID
    byId: protectedProcedure
      .input(z.object({ id: z.number() }))
      .query(async ({ input }) => {
        return await getCompanyById(input.id);
      }),

    // Get all companies (for leaderboard)
    all: protectedProcedure.query(async () => {
      return await getAllCompanies();
    }),
  }),

  // ============================================================================
  // BUSINESS UNIT ROUTES
  // ============================================================================
  businessUnit: router({
    // Get all units for current company
    list: protectedProcedure.query(async ({ ctx }) => {
      const company = await getCompanyByUserId(ctx.user.id);
      if (!company) return [];
      return await getBusinessUnitsByCompany(company.id);
    }),

    // Get single unit by ID
    byId: protectedProcedure
      .input(z.object({ id: z.number() }))
      .query(async ({ input }) => {
        return await getBusinessUnitById(input.id);
      }),

    // Create a new business unit
    create: protectedProcedure
      .input(
        z.object({
          type: z.enum(["office", "store", "factory", "mine", "farm", "laboratory"]),
          name: z.string().min(2).max(128),
          cityId: z.number(),
          size: z.number().min(50).max(10000).optional(),
        })
      )
      .mutation(async ({ ctx, input }) => {
        const company = await getCompanyByUserId(ctx.user.id);
        if (!company) {
          throw new Error("You need to create a company first");
        }

        // Calculate construction cost based on type and size
        const baseCosts: Record<string, number> = {
          office: 50000,
          store: 100000,
          factory: 500000,
          mine: 1000000,
          farm: 200000,
          laboratory: 750000,
        };

        const size = input.size || 100;
        const cost = baseCosts[input.type] * (size / 100);

        if (parseFloat(company.cash) < cost) {
          throw new Error(`Insufficient funds. Need $${cost.toLocaleString()}`);
        }

        // Deduct cost
        const newCash = (parseFloat(company.cash) - cost).toFixed(2);
        await updateCompanyCash(company.id, newCash);

        // Create the unit
        const unit = await createBusinessUnit({
          companyId: company.id,
          type: input.type,
          name: input.name,
          cityId: input.cityId,
          size: size,
        });

        if (unit) {
          // Create initial employees record
          await createEmployees({
            businessUnitId: unit.id,
            count: 0,
            salary: "1000.00",
          });

          // Record transaction
          await createTransaction({
            type: "construction",
            companyId: company.id,
            amount: (-cost).toFixed(2),
            description: `Built ${input.type}: ${input.name}`,
            relatedUnitId: unit.id,
          });

          // Notification
          await createNotification({
            userId: ctx.user.id,
            type: "success",
            title: "Construction Complete",
            message: `${input.name} (${input.type}) has been built in the selected city.`,
          });
        }

        return unit;
      }),

    // Update business unit
    update: protectedProcedure
      .input(
        z.object({
          id: z.number(),
          name: z.string().min(2).max(128).optional(),
          isActive: z.boolean().optional(),
        })
      )
      .mutation(async ({ ctx, input }) => {
        const unit = await getBusinessUnitById(input.id);
        if (!unit) throw new Error("Unit not found");

        const company = await getCompanyByUserId(ctx.user.id);
        if (!company || unit.companyId !== company.id) {
          throw new Error("Not authorized");
        }

        await updateBusinessUnit(input.id, {
          name: input.name,
          isActive: input.isActive,
        });

        return await getBusinessUnitById(input.id);
      }),

    // Get employees for a unit
    employees: protectedProcedure
      .input(z.object({ unitId: z.number() }))
      .query(async ({ input }) => {
        return await getEmployeesByUnit(input.unitId);
      }),

    // Hire/update employees
    updateEmployees: protectedProcedure
      .input(
        z.object({
          unitId: z.number(),
          count: z.number().min(0),
          salary: z.number().min(100),
        })
      )
      .mutation(async ({ ctx, input }) => {
        const unit = await getBusinessUnitById(input.unitId);
        if (!unit) throw new Error("Unit not found");

        const company = await getCompanyByUserId(ctx.user.id);
        if (!company || unit.companyId !== company.id) {
          throw new Error("Not authorized");
        }

        await updateEmployees(input.unitId, {
          count: input.count,
          salary: input.salary.toFixed(2),
        });

        return await getEmployeesByUnit(input.unitId);
      }),

    // Get inventory for a unit
    inventory: protectedProcedure
      .input(z.object({ unitId: z.number() }))
      .query(async ({ input }) => {
        return await getInventoryByUnit(input.unitId);
      }),
  }),

  // ============================================================================
  // CITY ROUTES
  // ============================================================================
  city: router({
    list: protectedProcedure.query(async () => {
      return await getAllCities();
    }),

    byId: protectedProcedure
      .input(z.object({ id: z.number() }))
      .query(async ({ input }) => {
        return await getCityById(input.id);
      }),
  }),

  // ============================================================================
  // RESOURCE ROUTES
  // ============================================================================
  resource: router({
    list: protectedProcedure.query(async () => {
      return await getAllResourceTypes();
    }),

    byId: protectedProcedure
      .input(z.object({ id: z.number() }))
      .query(async ({ input }) => {
        return await getResourceTypeById(input.id);
      }),
  }),

  // ============================================================================
  // MARKET ROUTES
  // ============================================================================
  market: router({
    listings: protectedProcedure
      .input(
        z.object({
          resourceTypeId: z.number().optional(),
          cityId: z.number().optional(),
        }).optional()
      )
      .query(async ({ input }) => {
        return await getMarketListings(input?.resourceTypeId, input?.cityId);
      }),

    createListing: protectedProcedure
      .input(
        z.object({
          businessUnitId: z.number(),
          resourceTypeId: z.number(),
          quantity: z.number().positive(),
          pricePerUnit: z.number().positive(),
        })
      )
      .mutation(async ({ ctx, input }) => {
        const company = await getCompanyByUserId(ctx.user.id);
        if (!company) throw new Error("No company found");

        const unit = await getBusinessUnitById(input.businessUnitId);
        if (!unit || unit.companyId !== company.id) {
          throw new Error("Not authorized");
        }

        await createMarketListing({
          companyId: company.id,
          businessUnitId: input.businessUnitId,
          resourceTypeId: input.resourceTypeId,
          quantity: input.quantity.toFixed(4),
          pricePerUnit: input.pricePerUnit.toFixed(2),
          cityId: unit.cityId,
        });

        return { success: true };
      }),

    // Get a specific listing
    byId: protectedProcedure
      .input(z.object({ id: z.number() }))
      .query(async ({ input }) => {
        return await getMarketListingById(input.id);
      }),

    // Purchase from market
    purchase: protectedProcedure
      .input(
        z.object({
          listingId: z.number(),
          quantity: z.number().positive(),
          destinationUnitId: z.number(),
        })
      )
      .mutation(async ({ ctx, input }) => {
        const company = await getCompanyByUserId(ctx.user.id);
        if (!company) throw new Error("No company found");

        // Verify the destination unit belongs to the buyer
        const unit = await getBusinessUnitById(input.destinationUnitId);
        if (!unit || unit.companyId !== company.id) {
          throw new Error("Invalid destination unit");
        }

        const result = await purchaseFromMarket(
          company.id,
          input.listingId,
          input.quantity,
          input.destinationUnitId
        );

        if (!result.success) {
          throw new Error(result.message);
        }

        return result;
      }),

    // Cancel a listing
    cancel: protectedProcedure
      .input(z.object({ listingId: z.number() }))
      .mutation(async ({ ctx, input }) => {
        const company = await getCompanyByUserId(ctx.user.id);
        if (!company) throw new Error("No company found");

        const result = await cancelMarketListing(input.listingId, company.id);
        if (!result.success) {
          throw new Error(result.message);
        }

        return result;
      }),
  }),

  // ============================================================================
  // PRODUCTION ROUTES
  // ============================================================================
  production: router({
    // Get available recipes for a unit type
    recipes: protectedProcedure
      .input(
        z.object({
          unitType: z.enum(["factory", "farm", "mine", "laboratory"]).optional(),
        }).optional()
      )
      .query(async ({ input }) => {
        return await getProductionRecipes(input?.unitType);
      }),

    // Get a specific recipe
    recipeById: protectedProcedure
      .input(z.object({ id: z.number() }))
      .query(async ({ input }) => {
        return await getProductionRecipeById(input.id);
      }),

    // Get production queue for a unit
    queue: protectedProcedure
      .input(z.object({ unitId: z.number() }))
      .query(async ({ ctx, input }) => {
        const company = await getCompanyByUserId(ctx.user.id);
        if (!company) return [];

        const unit = await getBusinessUnitById(input.unitId);
        if (!unit || unit.companyId !== company.id) {
          throw new Error("Not authorized");
        }

        return await getProductionQueue(input.unitId);
      }),

    // Start production
    start: protectedProcedure
      .input(
        z.object({
          unitId: z.number(),
          recipeId: z.number(),
          quantity: z.number().positive(),
        })
      )
      .mutation(async ({ ctx, input }) => {
        const company = await getCompanyByUserId(ctx.user.id);
        if (!company) throw new Error("No company found");

        const unit = await getBusinessUnitById(input.unitId);
        if (!unit || unit.companyId !== company.id) {
          throw new Error("Not authorized");
        }

        // Verify recipe is valid for this unit type
        const recipe = await getProductionRecipeById(input.recipeId);
        if (!recipe || recipe.unitType !== unit.type) {
          throw new Error("Invalid recipe for this unit type");
        }

        // Add to production queue
        await addToProductionQueue({
          businessUnitId: input.unitId,
          recipeId: input.recipeId,
          quantity: input.quantity,
        });

        await createNotification({
          userId: ctx.user.id,
          type: "production",
          title: "Production Started",
          message: `Started producing ${input.quantity} units at ${unit.name}`,
        });

        return { success: true };
      }),
  }),

  // ============================================================================
  // AGENTIC SIMULATION ROUTES
  // ============================================================================
  
  // Character Personas
  persona: router({
    list: protectedProcedure.query(async () => {
      return await getAllCharacterPersonas();
    }),
    
    byId: protectedProcedure
      .input(z.object({ id: z.number() }))
      .query(async ({ input }) => {
        return await getCharacterPersonaById(input.id);
      }),
  }),

  // Character Traits
  trait: router({
    list: protectedProcedure.query(async () => {
      return await getAllCharacterTraits();
    }),
    
    byCategory: protectedProcedure
      .input(z.object({ 
        category: z.enum(["professional", "social", "cognitive", "emotional"]) 
      }))
      .query(async ({ input }) => {
        return await getCharacterTraitsByCategory(input.category);
      }),
  }),

  // Agents (Customers, Suppliers, Employees, Partners)
  agent: router({
    // Create a new agent
    create: protectedProcedure
      .input(
        z.object({
          type: z.enum(["customer", "supplier", "employee", "partner", "investor", "competitor"]),
          name: z.string().min(2).max(128),
          personaId: z.number(),
          cityId: z.number(),
          companyId: z.number().optional(),
          businessUnitId: z.number().optional(),
        })
      )
      .mutation(async ({ ctx, input }) => {
        const company = await getCompanyByUserId(ctx.user.id);
        
        // Validate ownership if company/unit specified
        if (input.companyId && (!company || input.companyId !== company.id)) {
          throw new Error("Not authorized to create agent for this company");
        }
        
        if (input.businessUnitId) {
          const unit = await getBusinessUnitById(input.businessUnitId);
          if (!unit || !company || unit.companyId !== company.id) {
            throw new Error("Not authorized to create agent for this business unit");
          }
        }

        const agent = await createAgent({
          type: input.type,
          name: input.name,
          personaId: input.personaId,
          cityId: input.cityId,
          companyId: input.companyId,
          businessUnitId: input.businessUnitId,
        });

        return agent;
      }),

    // Get agent by ID
    byId: protectedProcedure
      .input(z.object({ id: z.number() }))
      .query(async ({ input }) => {
        return await getAgentById(input.id);
      }),

    // Get agents by type
    byType: protectedProcedure
      .input(z.object({ 
        type: z.enum(["customer", "supplier", "employee", "partner", "investor", "competitor"]) 
      }))
      .query(async ({ input }) => {
        return await getAgentsByType(input.type);
      }),

    // Get agents for current user's company
    byCompany: protectedProcedure.query(async ({ ctx }) => {
      const company = await getCompanyByUserId(ctx.user.id);
      if (!company) return [];
      return await getAgentsByCompany(company.id);
    }),

    // Get agents by business unit
    byBusinessUnit: protectedProcedure
      .input(z.object({ businessUnitId: z.number() }))
      .query(async ({ ctx, input }) => {
        const company = await getCompanyByUserId(ctx.user.id);
        const unit = await getBusinessUnitById(input.businessUnitId);
        
        if (!unit || !company || unit.companyId !== company.id) {
          throw new Error("Not authorized");
        }
        
        return await getAgentsByBusinessUnit(input.businessUnitId);
      }),

    // Update agent
    update: protectedProcedure
      .input(
        z.object({
          id: z.number(),
          updates: z.object({
            name: z.string().optional(),
            isActive: z.boolean().optional(),
          }),
        })
      )
      .mutation(async ({ ctx, input }) => {
        const agent = await getAgentById(input.id);
        if (!agent) throw new Error("Agent not found");

        // Verify ownership
        if (agent.companyId) {
          const company = await getCompanyByUserId(ctx.user.id);
          if (!company || agent.companyId !== company.id) {
            throw new Error("Not authorized");
          }
        }

        await updateAgent(input.id, input.updates);
        return await getAgentById(input.id);
      }),

    // Update agent emotional state
    updateEmotions: protectedProcedure
      .input(
        z.object({
          id: z.number(),
          emotions: z.object({
            happiness: z.number().min(0).max(100).optional(),
            satisfaction: z.number().min(0).max(100).optional(),
            stress: z.number().min(0).max(100).optional(),
            loyalty: z.number().min(0).max(100).optional(),
            trust: z.number().min(0).max(100).optional(),
          }),
        })
      )
      .mutation(async ({ ctx, input }) => {
        const agent = await getAgentById(input.id);
        if (!agent) throw new Error("Agent not found");

        if (agent.companyId) {
          const company = await getCompanyByUserId(ctx.user.id);
          if (!company || agent.companyId !== company.id) {
            throw new Error("Not authorized");
          }
        }

        await updateAgentEmotionalState(input.id, input.emotions);
        return await getAgentById(input.id);
      }),

    // Get agent traits
    traits: protectedProcedure
      .input(z.object({ agentId: z.number() }))
      .query(async ({ input }) => {
        return await getAgentTraits(input.agentId);
      }),

    // Add trait to agent
    addTrait: protectedProcedure
      .input(
        z.object({
          agentId: z.number(),
          traitId: z.number(),
          intensity: z.number().min(0).max(100).optional(),
        })
      )
      .mutation(async ({ ctx, input }) => {
        const agent = await getAgentById(input.agentId);
        if (!agent) throw new Error("Agent not found");

        if (agent.companyId) {
          const company = await getCompanyByUserId(ctx.user.id);
          if (!company || agent.companyId !== company.id) {
            throw new Error("Not authorized");
          }
        }

        await addTraitToAgent(input.agentId, input.traitId, input.intensity);
        return { success: true };
      }),

    // Get agent history
    history: protectedProcedure
      .input(z.object({ agentId: z.number(), limit: z.number().optional() }))
      .query(async ({ input }) => {
        return await getAgentHistory(input.agentId, input.limit);
      }),

    // Make a decision for an agent using the AgentBrain service
    makeDecision: protectedProcedure
      .input(
        z.object({
          agentId: z.number(),
          context: z.object({
            type: z.enum(["trade", "negotiation", "investment", "hiring", "partnership", "conflict", "cooperation"]),
            situation: z.string(),
            options: z.array(z.object({
              id: z.string(),
              description: z.string(),
              expectedOutcome: z.string(),
              riskLevel: z.number().min(0).max(100),
              potentialReward: z.number().min(0).max(100),
              requiresCooperation: z.boolean(),
              requiresConflict: z.boolean(),
            })),
            relatedAgentId: z.number().optional(),
            relatedCompanyId: z.number().optional(),
            financialStakes: z.number().optional(),
            riskLevel: z.number().optional(),
          }),
        })
      )
      .mutation(async ({ ctx, input }) => {
        const agent = await getAgentById(input.agentId);
        if (!agent) throw new Error("Agent not found");

        // Verify ownership if agent belongs to a company
        if (agent.companyId) {
          const company = await getCompanyByUserId(ctx.user.id);
          if (!company || agent.companyId !== company.id) {
            throw new Error("Not authorized");
          }
        }

        return await agentBrain.makeDecision(input.agentId, input.context as DecisionContext);
      }),

    // Process the outcome of a decision
    processDecisionOutcome: protectedProcedure
      .input(
        z.object({
          agentId: z.number(),
          outcome: z.enum(["success", "failure", "neutral"]),
          decisionType: z.string(),
          reasoning: z.string(),
        })
      )
      .mutation(async ({ ctx, input }) => {
        const agent = await getAgentById(input.agentId);
        if (!agent) throw new Error("Agent not found");

        if (agent.companyId) {
          const company = await getCompanyByUserId(ctx.user.id);
          if (!company || agent.companyId !== company.id) {
            throw new Error("Not authorized");
          }
        }

        await agentBrain.processDecisionOutcome(
          input.agentId,
          input.outcome,
          input.decisionType,
          input.reasoning
        );
        return { success: true };
      }),

    // Create agent with personality (using AgentBrain service)
    createWithPersonality: protectedProcedure
      .input(
        z.object({
          name: z.string().min(2).max(128),
          type: z.enum(["customer", "supplier", "employee", "partner", "investor", "competitor"]),
          personaId: z.number(),
          cityId: z.number(),
          companyId: z.number().optional(),
          businessUnitId: z.number().optional(),
        })
      )
      .mutation(async ({ ctx, input }) => {
        const company = await getCompanyByUserId(ctx.user.id);

        if (input.companyId && (!company || input.companyId !== company.id)) {
          throw new Error("Not authorized to create agent for this company");
        }

        return await agentBrain.createAgentWithPersonality({
          name: input.name,
          type: input.type,
          personaId: input.personaId,
          cityId: input.cityId,
          companyId: input.companyId,
          businessUnitId: input.businessUnitId,
        });
      }),
  }),

  // Relationships
  relationship: router({
    // Create relationship
    create: protectedProcedure
      .input(
        z.object({
          agent1Id: z.number(),
          agent2Id: z.number(),
          type: z.enum(["business", "personal", "professional", "familial", "competitive"]),
        })
      )
      .mutation(async ({ input }) => {
        return await createRelationship({
          agent1Id: input.agent1Id,
          agent2Id: input.agent2Id,
          type: input.type,
        });
      }),

    // Get relationship between two agents
    between: protectedProcedure
      .input(z.object({ agent1Id: z.number(), agent2Id: z.number() }))
      .query(async ({ input }) => {
        return await getRelationshipBetweenAgents(input.agent1Id, input.agent2Id);
      }),

    // Get all relationships for an agent
    byAgent: protectedProcedure
      .input(z.object({ agentId: z.number() }))
      .query(async ({ input }) => {
        return await getAgentRelationships(input.agentId);
      }),

    // Record interaction
    recordInteraction: protectedProcedure
      .input(
        z.object({
          agent1Id: z.number(),
          agent2Id: z.number(),
          strengthChange: z.number().optional(),
          positivityChange: z.number().optional(),
        })
      )
      .mutation(async ({ input }) => {
        await recordRelationshipInteraction(
          input.agent1Id,
          input.agent2Id,
          input.strengthChange,
          input.positivityChange
        );
        return { success: true };
      }),
  }),

  // Groups
  group: router({
    // Create group
    create: protectedProcedure
      .input(
        z.object({
          name: z.string().min(2).max(128),
          type: z.enum(["department", "team", "union", "association", "club", "network"]),
          description: z.string().optional(),
          cityId: z.number().optional(),
        })
      )
      .mutation(async ({ ctx, input }) => {
        const company = await getCompanyByUserId(ctx.user.id);
        
        return await createAgentGroup({
          name: input.name,
          type: input.type,
          description: input.description,
          companyId: company?.id,
          cityId: input.cityId,
        });
      }),

    // Get group by ID
    byId: protectedProcedure
      .input(z.object({ id: z.number() }))
      .query(async ({ input }) => {
        return await getAgentGroupById(input.id);
      }),

    // Get groups by company
    byCompany: protectedProcedure.query(async ({ ctx }) => {
      const company = await getCompanyByUserId(ctx.user.id);
      if (!company) return [];
      return await getAgentGroupsByCompany(company.id);
    }),

    // Get group members
    members: protectedProcedure
      .input(z.object({ groupId: z.number() }))
      .query(async ({ input }) => {
        return await getGroupMembers(input.groupId);
      }),

    // Add agent to group
    addMember: protectedProcedure
      .input(
        z.object({
          groupId: z.number(),
          agentId: z.number(),
          role: z.enum(["leader", "core_member", "member", "associate"]).optional(),
        })
      )
      .mutation(async ({ input }) => {
        await addAgentToGroup({
          groupId: input.groupId,
          agentId: input.agentId,
          role: input.role || "member",
        });
        return { success: true };
      }),

    // Remove agent from group
    removeMember: protectedProcedure
      .input(z.object({ groupId: z.number(), agentId: z.number() }))
      .mutation(async ({ input }) => {
        await removeAgentFromGroup(input.groupId, input.agentId);
        return { success: true };
      }),
  }),

  // Communities
  community: router({
    // Create community
    create: protectedProcedure
      .input(
        z.object({
          name: z.string().min(2).max(128),
          cityId: z.number(),
          type: z.enum(["residential", "business", "industrial", "cultural", "virtual"]),
          description: z.string().optional(),
        })
      )
      .mutation(async ({ input }) => {
        return await createCommunity({
          name: input.name,
          cityId: input.cityId,
          type: input.type,
          description: input.description,
        });
      }),

    // Get community by ID
    byId: protectedProcedure
      .input(z.object({ id: z.number() }))
      .query(async ({ input }) => {
        return await getCommunityById(input.id);
      }),

    // Get communities by city
    byCity: protectedProcedure
      .input(z.object({ cityId: z.number() }))
      .query(async ({ input }) => {
        return await getCommunitiesByCity(input.cityId);
      }),

    // Get community members
    members: protectedProcedure
      .input(z.object({ communityId: z.number() }))
      .query(async ({ input }) => {
        return await getCommunityMembers(input.communityId);
      }),

    // Add agent to community
    addMember: protectedProcedure
      .input(z.object({ communityId: z.number(), agentId: z.number() }))
      .mutation(async ({ input }) => {
        await addAgentToCommunity({
          communityId: input.communityId,
          agentId: input.agentId,
        });
        return { success: true };
      }),
  }),

  // Events
  event: router({
    // Create event
    create: protectedProcedure
      .input(
        z.object({
          type: z.enum([
            "interaction",
            "transaction",
            "milestone",
            "crisis",
            "celebration",
            "conflict",
            "negotiation",
            "collaboration",
          ]),
          initiatorAgentId: z.number(),
          targetAgentId: z.number().optional(),
          title: z.string().min(2).max(256),
          description: z.string().optional(),
          scheduledAt: z.date(),
          duration: z.number().optional(),
          emotionalImpact: z
            .object({
              happiness: z.number().optional(),
              satisfaction: z.number().optional(),
              stress: z.number().optional(),
              loyalty: z.number().optional(),
              trust: z.number().optional(),
            })
            .optional(),
          relationshipImpact: z
            .object({
              agentIds: z.array(z.number()),
              strengthChange: z.number(),
              positivityChange: z.number(),
            })
            .optional(),
        })
      )
      .mutation(async ({ input }) => {
        return await createAgentEvent({
          type: input.type,
          initiatorAgentId: input.initiatorAgentId,
          targetAgentId: input.targetAgentId,
          title: input.title,
          description: input.description,
          scheduledAt: input.scheduledAt,
          duration: input.duration,
          emotionalImpact: input.emotionalImpact,
          relationshipImpact: input.relationshipImpact,
        });
      }),

    // Get event by ID
    byId: protectedProcedure
      .input(z.object({ id: z.number() }))
      .query(async ({ input }) => {
        return await getAgentEventById(input.id);
      }),

    // Get events for an agent
    byAgent: protectedProcedure
      .input(z.object({ agentId: z.number() }))
      .query(async ({ input }) => {
        return await getAgentEvents(input.agentId);
      }),

    // Get scheduled events
    scheduled: protectedProcedure.query(async () => {
      return await getScheduledEvents();
    }),

    // Process/complete an event
    process: protectedProcedure
      .input(z.object({ eventId: z.number() }))
      .mutation(async ({ input }) => {
        await processAgentEvent(input.eventId);
        return { success: true };
      }),
  }),

  // ============================================================================
  // DREAMCOG INTEGRATION ROUTES
  // ============================================================================

  // Big Five Personality
  personality: router({
    // Get or create personality profile for agent
    get: protectedProcedure
      .input(z.object({ agentId: z.number() }))
      .query(async ({ input }) => {
        return await getAgentBigFivePersonality(input.agentId);
      }),

    // Create personality profile
    create: protectedProcedure
      .input(
        z.object({
          agentId: z.number(),
          openness: z.number().min(0).max(100).optional(),
          conscientiousness: z.number().min(0).max(100).optional(),
          extraversion: z.number().min(0).max(100).optional(),
          agreeableness: z.number().min(0).max(100).optional(),
          neuroticism: z.number().min(0).max(100).optional(),
          formalityLevel: z.number().min(0).max(100).optional(),
          verbosityLevel: z.number().min(0).max(100).optional(),
          emotionalExpression: z.number().min(0).max(100).optional(),
          humorLevel: z.number().min(0).max(100).optional(),
          directnessLevel: z.number().min(0).max(100).optional(),
          impulsiveness: z.number().min(0).max(100).optional(),
          riskTaking: z.number().min(0).max(100).optional(),
          empathy: z.number().min(0).max(100).optional(),
          leadership: z.number().min(0).max(100).optional(),
          independence: z.number().min(0).max(100).optional(),
        })
      )
      .mutation(async ({ input }) => {
        return await createAgentBigFivePersonality(input);
      }),

    // Update personality profile
    update: protectedProcedure
      .input(
        z.object({
          agentId: z.number(),
          updates: z.object({
            openness: z.number().min(0).max(100).optional(),
            conscientiousness: z.number().min(0).max(100).optional(),
            extraversion: z.number().min(0).max(100).optional(),
            agreeableness: z.number().min(0).max(100).optional(),
            neuroticism: z.number().min(0).max(100).optional(),
            formalityLevel: z.number().min(0).max(100).optional(),
            verbosityLevel: z.number().min(0).max(100).optional(),
            emotionalExpression: z.number().min(0).max(100).optional(),
            humorLevel: z.number().min(0).max(100).optional(),
            directnessLevel: z.number().min(0).max(100).optional(),
            impulsiveness: z.number().min(0).max(100).optional(),
            riskTaking: z.number().min(0).max(100).optional(),
            empathy: z.number().min(0).max(100).optional(),
            leadership: z.number().min(0).max(100).optional(),
            independence: z.number().min(0).max(100).optional(),
          }),
        })
      )
      .mutation(async ({ input }) => {
        await updateAgentBigFivePersonality(input.agentId, input.updates);
        return { success: true };
      }),
  }),

  // Agent Motivations
  motivation: router({
    // Create motivation
    create: protectedProcedure
      .input(
        z.object({
          agentId: z.number(),
          motivationType: z.enum(["short_term", "long_term", "core_value"]),
          description: z.string(),
          priority: z.number().min(1).max(10).optional(),
        })
      )
      .mutation(async ({ input }) => {
        return await createAgentMotivation(input);
      }),

    // Get agent motivations
    byAgent: protectedProcedure
      .input(
        z.object({
          agentId: z.number(),
          activeOnly: z.boolean().optional(),
        })
      )
      .query(async ({ input }) => {
        return await getAgentMotivations(input.agentId, input.activeOnly);
      }),

    // Update motivation
    update: protectedProcedure
      .input(
        z.object({
          id: z.number(),
          updates: z.object({
            description: z.string().optional(),
            priority: z.number().min(1).max(10).optional(),
            progress: z.number().min(0).max(100).optional(),
            isActive: z.boolean().optional(),
          }),
        })
      )
      .mutation(async ({ input }) => {
        await updateAgentMotivation(input.id, input.updates);
        return { success: true };
      }),
  }),

  // Agent Memories
  memory: router({
    // Create memory
    create: protectedProcedure
      .input(
        z.object({
          agentId: z.number(),
          memoryType: z.enum([
            "event",
            "interaction",
            "knowledge",
            "emotion",
            "skill",
            "trauma",
            "achievement",
          ]),
          content: z.string(),
          emotionalImpact: z.number().min(-100).max(100).optional(),
          importance: z.number().min(1).max(10).optional(),
          memoryDate: z.date(),
          eventId: z.number().optional(),
          relatedAgentId: z.number().optional(),
          locationId: z.number().optional(),
        })
      )
      .mutation(async ({ input }) => {
        return await createAgentMemory(input);
      }),

    // Get agent memories
    byAgent: protectedProcedure
      .input(
        z.object({
          agentId: z.number(),
          limit: z.number().optional(),
        })
      )
      .query(async ({ input }) => {
        return await getAgentMemories(input.agentId, input.limit);
      }),
  }),

  // Relationship Events
  relationshipEvent: router({
    // Create relationship event
    create: protectedProcedure
      .input(
        z.object({
          relationshipId: z.number(),
          eventType: z.enum([
            "first_meeting",
            "conflict",
            "bonding",
            "betrayal",
            "reconciliation",
            "milestone",
            "other",
          ]),
          description: z.string(),
          impactOnTrust: z.number().min(-100).max(100).optional(),
          impactOnAffection: z.number().min(-100).max(100).optional(),
          impactOnRespect: z.number().min(-100).max(100).optional(),
          eventDate: z.date(),
        })
      )
      .mutation(async ({ input }) => {
        return await createRelationshipEvent(input);
      }),

    // Get relationship events
    byRelationship: protectedProcedure
      .input(z.object({ relationshipId: z.number() }))
      .query(async ({ input }) => {
        return await getRelationshipEvents(input.relationshipId);
      }),
  }),

  // Worlds
  world: router({
    // Create world
    create: protectedProcedure
      .input(
        z.object({
          name: z.string().min(2).max(128),
          description: z.string().optional(),
          genre: z.string().optional(),
          timePeriod: z.string().optional(),
          technologyLevel: z.string().optional(),
          magicSystem: z.string().optional(),
          culturalNotes: z.string().optional(),
          rules: z
            .object({
              physicsRules: z.array(z.string()).optional(),
              socialRules: z.array(z.string()).optional(),
              magicRules: z.array(z.string()).optional(),
            })
            .optional(),
          isPublic: z.boolean().optional(),
        })
      )
      .mutation(async ({ ctx, input }) => {
        return await createWorld({
          ...input,
          userId: ctx.user.id,
        });
      }),

    // Get world by ID
    byId: protectedProcedure
      .input(z.object({ id: z.number() }))
      .query(async ({ input }) => {
        return await getWorldById(input.id);
      }),

    // Get user's worlds
    list: protectedProcedure.query(async ({ ctx }) => {
      return await getWorldsByUserId(ctx.user.id);
    }),

    // Update world
    update: protectedProcedure
      .input(
        z.object({
          id: z.number(),
          updates: z.object({
            name: z.string().optional(),
            description: z.string().optional(),
            genre: z.string().optional(),
            timePeriod: z.string().optional(),
            technologyLevel: z.string().optional(),
            magicSystem: z.string().optional(),
            culturalNotes: z.string().optional(),
            rules: z
              .object({
                physicsRules: z.array(z.string()).optional(),
                socialRules: z.array(z.string()).optional(),
                magicRules: z.array(z.string()).optional(),
              })
              .optional(),
            isPublic: z.boolean().optional(),
          }),
        })
      )
      .mutation(async ({ ctx, input }) => {
        const world = await getWorldById(input.id);
        if (!world) {
          throw new Error("World not found");
        }
        if (world.userId !== ctx.user.id) {
          throw new Error("Not authorized to update this world");
        }
        await updateWorld(input.id, input.updates);
        return { success: true };
      }),
  }),

  // Locations
  location: router({
    // Create location
    create: protectedProcedure
      .input(
        z.object({
          worldId: z.number(),
          name: z.string().min(2).max(128),
          locationType: z.enum([
            "city",
            "building",
            "wilderness",
            "dungeon",
            "realm",
            "dimension",
            "other",
          ]),
          description: z.string().optional(),
          parentLocationId: z.number().optional(),
          attributes: z
            .object({
              climate: z.string().optional(),
              population: z.string().optional(),
              dangerLevel: z.number().optional(),
              resources: z.array(z.string()).optional(),
              notableFeatures: z.array(z.string()).optional(),
            })
            .optional(),
        })
      )
      .mutation(async ({ input }) => {
        return await createLocation(input);
      }),

    // Get location by ID
    byId: protectedProcedure
      .input(z.object({ id: z.number() }))
      .query(async ({ input }) => {
        return await getLocationById(input.id);
      }),

    // Get locations by world
    byWorld: protectedProcedure
      .input(z.object({ worldId: z.number() }))
      .query(async ({ input }) => {
        return await getLocationsByWorldId(input.worldId);
      }),

    // Get sub-locations
    subLocations: protectedProcedure
      .input(z.object({ parentLocationId: z.number() }))
      .query(async ({ input }) => {
        return await getSubLocations(input.parentLocationId);
      }),
  }),

  // Lore Entries
  lore: router({
    // Create lore entry
    create: protectedProcedure
      .input(
        z.object({
          worldId: z.number(),
          category: z.enum([
            "history",
            "legend",
            "culture",
            "religion",
            "politics",
            "science",
            "magic",
            "species",
            "language",
            "artifact",
            "other",
          ]),
          title: z.string().min(2).max(256),
          content: z.string(),
          isPublic: z.boolean().optional(),
          isSecret: z.boolean().optional(),
          relatedLocationId: z.number().optional(),
          relatedAgentId: z.number().optional(),
          tags: z.array(z.string()).optional(),
        })
      )
      .mutation(async ({ input }) => {
        return await createLoreEntry(input);
      }),

    // Get lore entry by ID
    byId: protectedProcedure
      .input(z.object({ id: z.number() }))
      .query(async ({ input }) => {
        return await getLoreEntryById(input.id);
      }),

    // Get lore entries by world
    byWorld: protectedProcedure
      .input(
        z.object({
          worldId: z.number(),
          category: z.string().optional(),
        })
      )
      .query(async ({ input }) => {
        return await getLoreEntriesByWorldId(input.worldId, input.category);
      }),
  }),

  // World Events
  worldEvent: router({
    // Create world event
    create: protectedProcedure
      .input(
        z.object({
          worldId: z.number(),
          title: z.string().min(2).max(256),
          description: z.string().optional(),
          eventType: z.enum([
            "battle",
            "discovery",
            "political",
            "natural",
            "magical",
            "social",
            "economic",
            "other",
          ]),
          importance: z.number().min(1).max(10).optional(),
          eventDate: z.string(),
          duration: z.string().optional(),
          locationId: z.number().optional(),
          involvedAgentIds: z.array(z.number()).optional(),
          involvedGroupIds: z.array(z.number()).optional(),
          consequences: z.string().optional(),
        })
      )
      .mutation(async ({ input }) => {
        return await createWorldEvent(input);
      }),

    // Get world event by ID
    byId: protectedProcedure
      .input(z.object({ id: z.number() }))
      .query(async ({ input }) => {
        return await getWorldEventById(input.id);
      }),

    // Get world events by world
    byWorld: protectedProcedure
      .input(z.object({ worldId: z.number() }))
      .query(async ({ input }) => {
        return await getWorldEventsByWorldId(input.worldId);
      }),
  }),

  // Scheduled World Events
  scheduledWorldEvent: router({
    // Create scheduled world event
    create: protectedProcedure
      .input(
        z.object({
          worldId: z.number(),
          eventName: z.string().min(2).max(256),
          description: z.string().optional(),
          scheduledFor: z.date(),
          eventTrigger: z
            .object({
              triggerType: z.enum(["time", "condition", "manual"]),
              conditions: z
                .array(
                  z.object({
                    type: z.enum(["threshold", "time", "relationship", "emotion"]),
                    metric: z.string(),
                    operator: z.enum([">", "<", "==", "!=", "between"]),
                    value: z.union([z.number(), z.string()]),
                  })
                )
                .optional(),
            })
            .optional(),
          targetAgentIds: z.array(z.number()).optional(),
          targetLocationId: z.number().optional(),
          priority: z.number().min(1).max(10).optional(),
          isRecurring: z.boolean().optional(),
        })
      )
      .mutation(async ({ input }) => {
        return await createScheduledWorldEvent(input);
      }),

    // Get scheduled world event by ID
    byId: protectedProcedure
      .input(z.object({ id: z.number() }))
      .query(async ({ input }) => {
        return await getScheduledWorldEventById(input.id);
      }),

    // Get pending scheduled events
    pending: protectedProcedure
      .input(z.object({ worldId: z.number() }))
      .query(async ({ input }) => {
        return await getPendingScheduledWorldEvents(input.worldId);
      }),

    // Update scheduled event
    update: protectedProcedure
      .input(
        z.object({
          id: z.number(),
          updates: z.object({
            status: z.enum(["pending", "active", "completed", "cancelled"]).optional(),
            scheduledFor: z.date().optional(),
          }),
        })
      )
      .mutation(async ({ input }) => {
        await updateScheduledWorldEvent(input.id, input.updates);
        return { success: true };
      }),
  }),

  // ============================================================================
  // TRANSACTION ROUTES
  // ============================================================================
  transaction: router({
    list: protectedProcedure
      .input(z.object({ limit: z.number().min(1).max(100).optional() }).optional())
      .query(async ({ ctx, input }) => {
        const company = await getCompanyByUserId(ctx.user.id);
        if (!company) return [];
        return await getTransactionsByCompany(company.id, input?.limit || 50);
      }),
  }),

  // ============================================================================
  // NOTIFICATION ROUTES
  // ============================================================================
  notification: router({
    list: protectedProcedure.query(async ({ ctx }) => {
      return await getNotificationsByUser(ctx.user.id);
    }),

    markRead: protectedProcedure
      .input(z.object({ id: z.number() }))
      .mutation(async ({ input }) => {
        await markNotificationRead(input.id);
        return { success: true };
      }),
  }),

  // ============================================================================
  // GAME STATE ROUTES
  // ============================================================================
  game: router({
    state: protectedProcedure.query(async () => {
      return await getGameState();
    }),
  }),

  // ============================================================================
  // API KEYS ROUTES (DreamCog)
  // ============================================================================
  apiKeys: router({
    list: protectedProcedure.query(async ({ ctx }) => {
      return await getApiKeysByUserId(ctx.user.id);
    }),
    create: protectedProcedure
      .input(z.object({ keyName: z.string(), encryptedKey: z.string() }))
      .mutation(async ({ ctx, input }) => {
        return await createApiKey({ userId: ctx.user.id, ...input });
      }),
    delete: protectedProcedure
      .input(z.object({ id: z.number() }))
      .mutation(async ({ ctx, input }) => {
        await deleteApiKey(input.id, ctx.user.id);
        return { success: true };
      }),
    verify: protectedProcedure
      .input(z.object({ id: z.number() }))
      .mutation(async ({ ctx, input }) => {
        return await verifyApiKey(input.id, ctx.user.id);
      }),
  }),

  // ============================================================================
  // CHARACTERS ROUTES (DreamCog)
  // ============================================================================
  characters: router({
    list: protectedProcedure.query(async ({ ctx }) => {
      return await getStoryCharactersByUserId(ctx.user.id);
    }),
    create: protectedProcedure
      .input(z.object({
        name: z.string(),
        label: z.string(),
        promptDescription: z.string().optional(),
        displayDescription: z.string().optional(),
        imageUrl: z.string().optional(),
        isUserCharacter: z.boolean().optional(),
      }))
      .mutation(async ({ ctx, input }) => {
        return await createStoryCharacter({ userId: ctx.user.id, ...input });
      }),
    update: protectedProcedure
      .input(z.object({
        id: z.number(),
        name: z.string().optional(),
        label: z.string().optional(),
        promptDescription: z.string().optional(),
        displayDescription: z.string().optional(),
        imageUrl: z.string().optional(),
      }))
      .mutation(async ({ ctx, input }) => {
        const { id, ...data } = input;
        await updateStoryCharacter(id, ctx.user.id, data);
        return { success: true };
      }),
    delete: protectedProcedure
      .input(z.object({ id: z.number() }))
      .mutation(async ({ ctx, input }) => {
        await deleteStoryCharacter(input.id, ctx.user.id);
        return { success: true };
      }),
  }),

  // ============================================================================
  // SCENARIOS ROUTES (DreamCog)
  // ============================================================================
  scenarios: router({
    list: protectedProcedure.query(async ({ ctx }) => {
      return await getScenariosByUserId(ctx.user.id);
    }),
    public: publicProcedure
      .input(z.object({ search: z.string().optional() }).optional())
      .query(async ({ input }) => {
        return await getPublicScenarios(input?.search);
      }),
    byId: protectedProcedure
      .input(z.object({ id: z.number() }))
      .query(async ({ input }) => {
        return await getScenarioById(input.id);
      }),
    create: protectedProcedure
      .input(z.object({
        title: z.string(),
        promptDescription: z.string().optional(),
        displayDescription: z.string().optional(),
        imageUrl: z.string().optional(),
        isPublic: z.boolean().optional(),
        worldId: z.number().optional(),
      }))
      .mutation(async ({ ctx, input }) => {
        return await createScenario({ userId: ctx.user.id, ...input });
      }),
    update: protectedProcedure
      .input(z.object({
        id: z.number(),
        title: z.string().optional(),
        promptDescription: z.string().optional(),
        displayDescription: z.string().optional(),
        imageUrl: z.string().optional(),
        isPublic: z.boolean().optional(),
      }))
      .mutation(async ({ ctx, input }) => {
        const { id, ...data } = input;
        await updateScenario(id, ctx.user.id, data);
        return { success: true };
      }),
    delete: protectedProcedure
      .input(z.object({ id: z.number() }))
      .mutation(async ({ ctx, input }) => {
        await deleteScenario(input.id, ctx.user.id);
        return { success: true };
      }),
    characters: protectedProcedure
      .input(z.object({ scenarioId: z.number() }))
      .query(async ({ input }) => {
        return await getScenarioCharacters(input.scenarioId);
      }),
    interactions: protectedProcedure
      .input(z.object({ scenarioId: z.number() }))
      .query(async ({ input }) => {
        return await getScenarioInteractions(input.scenarioId);
      }),

    // Add character to scenario
    addCharacter: protectedProcedure
      .input(z.object({
        scenarioId: z.number(),
        name: z.string().min(1),
        label: z.string().min(1).regex(/^[a-z0-9_]+$/, "Label must be lowercase letters, numbers, and underscores"),
        promptDescription: z.string().optional(),
        isUserCharacter: z.boolean().optional(),
        orderIndex: z.number().optional(),
      }))
      .mutation(async ({ ctx, input }) => {
        const scenario = await getScenarioById(input.scenarioId);
        if (!scenario || scenario.userId !== ctx.user.id) {
          throw new Error("Scenario not found or not authorized");
        }
        return await addScenarioCharacter(input);
      }),

    // Update scenario character
    updateCharacter: protectedProcedure
      .input(z.object({
        id: z.number(),
        scenarioId: z.number(),
        name: z.string().min(1).optional(),
        label: z.string().min(1).regex(/^[a-z0-9_]+$/, "Label must be lowercase letters, numbers, and underscores").optional(),
        promptDescription: z.string().optional(),
        isUserCharacter: z.boolean().optional(),
        orderIndex: z.number().optional(),
      }))
      .mutation(async ({ ctx, input }) => {
        const scenario = await getScenarioById(input.scenarioId);
        if (!scenario || scenario.userId !== ctx.user.id) {
          throw new Error("Scenario not found or not authorized");
        }
        const { id, scenarioId, ...data } = input;
        await updateScenarioCharacter(id, data);
        return { success: true };
      }),

    // Remove character from scenario
    removeCharacter: protectedProcedure
      .input(z.object({
        id: z.number(),
        scenarioId: z.number(),
      }))
      .mutation(async ({ ctx, input }) => {
        const scenario = await getScenarioById(input.scenarioId);
        if (!scenario || scenario.userId !== ctx.user.id) {
          throw new Error("Scenario not found or not authorized");
        }
        await deleteScenarioCharacter(input.id);
        return { success: true };
      }),

    // Add interaction to scenario
    addInteraction: protectedProcedure
      .input(z.object({
        scenarioId: z.number(),
        interactionType: z.enum(["message", "text", "instruction"]),
        characterLabel: z.string().optional(),
        content: z.string().min(1),
        isSticky: z.boolean().optional(),
        orderIndex: z.number().optional(),
      }))
      .mutation(async ({ ctx, input }) => {
        const scenario = await getScenarioById(input.scenarioId);
        if (!scenario || scenario.userId !== ctx.user.id) {
          throw new Error("Scenario not found or not authorized");
        }
        return await addScenarioInteraction(input);
      }),

    // Update scenario interaction
    updateInteraction: protectedProcedure
      .input(z.object({
        id: z.number(),
        scenarioId: z.number(),
        interactionType: z.enum(["message", "text", "instruction"]).optional(),
        characterLabel: z.string().optional(),
        content: z.string().min(1).optional(),
        isSticky: z.boolean().optional(),
        orderIndex: z.number().optional(),
      }))
      .mutation(async ({ ctx, input }) => {
        const scenario = await getScenarioById(input.scenarioId);
        if (!scenario || scenario.userId !== ctx.user.id) {
          throw new Error("Scenario not found or not authorized");
        }
        const { id, scenarioId, ...data } = input;
        await updateScenarioInteraction(id, data);
        return { success: true };
      }),

    // Remove interaction from scenario
    removeInteraction: protectedProcedure
      .input(z.object({
        id: z.number(),
        scenarioId: z.number(),
      }))
      .mutation(async ({ ctx, input }) => {
        const scenario = await getScenarioById(input.scenarioId);
        if (!scenario || scenario.userId !== ctx.user.id) {
          throw new Error("Scenario not found or not authorized");
        }
        await deleteScenarioInteraction(input.id);
        return { success: true };
      }),
  }),

  // ============================================================================
  // CHAT ROUTES (DreamCog)
  // ============================================================================
  chat: router({
    sessions: protectedProcedure.query(async ({ ctx }) => {
      return await getChatSessionsByUserId(ctx.user.id);
    }),
    sessionById: protectedProcedure
      .input(z.object({ id: z.number() }))
      .query(async ({ ctx, input }) => {
        return await getChatSessionById(input.id, ctx.user.id);
      }),
    createSession: protectedProcedure
      .input(z.object({
        title: z.string(),
        scenarioId: z.number().optional(),
        worldId: z.number().optional(),
        systemPrompt: z.string().optional(),
        modelId: z.string().optional(),
      }))
      .mutation(async ({ ctx, input }) => {
        return await createChatSession({ userId: ctx.user.id, ...input });
      }),
    updateSession: protectedProcedure
      .input(z.object({
        id: z.number(),
        title: z.string().optional(),
      }))
      .mutation(async ({ ctx, input }) => {
        const { id, ...data } = input;
        await updateChatSession(id, ctx.user.id, data);
        return { success: true };
      }),
    deleteSession: protectedProcedure
      .input(z.object({ id: z.number() }))
      .mutation(async ({ ctx, input }) => {
        await deleteChatSession(input.id, ctx.user.id);
        return { success: true };
      }),
    messages: protectedProcedure
      .input(z.object({ sessionId: z.number() }))
      .query(async ({ input }) => {
        return await getChatMessages(input.sessionId);
      }),
    addMessage: protectedProcedure
      .input(z.object({
        sessionId: z.number(),
        messageType: z.enum(["message", "text", "instruction", "user", "system"]),
        content: z.string(),
        characterLabel: z.string().optional(),
        characterName: z.string().optional(),
        isSticky: z.boolean().optional(),
      }))
      .mutation(async ({ input }) => {
        return await addChatMessage(input);
      }),
  }),

  // ============================================================================
  // STORIES ROUTES (DreamCog)
  // ============================================================================
  stories: router({
    list: protectedProcedure.query(async ({ ctx }) => {
      return await getStoriesByUserId(ctx.user.id);
    }),
    byId: protectedProcedure
      .input(z.object({ id: z.number() }))
      .query(async ({ ctx, input }) => {
        return await getStoryById(input.id, ctx.user.id);
      }),
    create: protectedProcedure
      .input(z.object({
        title: z.string(),
        content: z.string().optional(),
        worldId: z.number().optional(),
      }))
      .mutation(async ({ ctx, input }) => {
        return await createStory({ userId: ctx.user.id, ...input });
      }),
    update: protectedProcedure
      .input(z.object({
        id: z.number(),
        title: z.string().optional(),
        content: z.string().optional(),
      }))
      .mutation(async ({ ctx, input }) => {
        const { id, ...data } = input;
        await updateStory(id, ctx.user.id, data);
        return { success: true };
      }),
    delete: protectedProcedure
      .input(z.object({ id: z.number() }))
      .mutation(async ({ ctx, input }) => {
        await deleteStory(input.id, ctx.user.id);
        return { success: true };
      }),
    characters: protectedProcedure
      .input(z.object({ storyId: z.number() }))
      .query(async ({ input }) => {
        return await getStoryCharacterLinks(input.storyId);
      }),
  }),

  // ============================================================================
  // IMAGE GENERATION ROUTES (DreamCog)
  // ============================================================================
  images: router({
    list: protectedProcedure
      .input(z.object({ limit: z.number().optional() }).optional())
      .query(async ({ ctx, input }) => {
        return await getGeneratedImagesByUserId(ctx.user.id, input?.limit);
      }),
    create: protectedProcedure
      .input(z.object({
        includePrompt: z.string(),
        excludePrompt: z.string().optional(),
        cfgScale: z.number().optional(),
        fidelity: z.number().optional(),
        aspectRatio: z.string().optional(),
        style: z.string().optional(),
        seed: z.number().optional(),
        imageUrl: z.string().optional(),
        characterId: z.number().optional(),
        scenarioId: z.number().optional(),
      }))
      .mutation(async ({ ctx, input }) => {
        return await createGeneratedImage({ userId: ctx.user.id, ...input });
      }),
    delete: protectedProcedure
      .input(z.object({ id: z.number() }))
      .mutation(async ({ ctx, input }) => {
        await deleteGeneratedImage(input.id, ctx.user.id);
        return { success: true };
      }),
  }),

  // ============================================================================
  // EVENT BRIDGE ROUTES (Cross-system event propagation)
  // ============================================================================
  eventBridge: router({
    // Get event propagation history
    history: protectedProcedure
      .input(z.object({ limit: z.number().optional() }).optional())
      .query(async ({ input }) => {
        return await getEventPropagationHistory(input?.limit);
      }),

    // Get propagation history by source type
    bySourceType: protectedProcedure
      .input(z.object({
        sourceType: z.enum(["business", "narrative"]),
        limit: z.number().optional(),
      }))
      .query(async ({ input }) => {
        return await getEventPropagationBySourceType(input.sourceType, input.limit);
      }),

    // Propagate a business event to narrative
    propagateBusinessEvent: protectedProcedure
      .input(z.object({
        type: z.enum(["bankruptcy", "merger", "market_crash", "expansion", "layoff", "innovation", "scandal", "success"]),
        companyId: z.number(),
        magnitude: z.number().min(1).max(100),
        description: z.string(),
        affectedResources: z.array(z.number()).optional(),
        affectedCities: z.array(z.number()).optional(),
      }))
      .mutation(async ({ ctx, input }) => {
        const company = await getCompanyByUserId(ctx.user.id);
        if (!company || company.id !== input.companyId) {
          throw new Error("Not authorized to propagate events for this company");
        }
        return await eventBridge.propagateBusinessEvent(input as BusinessEvent);
      }),

    // Propagate a narrative event to business
    propagateNarrativeEvent: protectedProcedure
      .input(z.object({
        type: z.enum(["conflict", "alliance", "betrayal", "discovery", "crisis", "celebration", "tragedy"]),
        worldId: z.number(),
        importance: z.number().min(1).max(100),
        description: z.string(),
        affectedAgentIds: z.array(z.number()).optional(),
      }))
      .mutation(async ({ ctx, input }) => {
        const world = await getWorldById(input.worldId);
        if (!world || world.userId !== ctx.user.id) {
          throw new Error("Not authorized to propagate events for this world");
        }
        return await eventBridge.propagateNarrativeEvent(input as NarrativeEvent);
      }),

    // Process scheduled events
    processScheduledEvents: protectedProcedure
      .mutation(async () => {
        await eventBridge.processScheduledEvents();
        return { success: true };
      }),
  }),

  // ============================================================================
  // SIMULATION ENGINE ROUTES (Sims-inspired life mechanics)
  // ============================================================================
  simulation: router({
    // Get agent simulation state (needs, skills, mood)
    state: protectedProcedure
      .input(z.object({ agentId: z.number() }))
      .query(async ({ input }) => {
        return await simulationEngine.getSimulationState(input.agentId);
      }),

    // Get available actions for an agent
    availableActions: protectedProcedure
      .input(z.object({ agentId: z.number() }))
      .query(async ({ input }) => {
        const state = await simulationEngine.getSimulationState(input.agentId);
        if (!state) return [];
        return simulationEngine.getAvailableActions(state);
      }),

    // Get recommended actions based on current needs
    recommendedActions: protectedProcedure
      .input(z.object({ agentId: z.number() }))
      .query(async ({ input }) => {
        const state = await simulationEngine.getSimulationState(input.agentId);
        if (!state) return [];
        return simulationEngine.getRecommendedActions(state);
      }),

    // Execute an action for an agent
    executeAction: protectedProcedure
      .input(z.object({
        agentId: z.number(),
        actionId: z.string(),
      }))
      .mutation(async ({ ctx, input }) => {
        const agent = await getAgentById(input.agentId);
        if (!agent) throw new Error("Agent not found");

        // Verify ownership if agent belongs to a company
        if (agent.companyId) {
          const company = await getCompanyByUserId(ctx.user.id);
          if (!company || agent.companyId !== company.id) {
            throw new Error("Not authorized");
          }
        }

        return await simulationEngine.executeAction(input.agentId, input.actionId);
      }),

    // Simulate time passage for an agent
    simulateTime: protectedProcedure
      .input(z.object({
        agentId: z.number(),
        minutes: z.number().min(1).max(1440), // max 24 hours
      }))
      .mutation(async ({ ctx, input }) => {
        const agent = await getAgentById(input.agentId);
        if (!agent) throw new Error("Agent not found");

        if (agent.companyId) {
          const company = await getCompanyByUserId(ctx.user.id);
          if (!company || agent.companyId !== company.id) {
            throw new Error("Not authorized");
          }
        }

        return await simulationEngine.simulateTimePassage(input.agentId, input.minutes);
      }),

    // Generate an autonomous action recommendation
    generateAutonomousAction: protectedProcedure
      .input(z.object({ agentId: z.number() }))
      .query(async ({ input }) => {
        return await simulationEngine.generateAutonomousAction(input.agentId);
      }),

    // Get all available simulation actions (static list)
    allActions: publicProcedure.query(() => {
      return simulationEngine.SIMULATION_ACTIONS;
    }),
  }),

  // ============================================================================
  // TECHNOLOGY RESEARCH ROUTES
  // ============================================================================
  technology: router({
    // List all available technologies
    list: protectedProcedure.query(async () => {
      return await getAllTechnologies();
    }),

    // Get specific technology details
    byId: protectedProcedure
      .input(z.object({ id: z.number() }))
      .query(async ({ input }) => {
        return await getTechnologyById(input.id);
      }),

    // Get company's researched and in-progress technologies
    companyTechnologies: protectedProcedure.query(async ({ ctx }) => {
      const company = await getCompanyByUserId(ctx.user.id);
      if (!company) return [];
      return await getCompanyTechnologies(company.id);
    }),

    // Start researching a technology
    startResearch: protectedProcedure
      .input(z.object({ technologyId: z.number() }))
      .mutation(async ({ ctx, input }) => {
        const company = await getCompanyByUserId(ctx.user.id);
        if (!company) throw new Error("Company not found");

        // Check if technology exists
        const tech = await getTechnologyById(input.technologyId);
        if (!tech) throw new Error("Technology not found");

        // Check prerequisites (prerequisites is an array of tech IDs)
        if (tech.prerequisites && tech.prerequisites.length > 0) {
          for (const prereqId of tech.prerequisites) {
            const hasPrereq = await hasCompanyResearchedTech(company.id, prereqId);
            if (!hasPrereq) throw new Error("Prerequisite technology not researched");
          }
        }

        // Check research cost against company cash
        const researchCost = tech.researchCost;
        if (parseFloat(company.cash) < researchCost) {
          throw new Error("Insufficient funds for research");
        }

        // Deduct research cost
        const newCash = parseFloat(company.cash) - researchCost;
        await updateCompanyCash(company.id, newCash.toFixed(2));

        // Create transaction
        await createTransaction({
          companyId: company.id,
          type: "other",
          amount: researchCost.toFixed(2),
          description: `Research started: ${tech.name}`,
        });

        return await startTechnologyResearch(company.id, input.technologyId);
      }),

    // Check if company has researched a technology
    hasResearched: protectedProcedure
      .input(z.object({ technologyId: z.number() }))
      .query(async ({ ctx, input }) => {
        const company = await getCompanyByUserId(ctx.user.id);
        if (!company) return false;
        return await hasCompanyResearchedTech(company.id, input.technologyId);
      }),
  }),

  // ============================================================================
  // GAME TURN PROCESSING ROUTES
  // ============================================================================
  gameTurn: router({
    // Advance the game by one turn (processes production, payroll, research)
    advance: protectedProcedure.mutation(async ({ ctx }) => {
      // Only allow admin or owner to advance turns
      const company = await getCompanyByUserId(ctx.user.id);
      if (!company) throw new Error("Company not found");

      return await processTurnAdvancement();
    }),

    // Process payroll for the current user's company
    processPayroll: protectedProcedure.mutation(async ({ ctx }) => {
      const company = await getCompanyByUserId(ctx.user.id);
      if (!company) throw new Error("Company not found");

      return await processCompanyPayroll(company.id);
    }),

    // Get ready production items for processing
    readyProduction: protectedProcedure.query(async ({ ctx }) => {
      const company = await getCompanyByUserId(ctx.user.id);
      if (!company) return [];

      const units = await getBusinessUnitsByCompany(company.id);
      const unitIds = units.map(u => u.id);

      const allReady = await getReadyProductionItems();
      return allReady.filter(item => unitIds.includes(item.queue.businessUnitId));
    }),

    // Complete a specific production item
    completeProduction: protectedProcedure
      .input(z.object({ queueItemId: z.number() }))
      .mutation(async ({ ctx, input }) => {
        const company = await getCompanyByUserId(ctx.user.id);
        if (!company) throw new Error("Company not found");

        // Verify ownership
        const allReady = await getReadyProductionItems();
        const item = allReady.find(i => i.queue.id === input.queueItemId);
        if (!item) throw new Error("Production item not found or not ready");

        const units = await getBusinessUnitsByCompany(company.id);
        if (!units.some(u => u.id === item.queue.businessUnitId)) {
          throw new Error("Not authorized to complete this production");
        }

        return await completeProductionItem(input.queueItemId);
      }),
  }),
});

export type AppRouter = typeof appRouter;

import { describe, it, expect, vi, beforeEach } from "vitest";
import { appRouter } from "./routers";

// Mock the database functions
vi.mock("./db", () => ({
  initializeGameData: vi.fn().mockResolvedValue(undefined),
  seedProductionRecipes: vi.fn().mockResolvedValue(undefined),
  getCompanyByUserId: vi.fn(),
  createCompany: vi.fn(),
  getAllCompanies: vi.fn(),
  getBusinessUnitsByCompany: vi.fn(),
  createBusinessUnit: vi.fn(),
  getBusinessUnitById: vi.fn(),
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

describe("Game API Routes", () => {
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

  describe("company.mine", () => {
    it("returns null when user has no company", async () => {
      vi.mocked(db.getCompanyByUserId).mockResolvedValue(null);
      
      const result = await caller.company.mine();
      
      expect(result).toBeNull();
      expect(db.getCompanyByUserId).toHaveBeenCalledWith(mockUser.id);
    });

    it("returns company when user has one", async () => {
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
      
      vi.mocked(db.getCompanyByUserId).mockResolvedValue(mockCompany);
      
      const result = await caller.company.mine();
      
      expect(result).toEqual(mockCompany);
    });
  });

  describe("company.create", () => {
    it("creates a new company for user without one", async () => {
      vi.mocked(db.getCompanyByUserId).mockResolvedValue(null);
      vi.mocked(db.createCompany).mockResolvedValue({
        id: 1,
        userId: mockUser.id,
        name: "New Corp",
        description: "My new company",
        cash: "1000000.00",
        reputation: "50.00",
        totalAssets: "1000000.00",
        createdAt: new Date(),
      });
      
      const result = await caller.company.create({
        name: "New Corp",
        description: "My new company",
      });
      
      expect(result).toBeDefined();
      expect(db.createCompany).toHaveBeenCalledWith({
        userId: mockUser.id,
        name: "New Corp",
        description: "My new company",
        cash: "1000000.00",
      });
    });

    it("throws error when user already has a company", async () => {
      vi.mocked(db.getCompanyByUserId).mockResolvedValue({
        id: 1,
        userId: mockUser.id,
        name: "Existing Corp",
        description: null,
        cash: "1000000.00",
        reputation: "50.00",
        totalAssets: "1000000.00",
        createdAt: new Date(),
      });
      
      await expect(caller.company.create({
        name: "New Corp",
      })).rejects.toThrow("You already have a company");
    });
  });

  describe("city.list", () => {
    it("returns all cities", async () => {
      const mockCities = [
        { id: 1, name: "New York", country: "USA", population: 8000000, wealthIndex: "1.2000", taxRate: "0.2500", latitude: "40.7128", longitude: "-74.0060" },
        { id: 2, name: "London", country: "UK", population: 9000000, wealthIndex: "1.1500", taxRate: "0.2200", latitude: "51.5074", longitude: "-0.1278" },
      ];
      
      vi.mocked(db.getAllCities).mockResolvedValue(mockCities);
      
      const result = await caller.city.list();
      
      expect(result).toEqual(mockCities);
      expect(result.length).toBe(2);
    });
  });

  describe("resource.list", () => {
    it("returns all resource types", async () => {
      const mockResources = [
        { id: 1, name: "Iron Ore", category: "raw_materials", basePrice: "50.00", unit: "ton", icon: "⛏️", description: null },
        { id: 2, name: "Steel", category: "manufactured", basePrice: "150.00", unit: "ton", icon: "🔩", description: null },
      ];
      
      vi.mocked(db.getAllResourceTypes).mockResolvedValue(mockResources);
      
      const result = await caller.resource.list();
      
      expect(result).toEqual(mockResources);
      expect(result.length).toBe(2);
    });
  });

  describe("market.listings", () => {
    it("returns market listings", async () => {
      const mockListings = [
        {
          listing: { id: 1, companyId: 1, businessUnitId: 1, resourceTypeId: 1, quantity: "100.0000", quality: "0.8500", pricePerUnit: "55.00", cityId: 1, isActive: true, createdAt: new Date(), expiresAt: null },
          company: { id: 1, name: "Seller Corp" },
          resourceType: { id: 1, name: "Iron Ore", icon: "⛏️", unit: "ton" },
          city: { id: 1, name: "New York" },
        },
      ];
      
      vi.mocked(db.getMarketListings).mockResolvedValue(mockListings);
      
      const result = await caller.market.listings();
      
      expect(result).toEqual(mockListings);
    });

    it("filters listings by resource type", async () => {
      vi.mocked(db.getMarketListings).mockResolvedValue([]);
      
      await caller.market.listings({ resourceTypeId: 1 });
      
      expect(db.getMarketListings).toHaveBeenCalledWith(1, undefined);
    });
  });

  describe("production.recipes", () => {
    it("returns all production recipes", async () => {
      const mockRecipes = [
        { id: 1, unitType: "factory", outputResourceId: 2, outputQuantity: "50.0000", inputResources: [], laborRequired: 10, timeRequired: 1, description: "Steel Production" },
      ];
      
      vi.mocked(db.getProductionRecipes).mockResolvedValue(mockRecipes as any);
      
      const result = await caller.production.recipes();
      
      expect(result).toEqual(mockRecipes);
    });

    it("filters recipes by unit type", async () => {
      vi.mocked(db.getProductionRecipes).mockResolvedValue([]);
      
      await caller.production.recipes({ unitType: "factory" });
      
      expect(db.getProductionRecipes).toHaveBeenCalledWith("factory");
    });
  });

  describe("notification.list", () => {
    it("returns user notifications", async () => {
      const mockNotifications = [
        { id: 1, userId: mockUser.id, type: "info", title: "Welcome", message: "Welcome to Virtunomics!", isRead: false, createdAt: new Date() },
      ];
      
      vi.mocked(db.getNotificationsByUser).mockResolvedValue(mockNotifications);
      
      const result = await caller.notification.list();
      
      expect(result).toEqual(mockNotifications);
      expect(db.getNotificationsByUser).toHaveBeenCalledWith(mockUser.id);
    });
  });

  describe("game.state", () => {
    it("returns current game state", async () => {
      const mockState = {
        id: 1,
        currentTurn: 1,
        turnStartedAt: new Date(),
        turnDuration: 3600,
        serverTime: new Date(),
        isActive: true,
      };
      
      vi.mocked(db.getGameState).mockResolvedValue(mockState);
      
      const result = await caller.game.state();
      
      expect(result).toEqual(mockState);
    });
  });
});

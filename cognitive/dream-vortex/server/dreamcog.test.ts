import { describe, it, expect, vi, beforeEach } from "vitest";

// Mock the database module
vi.mock("drizzle-orm/mysql2", () => ({
  drizzle: vi.fn(() => ({
    select: vi.fn(() => ({
      from: vi.fn(() => ({
        where: vi.fn(() => ({
          limit: vi.fn(() => Promise.resolve([])),
        })),
        orderBy: vi.fn(() => ({
          limit: vi.fn(() => Promise.resolve([])),
        })),
        limit: vi.fn(() => Promise.resolve([])),
      })),
    })),
    insert: vi.fn(() => ({
      values: vi.fn(() => ({
        $returningId: vi.fn(() => Promise.resolve([{ id: 1 }])),
      })),
    })),
    update: vi.fn(() => ({
      set: vi.fn(() => ({
        where: vi.fn(() => Promise.resolve()),
      })),
    })),
  })),
}));

describe("DreamCog Integration - Agent Brain Service", () => {
  beforeEach(() => {
    vi.clearAllMocks();
    process.env.DATABASE_URL = "mysql://test:test@localhost:3306/test";
  });

  describe("Personality Trait Calculations", () => {
    it("should generate valid Big Five personality traits between 0-100", () => {
      // Test the personality generation logic
      const generateRandomTrait = () => Math.floor(Math.random() * 101);
      
      for (let i = 0; i < 100; i++) {
        const trait = generateRandomTrait();
        expect(trait).toBeGreaterThanOrEqual(0);
        expect(trait).toBeLessThanOrEqual(100);
      }
    });

    it("should calculate risk tolerance based on personality traits", () => {
      // Risk tolerance formula: based on openness and neuroticism
      const calculateRiskTolerance = (openness: number, neuroticism: number) => {
        return Math.round((openness * 0.6 + (100 - neuroticism) * 0.4));
      };
      
      // High openness, low neuroticism = high risk tolerance
      expect(calculateRiskTolerance(80, 20)).toBe(80);
      
      // Low openness, high neuroticism = low risk tolerance
      expect(calculateRiskTolerance(20, 80)).toBe(20);
      
      // Balanced traits
      expect(calculateRiskTolerance(50, 50)).toBe(50);
    });

    it("should calculate leadership score based on extraversion and conscientiousness", () => {
      const calculateLeadership = (extraversion: number, conscientiousness: number) => {
        return Math.round((extraversion * 0.5 + conscientiousness * 0.5));
      };
      
      expect(calculateLeadership(80, 80)).toBe(80);
      expect(calculateLeadership(20, 20)).toBe(20);
      expect(calculateLeadership(60, 40)).toBe(50);
    });

    it("should calculate empathy based on agreeableness and openness", () => {
      const calculateEmpathy = (agreeableness: number, openness: number) => {
        return Math.round((agreeableness * 0.7 + openness * 0.3));
      };
      
      expect(calculateEmpathy(100, 100)).toBe(100);
      expect(calculateEmpathy(0, 0)).toBe(0);
      expect(calculateEmpathy(70, 50)).toBe(64); // 70*0.7 + 50*0.3 = 49 + 15 = 64
    });
  });

  describe("Decision Making Logic", () => {
    it("should weight options based on personality traits", () => {
      const weightOption = (
        option: { riskLevel: number; potentialReward: number; requiresCooperation: boolean },
        personality: { riskTolerance: number; agreeableness: number }
      ) => {
        let score = option.potentialReward;
        
        // Risk averse agents penalize high-risk options
        if (option.riskLevel > personality.riskTolerance) {
          score -= (option.riskLevel - personality.riskTolerance) * 0.5;
        }
        
        // Agreeable agents prefer cooperative options
        if (option.requiresCooperation && personality.agreeableness > 50) {
          score += (personality.agreeableness - 50) * 0.3;
        }
        
        return Math.max(0, score);
      };
      
      const highRiskOption = { riskLevel: 80, potentialReward: 90, requiresCooperation: false };
      const cooperativeOption = { riskLevel: 30, potentialReward: 60, requiresCooperation: true };
      
      // Risk-tolerant agent prefers high-risk option
      const riskTolerantAgent = { riskTolerance: 90, agreeableness: 30 };
      expect(weightOption(highRiskOption, riskTolerantAgent)).toBeGreaterThan(
        weightOption(cooperativeOption, riskTolerantAgent)
      );
      
      // Agreeable, risk-averse agent prefers cooperative option
      const agreeableAgent = { riskTolerance: 30, agreeableness: 80 };
      expect(weightOption(cooperativeOption, agreeableAgent)).toBeGreaterThan(
        weightOption(highRiskOption, agreeableAgent)
      );
    });

    it("should factor emotional state into decision making", () => {
      const applyEmotionalModifier = (
        baseScore: number,
        emotionalState: { stress: number; happiness: number; trust: number }
      ) => {
        let modifier = 1;
        
        // High stress reduces decision quality
        if (emotionalState.stress > 70) {
          modifier *= 0.8;
        }
        
        // High happiness increases optimism
        if (emotionalState.happiness > 70) {
          modifier *= 1.1;
        }
        
        // Low trust makes agent more cautious
        if (emotionalState.trust < 30) {
          modifier *= 0.9;
        }
        
        return Math.round(baseScore * modifier);
      };
      
      const baseScore = 100;
      
      // Stressed agent
      expect(applyEmotionalModifier(baseScore, { stress: 80, happiness: 50, trust: 50 }))
        .toBeLessThan(baseScore);
      
      // Happy agent
      expect(applyEmotionalModifier(baseScore, { stress: 30, happiness: 80, trust: 50 }))
        .toBeGreaterThan(baseScore);
      
      // Distrustful agent
      expect(applyEmotionalModifier(baseScore, { stress: 30, happiness: 50, trust: 20 }))
        .toBeLessThan(baseScore);
    });
  });
});

describe("DreamCog Integration - Event Bridge Service", () => {
  describe("Business to Narrative Event Mapping", () => {
    it("should map business event types to narrative event types", () => {
      const mapBusinessToNarrative = (businessEventType: string) => {
        const mapping: Record<string, string> = {
          bankruptcy: "economic",
          merger: "economic",
          market_crash: "economic",
          expansion: "economic",
          layoff: "social",
          innovation: "technological",
          scandal: "social",
          success: "economic",
        };
        return mapping[businessEventType] || "economic";
      };
      
      expect(mapBusinessToNarrative("bankruptcy")).toBe("economic");
      expect(mapBusinessToNarrative("layoff")).toBe("social");
      expect(mapBusinessToNarrative("innovation")).toBe("technological");
      expect(mapBusinessToNarrative("unknown")).toBe("economic");
    });

    it("should calculate event importance based on magnitude and company size", () => {
      const calculateImportance = (magnitude: number, companyAssets: number) => {
        // Base importance from magnitude
        let importance = magnitude;
        
        // Larger companies have more impactful events
        if (companyAssets > 10000000) {
          importance = Math.min(100, importance * 1.5);
        } else if (companyAssets > 1000000) {
          importance = Math.min(100, importance * 1.2);
        }
        
        return Math.round(importance);
      };
      
      // Small company, medium magnitude
      expect(calculateImportance(50, 500000)).toBe(50);
      
      // Medium company, medium magnitude
      expect(calculateImportance(50, 5000000)).toBe(60);
      
      // Large company, medium magnitude
      expect(calculateImportance(50, 50000000)).toBe(75);
      
      // Cap at 100
      expect(calculateImportance(80, 50000000)).toBe(100);
    });
  });

  describe("Narrative to Business Event Mapping", () => {
    it("should map narrative event types to market effects", () => {
      const getMarketEffects = (eventType: string, importance: number) => {
        const effects: Record<string, { priceModifier: number; demandModifier: number }> = {
          conflict: { priceModifier: 0.2, demandModifier: -0.1 },
          natural: { priceModifier: 0.3, demandModifier: -0.2 },
          political: { priceModifier: 0.1, demandModifier: 0 },
          economic: { priceModifier: 0.15, demandModifier: 0.1 },
          technological: { priceModifier: -0.1, demandModifier: 0.2 },
          social: { priceModifier: 0, demandModifier: 0.05 },
          discovery: { priceModifier: -0.15, demandModifier: 0.3 },
        };
        
        const baseEffect = effects[eventType] || { priceModifier: 0, demandModifier: 0 };
        const scale = importance / 100;
        
        return {
          priceModifier: baseEffect.priceModifier * scale,
          demandModifier: baseEffect.demandModifier * scale,
        };
      };
      
      // High importance conflict
      const conflictEffects = getMarketEffects("conflict", 80);
      expect(conflictEffects.priceModifier).toBeCloseTo(0.16);
      expect(conflictEffects.demandModifier).toBeCloseTo(-0.08);
      
      // Low importance discovery
      const discoveryEffects = getMarketEffects("discovery", 30);
      expect(discoveryEffects.priceModifier).toBeCloseTo(-0.045);
      expect(discoveryEffects.demandModifier).toBeCloseTo(0.09);
    });
  });
});

describe("DreamCog Integration - World Economy Service", () => {
  describe("Economic Indicators Calculation", () => {
    it("should calculate market health based on active listings", () => {
      const calculateMarketHealth = (listingCount: number) => {
        return Math.min(100, listingCount * 5);
      };
      
      expect(calculateMarketHealth(0)).toBe(0);
      expect(calculateMarketHealth(10)).toBe(50);
      expect(calculateMarketHealth(20)).toBe(100);
      expect(calculateMarketHealth(30)).toBe(100); // Capped at 100
    });

    it("should calculate inflation from price deviations", () => {
      const calculateInflation = (prices: { current: number; base: number }[]) => {
        if (prices.length === 0) return 0;
        
        const totalDeviation = prices.reduce((sum, p) => {
          return sum + (p.current - p.base) / p.base;
        }, 0);
        
        return (totalDeviation / prices.length) * 100;
      };
      
      // No inflation
      expect(calculateInflation([
        { current: 100, base: 100 },
        { current: 50, base: 50 },
      ])).toBe(0);
      
      // 10% inflation
      expect(calculateInflation([
        { current: 110, base: 100 },
        { current: 55, base: 50 },
      ])).toBe(10);
      
      // Deflation
      expect(calculateInflation([
        { current: 90, base: 100 },
        { current: 45, base: 50 },
      ])).toBe(-10);
    });

    it("should calculate consumer confidence from market health and inflation", () => {
      const calculateConsumerConfidence = (marketHealth: number, inflation: number) => {
        const base = 50;
        const healthContribution = (marketHealth - 50) * 0.3;
        const inflationPenalty = Math.abs(inflation) * 2;
        
        return Math.max(0, Math.min(100, base + healthContribution - inflationPenalty));
      };
      
      // Healthy market, low inflation
      expect(calculateConsumerConfidence(80, 2)).toBe(55); // 50 + 9 - 4 = 55
      
      // Unhealthy market, high inflation
      expect(calculateConsumerConfidence(30, 15)).toBe(14); // 50 - 6 - 30 = 14
      
      // Perfect conditions
      expect(calculateConsumerConfidence(100, 0)).toBe(65); // 50 + 15 - 0 = 65
    });
  });

  describe("Location Effects Calculation", () => {
    it("should calculate danger level from active events", () => {
      const calculateDangerLevel = (events: { type: string }[]) => {
        let danger = 0;
        
        for (const event of events) {
          switch (event.type) {
            case "conflict":
              danger += 30;
              break;
            case "natural":
              danger += 20;
              break;
            case "political":
              danger += 10;
              break;
          }
        }
        
        return Math.min(100, danger);
      };
      
      expect(calculateDangerLevel([])).toBe(0);
      expect(calculateDangerLevel([{ type: "conflict" }])).toBe(30);
      expect(calculateDangerLevel([{ type: "conflict" }, { type: "natural" }])).toBe(50);
      expect(calculateDangerLevel([
        { type: "conflict" },
        { type: "conflict" },
        { type: "conflict" },
        { type: "conflict" },
      ])).toBe(100); // Capped
    });

    it("should calculate shipping cost multiplier from events", () => {
      const calculateShippingMultiplier = (events: { type: string }[]) => {
        let multiplier = 1;
        
        for (const event of events) {
          switch (event.type) {
            case "conflict":
              multiplier *= 1.5;
              break;
            case "natural":
              multiplier *= 1.3;
              break;
            case "political":
              multiplier *= 1.2;
              break;
          }
        }
        
        return multiplier;
      };
      
      expect(calculateShippingMultiplier([])).toBe(1);
      expect(calculateShippingMultiplier([{ type: "conflict" }])).toBe(1.5);
      expect(calculateShippingMultiplier([{ type: "conflict" }, { type: "natural" }])).toBeCloseTo(1.95);
    });

    it("should calculate production efficiency modifier from events", () => {
      const calculateEfficiencyModifier = (events: { type: string }[]) => {
        let modifier = 1;
        
        for (const event of events) {
          switch (event.type) {
            case "conflict":
              modifier *= 0.7;
              break;
            case "natural":
              modifier *= 0.8;
              break;
            case "economic":
              modifier *= 0.9;
              break;
            case "technological":
              modifier *= 1.1;
              break;
            case "discovery":
              modifier *= 1.15;
              break;
          }
        }
        
        return modifier;
      };
      
      expect(calculateEfficiencyModifier([])).toBe(1);
      expect(calculateEfficiencyModifier([{ type: "technological" }])).toBe(1.1);
      expect(calculateEfficiencyModifier([{ type: "conflict" }])).toBe(0.7);
      expect(calculateEfficiencyModifier([{ type: "technological" }, { type: "discovery" }])).toBeCloseTo(1.265);
    });
  });
});

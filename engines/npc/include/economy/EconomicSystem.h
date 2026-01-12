/**
 * EconomicSystem.h - NPC Economic Agent System
 *
 * Implements economic agents and market simulation for NPCs including:
 * - Individual economic decision making
 * - Market dynamics
 * - Trade and negotiation
 * - Resource management
 *
 * Part of Phase 4: Economy
 */

#ifndef ULTIMA_NPC_ECONOMIC_SYSTEM_H
#define ULTIMA_NPC_ECONOMIC_SYSTEM_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <functional>
#include <cfloat>

namespace Ultima {
namespace NPC {
namespace Economy {

/**
 * Resource/Item types
 */
enum class ResourceType {
    // Raw materials
    Wood,
    Stone,
    Iron,
    Copper,
    Silver,
    Gold,
    Cloth,
    Leather,
    Food,
    Water,

    // Crafted goods
    Weapon,
    Armor,
    Tool,
    Clothing,
    Jewelry,
    Potion,
    Scroll,
    Furniture,

    // Services
    Labor,
    Protection,
    Healing,
    Teaching,
    Entertainment,

    // Abstract
    Information,
    Favor,
    Custom
};

/**
 * Resource/Item
 */
struct Resource {
    std::string id;
    std::string name;
    ResourceType type;

    double baseValue = 1.0;         // Base price
    double quality = 1.0;           // 0-1 quality multiplier
    double quantity = 1.0;
    double decay = 0.0;             // Decay rate per time unit

    // Metadata
    std::string origin;             // Where it came from
    std::string owner;
    bool isStackable = true;

    /**
     * Calculate market value
     */
    double getMarketValue(double marketMultiplier = 1.0) const;
};

/**
 * Inventory
 */
class Inventory {
public:
    Inventory(double maxCapacity = 100.0);
    ~Inventory();

    /**
     * Add resource
     */
    bool add(const Resource& resource);

    /**
     * Remove resource
     */
    bool remove(const std::string& resourceId, double quantity = 1.0);

    /**
     * Check if has resource
     */
    bool has(const std::string& resourceId, double minQuantity = 1.0) const;
    bool hasType(ResourceType type, double minQuantity = 1.0) const;

    /**
     * Get resource
     */
    std::optional<Resource> get(const std::string& resourceId) const;

    /**
     * Get all resources of type
     */
    std::vector<Resource> getByType(ResourceType type) const;

    /**
     * Get total value
     */
    double getTotalValue() const;

    /**
     * Get current capacity used
     */
    double getUsedCapacity() const;
    double getMaxCapacity() const { return maxCapacity_; }

    /**
     * Get all resources
     */
    std::vector<Resource> getAllResources() const;

private:
    double maxCapacity_;
    std::map<std::string, Resource> resources_;
};

/**
 * Market listing
 */
struct MarketListing {
    std::string id;
    std::string sellerId;
    Resource resource;
    double askingPrice;
    double minPrice;                // Won't sell below this
    uint32_t listedTime;
    uint32_t expirationTime;
    bool isActive = true;
};

/**
 * Trade offer
 */
struct TradeOffer {
    std::string id;
    std::string buyerId;
    std::string sellerId;

    std::vector<Resource> offered;  // What buyer offers
    std::vector<Resource> requested;// What buyer wants
    double goldOffered = 0.0;
    double goldRequested = 0.0;

    uint32_t createdTime;
    uint32_t expirationTime;
    bool isCounterOffer = false;
    std::string originalOfferId;
};

/**
 * Transaction record
 */
struct Transaction {
    std::string id;
    std::string buyerId;
    std::string sellerId;
    std::string resourceId;
    double quantity;
    double price;
    uint32_t timestamp;
    std::string location;
};

/**
 * Market
 */
class Market {
public:
    Market(const std::string& id, const std::string& location);
    ~Market();

    std::string id;
    std::string location;
    std::string name;

    /**
     * List item for sale
     */
    std::string listItem(const std::string& sellerId,
                        const Resource& resource,
                        double askingPrice,
                        double minPrice = 0.0);

    /**
     * Remove listing
     */
    void removeListing(const std::string& listingId);

    /**
     * Get listings
     */
    std::vector<MarketListing> getListings(ResourceType type = ResourceType::Custom) const;
    std::vector<MarketListing> getListingsBySeller(const std::string& sellerId) const;

    /**
     * Find best price for resource
     */
    std::optional<MarketListing> findBestPrice(ResourceType type,
                                               double maxPrice = DBL_MAX) const;

    /**
     * Execute purchase
     */
    bool executePurchase(const std::string& listingId,
                        const std::string& buyerId,
                        double offeredPrice);

    /**
     * Get price history
     */
    std::vector<Transaction> getPriceHistory(const std::string& resourceId,
                                             int maxRecords = 100) const;

    /**
     * Get average price
     */
    double getAveragePrice(ResourceType type, int recentCount = 10) const;

    /**
     * Update market (decay old listings)
     */
    void update(uint32_t currentTime);

    /**
     * Market statistics
     */
    double getSupply(ResourceType type) const;
    double getDemand(ResourceType type) const;
    double getPriceVolatility(ResourceType type) const;

private:
    std::map<std::string, MarketListing> listings_;
    std::vector<Transaction> transactions_;

    // Demand tracking
    std::map<ResourceType, double> demandTracker_;

    std::string generateId();
};

/**
 * Economic agent - NPC's economic behavior
 */
class EconomicAgent {
public:
    EconomicAgent(const std::string& entityId);
    ~EconomicAgent();

    std::string entityId;

    // Wealth
    double gold = 0.0;
    Inventory inventory;

    // Economic traits
    double riskTolerance = 0.5;     // Investment risk preference
    double priceMemory = 0.5;       // How well remembers prices
    double negotiationSkill = 0.5; // Bargaining ability
    double honesty = 0.7;           // In trade dealings
    double materialism = 0.5;       // Desire for wealth

    // Income/Expenses
    std::string occupation;
    double dailyIncome = 0.0;
    double dailyExpenses = 0.0;

    /**
     * Evaluate resource utility
     */
    double evaluateUtility(const Resource& resource) const;

    /**
     * Calculate willingness to pay
     */
    double getWillingnessToPay(const Resource& resource,
                               double marketPrice) const;

    /**
     * Calculate minimum sale price
     */
    double getMinSalePrice(const Resource& resource,
                          double marketPrice) const;

    /**
     * Make purchase decision
     */
    bool shouldBuy(const MarketListing& listing) const;

    /**
     * Make sale decision
     */
    bool shouldSell(const Resource& resource, double offeredPrice) const;

    /**
     * Generate counter offer
     */
    std::optional<TradeOffer> generateCounterOffer(const TradeOffer& offer) const;

    /**
     * Evaluate trade offer
     */
    double evaluateOffer(const TradeOffer& offer) const;

    /**
     * Daily economic update
     */
    void dailyUpdate();

    /**
     * Get net worth
     */
    double getNetWorth() const;

private:
    // Price expectations
    std::map<ResourceType, double> priceExpectations_;

    // Needs
    std::map<ResourceType, double> resourceNeeds_;

    void updatePriceExpectation(ResourceType type, double observedPrice);
};

/**
 * Negotiation session
 */
class Negotiation {
public:
    enum class State {
        Initial,
        Offered,
        CounterOffered,
        Accepted,
        Rejected,
        Abandoned
    };

    Negotiation(EconomicAgent* buyer, EconomicAgent* seller);
    ~Negotiation();

    /**
     * Make offer
     */
    void makeOffer(const TradeOffer& offer);

    /**
     * Counter offer
     */
    void counterOffer(const TradeOffer& offer);

    /**
     * Accept current offer
     */
    bool accept();

    /**
     * Reject current offer
     */
    void reject();

    /**
     * Get current state
     */
    State getState() const { return state_; }

    /**
     * Get current offer
     */
    const TradeOffer& getCurrentOffer() const { return currentOffer_; }

    /**
     * Auto-negotiate (AI vs AI)
     */
    bool autoNegotiate(int maxRounds = 5);

private:
    EconomicAgent* buyer_;
    EconomicAgent* seller_;
    State state_ = State::Initial;
    TradeOffer currentOffer_;
    int roundCount_ = 0;

    void executeTransaction();
};

/**
 * Production
 */
struct ProductionRecipe {
    std::string id;
    std::string name;
    ResourceType outputType;
    double outputQuantity = 1.0;
    double outputQuality = 1.0;

    std::map<ResourceType, double> inputs;  // Required resources
    double laborRequired = 1.0;             // Time units
    double skillRequired = 0.0;             // Min skill level

    std::string toolRequired;               // Required tool
};

/**
 * Production facility
 */
class ProductionFacility {
public:
    ProductionFacility(const std::string& id);
    ~ProductionFacility();

    std::string id;
    std::string name;
    std::string ownerId;
    std::string location;

    // Capacity
    double efficiency = 1.0;        // Production speed multiplier
    double condition = 1.0;         // Wear and tear
    int workerCapacity = 1;

    // Workers
    std::vector<std::string> workerIds;

    // Production queue
    std::vector<std::pair<ProductionRecipe, double>> productionQueue;  // recipe, progress

    /**
     * Add recipe to queue
     */
    bool queueProduction(const ProductionRecipe& recipe);

    /**
     * Update production
     */
    std::vector<Resource> update(double deltaTime);

    /**
     * Check if can produce
     */
    bool canProduce(const ProductionRecipe& recipe, const Inventory& materials) const;

    /**
     * Get production rate
     */
    double getProductionRate() const;

private:
    void degradeCondition(double amount);
};

/**
 * Market Simulation
 */
class MarketSimulation {
public:
    MarketSimulation();
    ~MarketSimulation();

    /**
     * Add market
     */
    void addMarket(std::shared_ptr<Market> market);

    /**
     * Add economic agent
     */
    void addAgent(std::shared_ptr<EconomicAgent> agent);

    /**
     * Set base prices
     */
    void setBasePrice(ResourceType type, double price);

    /**
     * Simulate market tick
     */
    void simulate(double deltaTime);

    /**
     * Get market by location
     */
    Market* getMarket(const std::string& location);

    /**
     * Get agent
     */
    EconomicAgent* getAgent(const std::string& entityId);

    /**
     * Calculate price index
     */
    double getPriceIndex() const;

    /**
     * Get economic indicators
     */
    double getInflationRate() const;
    double getTotalWealth() const;
    double getGiniCoefficient() const;  // Wealth inequality

    /**
     * Inject/remove money (economic events)
     */
    void injectMoney(double amount);
    void removeMoney(double amount);

    /**
     * Supply shock
     */
    void supplyShock(ResourceType type, double multiplier);

    /**
     * Demand shock
     */
    void demandShock(ResourceType type, double multiplier);

private:
    std::map<std::string, std::shared_ptr<Market>> markets_;
    std::map<std::string, std::shared_ptr<EconomicAgent>> agents_;
    std::map<ResourceType, double> basePrices_;

    // Economic state
    double moneySupply_ = 10000.0;
    double previousPriceIndex_ = 1.0;
    double inflationRate_ = 0.0;

    void matchOrders();
    void updatePrices();
    void calculateEconomicIndicators();
};

/**
 * NPC Economic Decision System
 */
class NPCEconomicDecision {
public:
    NPCEconomicDecision();
    ~NPCEconomicDecision();

    /**
     * Decide what to buy
     */
    std::vector<std::pair<ResourceType, double>> decidePurchases(
        const EconomicAgent& agent,
        const Market& market);

    /**
     * Decide what to sell
     */
    std::vector<std::pair<std::string, double>> decideSales(
        const EconomicAgent& agent,
        const Market& market);

    /**
     * Decide occupation/work
     */
    std::string decideOccupation(const EconomicAgent& agent,
                                 const std::vector<std::string>& availableJobs);

    /**
     * Evaluate business opportunity
     */
    double evaluateOpportunity(const EconomicAgent& agent,
                              const ProductionFacility& facility);

    /**
     * Make investment decision
     */
    bool shouldInvest(const EconomicAgent& agent,
                     double investmentAmount,
                     double expectedReturn,
                     double risk);

private:
    double calculateExpectedUtility(double outcome, double probability,
                                   double riskTolerance) const;
};

/**
 * Integrated Economic System
 */
class EconomicSystem {
public:
    EconomicSystem();
    ~EconomicSystem();

    /**
     * Initialize system
     */
    void initialize();

    /**
     * Register NPC as economic agent
     */
    void registerAgent(const std::string& entityId, double initialGold = 100.0);

    /**
     * Get agent
     */
    EconomicAgent* getAgent(const std::string& entityId);

    /**
     * Create market
     */
    void createMarket(const std::string& location, const std::string& name = "");

    /**
     * Get market
     */
    Market* getMarket(const std::string& location);

    /**
     * Register production recipe
     */
    void registerRecipe(const ProductionRecipe& recipe);

    /**
     * Create production facility
     */
    void createFacility(const std::string& id, const std::string& location,
                       const std::string& ownerId);

    /**
     * Process NPC economic turn
     */
    void processNPCEconomics(const std::string& entityId);

    /**
     * Update system
     */
    void update(double deltaTime);

    /**
     * Get economic statistics
     */
    struct EconomicStats {
        double totalWealth;
        double averageWealth;
        double giniCoefficient;
        double inflationRate;
        std::map<ResourceType, double> priceIndex;
    };
    EconomicStats getStatistics() const;

    /**
     * Save/load
     */
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);

private:
    MarketSimulation simulation_;
    NPCEconomicDecision decisionSystem_;
    std::map<std::string, ProductionRecipe> recipes_;
    std::map<std::string, std::unique_ptr<ProductionFacility>> facilities_;
};

} // namespace Economy
} // namespace NPC
} // namespace Ultima

#endif // ULTIMA_NPC_ECONOMIC_SYSTEM_H

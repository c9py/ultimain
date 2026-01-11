/**
 * EconomicSystem.cpp - Economic System Implementation
 * Stub implementation for Phase 4
 */

#include "economy/EconomicSystem.h"
#include <algorithm>
#include <random>
#include <cmath>

namespace Ultima {
namespace NPC {
namespace Economy {

static std::mt19937 rng(std::random_device{}());

// Resource Implementation
double Resource::getMarketValue(double marketMultiplier) const {
    return baseValue * quality * quantity * marketMultiplier;
}

// Inventory Implementation
Inventory::Inventory(double maxCapacity) : maxCapacity_(maxCapacity) {}
Inventory::~Inventory() = default;

bool Inventory::add(const Resource& resource) {
    if (getUsedCapacity() + resource.quantity > maxCapacity_) {
        return false;
    }

    if (resources_.count(resource.id) && resource.isStackable) {
        resources_[resource.id].quantity += resource.quantity;
    } else {
        resources_[resource.id] = resource;
    }
    return true;
}

bool Inventory::remove(const std::string& resourceId, double quantity) {
    auto it = resources_.find(resourceId);
    if (it == resources_.end()) return false;

    if (it->second.quantity < quantity) return false;

    it->second.quantity -= quantity;
    if (it->second.quantity <= 0) {
        resources_.erase(it);
    }
    return true;
}

bool Inventory::has(const std::string& resourceId, double minQuantity) const {
    auto it = resources_.find(resourceId);
    return it != resources_.end() && it->second.quantity >= minQuantity;
}

double Inventory::getTotalValue() const {
    double total = 0.0;
    for (const auto& [id, res] : resources_) {
        total += res.getMarketValue();
    }
    return total;
}

double Inventory::getUsedCapacity() const {
    double used = 0.0;
    for (const auto& [id, res] : resources_) {
        used += res.quantity;
    }
    return used;
}

// Market Implementation
Market::Market(const std::string& i, const std::string& loc)
    : id(i), location(loc), name(loc + " Market") {}
Market::~Market() = default;

std::string Market::generateId() {
    static int counter = 0;
    return "listing_" + std::to_string(++counter);
}

std::string Market::listItem(const std::string& sellerId,
                            const Resource& resource,
                            double askingPrice,
                            double minPrice) {
    MarketListing listing;
    listing.id = generateId();
    listing.sellerId = sellerId;
    listing.resource = resource;
    listing.askingPrice = askingPrice;
    listing.minPrice = minPrice > 0 ? minPrice : askingPrice * 0.7;
    listing.isActive = true;

    listings_[listing.id] = listing;
    return listing.id;
}

void Market::removeListing(const std::string& listingId) {
    listings_.erase(listingId);
}

std::vector<MarketListing> Market::getListings(ResourceType type) const {
    std::vector<MarketListing> results;
    for (const auto& [id, listing] : listings_) {
        if (!listing.isActive) continue;
        if (type == ResourceType::Custom || listing.resource.type == type) {
            results.push_back(listing);
        }
    }
    return results;
}

bool Market::executePurchase(const std::string& listingId,
                            const std::string& buyerId,
                            double offeredPrice) {
    auto it = listings_.find(listingId);
    if (it == listings_.end() || !it->second.isActive) return false;

    if (offeredPrice < it->second.minPrice) return false;

    Transaction tx;
    tx.id = "tx_" + std::to_string(transactions_.size());
    tx.buyerId = buyerId;
    tx.sellerId = it->second.sellerId;
    tx.resourceId = it->second.resource.id;
    tx.quantity = it->second.resource.quantity;
    tx.price = offeredPrice;

    transactions_.push_back(tx);
    listings_.erase(it);

    return true;
}

double Market::getAveragePrice(ResourceType type, int recentCount) const {
    double sum = 0.0;
    int count = 0;

    for (auto it = transactions_.rbegin(); it != transactions_.rend() && count < recentCount; ++it) {
        sum += it->price;
        count++;
    }

    return count > 0 ? sum / count : 0.0;
}

// EconomicAgent Implementation
EconomicAgent::EconomicAgent(const std::string& entityId)
    : entityId(entityId), inventory(100.0) {}
EconomicAgent::~EconomicAgent() = default;

double EconomicAgent::evaluateUtility(const Resource& resource) const {
    double need = resourceNeeds_.count(resource.type) ? resourceNeeds_.at(resource.type) : 0.5;
    return resource.quality * need;
}

double EconomicAgent::getWillingnessToPay(const Resource& resource,
                                          double marketPrice) const {
    double utility = evaluateUtility(resource);
    double baseWTP = marketPrice * (0.8 + 0.4 * utility);

    // Adjust for personality
    baseWTP *= (1.0 + (materialism - 0.5) * 0.2);

    return std::min(baseWTP, gold);
}

double EconomicAgent::getMinSalePrice(const Resource& resource,
                                      double marketPrice) const {
    double utility = evaluateUtility(resource);
    return marketPrice * (0.6 + 0.4 * (1.0 - utility));
}

bool EconomicAgent::shouldBuy(const MarketListing& listing) const {
    double wtp = getWillingnessToPay(listing.resource, listing.askingPrice);
    return wtp >= listing.askingPrice && gold >= listing.askingPrice;
}

double EconomicAgent::getNetWorth() const {
    return gold + inventory.getTotalValue();
}

void EconomicAgent::dailyUpdate() {
    gold += dailyIncome;
    gold -= dailyExpenses;
    gold = std::max(0.0, gold);
}

// MarketSimulation Implementation
MarketSimulation::MarketSimulation() = default;
MarketSimulation::~MarketSimulation() = default;

void MarketSimulation::addMarket(std::shared_ptr<Market> market) {
    markets_[market->location] = market;
}

void MarketSimulation::addAgent(std::shared_ptr<EconomicAgent> agent) {
    agents_[agent->entityId] = agent;
}

void MarketSimulation::setBasePrice(ResourceType type, double price) {
    basePrices_[type] = price;
}

void MarketSimulation::simulate(double deltaTime) {
    // Update agents
    for (auto& [id, agent] : agents_) {
        agent->dailyUpdate();
    }

    // Match orders
    matchOrders();

    // Update prices
    updatePrices();

    // Calculate indicators
    calculateEconomicIndicators();
}

Market* MarketSimulation::getMarket(const std::string& location) {
    auto it = markets_.find(location);
    return it != markets_.end() ? it->second.get() : nullptr;
}

EconomicAgent* MarketSimulation::getAgent(const std::string& entityId) {
    auto it = agents_.find(entityId);
    return it != agents_.end() ? it->second.get() : nullptr;
}

double MarketSimulation::getTotalWealth() const {
    double total = 0.0;
    for (const auto& [id, agent] : agents_) {
        total += agent->getNetWorth();
    }
    return total;
}

void MarketSimulation::matchOrders() {
    // Simple order matching
}

void MarketSimulation::updatePrices() {
    // Update prices based on supply/demand
}

void MarketSimulation::calculateEconomicIndicators() {
    // Calculate inflation, etc.
}

// EconomicSystem Implementation
EconomicSystem::EconomicSystem() = default;
EconomicSystem::~EconomicSystem() = default;

void EconomicSystem::initialize() {
    // Set up base prices
    simulation_.setBasePrice(ResourceType::Food, 1.0);
    simulation_.setBasePrice(ResourceType::Wood, 2.0);
    simulation_.setBasePrice(ResourceType::Iron, 5.0);
    simulation_.setBasePrice(ResourceType::Gold, 100.0);
}

void EconomicSystem::registerAgent(const std::string& entityId, double initialGold) {
    auto agent = std::make_shared<EconomicAgent>(entityId);
    agent->gold = initialGold;
    simulation_.addAgent(agent);
}

EconomicAgent* EconomicSystem::getAgent(const std::string& entityId) {
    return simulation_.getAgent(entityId);
}

void EconomicSystem::createMarket(const std::string& location, const std::string& name) {
    auto market = std::make_shared<Market>(location, location);
    if (!name.empty()) market->name = name;
    simulation_.addMarket(market);
}

Market* EconomicSystem::getMarket(const std::string& location) {
    return simulation_.getMarket(location);
}

void EconomicSystem::update(double deltaTime) {
    simulation_.simulate(deltaTime);
}

EconomicSystem::EconomicStats EconomicSystem::getStatistics() const {
    EconomicStats stats;
    stats.totalWealth = simulation_.getTotalWealth();
    return stats;
}

} // namespace Economy
} // namespace NPC
} // namespace Ultima

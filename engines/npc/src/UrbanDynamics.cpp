/**
 * UrbanDynamics.cpp - Urban Simulation Implementation
 * Stub implementation for Phase 5
 */

#include "urban/UrbanDynamics.h"
#include <algorithm>
#include <random>
#include <cmath>
#include <queue>

namespace Ultima {
namespace NPC {
namespace Urban {

static std::mt19937 rng(std::random_device{}());

// City Implementation
City::City(const std::string& i, int w, int h)
    : id(i), width(w), height(h) {
    grid.resize(height);
    for (int y = 0; y < height; ++y) {
        grid[y].resize(width);
        for (int x = 0; x < width; ++x) {
            grid[y][x].x = x;
            grid[y][x].y = y;
        }
    }
}

City::~City() = default;

Parcel* City::getParcel(int x, int y) {
    if (!isValid(x, y)) return nullptr;
    return &grid[y][x];
}

const Parcel* City::getParcel(int x, int y) const {
    if (!isValid(x, y)) return nullptr;
    return &grid[y][x];
}

void City::setLandUse(int x, int y, LandUse use) {
    if (auto* p = getParcel(x, y)) {
        p->landUse = use;
    }
}

std::string City::addBuilding(const Building& building) {
    std::string id = building.id.empty() ?
        "bld_" + std::to_string(buildings.size()) : building.id;

    Building b = building;
    b.id = id;
    buildings[id] = b;

    // Mark parcels
    for (int dy = 0; dy < b.height; ++dy) {
        for (int dx = 0; dx < b.width; ++dx) {
            if (auto* p = getParcel(b.x + dx, b.y + dy)) {
                p->hasBuilding = true;
            }
        }
    }

    return id;
}

void City::removeBuilding(const std::string& buildingId) {
    auto it = buildings.find(buildingId);
    if (it != buildings.end()) {
        // Clear parcels
        const Building& b = it->second;
        for (int dy = 0; dy < b.height; ++dy) {
            for (int dx = 0; dx < b.width; ++dx) {
                if (auto* p = getParcel(b.x + dx, b.y + dy)) {
                    p->hasBuilding = false;
                }
            }
        }
        buildings.erase(it);
    }
}

Building* City::getBuildingAt(int x, int y) {
    for (auto& [id, b] : buildings) {
        if (x >= b.x && x < b.x + b.width &&
            y >= b.y && y < b.y + b.height) {
            return &b;
        }
    }
    return nullptr;
}

void City::addRoad(int x1, int y1, int x2, int y2, Road::Type type) {
    Road r;
    r.id = "road_" + std::to_string(roads.size());
    r.x1 = x1;
    r.y1 = y1;
    r.x2 = x2;
    r.y2 = y2;
    r.type = type;
    roads.push_back(r);

    // Mark parcels
    int dx = (x2 > x1) ? 1 : (x2 < x1) ? -1 : 0;
    int dy = (y2 > y1) ? 1 : (y2 < y1) ? -1 : 0;
    int x = x1, y = y1;
    while (x != x2 || y != y2) {
        if (auto* p = getParcel(x, y)) {
            p->hasRoad = true;
            p->landUse = LandUse::Road;
        }
        if (x != x2) x += dx;
        if (y != y2) y += dy;
    }
}

bool City::isValid(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

std::vector<std::pair<int, int>> City::findPath(int x1, int y1, int x2, int y2) const {
    // Simple A* pathfinding
    std::vector<std::pair<int, int>> path;

    struct Node {
        int x, y;
        double g, h;
        int parentX, parentY;
        bool operator>(const Node& other) const { return g + h > other.g + other.h; }
    };

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open;
    std::map<std::pair<int, int>, bool> closed;
    std::map<std::pair<int, int>, std::pair<int, int>> parent;

    open.push({x1, y1, 0, 0, -1, -1});

    int dx[] = {0, 1, 0, -1};
    int dy[] = {-1, 0, 1, 0};

    while (!open.empty()) {
        Node current = open.top();
        open.pop();

        if (current.x == x2 && current.y == y2) {
            // Reconstruct path
            int px = x2, py = y2;
            while (px != -1) {
                path.push_back({px, py});
                auto p = parent[{px, py}];
                px = p.first;
                py = p.second;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        if (closed[{current.x, current.y}]) continue;
        closed[{current.x, current.y}] = true;

        for (int i = 0; i < 4; ++i) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];

            if (!isValid(nx, ny)) continue;
            if (closed[{nx, ny}]) continue;

            const Parcel* p = getParcel(nx, ny);
            if (p->hasBuilding && !(nx == x2 && ny == y2)) continue;

            double g = current.g + 1;
            double h = std::abs(nx - x2) + std::abs(ny - y2);

            parent[{nx, ny}] = {current.x, current.y};
            open.push({nx, ny, g, h, current.x, current.y});
        }
    }

    return path;
}

std::vector<Building*> City::getBuildingsByType(BuildingType type) {
    std::vector<Building*> result;
    for (auto& [id, b] : buildings) {
        if (b.type == type) {
            result.push_back(&b);
        }
    }
    return result;
}

void City::calculateStatistics() {
    population = 0;
    wealth = 0;
    roadCoverage = 0;

    int roadCount = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (grid[y][x].hasRoad) roadCount++;
        }
    }
    roadCoverage = static_cast<double>(roadCount) / (width * height);
}

// CityGenerator Implementation
CityGenerator::CityGenerator() = default;
CityGenerator::~CityGenerator() = default;

std::unique_ptr<City> CityGenerator::generate(const std::string& name) {
    auto city = std::make_unique<City>(name, params.width, params.height);
    city->name = name;

    if (params.seed != 0) {
        rng.seed(params.seed);
    }

    generateTerrain(*city);
    generateRoads(*city);
    generateDistricts(*city);
    generateBuildings(*city);
    generateInfrastructure(*city);

    city->calculateStatistics();

    return city;
}

void CityGenerator::generateTerrain(City& city) {
    // Set elevation and features
    for (int y = 0; y < city.height; ++y) {
        for (int x = 0; x < city.width; ++x) {
            auto* p = city.getParcel(x, y);
            p->elevation = 0.0;
            p->fertility = 0.5;
        }
    }
}

void CityGenerator::generateRoads(City& city) {
    int centerX = city.width / 2;
    int centerY = city.height / 2;

    // Main roads from center
    city.addRoad(0, centerY, city.width - 1, centerY, Road::Type::MainRoad);
    city.addRoad(centerX, 0, centerX, city.height - 1, Road::Type::MainRoad);

    // Grid streets
    for (int y = 5; y < city.height; y += 10) {
        city.addRoad(0, y, city.width - 1, y, Road::Type::Street);
    }
    for (int x = 5; x < city.width; x += 10) {
        city.addRoad(x, 0, x, city.height - 1, Road::Type::Street);
    }
}

void CityGenerator::generateDistricts(City& city) {
    // Create basic districts
    District market;
    market.id = "market";
    market.name = "Market District";
    market.primaryUse = LandUse::Commercial;
    city.districts.push_back(market);

    District residential;
    residential.id = "residential";
    residential.name = "Residential Quarter";
    residential.primaryUse = LandUse::Residential;
    city.districts.push_back(residential);
}

void CityGenerator::generateBuildings(City& city) {
    placeMarketSquare(city);
    placeTemple(city);

    if (params.hasWalls) {
        placeCastle(city);
    }

    // Fill with houses
    std::uniform_int_distribution<int> xDist(1, city.width - 2);
    std::uniform_int_distribution<int> yDist(1, city.height - 2);

    for (int i = 0; i < params.initialPopulation / 5; ++i) {
        int x = xDist(rng);
        int y = yDist(rng);

        auto* p = city.getParcel(x, y);
        if (p && !p->hasBuilding && !p->hasRoad) {
            Building house;
            house.type = BuildingType::House;
            house.x = x;
            house.y = y;
            house.capacity = 5;
            city.addBuilding(house);
            p->landUse = LandUse::Residential;
        }
    }
}

void CityGenerator::generateInfrastructure(City& city) {
    // Add wells
    for (int i = 0; i < 5; ++i) {
        std::uniform_int_distribution<int> xDist(1, city.width - 2);
        std::uniform_int_distribution<int> yDist(1, city.height - 2);

        Building well;
        well.type = BuildingType::Well;
        well.x = xDist(rng);
        well.y = yDist(rng);
        city.addBuilding(well);
    }
}

void CityGenerator::placeMarketSquare(City& city) {
    int cx = city.width / 2;
    int cy = city.height / 2;

    Building market;
    market.type = BuildingType::Market;
    market.x = cx - 2;
    market.y = cy - 2;
    market.width = 4;
    market.height = 4;
    market.name = "Central Market";
    city.addBuilding(market);

    for (int dy = -2; dy < 2; ++dy) {
        for (int dx = -2; dx < 2; ++dx) {
            if (auto* p = city.getParcel(cx + dx, cy + dy)) {
                p->landUse = LandUse::Commercial;
            }
        }
    }
}

void CityGenerator::placeTemple(City& city) {
    Building temple;
    temple.type = BuildingType::Temple;
    temple.x = city.width / 4;
    temple.y = city.height / 4;
    temple.width = 3;
    temple.height = 3;
    temple.name = "Temple";
    city.addBuilding(temple);
}

void CityGenerator::placeCastle(City& city) {
    Building castle;
    castle.type = BuildingType::Castle;
    castle.x = 2;
    castle.y = 2;
    castle.width = 5;
    castle.height = 5;
    castle.name = "Castle";
    city.addBuilding(castle);
}

// UrbanSimulation Implementation
UrbanSimulation::UrbanSimulation() = default;
UrbanSimulation::~UrbanSimulation() = default;

void UrbanSimulation::initialize() {}

void UrbanSimulation::addCity(std::shared_ptr<City> city) {
    cities_[city->id] = city;
}

City* UrbanSimulation::getCity(const std::string& cityId) {
    auto it = cities_.find(cityId);
    return it != cities_.end() ? it->second.get() : nullptr;
}

void UrbanSimulation::simulate(double deltaTime) {
    for (auto& [id, city] : cities_) {
        populationModel_.simulate(deltaTime, *city);
        landValueModel_.calculate(*city);
        growthModel_.simulate(*city, deltaTime);
    }
}

std::vector<std::string> UrbanSimulation::getCityIds() const {
    std::vector<std::string> ids;
    for (const auto& [id, _] : cities_) {
        ids.push_back(id);
    }
    return ids;
}

std::shared_ptr<City> UrbanSimulation::generateCity(const std::string& name,
                                                    const CityGenerator::Parameters& params) {
    generator_.params = params;
    auto city = generator_.generate(name);
    auto ptr = std::shared_ptr<City>(city.release());
    cities_[ptr->id] = ptr;
    return ptr;
}

// PopulationModel Implementation
PopulationModel::PopulationModel() {
    segments_.push_back({"peasant", 0, 0.03, 0.02, 0.0, 0.2});
    segments_.push_back({"merchant", 0, 0.02, 0.01, 0.0, 0.5});
    segments_.push_back({"noble", 0, 0.01, 0.005, 0.0, 0.9});
}

PopulationModel::~PopulationModel() = default;

void PopulationModel::initialize(int totalPopulation) {
    segments_[0].count = static_cast<int>(totalPopulation * 0.7);
    segments_[1].count = static_cast<int>(totalPopulation * 0.25);
    segments_[2].count = static_cast<int>(totalPopulation * 0.05);
}

void PopulationModel::simulate(double deltaTime, City& city) {
    calculateBirths(deltaTime);
    calculateDeaths(deltaTime);
    calculateMigration(deltaTime, city);
}

int PopulationModel::getTotalPopulation() const {
    int total = 0;
    for (const auto& seg : segments_) {
        total += seg.count;
    }
    return total;
}

void PopulationModel::calculateBirths(double deltaTime) {
    for (auto& seg : segments_) {
        int births = static_cast<int>(seg.count * seg.birthRate * deltaTime);
        seg.count += births;
    }
}

void PopulationModel::calculateDeaths(double deltaTime) {
    for (auto& seg : segments_) {
        int deaths = static_cast<int>(seg.count * seg.deathRate * deltaTime);
        seg.count = std::max(0, seg.count - deaths);
    }
}

void PopulationModel::calculateMigration(double deltaTime, const City& city) {
    // Migration based on city attractiveness
}

// LandValueModel Implementation
LandValueModel::LandValueModel() = default;
LandValueModel::~LandValueModel() = default;

void LandValueModel::calculate(City& city) {
    for (int y = 0; y < city.height; ++y) {
        for (int x = 0; x < city.width; ++x) {
            auto* p = city.getParcel(x, y);
            if (p) {
                p->landValue = getValue(city, x, y);
            }
        }
    }
}

double LandValueModel::getValue(const City& city, int x, int y) const {
    double value = 1.0;

    // Distance to center
    double dx = x - city.width / 2.0;
    double dy = y - city.height / 2.0;
    double distToCenter = std::sqrt(dx * dx + dy * dy);
    double maxDist = std::sqrt(city.width * city.width + city.height * city.height) / 2.0;
    value += factors.proximityToCenter * (1.0 - distToCenter / maxDist);

    // Road access
    const Parcel* p = city.getParcel(x, y);
    if (p && p->hasRoad) {
        value += factors.proximityToRoad;
    }

    return std::max(0.1, value);
}

// UrbanGrowthModel Implementation
UrbanGrowthModel::UrbanGrowthModel() = default;
UrbanGrowthModel::~UrbanGrowthModel() = default;

void UrbanGrowthModel::simulate(City& city, double deltaTime) {
    spreadGrowth(city);
    breedGrowth(city);
    roadInfluencedGrowth(city);
}

double UrbanGrowthModel::getGrowthProbability(const City& city, int x, int y) const {
    const Parcel* p = city.getParcel(x, y);
    if (!p || p->hasBuilding) return 0.0;

    double prob = params.breedCoefficient;

    if (p->hasRoad) {
        prob += params.roadGravity;
    }

    return std::min(1.0, prob);
}

void UrbanGrowthModel::spreadGrowth(City& city) {
    // Growth spreads from existing development
}

void UrbanGrowthModel::breedGrowth(City& city) {
    // New development in developed areas
}

void UrbanGrowthModel::roadInfluencedGrowth(City& city) {
    // Growth along roads
}

bool UrbanGrowthModel::canDevelop(const City& city, int x, int y) const {
    const Parcel* p = city.getParcel(x, y);
    return p && !p->hasBuilding && p->landUse == LandUse::Vacant;
}

} // namespace Urban
} // namespace NPC
} // namespace Ultima

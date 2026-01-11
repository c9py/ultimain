/**
 * UrbanDynamics.h - Urban Simulation and City Evolution
 *
 * Ported from UrbanSuite model for Ultima world simulation including:
 * - Procedural city generation and evolution
 * - Land use and zoning
 * - Population dynamics
 * - Infrastructure systems
 * - Integration with OSM converter
 *
 * Part of Phase 5: Urban Dynamics
 */

#ifndef ULTIMA_NPC_URBAN_DYNAMICS_H
#define ULTIMA_NPC_URBAN_DYNAMICS_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <functional>

namespace Ultima {
namespace NPC {
namespace Urban {

/**
 * Land use types
 */
enum class LandUse {
    Vacant,
    Residential,
    Commercial,
    Industrial,
    Agricultural,
    Institutional,  // Government, temples, schools
    Military,
    Recreational,
    Natural,        // Parks, forests
    Water,
    Road,
    Special         // Unique buildings
};

/**
 * Building types
 */
enum class BuildingType {
    // Residential
    Hovel,
    House,
    Manor,
    Castle,

    // Commercial
    Shop,
    Market,
    Inn,
    Tavern,
    Bank,
    Warehouse,

    // Industrial
    Workshop,
    Smithy,
    Mill,
    Mine,
    Farm,
    Dock,

    // Institutional
    Temple,
    Shrine,
    TownHall,
    Courthouse,
    Library,
    School,
    Hospital,

    // Military
    Barracks,
    Guardpost,
    Wall,
    Gate,
    Tower,

    // Special
    Monument,
    Well,
    Fountain,
    Bridge,
    Ruin
};

/**
 * Grid cell / Parcel
 */
struct Parcel {
    int x, y;
    LandUse landUse = LandUse::Vacant;
    BuildingType buildingType = BuildingType::Hovel;
    bool hasBuilding = false;

    // Ownership
    std::string ownerId;
    double landValue = 1.0;

    // Properties
    double elevation = 0.0;
    double fertility = 0.5;         // For agriculture
    double accessibility = 0.5;     // Distance to roads
    double desirability = 0.5;

    // Infrastructure
    bool hasRoad = false;
    bool hasWater = false;
    bool hasSewer = false;
    double crimeLevel = 0.0;
};

/**
 * Building
 */
struct Building {
    std::string id;
    BuildingType type;
    int x, y;
    int width = 1;
    int height = 1;

    std::string name;
    std::string ownerId;

    // Physical properties
    double condition = 1.0;         // 0-1 deterioration
    double size = 1.0;              // Relative size
    int capacity = 1;               // People/goods capacity
    int floors = 1;

    // Function
    std::vector<std::string> occupantIds;
    std::string function;           // Current use

    // Value
    double constructionCost;
    double currentValue;
    double rentalValue;

    // Age
    uint32_t constructionDate;
    uint32_t lastRenovation;
};

/**
 * Road segment
 */
struct Road {
    std::string id;
    int x1, y1;
    int x2, y2;

    enum class Type {
        Path,
        Street,
        MainRoad,
        Highway,
        Bridge
    } type = Type::Street;

    double condition = 1.0;
    double traffic = 0.0;
    bool isPassable = true;
};

/**
 * District / Neighborhood
 */
struct District {
    std::string id;
    std::string name;
    std::vector<std::pair<int, int>> bounds;  // Polygon vertices

    // Demographics
    int population = 0;
    double wealthLevel = 0.5;
    double crimeLevel = 0.0;
    std::string dominantClass;      // "poor", "middle", "wealthy"

    // Character
    LandUse primaryUse;
    double commercialActivity = 0.0;
    double industrialActivity = 0.0;
    double residentialDensity = 0.0;

    // Amenities
    double publicServices = 0.5;
    double entertainment = 0.3;
    double safety = 0.5;
};

/**
 * City
 */
class City {
public:
    City(const std::string& id, int width, int height);
    ~City();

    std::string id;
    std::string name;
    int width;
    int height;

    // Grid
    std::vector<std::vector<Parcel>> grid;

    // Structures
    std::map<std::string, Building> buildings;
    std::vector<Road> roads;
    std::vector<District> districts;

    // Demographics
    int population = 0;
    int maxPopulation = 1000;
    double growthRate = 0.01;

    // Economy
    double wealth = 0.0;
    double taxRate = 0.1;
    double treasury = 0.0;

    // Infrastructure
    double roadCoverage = 0.0;
    double waterCoverage = 0.0;

    /**
     * Get parcel at coordinates
     */
    Parcel* getParcel(int x, int y);
    const Parcel* getParcel(int x, int y) const;

    /**
     * Set land use
     */
    void setLandUse(int x, int y, LandUse use);

    /**
     * Add building
     */
    std::string addBuilding(const Building& building);

    /**
     * Remove building
     */
    void removeBuilding(const std::string& buildingId);

    /**
     * Get building at location
     */
    Building* getBuildingAt(int x, int y);

    /**
     * Add road
     */
    void addRoad(int x1, int y1, int x2, int y2, Road::Type type = Road::Type::Street);

    /**
     * Check if location is valid
     */
    bool isValid(int x, int y) const;

    /**
     * Find path between points
     */
    std::vector<std::pair<int, int>> findPath(int x1, int y1, int x2, int y2) const;

    /**
     * Get buildings of type
     */
    std::vector<Building*> getBuildingsByType(BuildingType type);

    /**
     * Calculate statistics
     */
    void calculateStatistics();

private:
    void updateAccessibility();
    void updateDesirability();
};

/**
 * Population model
 */
class PopulationModel {
public:
    struct PopulationSegment {
        std::string name;           // "peasant", "merchant", "noble"
        int count = 0;
        double birthRate = 0.02;
        double deathRate = 0.01;
        double migrationRate = 0.0;
        double wealthLevel = 0.5;
    };

    PopulationModel();
    ~PopulationModel();

    /**
     * Initialize population
     */
    void initialize(int totalPopulation);

    /**
     * Simulate population change
     */
    void simulate(double deltaTime, City& city);

    /**
     * Get total population
     */
    int getTotalPopulation() const;

    /**
     * Get segment
     */
    PopulationSegment* getSegment(const std::string& name);

    /**
     * Add migration
     */
    void addMigration(const std::string& segment, int count);

private:
    std::vector<PopulationSegment> segments_;

    void calculateBirths(double deltaTime);
    void calculateDeaths(double deltaTime);
    void calculateMigration(double deltaTime, const City& city);
};

/**
 * Land value model
 */
class LandValueModel {
public:
    LandValueModel();
    ~LandValueModel();

    /**
     * Calculate land values for city
     */
    void calculate(City& city);

    /**
     * Get value at location
     */
    double getValue(const City& city, int x, int y) const;

    /**
     * Factors affecting land value
     */
    struct ValueFactors {
        double proximityToCenter = 0.3;
        double proximityToRoad = 0.2;
        double proximityToWater = 0.1;
        double neighborhoodQuality = 0.2;
        double crimeEffect = -0.2;
        double pollutionEffect = -0.1;
    };

    ValueFactors factors;

private:
    double calculateProximityFactor(const City& city, int x, int y,
                                   LandUse targetUse) const;
};

/**
 * Urban growth model
 */
class UrbanGrowthModel {
public:
    UrbanGrowthModel();
    ~UrbanGrowthModel();

    /**
     * Growth parameters
     */
    struct Parameters {
        double spreadCoefficient = 0.5;     // Expansion rate
        double breedCoefficient = 0.3;      // New development rate
        double roadGravity = 0.4;           // Attraction to roads
        double slopeResistance = 0.5;       // Terrain effect
        double excludedPercent = 0.1;       // Protected areas
    };

    Parameters params;

    /**
     * Simulate growth step
     */
    void simulate(City& city, double deltaTime);

    /**
     * Get growth probability at location
     */
    double getGrowthProbability(const City& city, int x, int y) const;

    /**
     * Predict future growth
     */
    std::vector<std::pair<int, int>> predictGrowth(const City& city, int steps) const;

private:
    void spreadGrowth(City& city);
    void breedGrowth(City& city);
    void roadInfluencedGrowth(City& city);

    bool canDevelop(const City& city, int x, int y) const;
};

/**
 * Zoning system
 */
class ZoningSystem {
public:
    ZoningSystem();
    ~ZoningSystem();

    /**
     * Zone definition
     */
    struct Zone {
        std::string id;
        std::string name;
        std::vector<LandUse> allowedUses;
        double maxDensity = 1.0;
        double minLotSize = 1.0;
        bool allowMixedUse = false;
    };

    /**
     * Define zone
     */
    void defineZone(const Zone& zone);

    /**
     * Apply zoning to area
     */
    void applyZoning(City& city, int x, int y, int width, int height,
                    const std::string& zoneId);

    /**
     * Check if development is allowed
     */
    bool isAllowed(const City& city, int x, int y, LandUse use) const;

    /**
     * Get zone at location
     */
    std::string getZone(const City& city, int x, int y) const;

private:
    std::map<std::string, Zone> zones_;
    std::map<std::pair<int, int>, std::string> zoneMap_;
};

/**
 * OSM Integration - Converts OSM data to city structure
 */
class OSMIntegration {
public:
    OSMIntegration();
    ~OSMIntegration();

    /**
     * Load OSM data file
     */
    bool loadOSMFile(const std::string& filename);

    /**
     * Convert to Ultima city
     */
    std::unique_ptr<City> convertToCity(int targetWidth, int targetHeight);

    /**
     * Extract buildings
     */
    std::vector<Building> extractBuildings();

    /**
     * Extract roads
     */
    std::vector<Road> extractRoads();

    /**
     * Extract land use
     */
    std::map<std::pair<int, int>, LandUse> extractLandUse();

    /**
     * Map OSM tags to building types
     */
    static BuildingType mapBuildingType(const std::map<std::string, std::string>& tags);

    /**
     * Map OSM tags to land use
     */
    static LandUse mapLandUse(const std::map<std::string, std::string>& tags);

private:
    struct OSMNode {
        int64_t id;
        double lat, lon;
        std::map<std::string, std::string> tags;
    };

    struct OSMWay {
        int64_t id;
        std::vector<int64_t> nodeRefs;
        std::map<std::string, std::string> tags;
    };

    std::map<int64_t, OSMNode> nodes_;
    std::vector<OSMWay> ways_;

    double minLat_, maxLat_, minLon_, maxLon_;

    void calculateBounds();
    std::pair<int, int> projectToGrid(double lat, double lon,
                                      int width, int height) const;
};

/**
 * Procedural city generator
 */
class CityGenerator {
public:
    CityGenerator();
    ~CityGenerator();

    /**
     * Generation parameters
     */
    struct Parameters {
        int width = 100;
        int height = 100;
        int initialPopulation = 500;

        // Layout
        bool hasRiver = false;
        bool hasCoast = false;
        bool hasWalls = false;

        // Seeds
        int seed = 0;

        // Growth
        double organicness = 0.5;   // 0 = grid, 1 = organic
        double density = 0.5;
    };

    Parameters params;

    /**
     * Generate new city
     */
    std::unique_ptr<City> generate(const std::string& name);

    /**
     * Evolve existing city
     */
    void evolve(City& city, int years);

private:
    void generateTerrain(City& city);
    void generateRoads(City& city);
    void generateDistricts(City& city);
    void generateBuildings(City& city);
    void generateInfrastructure(City& city);

    void placeMarketSquare(City& city);
    void placeTemple(City& city);
    void placeCastle(City& city);
};

/**
 * Urban simulation controller
 */
class UrbanSimulation {
public:
    UrbanSimulation();
    ~UrbanSimulation();

    /**
     * Initialize simulation
     */
    void initialize();

    /**
     * Add city
     */
    void addCity(std::shared_ptr<City> city);

    /**
     * Get city
     */
    City* getCity(const std::string& cityId);

    /**
     * Simulate tick
     */
    void simulate(double deltaTime);

    /**
     * Get cities
     */
    std::vector<std::string> getCityIds() const;

    /**
     * Generate new city
     */
    std::shared_ptr<City> generateCity(const std::string& name,
                                       const CityGenerator::Parameters& params);

    /**
     * Import from OSM
     */
    std::shared_ptr<City> importFromOSM(const std::string& filename,
                                        const std::string& cityName);

    /**
     * Save/load
     */
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);

private:
    std::map<std::string, std::shared_ptr<City>> cities_;
    PopulationModel populationModel_;
    LandValueModel landValueModel_;
    UrbanGrowthModel growthModel_;
    ZoningSystem zoningSystem_;
    CityGenerator generator_;
    OSMIntegration osmIntegration_;
};

} // namespace Urban
} // namespace NPC
} // namespace Ultima

#endif // ULTIMA_NPC_URBAN_DYNAMICS_H

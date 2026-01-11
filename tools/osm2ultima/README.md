# OSM to Ultima Map Converter (`osm2ultima`)

This tool procedurally generates playable Ultima VII-style game maps from real-world OpenStreetMap (OSM) data. It allows you to transform any neighborhood, city, or region into a classic RPG world, complete with buildings, roads, forests, and even NPCs.

## Features

- **Real-World Map Generation**: Convert any location on Earth into an Ultima map.
- **Feature-to-Shape Mapping**: Translates OSM features (buildings, roads, forests, rivers) into corresponding Ultima VII game objects and terrain tiles.
- **Procedural Generation**: Populates areas with appropriate details, such as placing trees in forests, furniture in houses, and NPCs in towns.
- **Coordinate Transformation**: Accurately maps geographic coordinates (latitude/longitude) to Ultima's tile-based coordinate system.
- **Multiple Output Formats**: Exports the generated map to several formats for different use cases:
    - **GeoJSON**: For visualization and analysis in GIS software (e.g., QGIS).
    - **IREG files**: Binary object data compatible with the Exult engine.
    - **Text Map**: A simple ASCII representation for quick previews.
    - **JSON Summary**: A summary of the generated map's statistics.

## How It Works

The conversion process follows these steps:

1.  **Data Fetching**: The tool takes a geographic bounding box or a place name (e.g., "Covent Garden, London") as input. It then queries the Overpass API to download all relevant OSM data for that area, including nodes, ways, and relations for buildings, highways, natural features, and more.

2.  **Shape Mapping**: A comprehensive Python dictionary (`osm_shape_mapping.py`) maps thousands of OSM tags to specific Ultima VII shape numbers. For example:
    - `building=house` is mapped to wall, roof, door, and window shapes.
    - `natural=tree` is mapped to various tree shapes.
    - `highway=residential` is mapped to cobblestone terrain tiles.

3.  **Coordinate Transformation**: A `CoordinateTransformer` class converts the WGS 84 latitude/longitude coordinates from OSM into the 2D tile coordinates used by the Ultima engine. It scales the real-world area to fit the desired map size in the game.

4.  **Map Generation**: The `MapGenerator` class processes the OSM data element by element:
    - **Areas (Polygons)**: Land use and natural areas (like forests or parks) are filled with the corresponding base terrain (e.g., grass, sand, water).
    - **Lines (Ways)**: Roads and rivers are drawn as lines of terrain tiles.
    - **Buildings**: Building footprints are processed to create structures with floors, walls, roofs, and doors. NPCs are sometimes placed inside based on the building type.
    - **Points (Nodes)**: Individual features like trees, fountains, or benches are placed as objects on the map.

5.  **Exporting**: The final `UltimaMap` data structure, containing all the generated terrain and objects, is exported into the selected file formats.

## Usage

The tool is a command-line script that can be run from the `tools/osm2ultima` directory.

### Prerequisites

- Python 3
- `requests` library (`pip install requests`)

### Examples

**Generate a map from a place name:**

This will generate a 16x16 chunk map (256x256 tiles) of a 1km radius around the Eiffel Tower.

```bash
python3 osm2ultima.py --place "Eiffel Tower, Paris" --radius 1000 --size 16,16 --output paris_map
```

**Generate a map from a specific bounding box:**

This will generate a map of a small area in central London.

```bash
python3 osm2ultima.py --bbox "-0.128,51.51,-0.120,51.515" --size 8,8 --output london_map
```

### Command-Line Arguments

| Argument | Description |
|---|---|
| `--bbox "lon,lat,lon,lat"` | **Required**. The bounding box (min_lon, min_lat, max_lon, max_lat). |
| `--place "Place Name"` | **Required**. A place name to geocode and use as the center. |
| `--radius <meters>` | The radius in meters to use with `--place`. Default: `500`. |
| `--output <name>` | **Required**. The name of the output directory for the generated files. |
| `--size <w,h>` | The desired map size in chunks (1 chunk = 16x16 tiles). Default: `16,16`. |
| `--format <format>` | The output format. Can be `all`, `geojson`, `ireg`, or `text`. Default: `all`. |

## Included Sample: Covent Garden

A sample map of Covent Garden, London, is included in the `covent_garden_test` directory. It was generated with the following command:

```bash
python3 osm2ultima.py --place "Covent Garden, London" --radius 300 --size 4,4
```

- `terrain.txt`: A text preview of the generated terrain.
- `map.geojson`: A GeoJSON file you can open in QGIS or other GIS software to see the generated objects and terrain.
- `summary.json`: Statistics about the generated map, including object counts.

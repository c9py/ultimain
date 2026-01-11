# World Map Structure Analysis

This document describes how world maps are structured in both the Exult (Ultima VII) and ScummVM Ultima8 (Pagan) engines.

## Overview

Both engines use a hierarchical tile-based system, but with different dimensions and organizational approaches.

| Feature | Exult (Ultima VII) | ScummVM (Ultima VIII) |
|---------|-------------------|----------------------|
| **Total Map Size** | 192 × 192 chunks | 64 × 64 chunks |
| **Chunk Size** | 16 × 16 tiles | Variable (512 × 512 world units) |
| **Superchunks** | 12 × 12 (16 chunks each) | Not used |
| **Total Tiles** | 3072 × 3072 | N/A (3D coordinate system) |
| **Coordinate System** | 2D tile-based | 3D world coordinates |
| **Multiple Maps** | Yes (map #) | Yes (map number) |

## Exult (Ultima VII) Map Structure

### Hierarchy

```
World
└── Game_map (multiple maps supported)
    └── Superchunks (12 × 12 grid = 144 total)
        └── Chunks (16 × 16 per superchunk = 192 × 192 total)
            └── Tiles (16 × 16 per chunk)
                └── Objects (at various lift levels)
```

### Constants (from `exult_constants.h`)

```cpp
c_tiles_per_chunk = 16      // A chunk is 16×16 tiles
c_num_chunks = 192          // 12 × 16 = 192 chunks in each direction
c_chunks_per_schunk = 16    // 16 chunks per superchunk
c_num_tiles = 3072          // 16 × 192 = 3072 tiles in each direction
```

### Key Classes

**`Game_map`** - The main map container:
- Manages a 192 × 192 grid of chunks
- Tracks terrain data via `terrain_map[cx][cy]`
- Handles superchunk loading/caching for performance
- Supports map patches for modifications

**`Map_chunk`** - Individual 16×16 tile area:
- Contains lists of game objects
- Manages terrain rendering
- Handles collision detection via `Chunk_cache`

**`Chunk_terrain`** - Static terrain data:
- Loaded from `u7chunks` file
- Can be 2-byte or 3-byte format (v2_chunks flag)
- Shared across all maps

**`Chunk_cache`** - Runtime collision/interaction data:
- Tracks blocked tiles at each lift level (8 levels supported)
- Manages egg triggers (event areas)
- Tracks doors for pathfinding

### Data Files

| File | Purpose |
|------|---------|
| `u7chunks` | Chunk terrain definitions |
| `u7map` | Chunk-to-terrain mapping |
| `u7ifix*` | Fixed (static) objects per superchunk |
| `u7ireg*` | Dynamic objects per superchunk |

### Object Positioning

Objects in Exult use a tile-based coordinate system with lift:
- **X, Y**: Tile coordinates (0-3071)
- **Lift**: Height level (0-15, with extended support)
- **Shape/Frame**: Visual appearance

## ScummVM Ultima VIII Map Structure

### Hierarchy

```
World
└── Map (multiple maps, map 0 contains NPCs)
    └── CurrentMap (active map with spatial indexing)
        └── Chunks (64 × 64 grid)
            └── Items (at 3D world coordinates)
```

### Constants (from `current_map.h`)

```cpp
MAP_NUM_CHUNKS = 64         // 64 chunks in each direction
// Chunk size is 512 × 512 world units
// Total map size: 32768 × 32768 world units
```

### Key Classes

**`World`** - Global game state:
- Contains all Map objects
- Manages the CurrentMap (active map)
- Handles NPC data (stored in Map #0)
- Loads from `nonfixed.dat`, `itemcach.dat`, `npcdata.dat`

**`Map`** - Individual map data:
- Contains fixed items (static world geometry)
- Contains dynamic items (moveable objects)
- Fixed items can be cached out for memory efficiency

**`CurrentMap`** - Active map with spatial queries:
- 64 × 64 chunk grid for spatial indexing
- Fast lookup via `_fast` bitfield array
- Supports collision detection and pathfinding
- Handles item sweeping for movement

**`Item`** - Base game object:
- 3D position (x, y, z in world coordinates)
- Shape and frame for rendering
- Flags for behavior (solid, fixed, etc.)

### Data Files

| File | Purpose |
|------|---------|
| `fixed.dat` | Static world geometry per map |
| `nonfixed.dat` | Dynamic objects |
| `itemcach.dat` | Item cache data |
| `npcdata.dat` | NPC definitions and state |
| `glob*.dat` | Map glob (grouped object) data |

### Coordinate System

Ultima VIII uses a true 3D coordinate system:
- **X, Y**: Horizontal position in world units
- **Z**: Vertical position (height)
- World units are finer-grained than tiles
- Collision uses bounding boxes, not tile blocking

### Spatial Queries

The `CurrentMap` class provides efficient spatial operations:
- **`areaSearch`**: Find items matching a script in a radius
- **`surfaceSearch`**: Find items above/below an item
- **`sweepTest`**: Collision detection for movement
- **`getPositionInfo`**: Check if a position is valid

## Comparison

### Similarities

1. **Chunk-based organization**: Both use chunks for spatial partitioning
2. **Multiple maps**: Both support multiple game maps
3. **Fixed vs. dynamic items**: Both distinguish static and moveable objects
4. **Caching**: Both cache out unused data for memory efficiency

### Differences

| Aspect | Exult | ScummVM U8 |
|--------|-------|------------|
| **Coordinate precision** | Tile-based (coarse) | World units (fine) |
| **Height handling** | Discrete lift levels | Continuous Z coordinate |
| **Collision** | Tile blocking flags | 3D bounding box sweeps |
| **Superchunks** | Yes (for file organization) | No |
| **Terrain** | Separate terrain layer | Integrated with items |

### Integration Opportunities

1. **Shared file formats**: Both use Flex archives for data storage
2. **Common rendering**: Both use shape-based rendering
3. **Unified coordinate abstraction**: Could create a common interface
4. **Shared pathfinding**: Core algorithms are similar

## File Format Details

### Flex Archives

Both engines use the Flex archive format for storing game data:

```cpp
struct FlexHeader {
    char magic[80];      // "Ultima VII Data File"
    uint32 count;        // Number of entries
    // Followed by offset/size pairs for each entry
};
```

### Chunk Data (Exult)

Each chunk terrain entry contains:
- Shape numbers for each of 256 tiles (16×16)
- 2 or 3 bytes per tile depending on version

### Fixed Data (Ultima VIII)

Fixed items are stored with:
- Shape number (16-bit)
- Frame number (16-bit)
- X, Y, Z coordinates (16-bit each)
- Flags and extended data

## Conclusion

While both engines share common heritage and some file formats, they take fundamentally different approaches to world representation. Exult uses a traditional 2D tile-based system with discrete height levels, while Ultima VIII employs a true 3D coordinate system with continuous positioning. Any integration effort should focus on the shared Flex file format and shape rendering systems, while maintaining separate spatial indexing implementations.

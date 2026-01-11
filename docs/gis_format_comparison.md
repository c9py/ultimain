# Ultima Map Formats vs GIS Standards

This document analyzes how the Ultima game map formats compare to standard GIS (Geographic Information System) file formats.

## Executive Summary

The Ultima map formats are most similar to **raster-based tile maps** combined with **vector point features**, resembling a hybrid between:

1. **GeoTIFF/Raster tiles** (for terrain/chunks)
2. **GeoJSON/Shapefile points** (for objects/items)

However, they are **not directly compatible** with standard GIS formats due to:
- Custom coordinate systems (not lat/lon or projected)
- Binary packed formats (not text-based like GeoJSON)
- Game-specific attributes (shape, frame, quality, flags)

## Coordinate System Comparison

| Aspect | Ultima VII (Exult) | Ultima VIII | GeoJSON | Shapefile |
|--------|-------------------|-------------|---------|-----------|
| **Coordinate Type** | Integer tile coords | Integer world units | Float lat/lon or projected | Float coordinates |
| **Dimensions** | 2D + lift (discrete Z) | True 3D (x, y, z) | 2D or 3D | 2D or 3D |
| **Range** | 0-3071 tiles | 0-32767 units | Global or projected | Varies |
| **Precision** | 1 tile (~8 feet) | 1 unit (~0.5 feet) | Float64 | Float64 |
| **Wrapping** | Toroidal (wraps) | Bounded | Spherical or planar | Planar |

## Data Structure Comparison

### Ultima VII (Exult) IREG Format

```
Record Structure (10-13 bytes per object):
┌─────────┬──────────┬──────────┬───────────┬───────────┬─────────┬─────────┐
│ Length  │ X coord  │ Y coord  │ Shape Lo  │ Shape/Frm │ Lift    │ Quality │
│ 1 byte  │ 1 byte   │ 1 byte   │ 1 byte    │ 1 byte    │ 1 byte  │ 1 byte  │
└─────────┴──────────┴──────────┴───────────┴───────────┴─────────┴─────────┘

X coord = (chunk_x % 16) << 4 | tile_x
Y coord = (chunk_y % 16) << 4 | tile_y
```

### Ultima VIII Fixed Format

```
Record Structure (16 bytes per object):
┌──────────┬──────────┬─────────┬───────────┬───────────┬─────────┬─────────┬────────┬────────┬────────┐
│ X coord  │ Y coord  │ Z coord │ Shape     │ Frame     │ Flags   │ Quality │ NPC#   │ Map#   │ Next   │
│ 2 bytes  │ 2 bytes  │ 1 byte  │ 2 bytes   │ 1 byte    │ 2 bytes │ 2 bytes │ 1 byte │ 1 byte │ 2 bytes│
└──────────┴──────────┴─────────┴───────────┴───────────┴─────────┴─────────┴────────┴────────┴────────┘
```

### GeoJSON Point Feature

```json
{
  "type": "Feature",
  "geometry": {
    "type": "Point",
    "coordinates": [longitude, latitude, elevation]
  },
  "properties": {
    "shape": 123,
    "frame": 0,
    "quality": 100,
    "flags": ["fixed", "solid"]
  }
}
```

### Shapefile DBF Record

```
Fixed-width fields defined in .dbf header:
SHAPE_ID    N   10
FRAME       N    5
X_COORD     N   10
Y_COORD     N   10
Z_COORD     N   10
QUALITY     N    5
FLAGS       C   50
```

## Closest GIS Equivalents

### 1. Terrain/Chunks → Raster Tiles (GeoTIFF)

The chunk/terrain system is most similar to **tiled raster data**:

| Ultima Concept | GIS Equivalent |
|----------------|----------------|
| Chunk (16×16 tiles) | Tile in a tile pyramid |
| Superchunk (16×16 chunks) | Tile at a coarser zoom level |
| Terrain ID | Raster cell value / palette index |
| u7chunks file | Indexed color GeoTIFF |

**Similarity: ~70%** - Both use regular grids with indexed values, but Ultima uses a custom binary format rather than standard TIFF.

### 2. Objects/Items → Vector Points (GeoJSON/Shapefile)

Game objects are most similar to **point features with attributes**:

| Ultima Concept | GIS Equivalent |
|----------------|----------------|
| Item/Object | Point Feature |
| (x, y, z) coordinates | Point geometry |
| Shape number | Feature type / symbol ID |
| Frame | Sub-type or rotation |
| Quality | Numeric attribute |
| Flags | Boolean attributes |
| Container hierarchy | Feature relationships |

**Similarity: ~60%** - Conceptually similar, but Ultima uses packed binary with game-specific semantics.

### 3. Eggs/Triggers → Polygon/Region Features

Trigger areas (eggs) resemble **polygon features with event handlers**:

| Ultima Concept | GIS Equivalent |
|----------------|----------------|
| Egg area | Polygon geometry |
| Trigger type | Feature class |
| Usecode function | Linked script/action |
| Activation criteria | Attribute filter |

**Similarity: ~50%** - GIS has spatial triggers but not game-event semantics.

## Format Conversion Feasibility

### Ultima → GeoJSON

**Feasibility: High** (with caveats)

```python
def ultima_to_geojson(items, scale_factor=1.0):
    features = []
    for item in items:
        feature = {
            "type": "Feature",
            "geometry": {
                "type": "Point",
                "coordinates": [
                    item.x * scale_factor,
                    item.y * scale_factor,
                    item.z * scale_factor
                ]
            },
            "properties": {
                "shape": item.shape,
                "frame": item.frame,
                "quality": item.quality,
                "flags": item.flags,
                "game": "ultima7"  # or "ultima8"
            }
        }
        features.append(feature)
    
    return {
        "type": "FeatureCollection",
        "features": features,
        "crs": {
            "type": "name",
            "properties": {
                "name": "urn:ogc:def:crs:EPSG::3857"  # or custom CRS
            }
        }
    }
```

**Challenges:**
- No standard CRS (would need custom projection)
- Shape/frame semantics lost without symbol library
- Container hierarchy not representable in flat GeoJSON

### Ultima → Shapefile

**Feasibility: Medium**

- Point shapefile for objects ✓
- Polygon shapefile for chunks/regions ✓
- DBF attributes for properties ✓
- No native 3D support (Z in separate field)
- No hierarchical relationships

### GeoJSON → Ultima

**Feasibility: Low**

- Would need shape/frame mapping table
- Coordinate system transformation required
- Game-specific flags must be inferred or defaulted
- Container relationships must be reconstructed

## Recommended Interchange Format

For maximum compatibility, a **custom GeoJSON extension** would be ideal:

```json
{
  "type": "FeatureCollection",
  "ultima_metadata": {
    "game": "ultima7",
    "map_number": 0,
    "coordinate_system": "tile",
    "tile_size": 8,
    "chunk_size": 16,
    "world_size": [3072, 3072]
  },
  "features": [
    {
      "type": "Feature",
      "geometry": {
        "type": "Point",
        "coordinates": [1234, 5678, 4]
      },
      "properties": {
        "ultima_type": "item",
        "shape": 150,
        "frame": 0,
        "quality": 100,
        "lift": 4,
        "flags": {
          "fixed": false,
          "solid": true,
          "okay_to_take": true
        },
        "container_id": null
      }
    }
  ]
}
```

## Comparison Table

| Feature | Ultima VII | Ultima VIII | GeoJSON | Shapefile | GeoTIFF |
|---------|------------|-------------|---------|-----------|---------|
| **Format** | Binary | Binary | Text (JSON) | Binary | Binary |
| **Coordinates** | Integer | Integer | Float | Float | Raster grid |
| **3D Support** | Lift levels | True 3D | Optional Z | Limited | Bands |
| **Attributes** | Fixed schema | Fixed schema | Flexible | Fixed schema | Metadata |
| **Relationships** | Containers | Containers | None | None | None |
| **Streaming** | Chunked | Chunked | Full load | Full load | Tiled |
| **Compression** | None | None | Optional | None | LZW/etc |
| **Human readable** | No | No | Yes | Partial | No |

## Conclusion

The Ultima map formats occupy a middle ground between raster and vector GIS formats:

1. **Terrain data** is closest to **indexed raster tiles** (like paletted GeoTIFF)
2. **Object data** is closest to **point features** (like GeoJSON or Shapefile points)
3. **The coordinate system** is a custom game-specific projection, not compatible with standard CRS

For practical interchange:
- **Export**: Convert to GeoJSON with custom properties and a defined scale factor
- **Import**: Would require a mapping table for shapes and careful coordinate transformation
- **Visualization**: Could use standard GIS tools with custom symbology

The binary packed format is optimized for game performance, not interoperability, making direct GIS tool usage impractical without a conversion layer.

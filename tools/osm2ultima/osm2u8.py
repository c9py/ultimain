#!/usr/bin/env python3
"""
OSM to Ultima VIII Map Converter (osm2u8)

Converts OpenStreetMap data to Ultima VIII game map format (FIXED.DAT/NONFIXED.DAT).
This is a companion to osm2ultima.py which handles Ultima VII IREG format.

Usage:
    python osm2u8.py --bbox "min_lon,min_lat,max_lon,max_lat" --output map_dir
    python osm2u8.py --place "London, UK" --radius 500 --output london_u8

The output directory will contain:
    - fixed/FIXED.DAT: Fixed object data (immovable objects like terrain, walls)
    - nonfixed/NONFIXED.DAT: Non-fixed object data (movable objects, NPCs)
    - map.geojson: Visualization file for GIS tools
    - summary.json: Generation statistics

Output Format:
    U8 uses a 16-byte record format per object:
    - X position (2 bytes)
    - Y position (2 bytes)
    - Z position (1 byte)
    - Shape number (2 bytes)
    - Frame number (1 byte)
    - Flags (2 bytes)
    - Quality (2 bytes)
    - NPC number (1 byte)
    - Map number (1 byte)
    - Next object ID (2 bytes)

Coordinate System:
    U8 uses world coordinates in range 0-65535 (16-bit integers).
    OSM coordinates are scaled and mapped to this range.

Shape Mapping:
    U8 shapes are DIFFERENT from U7 shapes. This tool uses approximate
    mappings documented in u8_shape_mapping.py. Verify shapes against
    actual U8 game data for best results.
"""

import argparse
import hashlib
import json
import os
import random
import sys
from dataclasses import dataclass, field
from typing import Dict, List, Optional, Tuple, Any

# Import from existing osm2ultima modules
from osm2ultima import (
    OSMFetcher,
    UltimaObject,
    UltimaChunk,
    UltimaMap,
    MapGenerator,
)

# Import U8-specific modules
from u8_format import (
    U8Object,
    U8MapData,
    U8FixedDatWriter,
    U8_RECORD_SIZE,
    convert_osm_to_u8_coords,
)

from u8_shape_mapping import (
    U8CoordinateTransformer,
    get_u8_terrain_shape,
    get_u8_object_shapes,
    convert_u7_shape_to_u8,
    U8_TERRAIN_SHAPES,
    U8_OBJECT_SHAPES,
    U8_NPC_SHAPES,
    OSM_HIGHWAY_TO_U8_TERRAIN,
    U8_DEFAULT_SHAPE,
)


# =============================================================================
# U8 MAP GENERATOR
# =============================================================================

class U8MapGenerator(MapGenerator):
    """
    Generates Ultima VIII map from OSM data.
    
    Extends the base MapGenerator to produce U8-compatible output.
    Overrides coordinate transformation and shape mapping for U8 format.
    """

    def __init__(self, bbox: Tuple[float, float, float, float], 
                 map_size: Tuple[int, int] = (256, 256)):
        """
        Initialize U8 generator.
        
        Args:
            bbox: (min_lon, min_lat, max_lon, max_lat)
            map_size: (width_tiles, height_tiles) - target size in tiles
        """
        # Call parent init
        super().__init__(bbox, map_size)
        
        # Override with U8-specific coordinate transformer
        self.u8_transformer = U8CoordinateTransformer(
            min_lon=bbox[0],
            min_lat=bbox[1],
            max_lon=bbox[2],
            max_lat=bbox[3]
        )
        
        # U8-specific tracking
        self.u8_objects: List[U8Object] = []  # All generated U8 objects
        self.u8_fixed_objects: List[U8Object] = []  # Fixed objects (terrain, walls)
        self.u8_nonfixed_objects: List[U8Object] = []  # Non-fixed objects (items, NPCs)
        
        # Statistics
        self.u8_stats = {
            "terrain_objects": 0,
            "building_objects": 0,
            "road_objects": 0,
            "item_objects": 0,
            "npc_objects": 0,
        }

    def _osm_to_u8_world(self, lon: float, lat: float) -> Tuple[int, int]:
        """Convert OSM coordinates to U8 world coordinates."""
        return self.u8_transformer.osm_to_u8(lon, lat)

    def process_osm_data(self, osm_data: dict):
        """
        Process OSM data and populate U8 object lists.
        
        This overrides the parent method to generate U8-formatted output.
        """
        elements = osm_data.get("elements", [])
        
        # First pass: cache all nodes
        for element in elements:
            if element["type"] == "node":
                self.nodes[element["id"]] = (element["lon"], element["lat"])
        
        # Second pass: process ways and relations for U8
        for element in elements:
            if element["type"] == "way":
                self._process_way_u8(element)
            elif element["type"] == "node" and "tags" in element:
                self._process_node_u8(element)
        
        print(f"Processed {len(elements)} OSM elements for U8")
        print(f"Generated {len(self.u8_fixed_objects)} fixed objects")
        print(f"Generated {len(self.u8_nonfixed_objects)} non-fixed objects")

    def _process_node_u8(self, node: dict):
        """Process a single OSM node for U8 format."""
        tags = node.get("tags", {})
        lon, lat = node["lon"], node["lat"]
        world_x, world_y = self._osm_to_u8_world(lon, lat)
        
        # Get U8 object shapes for this node
        shapes = get_u8_object_shapes(tags)
        
        if shapes and "main" in shapes:
            shape = random.choice(shapes["main"])
            obj = U8Object(
                x=world_x,
                y=world_y,
                z=0,
                shape=shape,
                frame=0,
                flags=0,
                quality=0,
                npcnum=0,
                mapnum=0
            )
            self.u8_nonfixed_objects.append(obj)
            self.u8_stats["item_objects"] += 1

    def _process_way_u8(self, way: dict):
        """Process a single OSM way for U8 format."""
        tags = way.get("tags", {})
        nodes = way.get("nodes", [])
        
        if not nodes:
            return
        
        # Get coordinates for all nodes in the way
        coords = []
        for node_id in nodes:
            if node_id in self.nodes:
                coords.append(self.nodes[node_id])
        
        if not coords:
            return
        
        # Check if it's a closed way (polygon)
        is_polygon = len(nodes) > 2 and nodes[0] == nodes[-1]
        
        # Process based on tags
        if "highway" in tags:
            self._process_highway_u8(tags, coords)
        elif "building" in tags:
            self._process_building_u8(tags, coords)
        elif "landuse" in tags or "natural" in tags:
            self._process_area_u8(tags, coords, is_polygon)
        elif "waterway" in tags:
            self._process_waterway_u8(tags, coords)

    def _process_highway_u8(self, tags: dict, coords: List[Tuple[float, float]]):
        """Process a road/path for U8 format."""
        highway_type = tags.get("highway", "residential")
        terrain_type = OSM_HIGHWAY_TO_U8_TERRAIN.get(highway_type, "cobblestone")
        terrain_shapes = U8_TERRAIN_SHAPES.get(terrain_type, U8_TERRAIN_SHAPES["cobblestone"])
        
        # Generate ground objects along the road
        for i in range(len(coords) - 1):
            self._draw_u8_line(coords[i], coords[i + 1], terrain_shapes, is_fixed=True)
        
        self.u8_stats["road_objects"] += len(coords)

    def _process_building_u8(self, tags: dict, coords: List[Tuple[float, float]]):
        """Process a building polygon for U8 format."""
        if len(coords) < 3:
            return
        
        # Get building center
        center_lon = sum(c[0] for c in coords) / len(coords)
        center_lat = sum(c[1] for c in coords) / len(coords)
        center_x, center_y = self._osm_to_u8_world(center_lon, center_lat)
        
        # Get building shapes
        shapes = get_u8_object_shapes(tags)
        
        # Place floor objects
        floor_shapes = shapes.get("floor", U8_TERRAIN_SHAPES["ground"])
        
        # Get bounding box in world coords
        world_coords = [self._osm_to_u8_world(c[0], c[1]) for c in coords]
        min_x = min(c[0] for c in world_coords)
        max_x = max(c[0] for c in world_coords)
        min_y = min(c[1] for c in world_coords)
        max_y = max(c[1] for c in world_coords)
        
        # Place floor tiles (sampling at intervals)
        step = 512  # World units between samples
        for x in range(min_x, max_x + 1, step):
            for y in range(min_y, max_y + 1, step):
                obj = U8Object(
                    x=x, y=y, z=0,
                    shape=random.choice(floor_shapes),
                    frame=0,
                    flags=0,
                    quality=0
                )
                self.u8_fixed_objects.append(obj)
                self.u8_stats["terrain_objects"] += 1
        
        # Place wall objects along perimeter
        wall_shapes = shapes.get("walls", [497])
        for i in range(len(coords) - 1):
            self._draw_u8_line(coords[i], coords[i + 1], wall_shapes, 
                              is_fixed=True, lift=1)
        
        self.u8_stats["building_objects"] += 1

    def _process_area_u8(self, tags: dict, coords: List[Tuple[float, float]], is_polygon: bool):
        """Process a landuse or natural area for U8 format."""
        terrain_shapes = get_u8_terrain_shape(tags)
        
        if is_polygon and len(coords) >= 3:
            # Sample points within the polygon
            world_coords = [self._osm_to_u8_world(c[0], c[1]) for c in coords]
            min_x = min(c[0] for c in world_coords)
            max_x = max(c[0] for c in world_coords)
            min_y = min(c[1] for c in world_coords)
            max_y = max(c[1] for c in world_coords)
            
            step = 1024  # World units between samples (coarser for areas)
            for x in range(min_x, max_x + 1, step):
                for y in range(min_y, max_y + 1, step):
                    if self._point_in_polygon_world(x, y, world_coords):
                        obj = U8Object(
                            x=x, y=y, z=0,
                            shape=random.choice(terrain_shapes),
                            frame=0,
                            flags=0,
                            quality=0
                        )
                        self.u8_fixed_objects.append(obj)
                        self.u8_stats["terrain_objects"] += 1
        else:
            # Draw as line
            for i in range(len(coords) - 1):
                self._draw_u8_line(coords[i], coords[i + 1], terrain_shapes, is_fixed=True)

    def _process_waterway_u8(self, tags: dict, coords: List[Tuple[float, float]]):
        """Process a waterway for U8 format."""
        water_shapes = U8_TERRAIN_SHAPES.get("water", [347])
        
        for i in range(len(coords) - 1):
            self._draw_u8_line(coords[i], coords[i + 1], water_shapes, 
                              is_fixed=True, width=3)

    def _draw_u8_line(self, start: Tuple[float, float], end: Tuple[float, float],
                      shapes: List[int], is_fixed: bool = True, 
                      lift: int = 0, width: int = 1):
        """Draw a line of U8 objects between two OSM coordinates."""
        start_world = self._osm_to_u8_world(start[0], start[1])
        end_world = self._osm_to_u8_world(end[0], end[1])
        
        dx = abs(end_world[0] - start_world[0])
        dy = abs(end_world[1] - start_world[1])
        dist = max(dx, dy)
        
        if dist == 0:
            return
        
        # Sample along the line (not every pixel, but at reasonable intervals)
        step = max(256, dist // 10)  # At most 10 objects per line segment
        for i in range(0, dist + 1, step):
            t = i / dist
            x = int(start_world[0] + t * (end_world[0] - start_world[0]))
            y = int(start_world[1] + t * (end_world[1] - start_world[1]))
            
            obj = U8Object(
                x=x, y=y, z=lift * 8,
                shape=random.choice(shapes),
                frame=0,
                flags=0,
                quality=0
            )
            
            if is_fixed:
                self.u8_fixed_objects.append(obj)
            else:
                self.u8_nonfixed_objects.append(obj)

    def _point_in_polygon_world(self, x: int, y: int, polygon: List[Tuple[int, int]]) -> bool:
        """Check if a world coordinate is inside a polygon (ray casting)."""
        n = len(polygon)
        inside = False
        
        j = n - 1
        for i in range(n):
            xi, yi = polygon[i]
            xj, yj = polygon[j]
            
            if ((yi > y) != (yj > y)) and (x < (xj - xi) * (y - yi) / (yj - yi) + xi):
                inside = not inside
            
            j = i
        
        return inside


# =============================================================================
# U8 MAP EXPORTER
# =============================================================================

class U8MapExporter:
    """Exports generated map data to U8 format files."""

    def __init__(self, generator: U8MapGenerator):
        self.generator = generator

    def export_fixed_dat(self, output_dir: str, map_number: int = 0):
        """
        Export fixed objects to FIXED.DAT format.
        
        Args:
            output_dir: Directory to write fixed/FIXED.DAT
            map_number: Map number to populate (default 0 for main map)
        """
        fixed_dir = os.path.join(output_dir, "fixed")
        os.makedirs(fixed_dir, exist_ok=True)
        
        writer = U8FixedDatWriter(map_count=256)
        writer.add_map(map_number, self.generator.u8_fixed_objects)
        writer.write(os.path.join(fixed_dir, "FIXED.DAT"))
        
        print(f"Exported {len(self.generator.u8_fixed_objects)} fixed objects to {fixed_dir}/FIXED.DAT")

    def export_nonfixed_dat(self, output_dir: str, map_number: int = 0):
        """
        Export non-fixed objects to NONFIXED.DAT format.
        
        Args:
            output_dir: Directory to write nonfixed/NONFIXED.DAT
            map_number: Map number to populate (default 0 for main map)
        """
        nonfixed_dir = os.path.join(output_dir, "nonfixed")
        os.makedirs(nonfixed_dir, exist_ok=True)
        
        writer = U8FixedDatWriter(map_count=256)
        writer.add_map(map_number, self.generator.u8_nonfixed_objects)
        writer.write(os.path.join(nonfixed_dir, "NONFIXED.DAT"))
        
        print(f"Exported {len(self.generator.u8_nonfixed_objects)} non-fixed objects to {nonfixed_dir}/NONFIXED.DAT")

    def export_geojson(self, output_path: str):
        """Export map as GeoJSON for visualization."""
        features = []
        
        # Export fixed objects
        for obj in self.generator.u8_fixed_objects:
            feature = {
                "type": "Feature",
                "geometry": {
                    "type": "Point",
                    "coordinates": [obj.x, obj.y, obj.z]
                },
                "properties": {
                    "type": "fixed",
                    "shape": obj.shape,
                    "frame": obj.frame,
                    "flags": obj.flags,
                    "quality": obj.quality
                }
            }
            features.append(feature)
        
        # Export non-fixed objects
        for obj in self.generator.u8_nonfixed_objects:
            feature = {
                "type": "Feature",
                "geometry": {
                    "type": "Point",
                    "coordinates": [obj.x, obj.y, obj.z]
                },
                "properties": {
                    "type": "nonfixed",
                    "shape": obj.shape,
                    "frame": obj.frame,
                    "flags": obj.flags,
                    "quality": obj.quality,
                    "npcnum": obj.npcnum
                }
            }
            features.append(feature)
        
        geojson = {
            "type": "FeatureCollection",
            "features": features,
            "u8_metadata": {
                "format": "ultima8",
                "total_fixed": len(self.generator.u8_fixed_objects),
                "total_nonfixed": len(self.generator.u8_nonfixed_objects),
                "coordinate_system": "u8_world",
                "coordinate_range": [0, 65535]
            }
        }
        
        with open(output_path, "w") as f:
            json.dump(geojson, f, indent=2)
        
        print(f"Exported GeoJSON to {output_path}")

    def export_summary(self, output_path: str, seed: str = None):
        """Export a summary of the generated map."""
        # Count shapes
        shape_counts = {}
        for obj in self.generator.u8_fixed_objects + self.generator.u8_nonfixed_objects:
            shape_counts[obj.shape] = shape_counts.get(obj.shape, 0) + 1
        
        summary = {
            "format": "ultima8",
            "statistics": {
                "total_fixed_objects": len(self.generator.u8_fixed_objects),
                "total_nonfixed_objects": len(self.generator.u8_nonfixed_objects),
                "unique_shapes": len(shape_counts)
            },
            "generation_stats": self.generator.u8_stats,
            "shape_counts": dict(sorted(shape_counts.items(), key=lambda x: -x[1])[:20])
        }
        
        if seed:
            summary["seed"] = seed
        
        with open(output_path, "w") as f:
            json.dump(summary, f, indent=2)
        
        print(f"Exported summary to {output_path}")


# =============================================================================
# MAIN
# =============================================================================

def main():
    parser = argparse.ArgumentParser(
        description="Convert OpenStreetMap data to Ultima VIII map format",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python osm2u8.py --place "London, UK" --radius 500 --output london_u8
  python osm2u8.py --bbox "-0.128,51.51,-0.120,51.515" --output test_u8

Output:
  The output directory will contain:
    fixed/FIXED.DAT     - Fixed objects (terrain, walls)
    nonfixed/NONFIXED.DAT - Non-fixed objects (items, NPCs)
    map.geojson         - Visualization for GIS tools
    summary.json        - Generation statistics

Notes:
  U8 shape mappings are approximate. Verify against actual game data.
  See u8_shape_mapping.py for shape number documentation.
"""
    )
    
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--bbox", type=str, help="Bounding box: min_lon,min_lat,max_lon,max_lat")
    group.add_argument("--place", type=str, help="Place name to geocode")
    
    parser.add_argument("--radius", type=float, default=500, 
                        help="Radius in meters (for --place). Default: 500")
    parser.add_argument("--output", type=str, required=True, 
                        help="Output directory name")
    parser.add_argument("--map-number", type=int, default=0,
                        help="U8 map number to populate (0-255). Default: 0")
    parser.add_argument("--seed", type=str, default=None,
                        help="Random seed for reproducible generation")
    
    args = parser.parse_args()
    
    # Initialize random seed
    if args.seed is not None:
        try:
            seed_value = int(args.seed)
        except ValueError:
            seed_value = int(hashlib.md5(args.seed.encode()).hexdigest(), 16) % (2**32)
        random.seed(seed_value)
        print(f"Using random seed: {args.seed} (value: {seed_value})")
    
    # Get bounding box
    fetcher = OSMFetcher()
    
    if args.bbox:
        bbox = tuple(map(float, args.bbox.split(",")))
    else:
        lon, lat = fetcher.geocode_place(args.place)
        print(f"Found {args.place} at ({lon}, {lat})")
        bbox = fetcher.bbox_from_center(lon, lat, args.radius)
    
    print(f"Bounding box: {bbox}")
    
    # Fetch OSM data
    osm_data = fetcher.fetch_osm_data(bbox)
    
    # Generate U8 map
    generator = U8MapGenerator(bbox, map_size=(256, 256))
    generator.process_osm_data(osm_data)
    
    # Create output directory
    output_dir = os.path.join(os.getcwd(), args.output)
    os.makedirs(output_dir, exist_ok=True)
    
    # Export
    exporter = U8MapExporter(generator)
    exporter.export_fixed_dat(output_dir, map_number=args.map_number)
    exporter.export_nonfixed_dat(output_dir, map_number=args.map_number)
    exporter.export_geojson(os.path.join(output_dir, "map.geojson"))
    exporter.export_summary(os.path.join(output_dir, "summary.json"), seed=args.seed)
    
    # Print statistics
    print(f"\n=== U8 Generation Statistics ===")
    for key, value in generator.u8_stats.items():
        print(f"  {key}: {value}")
    print(f"\nU8 map generation complete! Output in: {output_dir}")
    print(f"\nTo use with Pentagram:")
    print(f"  1. Copy {output_dir}/fixed/FIXED.DAT to <U8_STATIC>/FIXED.DAT")
    print(f"  2. Copy {output_dir}/nonfixed/NONFIXED.DAT to <GAMEDAT>/NONFIXED.DAT")
    print(f"  (Backup originals first!)")


if __name__ == "__main__":
    main()

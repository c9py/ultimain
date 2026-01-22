#!/usr/bin/env python3
"""
OSM to Ultima Map Converter

Converts OpenStreetMap data to Ultima VII/VIII game maps.
Downloads a bounded region from OSM and generates game-compatible map files.

Usage:
    python osm2ultima.py --bbox "min_lon,min_lat,max_lon,max_lat" --output map_name
    python osm2ultima.py --place "London, UK" --radius 500 --output london_map
"""

import argparse
import hashlib
import json
import math
import os
import random
import struct
import sys
from dataclasses import dataclass, field
from typing import Dict, List, Optional, Tuple, Any
import requests

from osm_shape_mapping import (
    CoordinateTransformer,
    get_terrain_shape,
    get_object_shapes,
    get_npc_for_building,
    get_furniture_for_room,
    get_furniture_shapes,
    get_npc_with_dialogue,
    TERRAIN_SHAPES,
    OBJECT_SHAPES,
    OSM_HIGHWAY_TO_TERRAIN,
    NPC_PROFESSIONS,
)


# =============================================================================
# DATA STRUCTURES
# =============================================================================

@dataclass
class UltimaObject:
    """Represents an object to be placed in the Ultima map."""
    shape: int
    frame: int = 0
    x: int = 0  # tile x
    y: int = 0  # tile y
    lift: int = 0  # height/z level
    quality: int = 0
    flags: int = 0


@dataclass
class NPCProfile:
    """NPC profile for AI system integration."""
    id: str
    name: str
    profession: str
    building_type: str
    location: Tuple[int, int]  # (tile_x, tile_y)
    shape: int

    # Big Five personality traits (0.0 - 1.0)
    openness: float = 0.5
    conscientiousness: float = 0.5
    extraversion: float = 0.5
    agreeableness: float = 0.5
    neuroticism: float = 0.5

    # Dialogue and behavior
    dialogues: List[str] = field(default_factory=list)
    schedule: str = "daytime"
    knowledge_domains: List[str] = field(default_factory=list)

    # Relationships (npc_id -> initial relationship value)
    relationships: Dict[str, float] = field(default_factory=dict)
    
    def to_ireg_bytes(self) -> bytes:
        """Convert to IREG format bytes."""
        # Simplified IREG format (10 bytes)
        chunk_x = self.x // 16
        chunk_y = self.y // 16
        local_x = self.x % 16
        local_y = self.y % 16
        
        buf = bytearray(10)
        buf[0] = 10  # length
        buf[1] = ((chunk_x % 16) << 4) | local_x
        buf[2] = ((chunk_y % 16) << 4) | local_y
        buf[3] = self.shape & 0xff
        buf[4] = ((self.shape >> 8) & 3) | (self.frame << 2)
        buf[5] = (self.lift & 0x0f)  # nibble swap
        buf[6] = self.quality & 0xff
        buf[7] = 0  # temporary flag
        buf[8] = 0  # filler
        buf[9] = 0  # filler
        
        return bytes(buf)


@dataclass
class UltimaChunk:
    """Represents a 16x16 tile chunk."""
    terrain: List[List[int]] = field(default_factory=lambda: [[4] * 16 for _ in range(16)])  # default grass
    objects: List[UltimaObject] = field(default_factory=list)


@dataclass
class UltimaMap:
    """Represents the full Ultima map."""
    width_chunks: int = 16
    height_chunks: int = 16
    chunks: Dict[Tuple[int, int], UltimaChunk] = field(default_factory=dict)
    
    def get_chunk(self, cx: int, cy: int) -> UltimaChunk:
        """Get or create a chunk at the given coordinates."""
        if (cx, cy) not in self.chunks:
            self.chunks[(cx, cy)] = UltimaChunk()
        return self.chunks[(cx, cy)]
    
    def set_terrain(self, tile_x: int, tile_y: int, shape: int):
        """Set terrain at a specific tile."""
        cx, cy = tile_x // 16, tile_y // 16
        lx, ly = tile_x % 16, tile_y % 16
        chunk = self.get_chunk(cx, cy)
        chunk.terrain[ly][lx] = shape
    
    def add_object(self, obj: UltimaObject):
        """Add an object to the appropriate chunk."""
        cx, cy = obj.x // 16, obj.y // 16
        chunk = self.get_chunk(cx, cy)
        chunk.objects.append(obj)


# =============================================================================
# OSM DATA FETCHING
# =============================================================================

class OSMFetcher:
    """Fetches data from OpenStreetMap via Overpass API."""
    
    OVERPASS_URL = "https://overpass.kumi.systems/api/interpreter"
    NOMINATIM_URL = "https://nominatim.openstreetmap.org/search"
    
    def __init__(self):
        self.session = requests.Session()
        self.session.headers.update({
            "User-Agent": "OSM2Ultima/1.0 (https://github.com/c9py/ultimain)"
        })
    
    def geocode_place(self, place_name: str) -> Tuple[float, float]:
        """Get coordinates for a place name."""
        params = {
            "q": place_name,
            "format": "json",
            "limit": 1
        }
        response = self.session.get(self.NOMINATIM_URL, params=params)
        response.raise_for_status()
        results = response.json()
        
        if not results:
            raise ValueError(f"Could not find place: {place_name}")
        
        return float(results[0]["lon"]), float(results[0]["lat"])
    
    def bbox_from_center(self, lon: float, lat: float, radius_m: float) -> Tuple[float, float, float, float]:
        """Calculate bounding box from center point and radius in meters."""
        # Approximate degrees per meter at this latitude
        lat_deg_per_m = 1 / 111320
        lon_deg_per_m = 1 / (111320 * math.cos(math.radians(lat)))
        
        delta_lat = radius_m * lat_deg_per_m
        delta_lon = radius_m * lon_deg_per_m
        
        return (
            lon - delta_lon,  # min_lon
            lat - delta_lat,  # min_lat
            lon + delta_lon,  # max_lon
            lat + delta_lat   # max_lat
        )
    
    def fetch_osm_data(self, bbox: Tuple[float, float, float, float]) -> dict:
        """
        Fetch OSM data for a bounding box.
        Returns GeoJSON-like structure.
        """
        min_lon, min_lat, max_lon, max_lat = bbox
        
        # Overpass query for all relevant features
        query = f"""
        [out:json][timeout:60];
        (
          // Buildings
          way["building"]({min_lat},{min_lon},{max_lat},{max_lon});
          relation["building"]({min_lat},{min_lon},{max_lat},{max_lon});
          
          // Roads and paths
          way["highway"]({min_lat},{min_lon},{max_lat},{max_lon});
          
          // Natural features
          node["natural"]({min_lat},{min_lon},{max_lat},{max_lon});
          way["natural"]({min_lat},{min_lon},{max_lat},{max_lon});
          
          // Landuse
          way["landuse"]({min_lat},{min_lon},{max_lat},{max_lon});
          relation["landuse"]({min_lat},{min_lon},{max_lat},{max_lon});
          
          // Waterways
          way["waterway"]({min_lat},{min_lon},{max_lat},{max_lon});
          
          // Amenities
          node["amenity"]({min_lat},{min_lon},{max_lat},{max_lon});
          way["amenity"]({min_lat},{min_lon},{max_lat},{max_lon});
          
          // Barriers (fences, walls)
          way["barrier"]({min_lat},{min_lon},{max_lat},{max_lon});
          
          // Man-made structures
          node["man_made"]({min_lat},{min_lon},{max_lat},{max_lon});
          way["man_made"]({min_lat},{min_lon},{max_lat},{max_lon});
        );
        out body;
        >;
        out skel qt;
        """
        
        print(f"Fetching OSM data for bbox: {bbox}")
        response = self.session.post(self.OVERPASS_URL, data={"data": query})
        response.raise_for_status()
        
        return response.json()


# =============================================================================
# MAP GENERATOR
# =============================================================================

class MapGenerator:
    """Generates Ultima map from OSM data."""

    # Highway width configuration (in tiles)
    HIGHWAY_WIDTHS = {
        "motorway": 4,
        "trunk": 4,
        "primary": 3,
        "secondary": 3,
        "tertiary": 2,
        "residential": 2,
        "service": 1,
        "track": 1,
        "path": 1,
        "footway": 1,
        "pedestrian": 2,
        "cycleway": 1,
        "bridleway": 1,
        "steps": 1,
    }

    def __init__(self, bbox: Tuple[float, float, float, float], map_size: Tuple[int, int] = (16, 16)):
        """
        Initialize generator.

        bbox: (min_lon, min_lat, max_lon, max_lat)
        map_size: (width_chunks, height_chunks)
        """
        self.bbox = bbox
        self.map_size = map_size
        self.transformer = CoordinateTransformer(bbox, map_size)
        self.ultima_map = UltimaMap(width_chunks=map_size[0], height_chunks=map_size[1])

        # Node cache for resolving way coordinates
        self.nodes: Dict[int, Tuple[float, float]] = {}

        # Track road segments for intersection detection
        self.road_tiles: Dict[Tuple[int, int], str] = {}  # tile -> highway type

        # Track bridges for proper rendering
        self.bridges: List[Dict] = []

        # Track NPCs for AI profile export
        self.npc_profiles: List[NPCProfile] = []
        self.npc_counter = 0

        # Statistics
        self.stats = {
            "buildings_processed": 0,
            "roads_processed": 0,
            "npcs_placed": 0,
            "interiors_generated": 0,
        }
    
    def process_osm_data(self, osm_data: dict):
        """Process OSM data and populate the Ultima map."""
        elements = osm_data.get("elements", [])
        
        # First pass: cache all nodes
        for element in elements:
            if element["type"] == "node":
                self.nodes[element["id"]] = (element["lon"], element["lat"])
        
        # Second pass: process ways and relations
        for element in elements:
            if element["type"] == "way":
                self._process_way(element)
            elif element["type"] == "node" and "tags" in element:
                self._process_node(element)
        
        # Fill in default terrain for empty areas
        self._fill_default_terrain()
        
        print(f"Processed {len(elements)} OSM elements")
        print(f"Generated {len(self.ultima_map.chunks)} chunks")
    
    def _process_node(self, node: dict):
        """Process a single OSM node (point feature)."""
        tags = node.get("tags", {})
        lon, lat = node["lon"], node["lat"]
        tile_x, tile_y = self.transformer.osm_to_ultima(lon, lat)
        
        # Get object shapes for this node
        shapes = get_object_shapes(tags)
        
        if shapes:
            # Place the main object
            main_shapes = shapes.get("main", [])
            if main_shapes:
                shape = random.choice(main_shapes)
                obj = UltimaObject(
                    shape=shape,
                    frame=0,
                    x=tile_x,
                    y=tile_y,
                    lift=0
                )
                self.ultima_map.add_object(obj)
    
    def _process_way(self, way: dict):
        """Process a single OSM way (line or polygon)."""
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
            self._process_highway(tags, coords)
        elif "building" in tags:
            self._process_building(tags, coords)
        elif "landuse" in tags or "natural" in tags:
            self._process_area(tags, coords, is_polygon)
        elif "waterway" in tags:
            self._process_waterway(tags, coords)
        elif "barrier" in tags:
            self._process_barrier(tags, coords)
    
    def _process_highway(self, tags: dict, coords: List[Tuple[float, float]]):
        """Process a road/path with variable width and intersection detection."""
        highway_type = tags.get("highway", "residential")
        terrain_type = OSM_HIGHWAY_TO_TERRAIN.get(highway_type, "cobblestone")
        terrain_shapes = TERRAIN_SHAPES.get(terrain_type, TERRAIN_SHAPES["cobblestone"])

        # Get road width based on highway type
        width = self.HIGHWAY_WIDTHS.get(highway_type, 2)

        # Check if this is a bridge
        is_bridge = tags.get("bridge") == "yes" or tags.get("man_made") == "bridge"

        # Draw the road as a line of terrain tiles
        for i in range(len(coords) - 1):
            if is_bridge:
                self._draw_bridge(coords[i], coords[i + 1], width)
            else:
                self._draw_line_terrain(coords[i], coords[i + 1], terrain_shapes, width=width)

            # Track road tiles for intersection detection
            self._track_road_segment(coords[i], coords[i + 1], highway_type, width)

        self.stats["roads_processed"] += 1

    def _track_road_segment(self, start: Tuple[float, float], end: Tuple[float, float],
                            highway_type: str, width: int):
        """Track road tiles for intersection detection."""
        start_tile = self.transformer.osm_to_ultima(start[0], start[1])
        end_tile = self.transformer.osm_to_ultima(end[0], end[1])

        dx = abs(end_tile[0] - start_tile[0])
        dy = abs(end_tile[1] - start_tile[1])
        dist = max(dx, dy)

        if dist == 0:
            self.road_tiles[(start_tile[0], start_tile[1])] = highway_type
            return

        for i in range(dist + 1):
            t = i / dist
            x = int(start_tile[0] + t * (end_tile[0] - start_tile[0]))
            y = int(start_tile[1] + t * (end_tile[1] - start_tile[1]))

            for wx in range(-width // 2, width // 2 + 1):
                for wy in range(-width // 2, width // 2 + 1):
                    tile_key = (x + wx, y + wy)
                    # Mark as intersection if already a road tile
                    if tile_key in self.road_tiles:
                        self.road_tiles[tile_key] = "intersection"
                    else:
                        self.road_tiles[tile_key] = highway_type

    def _draw_bridge(self, start: Tuple[float, float], end: Tuple[float, float], width: int):
        """Draw a bridge structure."""
        bridge_shapes = OBJECT_SHAPES.get("bridge", [212, 213, 214, 215])
        planking_shapes = TERRAIN_SHAPES.get("planking", [17])

        start_tile = self.transformer.osm_to_ultima(start[0], start[1])
        end_tile = self.transformer.osm_to_ultima(end[0], end[1])

        dx = abs(end_tile[0] - start_tile[0])
        dy = abs(end_tile[1] - start_tile[1])
        dist = max(dx, dy)

        if dist == 0:
            return

        for i in range(dist + 1):
            t = i / dist
            x = int(start_tile[0] + t * (end_tile[0] - start_tile[0]))
            y = int(start_tile[1] + t * (end_tile[1] - start_tile[1]))

            # Draw planking terrain for walkable surface
            for wx in range(-width // 2, width // 2 + 1):
                for wy in range(-width // 2, width // 2 + 1):
                    tx, ty = x + wx, y + wy
                    if 0 <= tx < self.map_size[0] * 16 and 0 <= ty < self.map_size[1] * 16:
                        self.ultima_map.set_terrain(tx, ty, random.choice(planking_shapes))

            # Place bridge structure objects at edges
            if i == 0 or i == dist:
                obj = UltimaObject(
                    shape=random.choice(bridge_shapes),
                    x=x, y=y, lift=0
                )
                self.ultima_map.add_object(obj)
    
    def _process_building(self, tags: dict, coords: List[Tuple[float, float]]):
        """Process a building polygon with interior generation."""
        if len(coords) < 3:
            return

        # Get building center
        center_lon = sum(c[0] for c in coords) / len(coords)
        center_lat = sum(c[1] for c in coords) / len(coords)
        center_tile = self.transformer.osm_to_ultima(center_lon, center_lat)

        # Calculate approximate building size in tiles
        min_x = min(self.transformer.osm_to_ultima(c[0], c[1])[0] for c in coords)
        max_x = max(self.transformer.osm_to_ultima(c[0], c[1])[0] for c in coords)
        min_y = min(self.transformer.osm_to_ultima(c[0], c[1])[1] for c in coords)
        max_y = max(self.transformer.osm_to_ultima(c[0], c[1])[1] for c in coords)

        width = max(1, max_x - min_x)
        height = max(1, max_y - min_y)

        # Get building components
        building_type = tags.get("building", "house")
        shapes = get_object_shapes(tags)

        # Place floor
        floor_shapes = TERRAIN_SHAPES.get("floor", [189])
        for x in range(min_x, max_x + 1):
            for y in range(min_y, max_y + 1):
                self.ultima_map.set_terrain(x, y, random.choice(floor_shapes))

        # Place walls around perimeter
        wall_shapes = shapes.get("walls", OBJECT_SHAPES.get("wall", [151]))
        for x in range(min_x, max_x + 1):
            # Top wall
            obj = UltimaObject(shape=random.choice(wall_shapes), x=x, y=min_y, lift=0)
            self.ultima_map.add_object(obj)
            # Bottom wall
            obj = UltimaObject(shape=random.choice(wall_shapes), x=x, y=max_y, lift=0)
            self.ultima_map.add_object(obj)

        for y in range(min_y + 1, max_y):
            # Left wall
            obj = UltimaObject(shape=random.choice(wall_shapes), x=min_x, y=y, lift=0)
            self.ultima_map.add_object(obj)
            # Right wall
            obj = UltimaObject(shape=random.choice(wall_shapes), x=max_x, y=y, lift=0)
            self.ultima_map.add_object(obj)

        # Place door
        door_shapes = shapes.get("door", OBJECT_SHAPES.get("door", [270]))
        door_x = (min_x + max_x) // 2
        obj = UltimaObject(shape=random.choice(door_shapes), x=door_x, y=max_y, lift=0)
        self.ultima_map.add_object(obj)

        # Place roof (on lift level 4)
        roof_shapes = shapes.get("roof", OBJECT_SHAPES.get("roof_slate", [164]))
        for x in range(min_x, max_x + 1):
            for y in range(min_y, max_y + 1):
                obj = UltimaObject(shape=random.choice(roof_shapes), x=x, y=y, lift=4)
                self.ultima_map.add_object(obj)

        # Generate interior for buildings with sufficient size
        if width >= 4 and height >= 4:
            self._generate_interior(building_type, min_x + 1, min_y + 1, max_x - 1, max_y - 1)
            self.stats["interiors_generated"] += 1

        # Add NPCs with location-appropriate types
        self._place_building_npcs(building_type, min_x + 1, min_y + 1, max_x - 1, max_y - 1)

        self.stats["buildings_processed"] += 1

    def _generate_interior(self, building_type: str, min_x: int, min_y: int, max_x: int, max_y: int):
        """Generate interior rooms with furniture for a building."""
        width = max_x - min_x
        height = max_y - min_y

        if width < 2 or height < 2:
            return

        # Determine room layout based on building size
        rooms = self._plan_rooms(building_type, min_x, min_y, max_x, max_y)

        # Place furniture in each room
        for room in rooms:
            self._furnish_room(building_type, room)

    def _plan_rooms(self, building_type: str, min_x: int, min_y: int,
                    max_x: int, max_y: int) -> List[Dict]:
        """Plan room layout for a building."""
        rooms = []
        width = max_x - min_x
        height = max_y - min_y

        # Single room for small buildings
        if width < 6 or height < 6:
            rooms.append({
                "type": "main",
                "min_x": min_x, "min_y": min_y,
                "max_x": max_x, "max_y": max_y
            })
            return rooms

        # Multi-room layout for larger buildings
        if building_type in ["house", "residential"]:
            # Split into living area and bedroom
            mid_y = min_y + height // 2
            rooms.append({
                "type": "living",
                "min_x": min_x, "min_y": min_y,
                "max_x": max_x, "max_y": mid_y
            })
            rooms.append({
                "type": "bedroom",
                "min_x": min_x, "min_y": mid_y + 1,
                "max_x": max_x, "max_y": max_y
            })

            # Add interior wall
            wall_shapes = OBJECT_SHAPES.get("wall", [151])
            for x in range(min_x, max_x + 1):
                if x != (min_x + max_x) // 2:  # Leave doorway
                    obj = UltimaObject(shape=random.choice(wall_shapes), x=x, y=mid_y, lift=0)
                    self.ultima_map.add_object(obj)

        elif building_type in ["shop", "retail", "commercial"]:
            # Split into showroom and storage
            mid_y = min_y + (height * 2) // 3
            rooms.append({
                "type": "showroom",
                "min_x": min_x, "min_y": min_y,
                "max_x": max_x, "max_y": mid_y
            })
            rooms.append({
                "type": "storage",
                "min_x": min_x, "min_y": mid_y + 1,
                "max_x": max_x, "max_y": max_y
            })

        elif building_type in ["tavern"]:
            # Main hall, kitchen, bedroom
            mid_x = min_x + (width * 2) // 3
            mid_y = min_y + height // 2
            rooms.append({
                "type": "main",
                "min_x": min_x, "min_y": min_y,
                "max_x": mid_x, "max_y": max_y
            })
            rooms.append({
                "type": "kitchen",
                "min_x": mid_x + 1, "min_y": min_y,
                "max_x": max_x, "max_y": mid_y
            })
            rooms.append({
                "type": "bedroom",
                "min_x": mid_x + 1, "min_y": mid_y + 1,
                "max_x": max_x, "max_y": max_y
            })

        elif building_type in ["castle", "government"]:
            # Throne/hall, office areas
            mid_x = min_x + width // 2
            rooms.append({
                "type": "throne" if building_type == "castle" else "hall",
                "min_x": min_x, "min_y": min_y,
                "max_x": mid_x, "max_y": max_y
            })
            rooms.append({
                "type": "hall" if building_type == "castle" else "office",
                "min_x": mid_x + 1, "min_y": min_y,
                "max_x": max_x, "max_y": max_y
            })

        else:
            # Default: single room
            rooms.append({
                "type": "main",
                "min_x": min_x, "min_y": min_y,
                "max_x": max_x, "max_y": max_y
            })

        return rooms

    def _furnish_room(self, building_type: str, room: Dict):
        """Place furniture in a room."""
        furniture_list = get_furniture_for_room(building_type, room["type"])

        if not furniture_list:
            return

        room_width = room["max_x"] - room["min_x"]
        room_height = room["max_y"] - room["min_y"]

        # Place furniture items
        placed_positions = set()

        for furniture_name in furniture_list:
            furniture_shapes = get_furniture_shapes(furniture_name)
            if not furniture_shapes:
                continue

            # Find a position for this furniture
            attempts = 0
            while attempts < 10:
                x = random.randint(room["min_x"], room["max_x"])
                y = random.randint(room["min_y"], room["max_y"])

                if (x, y) not in placed_positions:
                    obj = UltimaObject(
                        shape=random.choice(furniture_shapes),
                        x=x, y=y, lift=0
                    )
                    self.ultima_map.add_object(obj)
                    placed_positions.add((x, y))
                    break

                attempts += 1

    def _place_building_npcs(self, building_type: str, min_x: int, min_y: int,
                             max_x: int, max_y: int):
        """Place NPCs appropriate to building type with dialogue data."""
        # Determine NPC types for this building
        npc_mapping = {
            "house": ["townsman", "townswoman"],
            "residential": ["townsman", "townswoman", "child"],
            "shop": ["shopkeeper"],
            "retail": ["shopkeeper"],
            "commercial": ["shopkeeper", "townsman"],
            "church": ["sage"],
            "castle": ["guard", "noble_male", "noble_female"],
            "fort": ["guard", "fighter"],
            "government": ["guard", "noble_male"],
            "hospital": ["sage"],
            "school": ["sage"],
            "tavern": ["entertainer", "townsman", "townswoman"],
            "farm": ["farmer"],
            "barn": ["farmer"],
            "industrial": ["blacksmith", "townsman"],
            "warehouse": ["townsman"],
        }

        npc_types = npc_mapping.get(building_type, ["townsman"])

        # Calculate building size for NPC count
        width = max_x - min_x
        height = max_y - min_y
        area = width * height

        # Determine number of NPCs (larger buildings get more)
        if area < 16:
            max_npcs = 1
            npc_chance = 0.4
        elif area < 36:
            max_npcs = 2
            npc_chance = 0.5
        else:
            max_npcs = 3
            npc_chance = 0.6

        if random.random() > npc_chance:
            return

        num_npcs = random.randint(1, max_npcs)
        placed_positions = set()
        building_npcs = []  # Track NPCs in this building for relationships

        for _ in range(num_npcs):
            npc_type = random.choice(npc_types)
            npc_data = get_npc_with_dialogue(npc_type)

            if not npc_data or not npc_data.get("shapes"):
                continue

            # Find position for NPC
            attempts = 0
            while attempts < 10:
                x = random.randint(min_x, max_x)
                y = random.randint(min_y, max_y)

                if (x, y) not in placed_positions:
                    shape = random.choice(npc_data["shapes"])
                    npc = UltimaObject(
                        shape=shape,
                        x=x, y=y, lift=0,
                        quality=hash(npc_type) % 256  # Store NPC type info
                    )
                    self.ultima_map.add_object(npc)
                    placed_positions.add((x, y))
                    self.stats["npcs_placed"] += 1

                    # Create NPC profile for AI integration
                    profile = self._create_npc_profile(
                        npc_type, building_type, x, y, shape, npc_data
                    )
                    self.npc_profiles.append(profile)
                    building_npcs.append(profile.id)
                    break

                attempts += 1

        # Create relationships between NPCs in the same building
        self._create_building_relationships(building_npcs)

    def _create_npc_profile(self, npc_type: str, building_type: str,
                            x: int, y: int, shape: int, npc_data: Dict) -> NPCProfile:
        """Create an NPC profile with personality traits based on profession."""
        self.npc_counter += 1
        npc_id = f"npc_{self.npc_counter:04d}"

        # Generate name (simple procedural names)
        first_names_male = ["Gareth", "Aldric", "Cedric", "Edmund", "Roland",
                           "Geoffrey", "Wilfred", "Oswald", "Reginald", "Bernard"]
        first_names_female = ["Elara", "Beatrice", "Gwyneth", "Isolde", "Rowena",
                             "Millicent", "Cordelia", "Elowen", "Rosalind", "Lysandra"]

        is_female = npc_type in ["townswoman", "noble_female"]
        names = first_names_female if is_female else first_names_male
        name = random.choice(names)

        # Personality traits based on profession
        personality_profiles = {
            "townsman": {"o": 0.4, "c": 0.5, "e": 0.5, "a": 0.6, "n": 0.4},
            "townswoman": {"o": 0.5, "c": 0.6, "e": 0.5, "a": 0.7, "n": 0.4},
            "child": {"o": 0.8, "c": 0.3, "e": 0.8, "a": 0.6, "n": 0.5},
            "guard": {"o": 0.3, "c": 0.8, "e": 0.4, "a": 0.4, "n": 0.3},
            "shopkeeper": {"o": 0.5, "c": 0.7, "e": 0.7, "a": 0.6, "n": 0.3},
            "blacksmith": {"o": 0.4, "c": 0.8, "e": 0.4, "a": 0.5, "n": 0.3},
            "farmer": {"o": 0.3, "c": 0.7, "e": 0.4, "a": 0.6, "n": 0.4},
            "mage": {"o": 0.9, "c": 0.6, "e": 0.3, "a": 0.4, "n": 0.5},
            "sage": {"o": 0.8, "c": 0.7, "e": 0.3, "a": 0.6, "n": 0.3},
            "noble_male": {"o": 0.5, "c": 0.6, "e": 0.6, "a": 0.3, "n": 0.4},
            "noble_female": {"o": 0.6, "c": 0.6, "e": 0.5, "a": 0.4, "n": 0.5},
            "entertainer": {"o": 0.8, "c": 0.4, "e": 0.9, "a": 0.7, "n": 0.5},
            "beggar": {"o": 0.4, "c": 0.3, "e": 0.4, "a": 0.5, "n": 0.7},
            "fighter": {"o": 0.4, "c": 0.6, "e": 0.5, "a": 0.3, "n": 0.4},
            "ranger": {"o": 0.6, "c": 0.6, "e": 0.3, "a": 0.5, "n": 0.3},
        }

        base_traits = personality_profiles.get(npc_type, {"o": 0.5, "c": 0.5, "e": 0.5, "a": 0.5, "n": 0.5})

        # Add some random variation (+/- 0.15)
        def vary(base: float) -> float:
            return max(0.0, min(1.0, base + random.uniform(-0.15, 0.15)))

        # Knowledge domains based on profession
        knowledge_mapping = {
            "townsman": ["local_gossip", "town_history"],
            "townswoman": ["local_gossip", "town_history", "cooking"],
            "child": ["games", "local_secrets"],
            "guard": ["combat", "law", "town_security"],
            "shopkeeper": ["commerce", "goods", "local_economy"],
            "blacksmith": ["smithing", "weapons", "armor", "metallurgy"],
            "farmer": ["agriculture", "weather", "animals"],
            "mage": ["magic", "arcana", "history", "alchemy"],
            "sage": ["history", "lore", "religion", "medicine"],
            "noble_male": ["politics", "heraldry", "etiquette"],
            "noble_female": ["politics", "heraldry", "etiquette", "fashion"],
            "entertainer": ["music", "stories", "gossip", "performance"],
            "fighter": ["combat", "weapons", "tactics"],
            "ranger": ["nature", "tracking", "survival", "beasts"],
        }

        return NPCProfile(
            id=npc_id,
            name=name,
            profession=npc_type,
            building_type=building_type,
            location=(x, y),
            shape=shape,
            openness=vary(base_traits["o"]),
            conscientiousness=vary(base_traits["c"]),
            extraversion=vary(base_traits["e"]),
            agreeableness=vary(base_traits["a"]),
            neuroticism=vary(base_traits["n"]),
            dialogues=npc_data.get("dialogues", []),
            schedule=npc_data.get("schedule", "daytime"),
            knowledge_domains=knowledge_mapping.get(npc_type, ["general"]),
        )

    def _create_building_relationships(self, npc_ids: List[str]):
        """Create positive relationships between NPCs in the same building."""
        if len(npc_ids) < 2:
            return

        for profile in self.npc_profiles:
            if profile.id in npc_ids:
                for other_id in npc_ids:
                    if other_id != profile.id:
                        # Positive relationship for building-mates (0.3-0.7)
                        profile.relationships[other_id] = random.uniform(0.3, 0.7)
    
    def _process_area(self, tags: dict, coords: List[Tuple[float, float]], is_polygon: bool):
        """Process a landuse or natural area."""
        terrain_shapes = get_terrain_shape(tags)
        
        if is_polygon and len(coords) >= 3:
            # Fill polygon with terrain
            self._fill_polygon_terrain(coords, terrain_shapes)
        else:
            # Draw as line
            for i in range(len(coords) - 1):
                self._draw_line_terrain(coords[i], coords[i + 1], terrain_shapes, width=1)
        
        # Add natural objects (trees, rocks, etc.)
        shapes = get_object_shapes(tags)
        if shapes and "main" in shapes:
            # Scatter objects in the area
            for coord in coords[::3]:  # Every 3rd coordinate
                tile = self.transformer.osm_to_ultima(coord[0], coord[1])
                obj = UltimaObject(
                    shape=random.choice(shapes["main"]),
                    x=tile[0],
                    y=tile[1],
                    lift=0
                )
                self.ultima_map.add_object(obj)
    
    def _process_waterway(self, tags: dict, coords: List[Tuple[float, float]]):
        """Process a waterway (river, stream, etc.)."""
        water_shapes = TERRAIN_SHAPES.get("water", [8])
        
        # Draw water as a line
        for i in range(len(coords) - 1):
            self._draw_line_terrain(coords[i], coords[i + 1], water_shapes, width=3)
    
    def _process_barrier(self, tags: dict, coords: List[Tuple[float, float]]):
        """Process a barrier (fence, wall, etc.)."""
        shapes = get_object_shapes(tags)
        
        if shapes and "main" in shapes:
            barrier_shapes = shapes["main"]
            
            # Place barrier objects along the line
            for i in range(len(coords) - 1):
                start = self.transformer.osm_to_ultima(coords[i][0], coords[i][1])
                end = self.transformer.osm_to_ultima(coords[i + 1][0], coords[i + 1][1])
                
                # Interpolate points
                dist = max(abs(end[0] - start[0]), abs(end[1] - start[1]))
                if dist == 0:
                    continue
                
                for j in range(dist + 1):
                    t = j / dist
                    x = int(start[0] + t * (end[0] - start[0]))
                    y = int(start[1] + t * (end[1] - start[1]))
                    
                    obj = UltimaObject(
                        shape=random.choice(barrier_shapes),
                        x=x,
                        y=y,
                        lift=0
                    )
                    self.ultima_map.add_object(obj)
    
    def _draw_line_terrain(self, start: Tuple[float, float], end: Tuple[float, float], 
                           shapes: List[int], width: int = 1):
        """Draw a line of terrain tiles."""
        start_tile = self.transformer.osm_to_ultima(start[0], start[1])
        end_tile = self.transformer.osm_to_ultima(end[0], end[1])
        
        # Bresenham-like line drawing
        dx = abs(end_tile[0] - start_tile[0])
        dy = abs(end_tile[1] - start_tile[1])
        dist = max(dx, dy)
        
        if dist == 0:
            self.ultima_map.set_terrain(start_tile[0], start_tile[1], random.choice(shapes))
            return
        
        for i in range(dist + 1):
            t = i / dist
            x = int(start_tile[0] + t * (end_tile[0] - start_tile[0]))
            y = int(start_tile[1] + t * (end_tile[1] - start_tile[1]))
            
            # Draw with width
            for wx in range(-width // 2, width // 2 + 1):
                for wy in range(-width // 2, width // 2 + 1):
                    tx, ty = x + wx, y + wy
                    if 0 <= tx < self.map_size[0] * 16 and 0 <= ty < self.map_size[1] * 16:
                        self.ultima_map.set_terrain(tx, ty, random.choice(shapes))
    
    def _fill_polygon_terrain(self, coords: List[Tuple[float, float]], shapes: List[int]):
        """Fill a polygon with terrain tiles using scanline algorithm."""
        # Convert to tile coordinates
        tile_coords = [self.transformer.osm_to_ultima(c[0], c[1]) for c in coords]
        
        if not tile_coords:
            return
        
        # Get bounding box
        min_x = min(c[0] for c in tile_coords)
        max_x = max(c[0] for c in tile_coords)
        min_y = min(c[1] for c in tile_coords)
        max_y = max(c[1] for c in tile_coords)
        
        # Simple scanline fill
        for y in range(min_y, max_y + 1):
            for x in range(min_x, max_x + 1):
                if self._point_in_polygon(x, y, tile_coords):
                    self.ultima_map.set_terrain(x, y, random.choice(shapes))
    
    def _point_in_polygon(self, x: int, y: int, polygon: List[Tuple[int, int]]) -> bool:
        """Check if a point is inside a polygon using ray casting."""
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
    
    def _fill_default_terrain(self):
        """Fill empty areas with default grass terrain."""
        grass_shapes = TERRAIN_SHAPES["grass"]
        
        for cy in range(self.map_size[1]):
            for cx in range(self.map_size[0]):
                chunk = self.ultima_map.get_chunk(cx, cy)
                for ly in range(16):
                    for lx in range(16):
                        if chunk.terrain[ly][lx] == 4:  # default grass
                            chunk.terrain[ly][lx] = random.choice(grass_shapes)


# =============================================================================
# MAP EXPORTER
# =============================================================================

class MapExporter:
    """Exports Ultima map to various formats."""

    def __init__(self, ultima_map: UltimaMap, npc_profiles: List[NPCProfile] = None):
        self.ultima_map = ultima_map
        self.npc_profiles = npc_profiles or []
    
    def export_geojson(self, output_path: str):
        """Export map as GeoJSON for visualization."""
        features = []
        
        # Export terrain as points
        for (cx, cy), chunk in self.ultima_map.chunks.items():
            for ly in range(16):
                for lx in range(16):
                    tile_x = cx * 16 + lx
                    tile_y = cy * 16 + ly
                    shape = chunk.terrain[ly][lx]
                    
                    feature = {
                        "type": "Feature",
                        "geometry": {
                            "type": "Point",
                            "coordinates": [tile_x, tile_y]
                        },
                        "properties": {
                            "type": "terrain",
                            "shape": shape,
                            "chunk": [cx, cy],
                            "local": [lx, ly]
                        }
                    }
                    features.append(feature)
            
            # Export objects
            for obj in chunk.objects:
                feature = {
                    "type": "Feature",
                    "geometry": {
                        "type": "Point",
                        "coordinates": [obj.x, obj.y, obj.lift]
                    },
                    "properties": {
                        "type": "object",
                        "shape": obj.shape,
                        "frame": obj.frame,
                        "quality": obj.quality,
                        "lift": obj.lift
                    }
                }
                features.append(feature)
        
        geojson = {
            "type": "FeatureCollection",
            "features": features,
            "ultima_metadata": {
                "map_size_chunks": [self.ultima_map.width_chunks, self.ultima_map.height_chunks],
                "map_size_tiles": [self.ultima_map.width_chunks * 16, self.ultima_map.height_chunks * 16],
                "total_chunks": len(self.ultima_map.chunks),
                "total_objects": sum(len(c.objects) for c in self.ultima_map.chunks.values())
            }
        }
        
        with open(output_path, "w") as f:
            json.dump(geojson, f, indent=2)
        
        print(f"Exported GeoJSON to {output_path}")
    
    def export_ireg(self, output_dir: str):
        """Export map objects in IREG format (Ultima VII format)."""
        os.makedirs(output_dir, exist_ok=True)
        
        # Group objects by superchunk (16x16 chunks)
        for (cx, cy), chunk in self.ultima_map.chunks.items():
            if not chunk.objects:
                continue
            
            # Calculate superchunk
            scx = cx // 16
            scy = cy // 16
            sc_index = scy * 12 + scx
            
            filename = os.path.join(output_dir, f"u7ireg{sc_index:02x}")
            
            with open(filename, "ab") as f:  # append mode
                for obj in chunk.objects:
                    f.write(obj.to_ireg_bytes())
        
        print(f"Exported IREG files to {output_dir}")
    
    def export_terrain_map(self, output_path: str):
        """Export terrain as a simple text map for visualization."""
        terrain_chars = {
            "grass": ".",
            "water": "~",
            "cobblestone": "#",
            "dirt": ",",
            "sand": ":",
            "swamp": "%",
            "floor": "_",
        }
        
        # Reverse lookup: shape -> char
        shape_to_char = {}
        for terrain_type, shapes in TERRAIN_SHAPES.items():
            char = terrain_chars.get(terrain_type, "?")
            for shape in shapes:
                shape_to_char[shape] = char
        
        lines = []
        for cy in range(self.ultima_map.height_chunks):
            for ly in range(16):
                line = ""
                for cx in range(self.ultima_map.width_chunks):
                    chunk = self.ultima_map.get_chunk(cx, cy)
                    for lx in range(16):
                        shape = chunk.terrain[ly][lx]
                        line += shape_to_char.get(shape, "?")
                lines.append(line)
        
        with open(output_path, "w") as f:
            f.write("\n".join(lines))
        
        print(f"Exported terrain map to {output_path}")
    
    def export_summary(self, output_path: str, generator_stats: Dict = None, seed: str = None):
        """Export a summary of the generated map."""
        total_objects = sum(len(c.objects) for c in self.ultima_map.chunks.values())

        # Count shapes
        shape_counts = {}
        for chunk in self.ultima_map.chunks.values():
            for obj in chunk.objects:
                shape_counts[obj.shape] = shape_counts.get(obj.shape, 0) + 1

        summary = {
            "map_size": {
                "chunks": [self.ultima_map.width_chunks, self.ultima_map.height_chunks],
                "tiles": [self.ultima_map.width_chunks * 16, self.ultima_map.height_chunks * 16]
            },
            "statistics": {
                "total_chunks": len(self.ultima_map.chunks),
                "total_objects": total_objects,
                "unique_shapes": len(shape_counts)
            },
            "shape_counts": dict(sorted(shape_counts.items(), key=lambda x: -x[1])[:20])
        }

        # Add generator statistics if provided
        if generator_stats:
            summary["generation_stats"] = generator_stats

        # Add seed for reproducibility
        if seed:
            summary["seed"] = seed

        with open(output_path, "w") as f:
            json.dump(summary, f, indent=2)

        print(f"Exported summary to {output_path}")

    def export_npc_profiles(self, output_path: str):
        """Export NPC profiles for AI system integration.

        Generates JSON compatible with the Ultima NPC AI system's NPCProfile format.
        """
        if not self.npc_profiles:
            print("No NPCs to export")
            return

        profiles_data = {
            "version": "1.0",
            "generator": "osm2ultima",
            "npc_count": len(self.npc_profiles),
            "npcs": []
        }

        for profile in self.npc_profiles:
            npc_data = {
                "id": profile.id,
                "name": profile.name,
                "profession": profile.profession,
                "building_type": profile.building_type,
                "location": {
                    "tile_x": profile.location[0],
                    "tile_y": profile.location[1]
                },
                "shape": profile.shape,
                "personality": {
                    "openness": round(profile.openness, 3),
                    "conscientiousness": round(profile.conscientiousness, 3),
                    "extraversion": round(profile.extraversion, 3),
                    "agreeableness": round(profile.agreeableness, 3),
                    "neuroticism": round(profile.neuroticism, 3)
                },
                "dialogues": profile.dialogues,
                "schedule": profile.schedule,
                "knowledge_domains": profile.knowledge_domains,
                "relationships": {
                    k: round(v, 3) for k, v in profile.relationships.items()
                }
            }
            profiles_data["npcs"].append(npc_data)

        # Generate relationship graph summary
        total_relationships = sum(len(p.relationships) for p in self.npc_profiles)
        profiles_data["relationship_count"] = total_relationships

        with open(output_path, "w") as f:
            json.dump(profiles_data, f, indent=2)

        print(f"Exported {len(self.npc_profiles)} NPC profiles to {output_path}")


# =============================================================================
# MAIN
# =============================================================================

def main():
    parser = argparse.ArgumentParser(description="Convert OpenStreetMap data to Ultima VII maps")
    
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--bbox", type=str, help="Bounding box: min_lon,min_lat,max_lon,max_lat")
    group.add_argument("--place", type=str, help="Place name to geocode")
    
    parser.add_argument("--radius", type=float, default=500, help="Radius in meters (for --place)")
    parser.add_argument("--output", type=str, required=True, help="Output directory name")
    parser.add_argument("--size", type=str, default="16,16", help="Map size in chunks (width,height)")
    parser.add_argument("--format", type=str, default="all",
                        choices=["all", "geojson", "ireg", "text"],
                        help="Output format")
    parser.add_argument("--seed", type=str, default=None,
                        help="Random seed for reproducible generation (string or integer)")

    args = parser.parse_args()

    # Initialize random seed for reproducible generation
    if args.seed is not None:
        # Convert string seed to integer hash if needed
        try:
            seed_value = int(args.seed)
        except ValueError:
            seed_value = int(hashlib.md5(args.seed.encode()).hexdigest(), 16) % (2**32)
        random.seed(seed_value)
        print(f"Using random seed: {args.seed} (value: {seed_value})")
    
    # Parse map size
    map_size = tuple(map(int, args.size.split(",")))
    
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
    
    # Generate map
    generator = MapGenerator(bbox, map_size)
    generator.process_osm_data(osm_data)
    
    # Create output directory
    output_dir = os.path.join(os.getcwd(), args.output)
    os.makedirs(output_dir, exist_ok=True)
    
    # Export
    exporter = MapExporter(generator.ultima_map, generator.npc_profiles)

    if args.format in ["all", "geojson"]:
        exporter.export_geojson(os.path.join(output_dir, "map.geojson"))

    if args.format in ["all", "ireg"]:
        exporter.export_ireg(os.path.join(output_dir, "ireg"))

    if args.format in ["all", "text"]:
        exporter.export_terrain_map(os.path.join(output_dir, "terrain.txt"))

    # Export NPC profiles for AI system
    exporter.export_npc_profiles(os.path.join(output_dir, "npc_profiles.json"))

    exporter.export_summary(
        os.path.join(output_dir, "summary.json"),
        generator_stats=generator.stats,
        seed=args.seed
    )

    # Print generation statistics
    print(f"\n=== Generation Statistics ===")
    print(f"Buildings processed: {generator.stats['buildings_processed']}")
    print(f"Roads processed: {generator.stats['roads_processed']}")
    print(f"Interiors generated: {generator.stats['interiors_generated']}")
    print(f"NPCs placed: {generator.stats['npcs_placed']}")
    print(f"\nMap generation complete! Output in: {output_dir}")


if __name__ == "__main__":
    main()

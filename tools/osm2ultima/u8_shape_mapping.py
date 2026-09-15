#!/usr/bin/env python3
"""
OSM to Ultima VIII Shape Mapping Dictionary

This module provides mappings between OpenStreetMap feature tags
and Ultima VIII shape numbers for procedural map generation.

IMPORTANT: U8 shapes are DIFFERENT from U7 shapes. This mapping is based on
observed shape usage in the Pentagram engine source code and U8 game data.
Many mappings are approximations or placeholders that should be verified
against actual U8 game data.

Shape number sources:
- engines/ultima8/world/Map.cpp (ground tiles: 301, 497, 409)
- engines/ultima8/games/U8Game.cpp (items, weapons, containers)
- Pentagram source code analysis

TODO: These shape mappings are preliminary. For accurate results:
1. Examine the actual U8 shapes.flx file
2. Cross-reference with U8 game documentation
3. Test generated maps in Pentagram
"""

from dataclasses import dataclass
from typing import Dict, List, Optional, Tuple


# =============================================================================
# U8 TERRAIN / GROUND SHAPES
# =============================================================================
# U8 terrain is handled differently from U7 - it uses the glob system for
# pre-defined map sections. Individual ground tiles are shape 301 family.

U8_TERRAIN_SHAPES = {
    # Ground/floor tiles (shape 301 is common ground tile in U8)
    "ground": [301],
    "stone_floor": [301],
    "dirt": [301],
    
    # Water/liquid shapes
    "water": [347],  # Water tile (referenced in Map.cpp map 49 fix)
    "lava": [347],  # Placeholder - needs verification
    
    # Wall/structure base
    "wall_base": [497],  # Referenced in Map.cpp map 62
    
    # Path/road shapes  
    "cobblestone": [409],  # Referenced in Map.cpp map 62
    "path": [409],
}


# =============================================================================
# U8 OBJECT SHAPES
# =============================================================================
# These are derived from the Pentagram source code U8Game.cpp cheat items
# and other references. Many need verification against actual game data.

U8_OBJECT_SHAPES = {
    # Containers
    "backpack": [529],
    "bag": [637],
    "barrel": [115],
    "chest": [78, 117],
    
    # Currency
    "money": [143],  # Frame 7 = gold coins
    
    # Weapons
    "sword": [420],
    "hammer": [815],
    "axe": [815],  # Placeholder - use hammer shape
    
    # Shields & Armor
    "shield": [539, 828],
    "armour": [64],
    "armor": [64],
    
    # Magic items
    "recall_item": [833],
    "focus": [396],  # Various frames for different foci
    "reagent": [395],  # Various frames for different reagents
    "flask": [579],
    "potion": [579],
    "disk": [750],  # Magic disk
    
    # Special items
    "skull": [814],
    "key": [641],  # Placeholder - needs verification
    
    # Decorative/environmental
    "rock": [301],  # Use ground shape
    "tree": [301],  # Placeholder - U8 trees need research
    "plant": [301],  # Placeholder
    
    # Effects
    "fireball": [260, 261],
}


# =============================================================================
# U8 NPC SHAPES
# =============================================================================
# NPC shapes in U8 are different from U7. These are placeholders.
# TODO: Research actual U8 NPC shape numbers

U8_NPC_SHAPES = {
    "avatar": [1],  # Main character shape (NPC 1)
    "townsman": [400],  # Placeholder
    "townswoman": [401],  # Placeholder
    "guard": [402],  # Placeholder
    "merchant": [403],  # Placeholder
    "mage": [404],  # Placeholder
    "noble": [405],  # Placeholder
    "child": [406],  # Placeholder
}


# =============================================================================
# OSM TAG TO U8 SHAPE MAPPING
# =============================================================================

OSM_BUILDING_TO_U8_SHAPES = {
    # Residential
    "house": {
        "floor": U8_TERRAIN_SHAPES["ground"],
        "walls": [497],
        "door": U8_OBJECT_SHAPES["chest"],  # Placeholder
    },
    "residential": {
        "floor": U8_TERRAIN_SHAPES["ground"],
        "walls": [497],
        "door": U8_OBJECT_SHAPES["chest"],
    },
    
    # Commercial
    "commercial": {
        "floor": U8_TERRAIN_SHAPES["cobblestone"],
        "walls": [497],
        "door": U8_OBJECT_SHAPES["chest"],
    },
    "shop": {
        "floor": U8_TERRAIN_SHAPES["cobblestone"],
        "walls": [497],
        "door": U8_OBJECT_SHAPES["chest"],
    },
    
    # Industrial
    "industrial": {
        "floor": U8_TERRAIN_SHAPES["stone_floor"],
        "walls": [497],
    },
    "warehouse": {
        "floor": U8_TERRAIN_SHAPES["stone_floor"],
        "walls": [497],
    },
}

OSM_HIGHWAY_TO_U8_TERRAIN = {
    "motorway": "cobblestone",
    "trunk": "cobblestone",
    "primary": "cobblestone",
    "secondary": "cobblestone",
    "tertiary": "cobblestone",
    "residential": "cobblestone",
    "service": "path",
    "track": "dirt",
    "path": "path",
    "footway": "path",
    "pedestrian": "cobblestone",
    "steps": "cobblestone",
}

OSM_NATURAL_TO_U8_TERRAIN = {
    "water": "water",
    "wetland": "water",
    "beach": "ground",
    "sand": "ground",
    "grassland": "ground",
    "wood": "ground",
    "forest": "ground",
    "scrub": "ground",
}

OSM_LANDUSE_TO_U8_TERRAIN = {
    "forest": "ground",
    "grass": "ground",
    "meadow": "ground",
    "farmland": "ground",
    "residential": "ground",
    "commercial": "cobblestone",
    "industrial": "cobblestone",
}


# =============================================================================
# U7 TO U8 SHAPE CONVERSION TABLE
# =============================================================================
# Since OSM2Ultima was built for U7, this table provides approximate mappings
# from U7 shapes to U8 shapes. This is necessarily lossy since the games have
# different art assets.

U7_TO_U8_SHAPE_MAP: Dict[int, int] = {
    # Ground/terrain (U7 grass shapes -> U8 ground)
    4: 301,   # grass -> ground
    31: 301,  # grass variant
    32: 301,
    33: 301,
    34: 301,
    35: 301,
    
    # Water
    8: 347,   # U7 water -> U8 water
    
    # Cobblestone/paths
    24: 409,  # U7 cobblestone -> U8 path
    
    # Stone floor
    21: 301,  # U7 stone floor -> U8 ground
    
    # Walls
    151: 497,  # U7 wall -> U8 wall
    152: 497,
    
    # Containers
    76: 78,    # U7 chest -> U8 chest
    
    # Furniture - most map to generic container/ground
    333: 301,  # table -> ground (placeholder)
    292: 301,  # seat
    312: 301,  # bed
}

# Default shape to use when no mapping exists
U8_DEFAULT_SHAPE = 301


def get_u8_terrain_shape(osm_tags: dict) -> List[int]:
    """
    Get appropriate U8 terrain shape numbers from OSM tags.
    
    Args:
        osm_tags: Dictionary of OSM tags
        
    Returns:
        List of U8 shape numbers
    """
    # Check landuse
    landuse = osm_tags.get("landuse")
    if landuse and landuse in OSM_LANDUSE_TO_U8_TERRAIN:
        terrain_type = OSM_LANDUSE_TO_U8_TERRAIN[landuse]
        return U8_TERRAIN_SHAPES.get(terrain_type, U8_TERRAIN_SHAPES["ground"])
    
    # Check natural
    natural = osm_tags.get("natural")
    if natural and natural in OSM_NATURAL_TO_U8_TERRAIN:
        terrain_type = OSM_NATURAL_TO_U8_TERRAIN[natural]
        return U8_TERRAIN_SHAPES.get(terrain_type, U8_TERRAIN_SHAPES["ground"])
    
    # Check highway
    highway = osm_tags.get("highway")
    if highway and highway in OSM_HIGHWAY_TO_U8_TERRAIN:
        terrain_type = OSM_HIGHWAY_TO_U8_TERRAIN[highway]
        return U8_TERRAIN_SHAPES.get(terrain_type, U8_TERRAIN_SHAPES["cobblestone"])
    
    # Default to ground
    return U8_TERRAIN_SHAPES["ground"]


def get_u8_object_shapes(osm_tags: dict) -> dict:
    """
    Get appropriate U8 object shapes from OSM tags.
    
    Args:
        osm_tags: Dictionary of OSM tags
        
    Returns:
        Dictionary mapping component names to shape lists
    """
    result = {}
    
    # Check building
    building = osm_tags.get("building")
    if building:
        building_type = building if building in OSM_BUILDING_TO_U8_SHAPES else "house"
        building_def = OSM_BUILDING_TO_U8_SHAPES.get(building_type, OSM_BUILDING_TO_U8_SHAPES["house"])
        for component, shapes in building_def.items():
            result[component] = shapes
        return result
    
    # Check natural features
    natural = osm_tags.get("natural")
    if natural == "tree":
        result["main"] = U8_OBJECT_SHAPES.get("tree", [301])
        return result
    if natural == "rock":
        result["main"] = U8_OBJECT_SHAPES.get("rock", [301])
        return result
    
    return result


def convert_u7_shape_to_u8(u7_shape: int) -> int:
    """
    Convert a U7 shape number to the nearest U8 equivalent.
    
    Args:
        u7_shape: Ultima VII shape number
        
    Returns:
        Ultima VIII shape number (approximate)
    """
    return U7_TO_U8_SHAPE_MAP.get(u7_shape, U8_DEFAULT_SHAPE)


@dataclass
class U8CoordinateTransformer:
    """
    Transform OSM lat/lon coordinates to Ultima VIII world coordinates.
    
    U8 uses world coordinates in the range 0-65535 (16-bit).
    """
    
    min_lon: float
    min_lat: float
    max_lon: float
    max_lat: float
    world_width: int = 65536
    world_height: int = 65536
    
    def __post_init__(self):
        self.lon_range = self.max_lon - self.min_lon
        self.lat_range = self.max_lat - self.min_lat
    
    def osm_to_u8(self, lon: float, lat: float) -> Tuple[int, int]:
        """
        Convert OSM coordinates to U8 world coordinates.
        
        Args:
            lon: Longitude
            lat: Latitude
            
        Returns:
            Tuple of (world_x, world_y)
        """
        # Normalize to 0-1 range
        norm_x = (lon - self.min_lon) / self.lon_range if self.lon_range > 0 else 0.5
        norm_y = (lat - self.min_lat) / self.lat_range if self.lat_range > 0 else 0.5
        
        # Flip Y axis (OSM has origin at bottom-left, U8 at top-left)
        norm_y = 1.0 - norm_y
        
        # Scale to U8 world coordinates
        world_x = int(norm_x * (self.world_width - 1))
        world_y = int(norm_y * (self.world_height - 1))
        
        # Clamp to valid range
        world_x = max(0, min(self.world_width - 1, world_x))
        world_y = max(0, min(self.world_height - 1, world_y))
        
        return (world_x, world_y)
    
    def osm_to_u8_tile(self, lon: float, lat: float, tile_size: int = 256) -> Tuple[int, int, int, int]:
        """
        Convert OSM coordinates to U8 tile coordinates.
        
        Args:
            lon: Longitude
            lat: Latitude
            tile_size: World units per tile
            
        Returns:
            Tuple of (world_x, world_y, tile_x, tile_y)
        """
        world_x, world_y = self.osm_to_u8(lon, lat)
        tile_x = world_x // tile_size
        tile_y = world_y // tile_size
        return (world_x, world_y, tile_x, tile_y)


if __name__ == "__main__":
    # Test the mapping
    print("=== U8 Shape Mapping Test ===\n")
    
    # Test terrain mapping
    test_tags = [
        {"landuse": "forest"},
        {"natural": "water"},
        {"highway": "residential"},
    ]
    
    print("U8 Terrain Mapping:")
    for tags in test_tags:
        shapes = get_u8_terrain_shape(tags)
        print(f"  {tags} -> shapes {shapes}")
    
    # Test object mapping
    print("\nU8 Object Mapping:")
    test_objects = [
        {"building": "house"},
        {"building": "shop"},
        {"natural": "tree"},
    ]
    
    for tags in test_objects:
        shapes = get_u8_object_shapes(tags)
        print(f"  {tags} -> {shapes}")
    
    # Test coordinate transformation
    print("\nU8 Coordinate Transformation:")
    transformer = U8CoordinateTransformer(
        min_lon=-0.1,
        min_lat=51.5,
        max_lon=0.0,
        max_lat=51.6
    )
    
    test_coords = [
        (-0.05, 51.55),  # center
        (-0.1, 51.5),    # corner
        (0.0, 51.6),     # opposite corner
    ]
    
    for lon, lat in test_coords:
        world_x, world_y = transformer.osm_to_u8(lon, lat)
        print(f"  ({lon}, {lat}) -> U8 world ({world_x}, {world_y})")
    
    # Test U7 to U8 conversion
    print("\nU7 to U8 Shape Conversion:")
    test_u7_shapes = [4, 8, 24, 151, 76, 999]
    for u7_shape in test_u7_shapes:
        u8_shape = convert_u7_shape_to_u8(u7_shape)
        print(f"  U7 shape {u7_shape} -> U8 shape {u8_shape}")

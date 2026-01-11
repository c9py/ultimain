"""
OSM to Ultima VII Shape Mapping Dictionary

This module provides mappings between OpenStreetMap feature tags
and Ultima VII shape numbers for procedural map generation.

Shape numbers are from Ultima VII: The Black Gate (shapes.vga)
"""

# =============================================================================
# TERRAIN / GROUND SHAPES (used for chunk terrain)
# =============================================================================

TERRAIN_SHAPES = {
    # Natural terrain
    "grass": [4, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 146, 147, 148],
    "sand": [10, 111],
    "water": [8],
    "swamp": [22, 113, 114, 115, 116, 117],
    "dirt": [23],
    "mud": [149],
    "rocky_grass": [46],
    "sandy_grass": [28, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133],
    "muddy_bank": [29, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109],
    "grassy_mud": [134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145],
    "cave_floor": [5, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63],
    
    # Constructed surfaces
    "sidewalk": [1],
    "cobblestone": [24],
    "planking": [17],  # wooden walkway
    "tile": [18],
    "stone_floor": [21],
    "carpet": [0, 27, 47, 186, 187, 190, 269, 294, 413],
    "floor": [189, 193, 367, 368, 369, 370, 441],
    "ford": [14, 15, 84, 112],  # shallow water crossing
    "rut": [16, 25],  # cart tracks/road
}

# =============================================================================
# OSM TAG TO TERRAIN MAPPING
# =============================================================================

OSM_LANDUSE_TO_TERRAIN = {
    # Natural
    "forest": "grass",
    "grass": "grass",
    "meadow": "grass",
    "farmland": "grass",
    "orchard": "grass",
    "vineyard": "grass",
    "allotments": "dirt",
    "beach": "sand",
    "sand": "sand",
    "wetland": "swamp",
    "marsh": "swamp",
    "mud": "mud",
    "rock": "rocky_grass",
    "bare_rock": "rocky_grass",
    
    # Urban
    "residential": "grass",  # will have buildings placed
    "commercial": "cobblestone",
    "industrial": "cobblestone",
    "retail": "cobblestone",
    "construction": "dirt",
    "brownfield": "dirt",
    "landfill": "dirt",
    
    # Special
    "cemetery": "grass",
    "military": "grass",
    "quarry": "rocky_grass",
}

OSM_NATURAL_TO_TERRAIN = {
    "water": "water",
    "wetland": "swamp",
    "beach": "sand",
    "sand": "sand",
    "mud": "mud",
    "bare_rock": "rocky_grass",
    "scree": "rocky_grass",
    "grassland": "grass",
    "heath": "grass",
    "scrub": "grass",
    "wood": "grass",
}

OSM_SURFACE_TO_TERRAIN = {
    "asphalt": "cobblestone",
    "concrete": "stone_floor",
    "paving_stones": "cobblestone",
    "cobblestone": "cobblestone",
    "gravel": "dirt",
    "unpaved": "dirt",
    "ground": "dirt",
    "grass": "grass",
    "sand": "sand",
    "wood": "planking",
    "metal": "tile",
}

# =============================================================================
# OBJECT SHAPES (placed on terrain)
# =============================================================================

OBJECT_SHAPES = {
    # Trees and vegetation
    "tree": [181, 310, 332, 453],  # various tree shapes
    "evergreen": [306],
    "dead_tree": [185, 325],
    "fallen_tree": [309, 315],
    "stump": [313],
    "tropical_plant": [326],
    "cypress_tree": [327],
    "baobab_tree": [328],
    "brambles": [320],
    "reeds": [321],
    "cattails": [323],
    "weeds": [314],
    "greer_plant": [160],
    
    # Rocks and natural features
    "small_rock": [203],
    "rock": [331, 341],
    "boulder": [342, 343],
    "large_rock": [316],
    "rock_outcropping": [163],
    "mountain": [180, 182, 183, 195, 196, 197, 324, 395, 396],
    
    # Water features
    "well": [470],
    "spring": [7, 13],
    "bubbles": [334, 335],
    "waves": [384],
    
    # Buildings and structures
    "wall": [151, 152, 205, 206, 218, 219, 220, 221, 253, 266, 273, 308, 344, 345, 346, 348, 349, 350, 351, 355, 357, 358, 359, 362, 365, 366, 371, 374, 393, 425],
    "door": [270, 376, 432, 433],
    "window": [438],
    "chimney": [439],
    "fireplace": [442],
    "roof_slate": [164, 165, 166, 167, 169],
    "roof_wood": [170, 171, 172, 173, 174, 175, 176],
    "roof_tile": [156],
    "broken_wall": [216, 217, 255, 347, 356],
    "broken_door": [208, 211],
    "broken_roof": [223],
    "stairs": [385, 386, 387, 426, 427, 428, 429, 430],
    "fortress": [191, 192, 260, 263, 352],
    "fortress_gateway": [257],
    
    # Fences and barriers
    "fence": [378, 420, 421, 422],
    "iron_bars": [264],
    "portcullis": [271, 272],
    "glass_wall": [373],
    
    # Signs and markers
    "sign": [360, 361, 379],
    "banner": [286],
    "flag_blue": [222],
    "flag_red": [232],
    "flag_green": [248],
    
    # Furniture and interior
    "table": [333],
    "desk": [283, 407],
    "drawers": [416],
    "nightstand": [406],
    "seat": [292],
    "bed": [312, 363],  # gargoyle futon
    "rug": [188, 483],
    "tapestry": [293],
    "painting": [282],
    "mirror": [268],
    "trophy": [311, 409],
    "statue": [486],
    "grandfather_clock": [252],
    "sundial": [284],
    
    # Lighting
    "sconce": [481],
    "lit_sconce": [435],
    "light_source": [336],
    "lit_light_source": [338],
    "beam_of_light": [168],
    
    # Crafting and work
    "blacksmith": [304],
    "bellows": [431],
    "loom": [261],
    "alchemist_device": [177],
    "laboratory_burner": [307],
    "mining_machine": [410],
    "conveyer_belt": [411],
    
    # Farm and agriculture
    "crops": [423],
    "chicken_coop": [210],
    "pumpkin": [302],
    "honeycomb": [404],
    
    # Transport
    "cart": [301],
    "wagon_floor": [436],
    "wagon_wheel": [437],
    "bridge": [212, 213, 214, 215],
    "gangplank": [150],
    "ferry": [402],
    "ship_hold": [405],
    "mast": [199],
    "sails": [251],
    
    # Storage
    "cask": [434],
    "keg": [258],
    "chest": [76],
    "garbage": [415],
    
    # Miscellaneous
    "debris": [201, 202],
    "scorch_mark": [204, 207],
    "dust": [209, 224],
    "trap": [200],
    "moongate": [157],
    "platform": [233, 364],
    "stand": [158],
}

# =============================================================================
# OSM TAG TO OBJECT MAPPING
# =============================================================================

OSM_AMENITY_TO_SHAPE = {
    "fountain": "well",
    "bench": "seat",
    "waste_basket": "garbage",
    "post_box": "chest",
    "clock": "grandfather_clock",
    "drinking_water": "well",
    "shelter": "roof_wood",
    "marketplace": "stand",
    "place_of_worship": "statue",
}

OSM_BUILDING_TO_SHAPES = {
    # Residential
    "house": {"walls": "wall", "roof": "roof_slate", "door": "door", "window": "window"},
    "residential": {"walls": "wall", "roof": "roof_slate", "door": "door", "window": "window"},
    "apartments": {"walls": "wall", "roof": "roof_tile", "door": "door", "window": "window"},
    "detached": {"walls": "wall", "roof": "roof_wood", "door": "door", "window": "window"},
    "terrace": {"walls": "wall", "roof": "roof_slate", "door": "door", "window": "window"},
    
    # Commercial
    "commercial": {"walls": "wall", "roof": "roof_tile", "door": "door", "window": "window"},
    "retail": {"walls": "wall", "roof": "roof_tile", "door": "door", "window": "window"},
    "shop": {"walls": "wall", "roof": "roof_wood", "door": "door", "window": "window"},
    "kiosk": {"walls": "wall", "roof": "roof_wood", "door": "door"},
    
    # Industrial
    "industrial": {"walls": "wall", "roof": "roof_tile", "door": "door"},
    "warehouse": {"walls": "wall", "roof": "roof_wood", "door": "door"},
    "barn": {"walls": "wall", "roof": "roof_wood", "door": "door"},
    "farm": {"walls": "wall", "roof": "roof_wood", "door": "door"},
    
    # Civic
    "church": {"walls": "wall", "roof": "roof_slate", "door": "door", "window": "window"},
    "chapel": {"walls": "wall", "roof": "roof_slate", "door": "door", "window": "window"},
    "cathedral": {"walls": "fortress", "roof": "roof_slate", "door": "door", "window": "window"},
    "public": {"walls": "wall", "roof": "roof_tile", "door": "door", "window": "window"},
    "civic": {"walls": "wall", "roof": "roof_tile", "door": "door", "window": "window"},
    "government": {"walls": "fortress", "roof": "roof_slate", "door": "door", "window": "window"},
    "hospital": {"walls": "wall", "roof": "roof_tile", "door": "door", "window": "window"},
    "school": {"walls": "wall", "roof": "roof_slate", "door": "door", "window": "window"},
    "university": {"walls": "wall", "roof": "roof_slate", "door": "door", "window": "window"},
    
    # Special
    "castle": {"walls": "fortress", "roof": "roof_slate", "door": "portcullis"},
    "fort": {"walls": "fortress", "roof": "roof_slate", "door": "portcullis"},
    "tower": {"walls": "fortress", "roof": "roof_slate"},
    "ruins": {"walls": "broken_wall", "roof": "broken_roof"},
    "bridge": {"floor": "bridge"},
}

OSM_NATURAL_TO_SHAPE = {
    "tree": "tree",
    "wood": "tree",
    "scrub": "brambles",
    "wetland": "reeds",
    "water": "waves",
    "spring": "spring",
    "rock": "rock",
    "stone": "rock",
    "peak": "mountain",
    "cliff": "rock_outcropping",
    "cave_entrance": "cave_floor",
}

OSM_HIGHWAY_TO_TERRAIN = {
    "motorway": "cobblestone",
    "trunk": "cobblestone",
    "primary": "cobblestone",
    "secondary": "cobblestone",
    "tertiary": "cobblestone",
    "residential": "cobblestone",
    "service": "dirt",
    "track": "rut",
    "path": "dirt",
    "footway": "sidewalk",
    "pedestrian": "cobblestone",
    "cycleway": "dirt",
    "bridleway": "dirt",
    "steps": "stone_floor",
}

OSM_BARRIER_TO_SHAPE = {
    "fence": "fence",
    "wall": "wall",
    "hedge": "brambles",
    "gate": "door",
    "bollard": "small_rock",
    "retaining_wall": "wall",
    "city_wall": "fortress",
}

OSM_MAN_MADE_TO_SHAPE = {
    "bridge": "bridge",
    "pier": "planking",
    "tower": "fortress",
    "water_tower": "fortress",
    "chimney": "chimney",
    "windmill": "mast",
    "lighthouse": "fortress",
    "well": "well",
    "storage_tank": "keg",
}

OSM_WATERWAY_TO_TERRAIN = {
    "river": "water",
    "stream": "water",
    "canal": "water",
    "drain": "water",
    "ditch": "muddy_bank",
    "dam": "stone_floor",
}

# =============================================================================
# NPC SHAPES (for populated areas)
# =============================================================================

NPC_SHAPES = {
    "townsman": [265, 319, 452],
    "townswoman": [459],  # wench
    "noble_male": [451, 456],
    "noble_female": [451, 456],
    "child": [471, 472],
    "guard": [394],
    "shopkeeper": [454, 455],
    "blacksmith": [304],
    "farmer": [319, 452],  # peasant
    "beggar": [449, 450],
    "mage": [154, 445, 446],
    "fighter": [259, 462, 463],
    "ranger": [460, 461],
    "paladin": [247, 464],
    "pirate": [401, 458],
    "gypsy": [457],
    "entertainer": [468, 469],
    "jester": [467],
    "sage": [318, 448],
}

# =============================================================================
# HELPER FUNCTIONS
# =============================================================================

def get_terrain_shape(osm_tags):
    """
    Get appropriate terrain shape number from OSM tags.
    Returns a list of possible shape numbers.
    """
    # Check landuse first
    landuse = osm_tags.get("landuse")
    if landuse and landuse in OSM_LANDUSE_TO_TERRAIN:
        terrain_type = OSM_LANDUSE_TO_TERRAIN[landuse]
        return TERRAIN_SHAPES.get(terrain_type, TERRAIN_SHAPES["grass"])
    
    # Check natural
    natural = osm_tags.get("natural")
    if natural and natural in OSM_NATURAL_TO_TERRAIN:
        terrain_type = OSM_NATURAL_TO_TERRAIN[natural]
        return TERRAIN_SHAPES.get(terrain_type, TERRAIN_SHAPES["grass"])
    
    # Check surface
    surface = osm_tags.get("surface")
    if surface and surface in OSM_SURFACE_TO_TERRAIN:
        terrain_type = OSM_SURFACE_TO_TERRAIN[surface]
        return TERRAIN_SHAPES.get(terrain_type, TERRAIN_SHAPES["grass"])
    
    # Check highway
    highway = osm_tags.get("highway")
    if highway and highway in OSM_HIGHWAY_TO_TERRAIN:
        terrain_type = OSM_HIGHWAY_TO_TERRAIN[highway]
        return TERRAIN_SHAPES.get(terrain_type, TERRAIN_SHAPES["cobblestone"])
    
    # Check waterway
    waterway = osm_tags.get("waterway")
    if waterway and waterway in OSM_WATERWAY_TO_TERRAIN:
        terrain_type = OSM_WATERWAY_TO_TERRAIN[waterway]
        return TERRAIN_SHAPES.get(terrain_type, TERRAIN_SHAPES["water"])
    
    # Default to grass
    return TERRAIN_SHAPES["grass"]


def get_object_shapes(osm_tags):
    """
    Get appropriate object shape numbers from OSM tags.
    Returns a dictionary of component -> shape list.
    """
    result = {}
    
    # Check building
    building = osm_tags.get("building")
    if building:
        building_type = building if building in OSM_BUILDING_TO_SHAPES else "house"
        building_def = OSM_BUILDING_TO_SHAPES.get(building_type, OSM_BUILDING_TO_SHAPES["house"])
        for component, shape_name in building_def.items():
            if shape_name in OBJECT_SHAPES:
                result[component] = OBJECT_SHAPES[shape_name]
        return result
    
    # Check amenity
    amenity = osm_tags.get("amenity")
    if amenity and amenity in OSM_AMENITY_TO_SHAPE:
        shape_name = OSM_AMENITY_TO_SHAPE[amenity]
        if shape_name in OBJECT_SHAPES:
            result["main"] = OBJECT_SHAPES[shape_name]
        return result
    
    # Check natural features
    natural = osm_tags.get("natural")
    if natural and natural in OSM_NATURAL_TO_SHAPE:
        shape_name = OSM_NATURAL_TO_SHAPE[natural]
        if shape_name in OBJECT_SHAPES:
            result["main"] = OBJECT_SHAPES[shape_name]
        return result
    
    # Check barrier
    barrier = osm_tags.get("barrier")
    if barrier and barrier in OSM_BARRIER_TO_SHAPE:
        shape_name = OSM_BARRIER_TO_SHAPE[barrier]
        if shape_name in OBJECT_SHAPES:
            result["main"] = OBJECT_SHAPES[shape_name]
        return result
    
    # Check man_made
    man_made = osm_tags.get("man_made")
    if man_made and man_made in OSM_MAN_MADE_TO_SHAPE:
        shape_name = OSM_MAN_MADE_TO_SHAPE[man_made]
        if shape_name in OBJECT_SHAPES:
            result["main"] = OBJECT_SHAPES[shape_name]
        return result
    
    return result


def get_npc_for_building(building_type):
    """
    Get appropriate NPC types for a building type.
    Returns a list of NPC shape numbers.
    """
    npc_mapping = {
        "house": ["townsman", "townswoman", "child"],
        "residential": ["townsman", "townswoman", "child"],
        "shop": ["shopkeeper"],
        "retail": ["shopkeeper"],
        "commercial": ["shopkeeper", "townsman"],
        "church": ["sage"],
        "castle": ["guard", "noble_male", "noble_female"],
        "fort": ["guard", "fighter"],
        "government": ["guard", "noble_male"],
        "hospital": ["sage"],
        "school": ["sage", "child"],
        "tavern": ["entertainer", "townsman", "townswoman"],
        "farm": ["farmer"],
        "barn": ["farmer"],
    }
    
    npc_types = npc_mapping.get(building_type, ["townsman"])
    shapes = []
    for npc_type in npc_types:
        if npc_type in NPC_SHAPES:
            shapes.extend(NPC_SHAPES[npc_type])
    return shapes


# =============================================================================
# COORDINATE TRANSFORMATION
# =============================================================================

class CoordinateTransformer:
    """
    Transform OSM lat/lon coordinates to Ultima tile coordinates.
    """
    
    def __init__(self, bbox, ultima_size=(192, 192)):
        """
        Initialize transformer with OSM bounding box and target Ultima map size.
        
        bbox: (min_lon, min_lat, max_lon, max_lat)
        ultima_size: (width_chunks, height_chunks) - default is full U7 map
        """
        self.min_lon, self.min_lat, self.max_lon, self.max_lat = bbox
        self.lon_range = self.max_lon - self.min_lon
        self.lat_range = self.max_lat - self.min_lat
        
        # Ultima map dimensions in tiles
        self.tiles_x = ultima_size[0] * 16  # chunks * tiles_per_chunk
        self.tiles_y = ultima_size[1] * 16
    
    def osm_to_ultima(self, lon, lat):
        """
        Convert OSM coordinates to Ultima tile coordinates.
        Returns (tile_x, tile_y)
        """
        # Normalize to 0-1 range
        norm_x = (lon - self.min_lon) / self.lon_range if self.lon_range > 0 else 0.5
        norm_y = (lat - self.min_lat) / self.lat_range if self.lat_range > 0 else 0.5
        
        # Flip Y axis (OSM has origin at bottom-left, Ultima at top-left)
        norm_y = 1.0 - norm_y
        
        # Scale to Ultima coordinates
        tile_x = int(norm_x * self.tiles_x)
        tile_y = int(norm_y * self.tiles_y)
        
        # Clamp to valid range
        tile_x = max(0, min(self.tiles_x - 1, tile_x))
        tile_y = max(0, min(self.tiles_y - 1, tile_y))
        
        return (tile_x, tile_y)
    
    def osm_to_chunk(self, lon, lat):
        """
        Convert OSM coordinates to Ultima chunk coordinates.
        Returns (chunk_x, chunk_y, tile_x_in_chunk, tile_y_in_chunk)
        """
        tile_x, tile_y = self.osm_to_ultima(lon, lat)
        chunk_x = tile_x // 16
        chunk_y = tile_y // 16
        local_x = tile_x % 16
        local_y = tile_y % 16
        return (chunk_x, chunk_y, local_x, local_y)


if __name__ == "__main__":
    # Test the mapping
    print("=== OSM to Ultima Shape Mapping Test ===\n")
    
    # Test terrain mapping
    test_tags = [
        {"landuse": "forest"},
        {"natural": "water"},
        {"highway": "residential"},
        {"surface": "cobblestone"},
    ]
    
    print("Terrain Mapping:")
    for tags in test_tags:
        shapes = get_terrain_shape(tags)
        print(f"  {tags} -> shapes {shapes[:3]}...")
    
    # Test object mapping
    print("\nObject Mapping:")
    test_objects = [
        {"building": "house"},
        {"building": "church"},
        {"natural": "tree"},
        {"amenity": "fountain"},
    ]
    
    for tags in test_objects:
        shapes = get_object_shapes(tags)
        print(f"  {tags} -> {shapes}")
    
    # Test coordinate transformation
    print("\nCoordinate Transformation:")
    # Example: small area in London
    bbox = (-0.1, 51.5, -0.09, 51.51)  # ~1km x 1km
    transformer = CoordinateTransformer(bbox, ultima_size=(16, 16))  # 16x16 chunks
    
    test_coords = [
        (-0.095, 51.505),  # center
        (-0.1, 51.5),      # corner
        (-0.09, 51.51),    # opposite corner
    ]
    
    for lon, lat in test_coords:
        tile = transformer.osm_to_ultima(lon, lat)
        chunk = transformer.osm_to_chunk(lon, lat)
        print(f"  ({lon}, {lat}) -> tile {tile}, chunk {chunk}")

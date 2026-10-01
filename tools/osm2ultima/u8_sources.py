"""OSM and procedural adapters into the shared placed-object list.

Terrain class is assigned from OSM tags or from the stamp cell. Shape ids
on generated U8 records stay placeholders and are not used as collision.
"""

import random
from typing import List, Sequence, Tuple, Union

from osm2u8 import U8MapGenerator
from u8_engine_constants import SQUARE, world_in_range
from u8_walk import (
    TERRAIN_LAND,
    TERRAIN_PATH,
    TERRAIN_SOLID,
    TERRAIN_WATER,
    PlacedObject,
)

STAMP_SPACING = SQUARE
Cell = Union[str, Tuple[str, int]]


def terrain_from_tags(tags: dict) -> str:
    """Map OSM tags onto land, path, water, or solid."""
    if "waterway" in tags or tags.get("natural") in ("water", "wetland"):
        return TERRAIN_WATER
    if "highway" in tags:
        return TERRAIN_PATH
    if "building" in tags:
        return TERRAIN_SOLID
    return TERRAIN_LAND


def procedural_stamp(
    grid: Sequence[Sequence[Cell]],
    origin: Tuple[int, int] = (0, 0),
    spacing: int = STAMP_SPACING,
    seed: int = 0,
) -> List[PlacedObject]:
    """Place a grid of cells `spacing` units apart, starting at `origin`.

    A cell is a terrain name, or a (terrain, top z) pair. The default
    spacing is 128 world units. `seed` fixes any later shape pick.
    """
    random.seed(seed)
    placed: List[PlacedObject] = []
    for row_index, row in enumerate(grid):
        for col_index, cell in enumerate(row):
            if isinstance(cell, str):
                terrain, top_z = cell, 0
            else:
                terrain, top_z = cell
            x = origin[0] + col_index * spacing
            y = origin[1] + row_index * spacing
            if not world_in_range(x, y):
                raise ValueError(
                    f"stamp coordinate ({x}, {y}) is outside the playfield"
                )
            placed.append(
                PlacedObject(x=x, y=y, z=top_z, terrain=terrain, flags=0)
            )
    return placed


def osm_to_placed(
    osm_data: dict,
    bbox: Tuple[float, float, float, float],
) -> List[PlacedObject]:
    """Adapt U8 fixed objects from an in-memory OSM dict.

    A way that cites a missing node id is skipped entirely. No network
    fetch is performed.
    """
    random.seed(0)
    generator = U8MapGenerator(bbox)
    for element in osm_data.get("elements", []):
        if element.get("type") == "node" and "id" in element:
            generator.nodes[element["id"]] = (element["lon"], element["lat"])

    placed: List[PlacedObject] = []
    for element in osm_data.get("elements", []):
        if element.get("type") != "way":
            continue
        node_ids = element.get("nodes") or []
        if any(node_id not in generator.nodes for node_id in node_ids):
            continue
        before = len(generator.u8_fixed_objects)
        generator._process_way_u8(element)
        terrain = terrain_from_tags(element.get("tags", {}))
        for obj in generator.u8_fixed_objects[before:]:
            if not world_in_range(obj.x, obj.y):
                continue
            placed.append(
                PlacedObject(
                    x=obj.x,
                    y=obj.y,
                    z=obj.z,
                    terrain=terrain,
                    flags=obj.flags,
                )
            )
    return placed

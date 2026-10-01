"""Ultima VIII engine literals for the map-modularity harness.

The numbers are copied from Pentagram. This module does not link the engine.

Sources:
- engines/ultima8/world/CurrentMap.h (MAP_NUM_CHUNKS)
- engines/ultima8/world/CurrentMap.cpp (U8 chunk size 512)
- engines/ultima8/misc/Direction.h (x_fact, y_fact, direction order)
- engines/ultima8/graphics/ShapeInfo.h (SI_SOLID, SI_SEA, SI_LAND)
- engines/ultima8/filesys/FlexFile.cpp (0x1A title, count at 0x54, index at 0x80)
- engines/ultima8/docs/u8mapfmt.txt (isometric screen formulas)
"""

from typing import Tuple

# engines/ultima8/world/CurrentMap.h
MAP_NUM_CHUNKS = 64

# engines/ultima8/world/CurrentMap.cpp sets mapChunkSize to 512 for U8.
# The playfield is MAP_NUM_CHUNKS * 512, so 0 <= coord < 32768.
U8_CHUNK_SIZE = 512
WORLD_SPAN = MAP_NUM_CHUNKS * U8_CHUNK_SIZE

# engines/ultima8/misc/Direction.h
# north, northeast, east, southeast, south, southwest, west, northwest
DIR_NORTH = 0
DIR_NORTHEAST = 1
DIR_EAST = 2
DIR_SOUTHEAST = 3
DIR_SOUTH = 4
DIR_SOUTHWEST = 5
DIR_WEST = 6
DIR_NORTHWEST = 7

X_FACT = (0, 1, 1, 1, 0, -1, -1, -1)
Y_FACT = (-1, -1, 0, 1, 1, 1, 0, -1)

# engines/ultima8/world/actors/AnimationTracker.cpp
# dx = 4 * x_fact[dir] * deltadir
WALK_STEP_UNITS = 4

# engines/ultima8/graphics/ShapeInfo.h
SI_SOLID = 0x0002
SI_SEA = 0x0004
SI_LAND = 0x0008

# engines/ultima8/filesys/FlexFile.cpp
FLEX_TITLE_LENGTH = 0x52
FLEX_MAGIC = 0x1A
FLEX_COUNT_OFFSET = 0x54
FLEX_INDEX_OFFSET = 0x80

# Procedural occupancy used by the synthetic walker. Each placed object
# owns the half-open square [x, x + SQUARE) by [y, y + SQUARE).
SQUARE = 128


def trunc_div(numerator: int, denominator: int) -> int:
    """Signed integer division that truncates toward zero."""
    if denominator == 0:
        raise ZeroDivisionError("division by zero")
    if numerator < 0:
        return -((-numerator) // denominator)
    return numerator // denominator


def screen_position(x: int, y: int, z: int) -> Tuple[int, int]:
    """Map world XYZ to screen XY using the U8 formulas in u8mapfmt.txt.

    ScreenX = (MapX - MapY) / 4
    ScreenY = (MapX + MapY) / 8 - MapZ
    Division truncates toward zero, matching C++ signed integer division.
    """
    screen_x = trunc_div(x - y, 4)
    screen_y = trunc_div(x + y, 8) - z
    return screen_x, screen_y


def chunk_index(x: int, y: int) -> Tuple[int, int]:
    """Bin a world point into the 64 by 64 chunk grid."""
    return x // U8_CHUNK_SIZE, y // U8_CHUNK_SIZE


def world_in_range(x: int, y: int) -> bool:
    """Return whether both coordinates lie in [0, WORLD_SPAN)."""
    return 0 <= x < WORLD_SPAN and 0 <= y < WORLD_SPAN


def require_world(x: int, y: int) -> Tuple[int, int]:
    """Return (x, y) or raise ValueError when the point is outside the playfield."""
    if not world_in_range(x, y):
        raise ValueError(
            f"world coordinate ({x}, {y}) is outside [0, {WORLD_SPAN})"
        )
    return x, y

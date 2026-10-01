"""Synthetic Ultima VIII walker over a placed-object list.

Collision comes from an explicit terrain class on each object. The 16-byte
record flags field is not consulted. Direction factors are the literals in
u8_engine_constants, copied from engines/ultima8/misc/Direction.h.
"""

from dataclasses import dataclass
from typing import Iterable, Optional, Sequence

from u8_engine_constants import (
    SQUARE,
    WALK_STEP_UNITS,
    X_FACT,
    Y_FACT,
    world_in_range,
)

TERRAIN_LAND = "land"
TERRAIN_WATER = "water"
TERRAIN_PATH = "path"
TERRAIN_SOLID = "solid"

SUPPORTING = (TERRAIN_LAND, TERRAIN_PATH)


@dataclass(frozen=True)
class PlacedObject:
    """One world item the walker can stand on.

    The object owns the half-open square [x, x+128) by [y, y+128).
    ``z`` is the top of that square. ``flags`` is stored so exporters can
    keep the record field at 0; the walker does not read it.
    """

    x: int
    y: int
    z: int
    terrain: str
    flags: int = 0


def square_contains(obj: PlacedObject, x: int, y: int) -> bool:
    return obj.x <= x < obj.x + SQUARE and obj.y <= y < obj.y + SQUARE


def object_at(objects: Iterable[PlacedObject], x: int, y: int) -> Optional[PlacedObject]:
    """Return the first object whose square contains the point."""
    for obj in objects:
        if square_contains(obj, x, y):
            return obj
    return None


def terrain_supports(obj: PlacedObject, walker_z: int) -> bool:
    """Land and path support at an equal top z. Water blocks.

    A solid floor supports when its top z equals the walker. Any other
    solid overlap blocks.
    """
    if obj.terrain == TERRAIN_WATER:
        return False
    if obj.terrain == TERRAIN_SOLID:
        return obj.z == walker_z
    if obj.terrain in SUPPORTING:
        return obj.z == walker_z
    return False


class Walker:
    """One-frame stand-in for an avatar step. deltadir stays 1."""

    def __init__(self, x: int, y: int, z: int, objects: Sequence[PlacedObject]):
        self.x = x
        self.y = y
        self.z = z
        self.objects = objects

    def step(self, direction: int, deltadir: int = 1) -> bool:
        """Advance one frame or leave the walker in place when blocked."""
        dx = WALK_STEP_UNITS * X_FACT[direction] * deltadir
        dy = WALK_STEP_UNITS * Y_FACT[direction] * deltadir
        next_x = self.x + dx
        next_y = self.y + dy
        if not world_in_range(next_x, next_y):
            return False
        ground = object_at(self.objects, next_x, next_y)
        if ground is None or not terrain_supports(ground, self.z):
            return False
        self.x = next_x
        self.y = next_y
        return True

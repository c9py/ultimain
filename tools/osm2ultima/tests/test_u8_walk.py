#!/usr/bin/env python3
"""Terrain-class walker. Record flags are not the collision source."""

import os
import sys
import unittest

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from u8_engine_constants import (
    DIR_EAST,
    DIR_NORTHEAST,
    chunk_index,
)
from u8_walk import TERRAIN_LAND, TERRAIN_PATH, TERRAIN_SOLID, TERRAIN_WATER, PlacedObject, Walker


def placed(x, y, terrain, z=0, flags=0):
    return PlacedObject(x=x, y=y, z=z, terrain=terrain, flags=flags)


class TestWalker(unittest.TestCase):
    def test_land_to_path_is_accepted(self):
        objects = [
            placed(0, 0, TERRAIN_LAND),
            placed(128, 0, TERRAIN_PATH),
        ]
        walker = Walker(124, 64, 0, objects)
        self.assertTrue(walker.step(DIR_EAST))
        self.assertEqual((walker.x, walker.y), (128, 64))
        self.assertTrue(all(obj.flags == 0 for obj in objects))

    def test_land_to_water_is_rejected(self):
        objects = [
            placed(0, 0, TERRAIN_LAND),
            placed(128, 0, TERRAIN_WATER),
        ]
        walker = Walker(124, 64, 0, objects)
        self.assertFalse(walker.step(DIR_EAST))
        self.assertEqual((walker.x, walker.y), (124, 64))

    def test_solid_floor_at_same_z_is_accepted(self):
        objects = [
            placed(0, 0, TERRAIN_LAND),
            placed(128, 0, TERRAIN_SOLID, z=0),
        ]
        walker = Walker(124, 64, 0, objects)
        self.assertTrue(walker.step(DIR_EAST))
        self.assertEqual(walker.z, 0)

    def test_overlapping_solid_is_rejected(self):
        objects = [
            placed(0, 0, TERRAIN_LAND),
            placed(128, 0, TERRAIN_SOLID, z=8),
        ]
        walker = Walker(124, 64, 0, objects)
        self.assertFalse(walker.step(DIR_EAST))
        self.assertEqual((walker.x, walker.y), (124, 64))

    def test_land_to_land_at_same_z(self):
        objects = [
            placed(0, 0, TERRAIN_LAND),
            placed(128, 0, TERRAIN_LAND),
        ]
        walker = Walker(124, 64, 0, objects)
        self.assertTrue(walker.step(DIR_EAST))

    def test_cardinal_step_is_four_units(self):
        objects = [placed(0, 0, TERRAIN_LAND)]
        walker = Walker(0, 64, 0, objects)
        self.assertTrue(walker.step(DIR_EAST, deltadir=1))
        self.assertEqual((walker.x, walker.y), (4, 64))

    def test_diagonal_step_changes_both_axes(self):
        objects = [placed(0, 0, TERRAIN_LAND)]
        walker = Walker(64, 64, 0, objects)
        self.assertTrue(walker.step(DIR_NORTHEAST))
        self.assertEqual((walker.x, walker.y), (68, 60))

    def test_empty_square_is_rejected(self):
        objects = [placed(0, 0, TERRAIN_LAND)]
        walker = Walker(124, 64, 0, objects)
        self.assertFalse(walker.step(DIR_EAST))

    def test_step_past_playfield_is_rejected(self):
        objects = [placed(32640, 0, TERRAIN_LAND)]
        walker = Walker(32764, 64, 0, objects)
        self.assertFalse(walker.step(DIR_EAST))
        self.assertEqual(walker.x, 32764)

    def test_destination_may_lie_in_another_chunk(self):
        objects = [
            placed(384, 0, TERRAIN_LAND),
            placed(512, 0, TERRAIN_PATH),
        ]
        walker = Walker(508, 64, 0, objects)
        self.assertEqual(chunk_index(walker.x, walker.y), (0, 0))
        self.assertTrue(walker.step(DIR_EAST))
        self.assertEqual(chunk_index(walker.x, walker.y), (1, 0))
        self.assertTrue(all(obj.flags == 0 for obj in objects))


if __name__ == "__main__":
    unittest.main()

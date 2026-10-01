#!/usr/bin/env python3
"""Layout checks for Ultima VIII isometric projection, chunks, and world range."""

import os
import sys
import unittest

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from u8_engine_constants import (
    MAP_NUM_CHUNKS,
    U8_CHUNK_SIZE,
    WORLD_SPAN,
    chunk_index,
    require_world,
    screen_position,
    world_in_range,
)


class TestU8Layout(unittest.TestCase):
    def test_engine_span(self):
        self.assertEqual(MAP_NUM_CHUNKS, 64)
        self.assertEqual(U8_CHUNK_SIZE, 512)
        self.assertEqual(WORLD_SPAN, 32768)

    def test_object_at_512_is_chunk_1_and_projects(self):
        self.assertEqual(chunk_index(512, 0), (1, 0))
        screen_x, screen_y = screen_position(512, 0, 0)
        self.assertEqual(screen_x, 128)
        self.assertEqual(screen_y, 64)

    def test_just_inside_first_chunk(self):
        self.assertEqual(chunk_index(511, 511), (0, 0))
        screen_x, screen_y = screen_position(511, 511, 8)
        self.assertEqual(screen_y, 119)
        self.assertEqual(screen_x, 0)

    def test_toward_zero_division_on_small_negative(self):
        # (0 - 1) / 4 is 0 toward zero. Python floor division would yield -1.
        screen_x, screen_y = screen_position(0, 1, 0)
        self.assertEqual(screen_x, 0)
        self.assertEqual(screen_y, 0)

    def test_exact_negative_screen_x(self):
        screen_x, _screen_y = screen_position(0, 4, 0)
        self.assertEqual(screen_x, -1)

    def test_nonzero_z_lowers_screen_y(self):
        _sx0, sy0 = screen_position(0, 0, 0)
        _sx1, sy1 = screen_position(0, 0, 1)
        self.assertEqual(sy0, 0)
        self.assertLess(sy1, sy0)

    def test_chunk_corners(self):
        self.assertEqual(chunk_index(0, 0), (0, 0))
        self.assertEqual(chunk_index(32767, 32767), (63, 63))
        self.assertEqual(chunk_index(512, 512), (1, 1))

    def test_world_range_rejects_bounds(self):
        self.assertFalse(world_in_range(32768, 0))
        self.assertFalse(world_in_range(-1, 0))
        self.assertTrue(world_in_range(0, 0))
        self.assertTrue(world_in_range(32767, 32767))
        with self.assertRaises(ValueError):
            require_world(32768, 0)
        with self.assertRaises(ValueError):
            require_world(-1, 0)


if __name__ == "__main__":
    unittest.main()

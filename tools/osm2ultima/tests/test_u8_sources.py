#!/usr/bin/env python3
"""OSM and procedural sources share one placed-object list."""

import os
import sys
import unittest
from unittest.mock import patch

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import osm2u8
from u8_engine_constants import DIR_EAST, world_in_range
from u8_sources import osm_to_placed, procedural_stamp
from u8_walk import TERRAIN_LAND, TERRAIN_PATH, TERRAIN_SOLID, TERRAIN_WATER, Walker

BBOX = (0.0, 0.0, 1.0, 1.0)


def _node(node_id, world_x, world_y=0):
    return {
        "type": "node",
        "id": node_id,
        "lon": world_x / 32767.0,
        "lat": 1.0 - (world_y / 32767.0),
    }


def _pattern_osm():
    """Hand-built elements that land on the same classes the stamp uses."""
    return {
        "elements": [
            _node(1, 0),
            _node(2, 200),
            _node(3, 128),
            _node(4, 129),
            _node(5, 256),
            _node(6, 257),
            _node(7, 384),
            _node(8, 640),
            {
                "type": "way",
                "id": 10,
                "nodes": [1, 2],
                "tags": {"landuse": "meadow"},
            },
            {
                "type": "way",
                "id": 11,
                "nodes": [3, 4],
                "tags": {"highway": "path"},
            },
            {
                "type": "way",
                "id": 12,
                "nodes": [5, 6],
                "tags": {"waterway": "stream"},
            },
            {
                "type": "way",
                "id": 13,
                "nodes": [7, 8, 7],
                "tags": {"building": "yes"},
            },
        ]
    }


def _class_outcomes(placed):
    """Step east onto path, water, a solid floor, and an overlapping solid."""

    def find(terrain, top_z, isolated=False):
        matches = [obj for obj in placed if obj.terrain == terrain and obj.z == top_z]
        if isolated:
            matches = [
                obj for obj in matches
                if not any(
                    other.x == obj.x and other.y == obj.y and other.z == 0
                    for other in placed
                )
            ]
        return matches[0]

    targets = (
        find(TERRAIN_PATH, 0),
        find(TERRAIN_WATER, 0),
        find(TERRAIN_SOLID, 0),
        find(TERRAIN_SOLID, 8, isolated=True),
    )
    results = []
    for target in targets:
        walker = Walker(target.x - 4, target.y + 64, 0, placed)
        results.append(walker.step(DIR_EAST))
    return tuple(results)


class TestSources(unittest.TestCase):
    def test_stamp_neighbors_are_128_apart(self):
        placed = procedural_stamp(
            [[TERRAIN_LAND, TERRAIN_PATH], [TERRAIN_WATER, (TERRAIN_SOLID, 0)]],
            origin=(0, 0),
            seed=0,
        )
        self.assertEqual(placed[1].x - placed[0].x, 128)
        self.assertEqual(placed[2].y - placed[0].y, 128)
        self.assertTrue(all(world_in_range(obj.x, obj.y) for obj in placed))
        self.assertTrue(all(obj.flags == 0 for obj in placed))

    def test_osm_and_stamp_agree_on_walker_classes(self):
        stamp = procedural_stamp(
            [[
                TERRAIN_LAND,
                TERRAIN_PATH,
                TERRAIN_WATER,
                (TERRAIN_SOLID, 0),
                (TERRAIN_SOLID, 8),
            ]],
            seed=0,
        )
        osm_placed = osm_to_placed(_pattern_osm(), BBOX)
        self.assertEqual(_class_outcomes(stamp), (True, False, True, False))
        self.assertEqual(_class_outcomes(osm_placed), _class_outcomes(stamp))
        self.assertTrue(all(world_in_range(obj.x, obj.y) for obj in osm_placed))
        self.assertTrue(all(obj.flags == 0 for obj in osm_placed))
        self.assertTrue(any(obj.terrain == TERRAIN_LAND for obj in osm_placed))

    def test_empty_osm_is_empty_list(self):
        self.assertEqual(osm_to_placed({"elements": []}, BBOX), [])

    def test_missing_node_is_skipped(self):
        elements = {
            "elements": [
                _node(1, 0),
                _node(2, 200),
                {
                    "type": "way",
                    "id": 1,
                    "nodes": [99, 99],
                    "tags": {"landuse": "grass"},
                },
                {
                    "type": "way",
                    "id": 2,
                    "nodes": [1, 2],
                    "tags": {"landuse": "meadow"},
                },
            ]
        }
        placed = osm_to_placed(elements, BBOX)
        self.assertEqual(len(placed), 1)
        self.assertEqual(placed[0].terrain, TERRAIN_LAND)
        self.assertTrue(world_in_range(placed[0].x, placed[0].y))

    def test_adapter_does_not_fetch(self):
        with patch.object(
            osm2u8.OSMFetcher,
            "fetch_osm_data",
            side_effect=AssertionError("network"),
        ):
            placed = osm_to_placed({"elements": []}, BBOX)
        self.assertEqual(placed, [])

    def test_modules_do_not_import_avatar(self):
        source_path = os.path.join(
            os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
            "u8_sources.py",
        )
        walk_path = os.path.join(
            os.path.dirname(source_path),
            "u8_walk.py",
        )
        for path in (source_path, walk_path):
            text = open(path, encoding="utf-8").read()
            self.assertNotIn("engines.avatar", text)
            self.assertNotIn("engines/avatar", text)


if __name__ == "__main__":
    unittest.main()

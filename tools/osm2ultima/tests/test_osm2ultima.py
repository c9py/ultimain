#!/usr/bin/env python3
"""
Unit tests for OSM2Ultima converter.
"""

import json
import math
import os
import sys
import tempfile
import unittest
from unittest.mock import MagicMock, patch

# Add parent directory to path for imports
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from osm2ultima import (
    UltimaObject,
    UltimaChunk,
    UltimaMap,
)

from osm_shape_mapping import (
    CoordinateTransformer,
    get_terrain_shape,
    get_object_shapes,
    TERRAIN_SHAPES,
    OBJECT_SHAPES,
    OSM_HIGHWAY_TO_TERRAIN,
)


class TestUltimaObject(unittest.TestCase):
    """Tests for UltimaObject data class."""

    def test_default_values(self):
        """Test default object values."""
        obj = UltimaObject(shape=100)
        self.assertEqual(obj.shape, 100)
        self.assertEqual(obj.frame, 0)
        self.assertEqual(obj.x, 0)
        self.assertEqual(obj.y, 0)
        self.assertEqual(obj.lift, 0)
        self.assertEqual(obj.quality, 0)
        self.assertEqual(obj.flags, 0)

    def test_custom_values(self):
        """Test object with custom values."""
        obj = UltimaObject(
            shape=150,
            frame=3,
            x=100,
            y=200,
            lift=2,
            quality=50,
            flags=1
        )
        self.assertEqual(obj.shape, 150)
        self.assertEqual(obj.frame, 3)
        self.assertEqual(obj.x, 100)
        self.assertEqual(obj.y, 200)
        self.assertEqual(obj.lift, 2)
        self.assertEqual(obj.quality, 50)
        self.assertEqual(obj.flags, 1)

    def test_to_ireg_bytes(self):
        """Test IREG byte conversion."""
        obj = UltimaObject(shape=100, x=32, y=48, lift=1)
        data = obj.to_ireg_bytes()
        self.assertIsInstance(data, bytes)
        self.assertEqual(len(data), 10)
        self.assertEqual(data[0], 10)  # Length byte

    def test_ireg_bytes_shape_encoding(self):
        """Test shape is encoded correctly in IREG format."""
        obj = UltimaObject(shape=255, x=0, y=0)
        data = obj.to_ireg_bytes()
        # Shape low byte should be at index 3
        self.assertEqual(data[3], 255)

    def test_ireg_bytes_position_encoding(self):
        """Test position encoding in IREG format."""
        obj = UltimaObject(shape=1, x=17, y=33)  # Chunk 1,2, local 1,1
        data = obj.to_ireg_bytes()
        # Position should be encoded in bytes 1 and 2
        self.assertIsInstance(data[1], int)
        self.assertIsInstance(data[2], int)


class TestUltimaChunk(unittest.TestCase):
    """Tests for UltimaChunk data class."""

    def test_default_terrain(self):
        """Test default terrain is grass (shape 4)."""
        chunk = UltimaChunk()
        self.assertEqual(len(chunk.terrain), 16)
        self.assertEqual(len(chunk.terrain[0]), 16)
        # Default terrain should be grass (4)
        self.assertEqual(chunk.terrain[0][0], 4)
        self.assertEqual(chunk.terrain[15][15], 4)

    def test_empty_objects(self):
        """Test chunk starts with no objects."""
        chunk = UltimaChunk()
        self.assertEqual(len(chunk.objects), 0)

    def test_add_object(self):
        """Test adding objects to chunk."""
        chunk = UltimaChunk()
        obj = UltimaObject(shape=100, x=5, y=5)
        chunk.objects.append(obj)
        self.assertEqual(len(chunk.objects), 1)
        self.assertEqual(chunk.objects[0].shape, 100)

    def test_modify_terrain(self):
        """Test modifying terrain."""
        chunk = UltimaChunk()
        chunk.terrain[5][5] = 10  # Set to water or something
        self.assertEqual(chunk.terrain[5][5], 10)


class TestUltimaMap(unittest.TestCase):
    """Tests for UltimaMap data class."""

    def test_default_dimensions(self):
        """Test default map dimensions."""
        uma = UltimaMap()
        self.assertEqual(uma.width_chunks, 16)
        self.assertEqual(uma.height_chunks, 16)

    def test_get_chunk_creates_new(self):
        """Test get_chunk creates chunk if not exists."""
        uma = UltimaMap()
        self.assertEqual(len(uma.chunks), 0)
        chunk = uma.get_chunk(0, 0)
        self.assertIsInstance(chunk, UltimaChunk)
        self.assertEqual(len(uma.chunks), 1)

    def test_get_chunk_returns_existing(self):
        """Test get_chunk returns existing chunk."""
        uma = UltimaMap()
        chunk1 = uma.get_chunk(3, 3)
        chunk1.terrain[0][0] = 99
        chunk2 = uma.get_chunk(3, 3)
        self.assertEqual(chunk2.terrain[0][0], 99)

    def test_set_terrain(self):
        """Test setting terrain by tile coordinates."""
        uma = UltimaMap()
        uma.set_terrain(17, 33, 10)  # Chunk (1,2), local (1,1)
        chunk = uma.get_chunk(1, 2)
        self.assertEqual(chunk.terrain[1][1], 10)

    def test_set_terrain_creates_chunk(self):
        """Test set_terrain creates chunk if needed."""
        uma = UltimaMap()
        uma.set_terrain(48, 64, 5)  # Chunk (3,4)
        self.assertIn((3, 4), uma.chunks)

    def test_add_object(self):
        """Test adding object to map."""
        uma = UltimaMap()
        obj = UltimaObject(shape=100, x=35, y=50)  # Chunk (2,3)
        uma.add_object(obj)
        chunk = uma.get_chunk(2, 3)
        self.assertEqual(len(chunk.objects), 1)
        self.assertEqual(chunk.objects[0].shape, 100)

    def test_multiple_objects_same_chunk(self):
        """Test adding multiple objects to same chunk."""
        uma = UltimaMap()
        for i in range(5):
            obj = UltimaObject(shape=100+i, x=5+i, y=5+i)
            uma.add_object(obj)
        chunk = uma.get_chunk(0, 0)
        self.assertEqual(len(chunk.objects), 5)


class TestCoordinateTransformer(unittest.TestCase):
    """Tests for CoordinateTransformer class."""

    def test_transformer_creation(self):
        """Test creating a coordinate transformer."""
        transformer = CoordinateTransformer(
            min_lon=-0.1,
            min_lat=51.5,
            max_lon=0.0,
            max_lat=51.6,
            map_width=256,
            map_height=256
        )
        self.assertIsNotNone(transformer)

    def test_transform_center(self):
        """Test transforming center coordinates."""
        transformer = CoordinateTransformer(
            min_lon=0.0,
            min_lat=0.0,
            max_lon=1.0,
            max_lat=1.0,
            map_width=100,
            map_height=100
        )
        x, y = transformer.transform(0.5, 0.5)
        self.assertAlmostEqual(x, 50, delta=1)
        self.assertAlmostEqual(y, 50, delta=1)

    def test_transform_corners(self):
        """Test transforming corner coordinates."""
        transformer = CoordinateTransformer(
            min_lon=0.0,
            min_lat=0.0,
            max_lon=1.0,
            max_lat=1.0,
            map_width=100,
            map_height=100
        )
        # Min corner
        x, y = transformer.transform(0.0, 0.0)
        self.assertAlmostEqual(x, 0, delta=1)

        # Max corner
        x, y = transformer.transform(1.0, 1.0)
        self.assertAlmostEqual(x, 100, delta=1)

    def test_transform_out_of_bounds(self):
        """Test transforming coordinates outside bounds."""
        transformer = CoordinateTransformer(
            min_lon=0.0,
            min_lat=0.0,
            max_lon=1.0,
            max_lat=1.0,
            map_width=100,
            map_height=100
        )
        # Out of bounds should still transform (clipping handled elsewhere)
        x, y = transformer.transform(-0.5, -0.5)
        self.assertLess(x, 0)


class TestShapeMappings(unittest.TestCase):
    """Tests for shape mapping functions."""

    def test_terrain_shapes_exist(self):
        """Test terrain shapes dictionary exists and has values."""
        self.assertIsInstance(TERRAIN_SHAPES, dict)
        self.assertGreater(len(TERRAIN_SHAPES), 0)

    def test_object_shapes_exist(self):
        """Test object shapes dictionary exists and has values."""
        self.assertIsInstance(OBJECT_SHAPES, dict)
        self.assertGreater(len(OBJECT_SHAPES), 0)

    def test_highway_mapping_exist(self):
        """Test highway to terrain mapping exists."""
        self.assertIsInstance(OSM_HIGHWAY_TO_TERRAIN, dict)
        self.assertGreater(len(OSM_HIGHWAY_TO_TERRAIN), 0)

    def test_get_terrain_shape_valid(self):
        """Test getting terrain shape for valid tag."""
        # Test common terrain types
        for tag_key in ['grass', 'water', 'forest', 'sand']:
            if tag_key in TERRAIN_SHAPES:
                shape = get_terrain_shape({'natural': tag_key}, {})
                self.assertIsInstance(shape, int)
                self.assertGreaterEqual(shape, 0)

    def test_get_terrain_shape_default(self):
        """Test getting default terrain shape for unknown tag."""
        shape = get_terrain_shape({'unknown_tag': 'unknown_value'}, {})
        self.assertIsInstance(shape, int)
        self.assertGreaterEqual(shape, 0)

    def test_get_object_shapes_building(self):
        """Test getting object shapes for buildings."""
        shapes = get_object_shapes({'building': 'yes'}, {})
        self.assertIsInstance(shapes, list)
        # Buildings should have at least one shape
        if shapes:
            self.assertIsInstance(shapes[0], int)


class TestMapGeneration(unittest.TestCase):
    """Integration tests for map generation."""

    def test_create_empty_map(self):
        """Test creating an empty map."""
        uma = UltimaMap(width_chunks=8, height_chunks=8)
        self.assertEqual(uma.width_chunks, 8)
        self.assertEqual(uma.height_chunks, 8)
        self.assertEqual(len(uma.chunks), 0)

    def test_populate_map_with_terrain(self):
        """Test populating map with terrain."""
        uma = UltimaMap()

        # Set various terrain types
        for x in range(32):
            for y in range(32):
                if x < 16 and y < 16:
                    uma.set_terrain(x, y, 4)  # Grass
                elif x >= 16 and y < 16:
                    uma.set_terrain(x, y, 10)  # Water
                else:
                    uma.set_terrain(x, y, 5)  # Forest

        # Verify terrain was set correctly
        chunk_00 = uma.get_chunk(0, 0)
        self.assertEqual(chunk_00.terrain[0][0], 4)  # Grass

        chunk_10 = uma.get_chunk(1, 0)
        self.assertEqual(chunk_10.terrain[0][0], 10)  # Water

    def test_populate_map_with_objects(self):
        """Test populating map with objects."""
        uma = UltimaMap()

        # Add some buildings
        for i in range(10):
            obj = UltimaObject(
                shape=150,  # Building shape
                x=i * 10,
                y=i * 10,
                lift=0
            )
            uma.add_object(obj)

        # Count total objects across all chunks
        total_objects = sum(len(c.objects) for c in uma.chunks.values())
        self.assertEqual(total_objects, 10)


class TestDataValidation(unittest.TestCase):
    """Tests for data validation."""

    def test_object_shape_range(self):
        """Test object shapes are within valid range."""
        # Ultima 7 shapes are 0-1023
        for shape in OBJECT_SHAPES.values():
            if isinstance(shape, int):
                self.assertGreaterEqual(shape, 0)
                self.assertLess(shape, 1024)
            elif isinstance(shape, list):
                for s in shape:
                    self.assertGreaterEqual(s, 0)
                    self.assertLess(s, 1024)

    def test_terrain_shape_range(self):
        """Test terrain shapes are within valid range."""
        for shape in TERRAIN_SHAPES.values():
            if isinstance(shape, int):
                self.assertGreaterEqual(shape, 0)
                self.assertLess(shape, 256)


class TestJSONTestData(unittest.TestCase):
    """Tests using the test JSON data file."""

    @classmethod
    def setUpClass(cls):
        """Load test JSON data."""
        test_data_path = os.path.join(
            os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
            'test_osm_data.json'
        )
        if os.path.exists(test_data_path):
            with open(test_data_path, 'r') as f:
                cls.test_data = json.load(f)
        else:
            cls.test_data = None

    def test_json_data_loads(self):
        """Test that JSON test data can be loaded."""
        if self.test_data is None:
            self.skipTest("Test data file not found")
        self.assertIsInstance(self.test_data, dict)

    def test_json_has_elements(self):
        """Test that JSON data has elements."""
        if self.test_data is None:
            self.skipTest("Test data file not found")
        if 'elements' in self.test_data:
            self.assertIsInstance(self.test_data['elements'], list)


if __name__ == '__main__':
    unittest.main(verbosity=2)

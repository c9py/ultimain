#!/usr/bin/env python3
"""
Unit tests for U8 format handling module.

Tests the 16-byte record packing/unpacking and FIXED.DAT file generation.
"""

import os
import struct
import sys
import tempfile
import unittest

# Add parent directory to path for imports
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from u8_format import (
    U8Object,
    U8MapData,
    U8FixedDatWriter,
    U8FixedDatReader,
    U8_RECORD_SIZE,
    U8_HEADER_SIZE,
    U8_MAP_COUNT_OFFSET,
    U8_DEFAULT_MAP_COUNT,
    convert_osm_to_u8_coords,
    convert_u8_to_tile_coords,
)


class TestU8Object(unittest.TestCase):
    """Tests for U8Object 16-byte record handling."""

    def test_record_size(self):
        """Test that U8 record size constant is correct."""
        self.assertEqual(U8_RECORD_SIZE, 16)

    def test_object_creation(self):
        """Test creating a U8Object with default values."""
        obj = U8Object(x=0, y=0, z=0, shape=100)
        self.assertEqual(obj.x, 0)
        self.assertEqual(obj.y, 0)
        self.assertEqual(obj.z, 0)
        self.assertEqual(obj.shape, 100)
        self.assertEqual(obj.frame, 0)
        self.assertEqual(obj.flags, 0)
        self.assertEqual(obj.quality, 0)
        self.assertEqual(obj.npcnum, 0)
        self.assertEqual(obj.mapnum, 0)
        self.assertEqual(obj.next_id, 0)

    def test_object_creation_full(self):
        """Test creating a U8Object with all values."""
        obj = U8Object(
            x=16384, y=32768, z=48,
            shape=301, frame=5,
            flags=0x0001, quality=100,
            npcnum=7, mapnum=3, next_id=42
        )
        self.assertEqual(obj.x, 16384)
        self.assertEqual(obj.y, 32768)
        self.assertEqual(obj.z, 48)
        self.assertEqual(obj.shape, 301)
        self.assertEqual(obj.frame, 5)
        self.assertEqual(obj.flags, 1)
        self.assertEqual(obj.quality, 100)
        self.assertEqual(obj.npcnum, 7)
        self.assertEqual(obj.mapnum, 3)
        self.assertEqual(obj.next_id, 42)

    def test_to_bytes_length(self):
        """Test that to_bytes produces exactly 16 bytes."""
        obj = U8Object(x=1000, y=2000, z=50, shape=301)
        data = obj.to_bytes()
        self.assertEqual(len(data), 16)

    def test_to_bytes_format(self):
        """Test the binary format matches U8 spec."""
        obj = U8Object(
            x=0x1234, y=0x5678, z=0x9A,
            shape=0xBCDE, frame=0xF0,
            flags=0x1111, quality=0x2222,
            npcnum=0x33, mapnum=0x44, next_id=0x5555
        )
        data = obj.to_bytes()
        
        # Unpack and verify each field
        x, y, z, shape, frame, flags, quality, npcnum, mapnum, next_id = struct.unpack(
            '<HHBHBHHBBH', data
        )
        self.assertEqual(x, 0x1234)
        self.assertEqual(y, 0x5678)
        self.assertEqual(z, 0x9A)
        self.assertEqual(shape, 0xBCDE)
        self.assertEqual(frame, 0xF0)
        self.assertEqual(flags, 0x1111)
        self.assertEqual(quality, 0x2222)
        self.assertEqual(npcnum, 0x33)
        self.assertEqual(mapnum, 0x44)
        self.assertEqual(next_id, 0x5555)

    def test_from_bytes(self):
        """Test unpacking from binary data."""
        # Create known binary data
        data = struct.pack(
            '<HHBHBHHBBH',
            1000, 2000, 50,  # x, y, z
            301, 3,         # shape, frame
            0x00, 100,      # flags, quality
            5, 0, 0         # npcnum, mapnum, next_id
        )
        
        obj = U8Object.from_bytes(data)
        self.assertEqual(obj.x, 1000)
        self.assertEqual(obj.y, 2000)
        self.assertEqual(obj.z, 50)
        self.assertEqual(obj.shape, 301)
        self.assertEqual(obj.frame, 3)
        self.assertEqual(obj.flags, 0)
        self.assertEqual(obj.quality, 100)
        self.assertEqual(obj.npcnum, 5)
        self.assertEqual(obj.mapnum, 0)
        self.assertEqual(obj.next_id, 0)

    def test_roundtrip(self):
        """Test that to_bytes and from_bytes are inverse operations."""
        original = U8Object(
            x=12345, y=54321, z=100,
            shape=500, frame=7,
            flags=0xFF, quality=1234,
            npcnum=42, mapnum=10, next_id=999
        )
        
        data = original.to_bytes()
        restored = U8Object.from_bytes(data)
        
        self.assertEqual(original.x, restored.x)
        self.assertEqual(original.y, restored.y)
        self.assertEqual(original.z, restored.z)
        self.assertEqual(original.shape, restored.shape)
        self.assertEqual(original.frame, restored.frame)
        self.assertEqual(original.flags, restored.flags)
        self.assertEqual(original.quality, restored.quality)
        self.assertEqual(original.npcnum, restored.npcnum)
        self.assertEqual(original.mapnum, restored.mapnum)
        self.assertEqual(original.next_id, restored.next_id)

    def test_from_bytes_wrong_length(self):
        """Test that from_bytes raises error for wrong data length."""
        with self.assertRaises(ValueError):
            U8Object.from_bytes(b'tooshort')
        
        with self.assertRaises(ValueError):
            U8Object.from_bytes(b'waytoolongdata12345')

    def test_coordinate_overflow(self):
        """Test that coordinates are properly masked to fit field sizes."""
        obj = U8Object(
            x=70000,   # > 65535, should be masked
            y=70000,
            z=300,     # > 255, should be masked
            shape=100
        )
        data = obj.to_bytes()
        restored = U8Object.from_bytes(data)
        
        # Values should be masked to fit their field sizes
        self.assertEqual(restored.x, 70000 & 0xFFFF)
        self.assertEqual(restored.y, 70000 & 0xFFFF)
        self.assertEqual(restored.z, 300 & 0xFF)


class TestU8MapData(unittest.TestCase):
    """Tests for U8MapData container."""

    def test_empty_map(self):
        """Test creating an empty map."""
        map_data = U8MapData(mapnum=0, objects=[])
        self.assertEqual(map_data.mapnum, 0)
        self.assertEqual(len(map_data.objects), 0)

    def test_map_with_objects(self):
        """Test creating a map with objects."""
        objects = [
            U8Object(x=100, y=200, z=0, shape=301),
            U8Object(x=300, y=400, z=0, shape=302),
            U8Object(x=500, y=600, z=0, shape=303),
        ]
        map_data = U8MapData(mapnum=5, objects=objects)
        
        self.assertEqual(map_data.mapnum, 5)
        self.assertEqual(len(map_data.objects), 3)

    def test_to_bytes(self):
        """Test serializing map data to bytes."""
        objects = [
            U8Object(x=100, y=200, z=0, shape=301),
            U8Object(x=300, y=400, z=0, shape=302),
        ]
        map_data = U8MapData(mapnum=0, objects=objects)
        data = map_data.to_bytes()
        
        self.assertEqual(len(data), 2 * U8_RECORD_SIZE)

    def test_from_bytes(self):
        """Test parsing map data from bytes."""
        # Create 2 objects worth of data
        obj1 = U8Object(x=100, y=200, z=0, shape=301)
        obj2 = U8Object(x=300, y=400, z=0, shape=302)
        data = obj1.to_bytes() + obj2.to_bytes()
        
        map_data = U8MapData.from_bytes(mapnum=3, data=data)
        
        self.assertEqual(map_data.mapnum, 3)
        self.assertEqual(len(map_data.objects), 2)
        self.assertEqual(map_data.objects[0].x, 100)
        self.assertEqual(map_data.objects[1].x, 300)


class TestU8FixedDatWriter(unittest.TestCase):
    """Tests for FIXED.DAT/NONFIXED.DAT file writing."""

    def test_create_writer(self):
        """Test creating a writer with default map count."""
        writer = U8FixedDatWriter()
        self.assertEqual(writer.map_count, U8_DEFAULT_MAP_COUNT)

    def test_add_map(self):
        """Test adding objects to a map."""
        writer = U8FixedDatWriter()
        objects = [
            U8Object(x=100, y=200, z=0, shape=301),
        ]
        writer.add_map(0, objects)
        
        self.assertIn(0, writer.maps)
        self.assertEqual(len(writer.maps[0].objects), 1)

    def test_add_map_invalid_number(self):
        """Test that invalid map numbers raise errors."""
        writer = U8FixedDatWriter()
        
        with self.assertRaises(ValueError):
            writer.add_map(-1, [])
        
        with self.assertRaises(ValueError):
            writer.add_map(256, [])  # Default is 256 maps (0-255)

    def test_write_empty_file(self):
        """Test writing a file with no objects."""
        writer = U8FixedDatWriter(map_count=256)
        
        with tempfile.NamedTemporaryFile(delete=False) as f:
            filepath = f.name
        
        try:
            writer.write(filepath)
            
            # Verify file was created
            self.assertTrue(os.path.exists(filepath))
            
            # Verify file size (header + map info)
            expected_size = U8_HEADER_SIZE + (256 * 8)
            self.assertEqual(os.path.getsize(filepath), expected_size)
            
            # Verify header
            with open(filepath, 'rb') as f:
                header = f.read(U8_HEADER_SIZE)
                map_count = struct.unpack_from('<H', header, U8_MAP_COUNT_OFFSET)[0]
                self.assertEqual(map_count, 256)
        finally:
            os.unlink(filepath)

    def test_write_with_objects(self):
        """Test writing a file with objects."""
        writer = U8FixedDatWriter(map_count=256)
        
        objects = [
            U8Object(x=1000, y=2000, z=50, shape=301),
            U8Object(x=3000, y=4000, z=100, shape=302),
        ]
        writer.add_map(0, objects)
        
        with tempfile.NamedTemporaryFile(delete=False) as f:
            filepath = f.name
        
        try:
            writer.write(filepath)
            
            # Calculate expected size
            header_size = U8_HEADER_SIZE
            map_info_size = 256 * 8
            data_size = 2 * U8_RECORD_SIZE
            expected_size = header_size + map_info_size + data_size
            
            self.assertEqual(os.path.getsize(filepath), expected_size)
        finally:
            os.unlink(filepath)


class TestU8FixedDatReader(unittest.TestCase):
    """Tests for FIXED.DAT/NONFIXED.DAT file reading."""

    def _create_test_file(self, map_count=256, objects_map=None):
        """Helper to create a test file and return its path."""
        writer = U8FixedDatWriter(map_count=map_count)
        
        if objects_map:
            for mapnum, objects in objects_map.items():
                writer.add_map(mapnum, objects)
        
        with tempfile.NamedTemporaryFile(delete=False) as f:
            filepath = f.name
        
        writer.write(filepath)
        return filepath

    def test_read_header(self):
        """Test reading file header."""
        filepath = self._create_test_file()
        
        try:
            reader = U8FixedDatReader(filepath)
            self.assertEqual(reader.map_count, 256)
            self.assertEqual(len(reader.map_infos), 256)
        finally:
            os.unlink(filepath)

    def test_read_empty_map(self):
        """Test reading an empty map."""
        filepath = self._create_test_file()
        
        try:
            reader = U8FixedDatReader(filepath)
            map_data = reader.read_map(0)
            
            self.assertEqual(map_data.mapnum, 0)
            self.assertEqual(len(map_data.objects), 0)
        finally:
            os.unlink(filepath)

    def test_read_map_with_objects(self):
        """Test reading a map with objects."""
        objects = [
            U8Object(x=1000, y=2000, z=50, shape=301),
            U8Object(x=3000, y=4000, z=100, shape=302),
        ]
        filepath = self._create_test_file(objects_map={0: objects})
        
        try:
            reader = U8FixedDatReader(filepath)
            map_data = reader.read_map(0)
            
            self.assertEqual(map_data.mapnum, 0)
            self.assertEqual(len(map_data.objects), 2)
            self.assertEqual(map_data.objects[0].x, 1000)
            self.assertEqual(map_data.objects[0].shape, 301)
            self.assertEqual(map_data.objects[1].x, 3000)
            self.assertEqual(map_data.objects[1].shape, 302)
        finally:
            os.unlink(filepath)

    def test_read_multiple_maps(self):
        """Test reading multiple populated maps."""
        objects_map = {
            0: [U8Object(x=100, y=100, z=0, shape=301)],
            5: [U8Object(x=500, y=500, z=0, shape=305)],
            10: [
                U8Object(x=1000, y=1000, z=0, shape=310),
                U8Object(x=1001, y=1001, z=0, shape=311),
            ],
        }
        filepath = self._create_test_file(objects_map=objects_map)
        
        try:
            reader = U8FixedDatReader(filepath)
            
            # Read all maps
            all_maps = reader.read_all_maps()
            
            # Only non-empty maps should be returned
            self.assertEqual(len(all_maps), 3)
            self.assertIn(0, all_maps)
            self.assertIn(5, all_maps)
            self.assertIn(10, all_maps)
            
            self.assertEqual(len(all_maps[0].objects), 1)
            self.assertEqual(len(all_maps[5].objects), 1)
            self.assertEqual(len(all_maps[10].objects), 2)
        finally:
            os.unlink(filepath)

    def test_roundtrip(self):
        """Test complete write/read roundtrip."""
        original_objects = [
            U8Object(x=12345, y=54321, z=100, shape=500, frame=7,
                    flags=0xFF, quality=1234, npcnum=42, mapnum=10, next_id=999),
            U8Object(x=11111, y=22222, z=33, shape=444, frame=5,
                    flags=0xAB, quality=555, npcnum=6, mapnum=7, next_id=88),
        ]
        
        filepath = self._create_test_file(objects_map={3: original_objects})
        
        try:
            reader = U8FixedDatReader(filepath)
            map_data = reader.read_map(3)
            
            self.assertEqual(len(map_data.objects), 2)
            
            for i, (orig, read) in enumerate(zip(original_objects, map_data.objects)):
                self.assertEqual(orig.x, read.x, f"Object {i} x mismatch")
                self.assertEqual(orig.y, read.y, f"Object {i} y mismatch")
                self.assertEqual(orig.z, read.z, f"Object {i} z mismatch")
                self.assertEqual(orig.shape, read.shape, f"Object {i} shape mismatch")
                self.assertEqual(orig.frame, read.frame, f"Object {i} frame mismatch")
                self.assertEqual(orig.flags, read.flags, f"Object {i} flags mismatch")
                self.assertEqual(orig.quality, read.quality, f"Object {i} quality mismatch")
                self.assertEqual(orig.npcnum, read.npcnum, f"Object {i} npcnum mismatch")
                self.assertEqual(orig.mapnum, read.mapnum, f"Object {i} mapnum mismatch")
                self.assertEqual(orig.next_id, read.next_id, f"Object {i} next_id mismatch")
        finally:
            os.unlink(filepath)


class TestCoordinateConversion(unittest.TestCase):
    """Tests for coordinate conversion functions."""

    def test_osm_to_u8_coords_origin(self):
        """Test converting origin tile to U8 coords."""
        world_x, world_y, world_z = convert_osm_to_u8_coords(0, 0, 0)
        self.assertEqual(world_x, 0)
        self.assertEqual(world_y, 0)
        self.assertEqual(world_z, 0)

    def test_osm_to_u8_coords_with_lift(self):
        """Test converting tile with lift to U8 coords."""
        world_x, world_y, world_z = convert_osm_to_u8_coords(10, 10, 5)
        # Default tile_size is 256, lift multiplier is 8
        self.assertEqual(world_x, 10 * 256)
        self.assertEqual(world_y, 10 * 256)
        self.assertEqual(world_z, 5 * 8)

    def test_osm_to_u8_coords_clamp(self):
        """Test that coordinates are clamped to max value."""
        # Try to convert very large tile values
        world_x, world_y, world_z = convert_osm_to_u8_coords(1000, 1000, 50)
        self.assertLessEqual(world_x, 65535)
        self.assertLessEqual(world_y, 65535)
        self.assertLessEqual(world_z, 255)

    def test_u8_to_tile_coords(self):
        """Test converting U8 coords back to tiles."""
        tile_x, tile_y, lift = convert_u8_to_tile_coords(2560, 5120, 40)
        self.assertEqual(tile_x, 10)  # 2560 / 256
        self.assertEqual(tile_y, 20)  # 5120 / 256
        self.assertEqual(lift, 5)     # 40 / 8

    def test_coordinate_roundtrip(self):
        """Test coordinate conversion roundtrip."""
        original_tile_x, original_tile_y, original_lift = 50, 100, 3
        
        # Convert to U8
        world_x, world_y, world_z = convert_osm_to_u8_coords(
            original_tile_x, original_tile_y, original_lift
        )
        
        # Convert back
        tile_x, tile_y, lift = convert_u8_to_tile_coords(world_x, world_y, world_z)
        
        self.assertEqual(tile_x, original_tile_x)
        self.assertEqual(tile_y, original_tile_y)
        self.assertEqual(lift, original_lift)


class TestPentragramCompatibility(unittest.TestCase):
    """
    Tests to verify compatibility with Pentagram engine's expected format.
    
    Based on engines/ultima8/world/Map.cpp loadFixedFormatObjects():
    - X: 2 bytes, little-endian
    - Y: 2 bytes, little-endian
    - Z: 1 byte
    - Shape: 2 bytes, little-endian
    - Frame: 1 byte
    - Flags: 2 bytes, little-endian
    - Quality: 2 bytes, little-endian
    - NPC number: 1 byte
    - Map number: 1 byte
    - Next object ID: 2 bytes, little-endian
    """

    def test_field_order(self):
        """Test that fields are in the order expected by Pentagram."""
        obj = U8Object(
            x=0x0001, y=0x0002, z=0x03,
            shape=0x0004, frame=0x05,
            flags=0x0006, quality=0x0007,
            npcnum=0x08, mapnum=0x09, next_id=0x000A
        )
        data = obj.to_bytes()
        
        # Parse each field manually according to Pentagram's order
        self.assertEqual(struct.unpack_from('<H', data, 0)[0], 0x0001)   # X
        self.assertEqual(struct.unpack_from('<H', data, 2)[0], 0x0002)   # Y
        self.assertEqual(struct.unpack_from('<B', data, 4)[0], 0x03)     # Z
        self.assertEqual(struct.unpack_from('<H', data, 5)[0], 0x0004)   # Shape
        self.assertEqual(struct.unpack_from('<B', data, 7)[0], 0x05)     # Frame
        self.assertEqual(struct.unpack_from('<H', data, 8)[0], 0x0006)   # Flags
        self.assertEqual(struct.unpack_from('<H', data, 10)[0], 0x0007)  # Quality
        self.assertEqual(struct.unpack_from('<B', data, 12)[0], 0x08)    # NPC#
        self.assertEqual(struct.unpack_from('<B', data, 13)[0], 0x09)    # Map#
        self.assertEqual(struct.unpack_from('<H', data, 14)[0], 0x000A)  # Next

    def test_little_endian(self):
        """Test that multi-byte fields are little-endian."""
        obj = U8Object(
            x=0x1234, y=0x5678, z=0,
            shape=0xABCD, frame=0,
            flags=0x1111, quality=0x2222,
            npcnum=0, mapnum=0, next_id=0x3333
        )
        data = obj.to_bytes()
        
        # Check little-endian byte order for X (0x1234)
        self.assertEqual(data[0], 0x34)  # Low byte first
        self.assertEqual(data[1], 0x12)  # High byte second
        
        # Check Y (0x5678)
        self.assertEqual(data[2], 0x78)
        self.assertEqual(data[3], 0x56)
        
        # Check shape (0xABCD)
        self.assertEqual(data[5], 0xCD)
        self.assertEqual(data[6], 0xAB)


if __name__ == '__main__':
    unittest.main(verbosity=2)

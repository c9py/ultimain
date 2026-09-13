#!/usr/bin/env python3
"""
Ultima VIII Map Format Handling

This module implements reading and writing of Ultima VIII FIXED.DAT/NONFIXED.DAT
format files. Based on the map format documentation in engines/ultima8/docs/u8mapfmt.txt
and the Pentagram engine implementation.

The U8 fixed format uses 16-byte records:
    Offset  Size  Description
    0       2     X position (world coordinates)
    2       2     Y position (world coordinates)
    4       1     Z position (world coordinates)
    5       2     Shape number (type)
    7       1     Frame number
    8       2     Flags
    10      2     Quality (or glob number for globs)
    12      1     NPC number (for NPCs)
    13      1     Map number
    14      2     ID of next object in list

The file has a header with map count and map info chunks pointing to
each map's data.
"""

import struct
from dataclasses import dataclass
from typing import List, Optional, Tuple, BinaryIO, Dict
import os


# U8 coordinate system constants
# U8 world coordinates are in the range 0-65535 (16-bit)
# Screen conversion formulas from u8mapfmt.txt:
#   ScreenX = (MapX - MapY) / 4
#   ScreenY = (MapX + MapY) / 8 - MapZ
U8_COORD_MAX = 65535
U8_COORD_BITS = 16

# Map file constants
U8_HEADER_SIZE = 128  # Map info starts at offset 128
U8_MAP_COUNT_OFFSET = 84  # Number of maps at offset 84
U8_MAP_INFO_SIZE = 8  # Each map info chunk is 8 bytes
U8_RECORD_SIZE = 16  # Each object record is 16 bytes
U8_DEFAULT_MAP_COUNT = 256  # U8 has 256 maps


@dataclass
class U8Object:
    """Represents an object in Ultima VIII format."""
    x: int  # X position (0-65535)
    y: int  # Y position (0-65535)
    z: int  # Z position (0-255)
    shape: int  # Shape/type number (0-65535)
    frame: int = 0  # Frame number (0-255)
    flags: int = 0  # Flags (16-bit)
    quality: int = 0  # Quality or glob number (16-bit)
    npcnum: int = 0  # NPC number (0-255)
    mapnum: int = 0  # Map number (0-255)
    next_id: int = 0  # Next object ID (usually 0 for exports)

    def to_bytes(self) -> bytes:
        """
        Pack this object into a 16-byte U8 fixed format record.
        
        Returns:
            bytes: 16-byte binary record
        """
        return struct.pack(
            '<HHBHBHHBBH',
            self.x & 0xFFFF,          # 2 bytes: X position
            self.y & 0xFFFF,          # 2 bytes: Y position
            self.z & 0xFF,            # 1 byte:  Z position
            self.shape & 0xFFFF,      # 2 bytes: Shape number
            self.frame & 0xFF,        # 1 byte:  Frame number
            self.flags & 0xFFFF,      # 2 bytes: Flags
            self.quality & 0xFFFF,    # 2 bytes: Quality
            self.npcnum & 0xFF,       # 1 byte:  NPC number
            self.mapnum & 0xFF,       # 1 byte:  Map number
            self.next_id & 0xFFFF     # 2 bytes: Next object ID
        )

    @classmethod
    def from_bytes(cls, data: bytes) -> 'U8Object':
        """
        Unpack a 16-byte U8 fixed format record into a U8Object.
        
        Args:
            data: 16-byte binary record
            
        Returns:
            U8Object instance
        """
        if len(data) != U8_RECORD_SIZE:
            raise ValueError(f"U8 record must be exactly {U8_RECORD_SIZE} bytes, got {len(data)}")
        
        x, y, z, shape, frame, flags, quality, npcnum, mapnum, next_id = struct.unpack(
            '<HHBHBHHBBH', data
        )
        
        return cls(
            x=x,
            y=y,
            z=z,
            shape=shape,
            frame=frame,
            flags=flags,
            quality=quality,
            npcnum=npcnum,
            mapnum=mapnum,
            next_id=next_id
        )

    def __repr__(self) -> str:
        return (
            f"U8Object(x={self.x}, y={self.y}, z={self.z}, "
            f"shape={self.shape}, frame={self.frame}, flags=0x{self.flags:04x}, "
            f"quality={self.quality}, npcnum={self.npcnum}, mapnum={self.mapnum})"
        )


@dataclass
class U8MapData:
    """Represents a single map's object data."""
    mapnum: int
    objects: List[U8Object]

    def to_bytes(self) -> bytes:
        """Serialize all objects in this map to binary."""
        return b''.join(obj.to_bytes() for obj in self.objects)

    @classmethod
    def from_bytes(cls, mapnum: int, data: bytes) -> 'U8MapData':
        """
        Parse map data from binary.
        
        Args:
            mapnum: Map number
            data: Binary data containing objects
            
        Returns:
            U8MapData instance
        """
        objects = []
        offset = 0
        while offset + U8_RECORD_SIZE <= len(data):
            obj = U8Object.from_bytes(data[offset:offset + U8_RECORD_SIZE])
            objects.append(obj)
            offset += U8_RECORD_SIZE
        return cls(mapnum=mapnum, objects=objects)


class U8FixedDatWriter:
    """
    Writes U8 FIXED.DAT or NONFIXED.DAT format files.
    
    The file structure is:
    - Header chunk at offset 0-127 (contains map count at offset 84)
    - Map info chunks starting at offset 128 (8 bytes per map)
    - Map data following the map info section
    """

    def __init__(self, map_count: int = U8_DEFAULT_MAP_COUNT):
        """
        Initialize writer.
        
        Args:
            map_count: Number of maps (default 256 for U8)
        """
        self.map_count = map_count
        self.maps: Dict[int, U8MapData] = {}

    def add_map(self, mapnum: int, objects: List[U8Object]) -> None:
        """
        Add or replace objects for a specific map.
        
        Args:
            mapnum: Map number (0-255)
            objects: List of U8Object instances
        """
        if not 0 <= mapnum < self.map_count:
            raise ValueError(f"Map number must be 0-{self.map_count-1}, got {mapnum}")
        self.maps[mapnum] = U8MapData(mapnum=mapnum, objects=objects)

    def write(self, filepath: str) -> None:
        """
        Write the complete FIXED.DAT or NONFIXED.DAT file.
        
        Args:
            filepath: Output file path
        """
        with open(filepath, 'wb') as f:
            self._write_to_stream(f)

    def _write_to_stream(self, f: BinaryIO) -> None:
        """Write the complete file to a stream."""
        # Pre-calculate all map data
        map_data_list = []
        for mapnum in range(self.map_count):
            if mapnum in self.maps and self.maps[mapnum].objects:
                data = self.maps[mapnum].to_bytes()
            else:
                data = b''
            map_data_list.append(data)

        # Calculate positions
        map_info_start = U8_HEADER_SIZE
        map_info_size = self.map_count * U8_MAP_INFO_SIZE
        data_start = map_info_start + map_info_size

        # Calculate map positions
        map_positions = []
        current_pos = data_start
        for data in map_data_list:
            map_positions.append(current_pos)
            current_pos += len(data)

        # Write header (128 bytes, mostly zeros)
        header = bytearray(U8_HEADER_SIZE)
        # Map count at offset 84 (2 bytes, little-endian)
        struct.pack_into('<H', header, U8_MAP_COUNT_OFFSET, self.map_count)
        f.write(header)

        # Write map info chunks (8 bytes each: 4 bytes position, 4 bytes size)
        for i, data in enumerate(map_data_list):
            pos = map_positions[i]
            size = len(data)
            f.write(struct.pack('<II', pos, size))

        # Write map data
        for data in map_data_list:
            f.write(data)


class U8FixedDatReader:
    """
    Reads U8 FIXED.DAT or NONFIXED.DAT format files.
    """

    def __init__(self, filepath: str):
        """
        Initialize reader with a file path.
        
        Args:
            filepath: Path to FIXED.DAT or NONFIXED.DAT
        """
        self.filepath = filepath
        self.map_count = 0
        self.map_infos: List[Tuple[int, int]] = []  # (position, size)
        self._read_header()

    def _read_header(self) -> None:
        """Read and parse the file header."""
        with open(self.filepath, 'rb') as f:
            # Read header
            header = f.read(U8_HEADER_SIZE)
            if len(header) < U8_HEADER_SIZE:
                raise ValueError("File too small for U8 fixed format header")

            # Get map count from offset 84
            self.map_count = struct.unpack_from('<H', header, U8_MAP_COUNT_OFFSET)[0]

            # Read map info chunks
            for i in range(self.map_count):
                info_data = f.read(U8_MAP_INFO_SIZE)
                if len(info_data) < U8_MAP_INFO_SIZE:
                    raise ValueError(f"Unexpected end of file reading map info {i}")
                pos, size = struct.unpack('<II', info_data)
                self.map_infos.append((pos, size))

    def read_map(self, mapnum: int) -> U8MapData:
        """
        Read objects for a specific map.
        
        Args:
            mapnum: Map number (0 to map_count-1)
            
        Returns:
            U8MapData instance with all objects for that map
        """
        if not 0 <= mapnum < self.map_count:
            raise ValueError(f"Map number must be 0-{self.map_count-1}, got {mapnum}")

        pos, size = self.map_infos[mapnum]
        if size == 0:
            return U8MapData(mapnum=mapnum, objects=[])

        with open(self.filepath, 'rb') as f:
            f.seek(pos)
            data = f.read(size)
            if len(data) < size:
                raise ValueError(f"Unexpected end of file reading map {mapnum}")
            return U8MapData.from_bytes(mapnum, data)

    def read_all_maps(self) -> Dict[int, U8MapData]:
        """
        Read all maps from the file.
        
        Returns:
            Dictionary mapping map number to U8MapData
        """
        maps = {}
        for mapnum in range(self.map_count):
            map_data = self.read_map(mapnum)
            if map_data.objects:  # Only include non-empty maps
                maps[mapnum] = map_data
        return maps


def convert_osm_to_u8_coords(
    tile_x: int, tile_y: int,
    tile_lift: int = 0,
    tile_size: int = 256,  # U8 world coords per tile
    world_max: int = U8_COORD_MAX
) -> Tuple[int, int, int]:
    """
    Convert OSM-derived tile coordinates to U8 world coordinates.
    
    U8 uses a world coordinate system ranging from 0-65535. This function
    maps tile coordinates (typically from OSM conversion) into that range.
    
    Args:
        tile_x: Tile X coordinate
        tile_y: Tile Y coordinate
        tile_lift: Height/lift level (0-based)
        tile_size: World units per tile (default 256 to fit 256 tiles in 65536 range)
        world_max: Maximum world coordinate value
        
    Returns:
        Tuple of (world_x, world_y, world_z)
    """
    world_x = min(tile_x * tile_size, world_max)
    world_y = min(tile_y * tile_size, world_max)
    world_z = tile_lift * 8  # Each lift level is typically 8 units
    return (world_x, world_y, min(world_z, 255))


def convert_u8_to_tile_coords(
    world_x: int, world_y: int, world_z: int,
    tile_size: int = 256
) -> Tuple[int, int, int]:
    """
    Convert U8 world coordinates back to tile coordinates.
    
    Args:
        world_x: U8 world X coordinate
        world_y: U8 world Y coordinate
        world_z: U8 world Z coordinate
        tile_size: World units per tile
        
    Returns:
        Tuple of (tile_x, tile_y, lift)
    """
    tile_x = world_x // tile_size
    tile_y = world_y // tile_size
    lift = world_z // 8
    return (tile_x, tile_y, lift)


if __name__ == "__main__":
    # Quick test of the format handling
    print("=== U8 Format Test ===\n")

    # Test object creation and serialization
    obj = U8Object(
        x=16384, y=16384, z=0,
        shape=301, frame=1,
        flags=0, quality=0,
        npcnum=0, mapnum=0
    )
    print(f"Created object: {obj}")

    # Test serialization
    data = obj.to_bytes()
    print(f"Serialized to {len(data)} bytes: {data.hex()}")

    # Test deserialization
    obj2 = U8Object.from_bytes(data)
    print(f"Deserialized object: {obj2}")

    # Verify round-trip
    assert obj.x == obj2.x
    assert obj.y == obj2.y
    assert obj.z == obj2.z
    assert obj.shape == obj2.shape
    assert obj.frame == obj2.frame
    print("\nRound-trip verification: PASSED")

    # Test coordinate conversion
    tile_x, tile_y, lift = 100, 100, 2
    world_x, world_y, world_z = convert_osm_to_u8_coords(tile_x, tile_y, lift)
    print(f"\nCoordinate conversion: tile ({tile_x}, {tile_y}, lift={lift}) -> world ({world_x}, {world_y}, {world_z})")

    # Convert back
    t_x, t_y, t_l = convert_u8_to_tile_coords(world_x, world_y, world_z)
    print(f"Reverse conversion: world ({world_x}, {world_y}, {world_z}) -> tile ({t_x}, {t_y}, lift={t_l})")

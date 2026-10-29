#!/usr/bin/env python3
"""Index Ultima VIII shapes and the world coordinates where each one stands.

Reads reference/u8_static (FIXED.DAT, GLOB.FLX, TYPEFLAG.DAT). Glob eggs
(typeflag family 3) are expanded the way GlobEgg::enterFastArea does:
world x = (egg_x & ~0x1FF) + (glob_x << 1) + 1, and the same for y.
Items nested in a container are not world positions; their x is depth.
"""

import argparse
import struct
from collections import defaultdict
from pathlib import Path

FAMILIES = {
    0: "generic",
    1: "quality",
    2: "quantity",
    3: "glob",
    4: "egg",
    5: "breakable",
    6: "container",
    7: "monster_egg",
    8: "teleport_egg",
    9: "reagent",
    15: "unknown",
}

# Names from engines/ultima8/docs/maps.txt (Pentology).
MAP_NAMES = {
    3: "Docks",
    4: "Fisherman's Reef",
    5: "East Road",
    6: "Cemetery",
    7: "Herdsman's Valley",
    8: "Plateau",
    12: "Daemon's Crag",
    14: "Obsidian Fortress",
    15: "Slayer",
    16: "Carthax Lake",
    21: "Stone Cove",
    24: "Lava Tunnel",
    25: "Lava Tunnel (after Carthax)",
    26: "Carthax Lake (full)",
    28: "Carthax Breaking Ground",
    31: "Hall of the Mountain King",
    37: "Argentrock Isle",
    39: "West Tenebrae",
    40: "Central Tenebrae",
    41: "East Tenebrae",
    43: "Ethereal Plane",
    44: "Plane of Fire",
    45: "Plane of Water",
    46: "Plane of Air",
    47: "Plane of Earth",
    49: "Upper Catacombs II",
    50: "Upper Catacombs",
    51: "Lower Catacombs",
    54: "Shrine of the Ancient Ones",
    57: "Pit of the Dead",
    62: "Ancient Necromancers",
    63: "Ghost",
}


def flex_index(data):
    count = struct.unpack_from("<I", data, 0x54)[0]
    return [struct.unpack_from("<II", data, 0x80 + 8 * i) for i in range(count)]


def load_shapes(path):
    raw = Path(path).read_bytes()
    shapes = []
    for i in range(len(raw) // 8):
        b0, b1, b2, b3, b4, b5, b6, b7 = raw[i * 8:(i + 1) * 8]
        flags = b0 | ((b1 & 0x0F) << 8) | ((b5 & 0xF0) << 8)
        shapes.append({
            "family": (b1 >> 4) & 0xF,
            "foot_x": (b2 >> 4) & 0xF,
            "foot_y": b3 & 0xF,
            "foot_z": (b3 >> 4) & 0xF,
            "editor": bool(flags & 0x1000),
        })
    return shapes


def load_globs(path):
    data = Path(path).read_bytes()
    globs = []
    for off, size in flex_index(data):
        items = []
        if size >= 2 and off + size <= len(data):
            count = struct.unpack_from("<H", data, off)[0]
            pos = off + 2
            for _ in range(count):
                if pos + 6 > off + size:
                    break
                gx, gy, gz, shape, frame = struct.unpack_from("<BBBHB", data, pos)
                items.append((gx, gy, gz, shape, frame))
                pos += 6
        globs.append(items)
    return globs


def placements(fixed_path, shapes, globs):
    data = Path(fixed_path).read_bytes()
    world = []
    contained = []
    for map_num, (off, size) in enumerate(flex_index(data)):
        if size < 16 or off + size > len(data):
            continue
        depth = 0
        containers = []
        pos = off
        for _ in range(size // 16):
            x, y, z, shape, frame, flags, quality, _npc, _map, _next = struct.unpack_from(
                "<HHBHBHHBBH", data, pos)
            pos += 16
            while depth != x and depth > 0:
                containers.pop()
                depth -= 1
            info = shapes[shape] if shape < len(shapes) else None
            if depth > 0:
                parent = containers[-1] if containers else 0
                contained.append((shape, map_num, x, y, z, frame, parent))
            else:
                family = info["family"] if info else -1
                if family == 3 and quality < len(globs) and globs[quality]:
                    base_x = x & ~0x1FF
                    base_y = y & ~0x1FF
                    for gx, gy, gz, gshape, gframe in globs[quality]:
                        world.append((
                            gshape, map_num,
                            base_x + (gx << 1) + 1,
                            base_y + (gy << 1) + 1,
                            z + gz, gframe,
                        ))
                else:
                    world.append((shape, map_num, x, y, z, frame))
            if info and info["family"] == 6:
                containers.append(shape)
                depth += 1
    return world, contained


def write_index(root, out_dir):
    shapes = load_shapes(root / "TYPEFLAG.DAT")
    globs = load_globs(root / "GLOB.FLX")
    world, contained = placements(root / "FIXED.DAT", shapes, globs)

    by_shape = defaultdict(list)
    for row in world:
        by_shape[row[0]].append(row)

    out_dir.mkdir(parents=True, exist_ok=True)
    shapes_path = out_dir / "shapes.tsv"
    coords_path = out_dir / "shape_coordinates.tsv"
    contained_path = out_dir / "contained.tsv"

    with shapes_path.open("w", encoding="utf-8") as fh:
        fh.write("shape\tfamily\tfamily_name\tfoot_x\tfoot_y\tfoot_z\teditor\tplacements\tmaps\n")
        for shape, info in enumerate(shapes):
            rows = by_shape.get(shape, [])
            maps = sorted({row[1] for row in rows})
            fh.write(
                f"{shape}\t{info['family']}\t{FAMILIES.get(info['family'], 'other')}\t"
                f"{info['foot_x']}\t{info['foot_y']}\t{info['foot_z']}\t"
                f"{int(info['editor'])}\t{len(rows)}\t"
                + ",".join(str(m) for m in maps) + "\n"
            )

    with coords_path.open("w", encoding="utf-8") as fh:
        fh.write("shape\tmap\tmap_name\tx\ty\tz\tframe\n")
        for shape in sorted(by_shape):
            for gshape, map_num, x, y, z, frame in by_shape[shape]:
                fh.write(
                    f"{gshape}\t{map_num}\t{MAP_NAMES.get(map_num, '')}\t{x}\t{y}\t{z}\t{frame}\n"
                )

    with contained_path.open("w", encoding="utf-8") as fh:
        fh.write("shape\tmap\tmap_name\tdepth\ty\tz\tframe\tcontainer_shape\n")
        for shape, map_num, depth, y, z, frame, parent in contained:
            fh.write(
                f"{shape}\t{map_num}\t{MAP_NAMES.get(map_num, '')}\t{depth}\t{y}\t{z}\t{frame}\t{parent}\n"
            )

    placed = sum(1 for info_rows in by_shape.values() if info_rows)
    print(f"shapes {len(shapes)}")
    print(f"placed shapes {placed}")
    print(f"world placements {len(world)}")
    print(f"contained {len(contained)}")
    print(f"wrote {shapes_path}")
    print(f"wrote {coords_path}")
    print(f"wrote {contained_path}")


def main():
    here = Path(__file__).resolve().parent
    default_root = here.parents[1] / "reference" / "u8_static"
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--static", type=Path, default=default_root)
    parser.add_argument("--out", type=Path, default=here / "out")
    args = parser.parse_args()
    write_index(args.static, args.out)


if __name__ == "__main__":
    main()

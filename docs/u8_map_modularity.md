# U8 Map Modularity

Headless checks in `tools/osm2ultima` compare an OSM fixture with a procedural stamp on Ultima VIII's item map. The checks copy engine literals into Python. They do not link Pentagram, and Pentagram was not executed.

## Shared seam

Both sources emit the same placed-object list. Each object carries a world position, a top `z`, and a terrain class: land, path, water, or solid. A synthetic walker reads that list. Land and path support one frame when the square's top `z` matches the walker. Water blocks. A solid supports the step when its top `z` equals the walker, and blocks the step when the destination overlaps the solid. The 16-byte record flags field stays 0 and is not the collision source. Agreement on those stored classes does not show that the two sources assign classes from OSM tags in the same way.

## Engine-specific seam

Ultima VIII stores items in world coordinates, not a terrain grid. Screen position is `(x - y) / 4` and `(x + y) / 8 - z`, with signed division that truncates toward zero. Chunks are 512 units on a side in a 64 by 64 grid, so a playfield coordinate satisfies `0 <= coord < 32768`. One walk frame uses `deltadir = 1` and the eight direction factors from `engines/ultima8/misc/Direction.h`, which moves a cardinal step by 4 world units. Exported `FIXED.DAT` and `NONFIXED.DAT` are Flex archives: `0x1A` through the first `0x52` bytes, a uint32 map-slot count at `0x54`, and an index at `0x80` whose payloads are raw 16-byte records.

## OSM adapter

`osm_to_placed` runs the existing `U8MapGenerator` on an in-memory element list and copies each fixed object into the placed-object list. The terrain class comes from the way tags (highway, waterway, building, or other land). Way and building sample steps are unchanged. A way that cites a missing node id is skipped. Shape numbers written into the DAT records are placeholders from `u8_shape_mapping.py`. Those numbers are not collision.

## Procedural adapter

`procedural_stamp` fills a small grid from the origin `(0, 0)` at 128-unit spacing and emits the same placed-object type. The walker can be pointed at that list on its own. The stamp does not resample OSM roads or buildings onto the 128 grid.

## Pentagram load blockers

Pentagram was not executed, and this evaluation does not show that Pentagram loaded a generated map. A load still needs real `fixed.dat`, `typeflag.dat`, and `u8shapes.flx`, which are not in this tree. Placeholder shape ids have not been checked against `typeflag.dat`. U8 globs are not emitted. Object flags are not collision. OSM building and road geometry is not resampled onto the 128-unit squares the walker uses.

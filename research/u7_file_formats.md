# Ultima VII internal formats - The Codex of Ultima Wisdom, a wiki for Ultima and Ultima Online

**URL:** https://wiki.ultimacodex.com/wiki/Ultima_VII_internal_formats

---

Ultima VII internal formats
Jump to navigation
Jump to search

This page documents the Ultima VII engine's internal formats.

Contents
1	Data Types and Structure
2	Files
3	Common File Formats
3.1	FLX Archives (.flx extension)
3.2	Shapes (.shp files and others)
3.3	ShapeFrameIndex
4	File Formats
4.1	Game Interface
4.1.1	NPC faces (static/faces.vga)
4.1.2	Fonts (static/fonts.vga)
4.1.3	Container and UI images (static/gumps.vga)
4.1.4	Mouse cursors (static/pointers.shp)
4.1.5	Magic effects and miscellaneous graphics (static/sprites.vga)
4.2	World Shapes (static/shapes.vga, static/shpdims.dat, static/wgtvol.dat, static/occlude.dat)
4.2.1	Graphics Archive (static/shapes.vga)
4.2.2	Dimensions (static/shpdims.dat)
4.2.3	Shape Weight/Volume (static/wgtvol.dat)
4.2.4	Shape Flags (static/tfa.dat)
4.2.5	Shape Occlusion Bit (static/occlude.dat)
4.3	Palettes
4.3.1	Translucency FLX Archive (static/xform.tbl)
4.4	World Data (static/u7chunks, static/u7map)
4.4.1	Chunk Templates (static/u7chunks)
4.4.2	Static Chunk Data (static/u7map)
4.4.3	Fixed Items Data (static/u7ifix??)
5	See Also
6	References
Data Types and Structure[edit]

Decimal values are given as 123; hexadecimal values are given as 1Fh. All files use little-endian coding. 1122h, for example, is stored as the byte sequence (22h 11h). Signed integers are stored in two's-complement arithmetic. These types are used throughout this document:

char - An ASCII character stored as a uint8.
uint8/uint16/uint24/uint32 - Unsigned one, two, three, and four-byte integers.
int8/int16/int32 - Signed one, two, and four-byte integers.
Files[edit]

This section lists the files in an Ultima VII installation and discusses what is known about them. Further details are found in the File Formats section below. "#" means a single digit (0-9); "*" means any number of any character; "?" means any single character.

*.exe - Game executables.
game##.u7 - Savegame files.
static/ - Contains unchanging game data.
static/*.adv - Sound device drivers.
static/*mus.dat, static/*sfx.dat - Music and sound effects information for the sound device drivers.
static/ammo.dat
static/armor.dat
static/endgame.dat
static/endscore.xmi
static/endshape.flx
static/equip.dat
static/faces.vga - A Shapes Flx archive containing NPC faces.
static/fonts.vga - A Shapes Flx archive containing the fonts.
static/gumps.vga - A Shapes Flx archive containing container and UI images.
static/initgame.dat
static/introadm.dat
static/intropal.dat
static/intrordm.dat
static/introsnd.dat
static/linkdep# (1 or 2)
static/mainshp.flx
static/monsters.dat
static/npcflag.dat
static/occlude.dat - Occlusion bit for world graphics. See the World Shapes section.
static/palettes.flx - Main game palettes. See the Palettes section.
static/paperdol.vga (SI only)
static/pointers.shp - Mouse cursors in a Shape file; see the Interface section.
static/ready.dat
static/schedule.dat
static/shapes.vga - Contains the world graphics, both the ground tiles and the movable shapes. See the World Shapes section below.
static/shpdims.dat - Appears to contain dimensions related to shapes; see the World Shapes section.
static/sprites.vga - A Shape archive containing magic effects and maps; see the Interface section.
static/text.flx
static/tfa.dat - Contains flags and data for the world shapes; see the World Shapes section.
static/u7chunks - Chunk templates; see the World Data section.
static/u7ifix?? (where ?? are hexadecimal digits) - Contains unmovable objects; see the World Data section.
static/u7intro.tim
static/u7map - Chunk template map; see the World Data section.
static/u7speech.spc
static/u7strax.drv
static/u7voice.flx
static/usecode
static/weapons.dat
static/wgtvol.dat - Contains weight and volume information for shapes; see the World Shapes section.
static/wihh.dat
static/xform.tbl - Tables for the translucent colours in the palette; see the Palettes section.
static/xmidi.ad
static/xmidi.mt
gamedat/ - Contains temporary data used during the running of a game.
gamedat/action.dat
gamedat/av_flags.dat
gamedat/flaginit
gamedat/frames.flg
gamedat/gametim?.dat (where ? is a-m)
gamedat/identity
gamedat/itemnode.dat
gamedat/mapcoord.dat
gamedat/misstrac.dat
gamedat/party
gamedat/path.dat
gamedat/path0.dat
gamedat/randseed.dat
gamedat/u7_ostia.dat
gamedat/u7ibuf.dat
gamedat/u7ireg?? (where ?? are hexadecimal digits)
gamedat/u7nbuf.dat
gamedat/weather.dat
Common File Formats[edit]

This section documents file formats that are shared among many different files.

FLX Archives (.flx extension)[edit]

This is an archive file used extensively throughout the game. Each record has the same type, which depends upon the file itself.

Offset	Length	Format	Name	Description
00h	80	char[80]	Comment	An optional comment that can inform the reader about the file. It's not used by the game.
50h	4	uint32	??	Always FFFF1A00h.
54h	4	uint32	Count	The number of records in the file.
58h	4	uint32	??	Always 0xCC.
5Ch	36	uint32[9]	??	Mostly zeroes.
80h	8*Count	Record[Count]	Records	Offsets and lengths of each record in the archive, described below.
This is the Record type:
00h	4	uint32	Offset	Offset of the start of the record data, from the start of the file. If this is zero, there is no record in this slot.
04h	4	uint32	Length	Length in bytes of the record data.
Shapes (.shp files and others)[edit]

This describes a graphics image. It is composed of a set of frames, with this header:

Offset	Length	Format	Name	Description
00h	4	uint32	Size	Size in bytes of the shape file.
04h	4*Count	uint32[Count]	Offsets	Offset of the ShapeFrames relative to the start of the shape. Count is ((Offsets[0] - 4) / 4). So to read the offsets, read the first offset, find out how many items are in the array, and then read the rest.
At each Offset is the ShapeFrame:
00h	2	uint16	MaxX	The maximum X value for any span, inclusive.
02h	2	uint16	OffsetX	The maximum negative X value for any span. So if this is 80, then no span starts at less than -80 X.
04h	2	uint16	OffsetY	The maximum negative Y value for any span. So if this is 40, then no span starts at less than -40 Y.
06h	2	uint16	MaxY	The maximum Y value for any span, inclusive.
08h	X	Spans[...]	Spans	The list of spans, terminated by a Span with a Data field of zero.
Each Span has this format:
00h	2	uint16	BlockData	The type and length of the span. BlockType is (BlockData & 1); BlockLength is (BlockData >> 1). If BlockData zero, then the frame is finished.
02h	2	int16	XStart	The horizontal start of the span. This can be negative, but is never less than -OffsetX.
04h	2	int16	YStart	The vertical start of the span. This can be negative, but is never less than -OffsetY.
If (Data & 1) is 0, then the span data has this uncompressed format:
06h	Length	uint8[Length]	SpanData	Raw pixel data.
If (Data & 1) is 1, then the span data is a run length encoded sequence that runs for as many blocks as needed to consume BlockLength pixels. Each RLE block has this format:
00h	1	uint8	RunData	RunType is (RunData & 1); RunLength is (RunData >> 1).
01h	X	X	RunContent	If RunType is 0, then this is (uint8[RunLength]) raw pixels. Otherwise this is (uint8 Value), where Value is written out for RunLength pixels.

The total width and height of a frame are (MaxX + OffsetX + 1) and (MaxY + OffsetY + 1) respectively. Here is pseudocode for decoding the RLE data:

while(true) {
    int BlockData = ReadUInt16();
    int BlockLength = BlockData >> 1;
    int BlockType = BlockData & 1;

    if(BlockData == 0)
        break;
    int OffsetX = ReadInt16();
    int OffsetY = ReadInt16();

    if(BlockType == 0)
        ReadImage(OffsetX, OffsetY, BlockLength);
    else {
        int EndX = OffsetX + BlockLength;

        while(OffsetX < EndX) {
            int RunData = ReadUInt8();
            int RunLength = RunData >> 1;
            int RunType = RunData & 1;

            if(RunType == 0)
                ReadImage(OffsetX, OffsetY, RunLength);
            else {
                var Value = ReadUInt8();
                ReadImageRun(OffsetX, OffsetY, RunLength, Value);
            }

            OffsetX += RunLength;
        }
    }
}
ShapeFrameIndex[edit]

This refers to both a shape indexing the archive from the World Shapes section, and a frame within them.

Shift	Bits	Mask	Name	Description
0	10	1023	ShapeIndex	An index into the "static/shapes.vga" archive. Values below 150/96h are ground tiles; otherwise they are static geometry.
10	5	31	FrameIndex	Index of the frame in the shape to use. If the shape is animated (such as shoreline) this is the first frame to use.
15	1	1	??	??
File Formats[edit]

This section documents the file formats for specific files.

Game Interface[edit]

This section covers files that deal with the game interface, such as the UI and cursors.

NPC faces (static/faces.vga)[edit]

This Shapes Flx archive contains the NPC faces. Most shapes have a single frame, unless if the NPC has multiple states (like the Avatar being male or female).

Fonts (static/fonts.vga)[edit]

This Shapes Flx archive contains the fonts. Each font contains 128 graphics for the ASCII character set, where empty characters are one pixel wide. Some fonts are "ye olde script", where Latin letters are in the capitals and Runic are in lowercase, with some punctuation used for multi-letter characters.

The fonts are:

Index	Description
0	Conversation font in yellow.
1	Carved wood ye olde script.
2	System font in black.
3	Chiselled ye olde script.
4	Book font in black.
5	Book font in white.
6	Gold chiselled ye old script.
8	Conversation font in yellow.
Container and UI images (static/gumps.vga)[edit]

This Shapes Flx archive contains container images and UI graphics. They are:

Index	Description
0	Donation box gump.
1	Crate gump.
2	Check and check depressed images.
3	Save game window; each entry has "DISK FULL - USE ANOTHER SLOT" written on it.
4	Save game line backgrounds. First image is for the line, second is for editing it.
5	Load button and load button pushed images.
6	Save button and save button pushed images.
7	Halo off and halo on images.
8	Barrel gump.
9	Bag gump.
10	Backpack gump.
11	Basket gump.
12	Tactics icons - Attack nearest, attack weakest, attack strongest, attack everyone, protect ally, defend, stay at range, run away, do something with everyone, be very confused.
13	Empty.
14	Numeric input window.
15	Numeric input slider button.
16	Numeric input right red and green arrows.
17	Numeric input left red and green arrows.
18-21	Down, up, right, and left arrows on a white background.
22	Chest gump.
23	Monkey paperdoll.
24	Save game button, and pressed.
25	Heart button up and down.
26	Ship's hold gump.
27	Dresser drawer gump.
28	Status icons - sleeping, poisoned, confused, hungry, shield, black dot, snow angel.
29	Music disabled/enabled buttons.
30	Speech disabled/enabled buttons.
31	Sound effects disabled/enabled buttons.
32	Open book window.
33	Awaken, create food, destroy trap, curse, conjure, charm, cause fear, create gold, and Armageddon icons.
34	Weather, cure, enchant, heal, lightning, dance, clone, death bolt, and death vortex icons.
35	Douse, detect trap, fire blast, swarm, mark, dispel field, fire ring, delayed blast, and mass death icons.
36	Fireworks, great douse, great light, protect all, mass curse, explosion, flame strike, energy field, and invisible all icons.
37	Gimmer, great ignite, mass cure, paralyze, recall, great heal, magic storm, energy mist, and resurrect icons.
38	Help, light, protection, peer, reveal, invisibility, poison field, mass charm, and summon icons.
39	Ignite, locate, telekinesis, poison, seance, fire field, sleep field, mass might, and swordstrike icons.
40	Thunder, awaken all, wizard eye, sleep, unlock magic, mass sleep, tremor, restoration, and time stop icons.
41	Page flipping animation.
42	Descending bookmark animation.
43	Spellbook gump.
44	Spellbook turn page left icon.
45	Spellbook turn page right icon.
46	Dove and flaming sword icons.
47	Character information window.
48	Tiny indistinguishable brown lines.
49	Wooden sign window.
50	Gravestone window.
51	Gold plaque window.
52	"Do you want to exit to DOS?", "Are you sure you want to load without saving first?" and "Are you sure you want to save over this old game?" text blocks.
53	Generic dead body gump.
54	Null record.
55	Scroll window.
56	Quit up and down buttons.
57	Male avatar equipment window.
58	Female avatar equipment window.
59	Iolo equipment window.
60	Spark equipment window.
61	Shamino equipment window.
62	Dupre equipment window.
63	Jaana equipment window.
64	Null record.
65	Sentri equipment window.
66	Julia equipment window.
67	Katrina equipment window.
68	Tseramed equipment window.
69	Confirmation window background.
70	Confirmation window yes up and down buttons.
71	Confirmation window no up and down buttons.
72-74	Null records.
Mouse cursors (static/pointers.shp)[edit]

This Shape file contains the mouse cursors. The frames are:

Index	Description
0	Finger pointer
1	Red cross
2	Use target (four green arrows pointing inwards)
3	"Too heavy"
4	"Out of range"
5	"Out of ammo"
6	"Won't fit"
7	Hourglass
8-15	Short move arrow north, northeast, east, southeast, south, southwest, west, northwest.
16-23	Medium move arrow north, etc.
24-31	Long move arrow north, etc.
32-39	Short combat arrow north, etc.
40-47	Long combat arrow north, etc.
48	Empty, unused.
49	"Blocked"
Magic effects and miscellaneous graphics (static/sprites.vga)[edit]

This Shape Flx Archive contains magic effects and oddities. These are the shapes in the archive:

Index	Description
0	A variety of magic sparkles
1	Huge fireball animation
2	Cloud shadows
3	Smoke animation
4	Medium fireball animation
5	Small fireball animation
6	Eight frames of nothing
7	Sparkles and expanding rings animation
8	Spinning blue death cloud
9	Puff with green sparkles
10	Crystal ball overlay
11	Blood splatter
12	Fireworks
13	Green balls of spinning light
14	Gray squib
15	Multicoloured comets spinning around
16	Mini fireworks
17	Lightning bolt
18	Expanding ring of blue balls
19	Expanding ring of crossbow bolts
20	Null record
21	Purple balls of spinning light
22	Map of Britannia
23	Spinning swords
24	Rising notes
25-29	Null record




World Shapes (static/shapes.vga, static/shpdims.dat, static/wgtvol.dat, static/occlude.dat)[edit]

The world shapes describe the graphics and information about all the objects in the world, excluding some special effects.

"static/shapes.vga" contains the graphical data. "static/shpdims.dat" includes data that are not well understood. "static/wgtvol.dat" includes weight and volume information. "static/tfa.dat" contains flags and type information about shapes.

Graphics Archive (static/shapes.vga)[edit]

This FLX archive file contains the world graphics. From index 0 to 149/95h (inclusive) it contains 8x8 tiles used for the ground, up to 32 per record. From index 150/96h and up it contains Shape records, documented above.

Dimensions (static/shpdims.dat)[edit]

This contains dimensional data for shapes (only shapes index 150/96h and up). It contains a set of (uint8 Vertical, uint8 Horizontal), with this bit format:

Value	Shift	Bits	Mask	Name	Description
Vertical	0	1	1	BlockVertical?	Blocks north-south travel.
Vertical	1	7	127	SizeY?	Vertical size of some sort.
Horizontal	0	1	1	BlockHorizontal?	Blocks east-west ravel.
Horizontal	1	7	127	SizeX?	Horizontal size of some sort.

This file is extremely poorly understood. None of its attributes make particular sense.

Shape Weight/Volume (static/wgtvol.dat)[edit]

This contains weight and volume data for all shapes. It contains a set of (uint8 Weight, uint8 Volume), where Weight is in units of 0.1 stone.

Shape Flags (static/tfa.dat)[edit]

This contains types and flags information for all shapes. It contains a set of (uint24 Bitfield), described below:

Shift	Bits	Mask	Name	Description
0	1	1	??	Exult suggests this is HasSoundEffect, but this includes stalactites and other oddities.
1	1	1	Rotatable	Creature that can be rotated from vertical to horizontal to use the same frames, just rotated (sick guy on bed, sea monster, snake).
2	1	1	Animated	Animate any object using this shape using the frames of the shape.
3	1	1	NotWalkable	Cannot be walked upon, such as a void or person.
4	1	1	Water	Can be swum through.
5	3	7	TileSizeZ	Height of the shape in tiles.
8	4	15	ShapeType	The type of the shape; see below.
12	1	1	Trap	Walking into this causes a trap effect - poison, sleep, fire, etc.
13	1	1	Door	If set, this is a door.
14	1	1	VehiclePart	This tile is part of a vehicle, such as a ship, barge, horse and carriage, and flying carpet.
15	1	1	NotSelectable	This cannot be selected, used for light glows and blood splatters.
16	3	7	TileSizeXMinus1	The horizontal width in tiles of the shape minus 1.
19	3	7	TileSizeYMinus1	The vertical height in tiles of the shape minus 1.
22	1	1	LightSource	If set, this is a light source.
23	1	1	Translucent	If set, frames of this shape use translucent palette indices.

These are the shape type values:

Value	Name	Description
0	Scenery	Unusable scenery objects - trees, ground, etc.
1	Unused	Not used by any object (SI not checked).
2	UsesQualiy	Objects use the quality field.
3	UsesQuantity	Objects use the quantity field.
4	Usable	Exult documentation suggests these items are breakable, but it includes looms, paintings, and other items that suggest more that it's for usable objects.
5	QualityFlags	The object's quality field is a set of flags.
6	Container	Corpses, chess, and desks.
7	ActivateOnApproach	Activates on approach - game eggs and sound triggers, blue and red gates.
8	SpellBook	Only set for the spellbook.
9	Barge	Exult documentation says barge, but this entry (shape 961) is empty.
10	Unused	Not used by any objects (SI not checked).
11	VirtueStone	Used only on the virtue stones (shape 330).
12	Enemies	Enemies that cannot be talked to but can only be fought.
13	Friendlies	Creatures that you might be able to talk to.
14	Buildings	Some building pieces, walls, carpets, and mountain covers.
15	Unused	Not used by any objects (SI not checked).
Shape Occlusion Bit (static/occlude.dat)[edit]

This file contains a bit that can be used to solve some bugs in object sorting. There is one bit for each world shape in ascending order (so the first shape is mask 1, the second shape is mask 2, and so on). This seems to largely be set for walls and ceilings.

Palettes[edit]

This FLX archive file contains the palettes used throughout the game. Each palette has the format (Color[256] Colors), where Color is (uint8 Red, uint8 Green, uint8 Blue), and each component is between 0 and 63. There are 12 palettes in use. Color 255 is transparent.

When not being used for translucent painting, there are six sections of the palette that rotate every 100 milliseconds. At that point, these color ranges move forward. For example, if the color range 252-254 rotate, color 252 takes 254's current value, 253 takes 252's, and 254 takes 253's. The color ranges are:

First index	Last index	Count
252/FCh	254/FEh	3
248/F8h	251/FBh	4
244/F4h	247/F7h	4
240/F0h	243/F3h	4
232/E8h	239/EFh	8
224/E0h	231/E7h	8
Translucency FLX Archive (static/xform.tbl)[edit]

For world palettes, colours 244/F4h to 254/FEh inclusive are special transparent colours that are used when the Translucent flag is set on a world shape's property. The FLX archive "static/xform.tbl" contains tables to transform an underlying colour into a new colour if the colour being drawn is one of those set. Each entry is (uint8[256] OutputIndices). The table is in reverse order; index 0 is for colour 254, index 1 is for colour 253, and so on.

The equations producing the mapping table are far more complex than simple transparency, and trying to replicate them is made more difficult by the limited palette of colours they had to map to. This table shows the closest matching colours that would produce the same effect that use simple transparency. Each colour value is from 0 (darkest/transparent) to 1 (brightest/opaque).

Index	Color	Red	Green	Blue	Alpha	Notes
0	254/FEh	0.999	0.999	1	0.266	White wash
1	253/FDh	0	0	0	0.251	Shadow
2	252/FCh	1	0.999	0.999	0.463	White frosting
3	251/FBh	1	0.033	0.101	0.327	Red fog
4	250/FAh	0.078	0.304	0	0.545	Green fog
5	249/F9h	0	0.043	1	0.342	Blue binary filter
6	248/F8h	1	0.931	0.19	0.208	Yellow haze
7	247/F7h	0.17	0.59	0	0.341	Green haze with blue patches
8	246/F6h	0.09	0.446	0.734	0.177	Grayscale
9	245/F5h	0.247	0	0	0.251	Reddish
10	244/F4h	0.602	0	0.999	0.228	Purple filter
11	Unused	1	0.003	0.22	0.177	Red filter
12	Unused	0.018	1	0.104	0.444	Green filter
13	Unused	0.593	0.602	1	0.875	Blue filter
14	Unused	0.999	1	0.248	0.79	Yellow filter
15	Unused	0.563	0	0.85	0.342	Purple filter
16	Unused	0.746	0.749	0.749	0.999	Gray wash with white highlights
World Data (static/u7chunks, static/u7map)[edit]

World data are separated into three sections: ground, fixed objects, and regular objects.

The ground tiles are a series of 8x8 pixel tiles that constitute the floor. Those are grouped into 16x16 chunks, along with static geometry. These template Chunks are referenced by 16x16 Regions, and there are 12x12 Regions in the world map.

Chunk Templates (static/u7chunks)[edit]

This file contains a list of chunk templates that are then indexed by the regions.

Offset	Length	Type	Name	Description
00h	X*512	Chunk[X]	Chunks	The list of chunks. There are as many chunks as there is space in the file; Ultima 7 comes with 3072/C00h.
Each Chunk has the format:
00h	512	ShapeFrameIndex[16 * 16]	Tiles	Tiles in the chunk in [x + y * 16] order.

A shape index refers to both tiles and shapes. The shape should be inserted into the world as static geometry at zero height.

Static Chunk Data (static/u7map)[edit]

This file contains the chunks for the static map. The map is partitioned into (Region[12 * 12] Regions) (in [x + y * 12] order), where Region is (uint16[16 * 16] Chunks), where each value is an index into "static/u7chunks" described above.

Fixed Items Data (static/u7ifix??)[edit]

These files contain the fixed objects for the region. They are named as u7ifixXX, where XX is a hexadecimal number that is (RegionX + RegionY * 12).

Within the FLX Archive files are 16*16 records for each chunk in the region in [ChunkX + ChunkY * 16] order. If an entry is empty, it contains no fixed objects. Otherwise it contains a list of (uint16 Bitfield, ShapeFrameIndex ShapeFrame), where the bitfield described below:

Shift	Bits	Mask	Name	Description
0	4	15	Y	Y coordinate inside the chunk.
4	4	15	X	X coordinate inside the chunk.
8	4	15	Z	Height level.
12	4	15	??	Always zero.
See Also[edit]
Item Stats in Ultima VII
Ultima VII Part Two SFX
References[edit]
u7tech.txt from the Exult source code - Originally written by Olivier Marcoux, extended by the Exult team over time.
Technical Details
Game	Ultima III ☥ Ultima IV ☥ Ultima V ☥ Ultima VI ☥ Ultima VII ☥ Ultima VIII ☥ Ultima IX ☥ Ultima Underworld
Categories: Technical DetailsUltima VII
Navigation menu
Not logged in
Talk
Contributions
Create account
Log in
Page
Discussion
Read
Edit
View history
Search
Main Page
Games
Characters
Geography
Books
Trinkets
Maps
Bestiary
Fan works
Random page
Community
The Ultima Codex
SotAWiki
Ultima Forever Wiki
Ultima Online Wiki
Community portal
Recent changes
Help
Forum
Create new page
Tools
What links here
Related changes
Special pages
Printable version
Permanent link
Page information
This page was last edited on 6 February 2025, at 06:43.
Content is available under Attribution-ShareAlike 3.0 Unported unless otherwise noted.
Privacy policy
About Ultima Codex
Disclaimers
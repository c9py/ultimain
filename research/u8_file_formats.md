# Ultima VIII internal formats - The Codex of Ultima Wisdom, a wiki for Ultima and Ultima Online

**URL:** https://wiki.ultimacodex.com/wiki/Ultima_VIII_internal_formats

---

Ultima VIII internal formats
Jump to navigation
Jump to search

This page deals with the details on the specifications of the file formats for the different files included in Ultima VIII.

Contents
1	Files
2	Common Formats
2.1	FLX Archives
2.2	Palettes
2.3	Shapes
3	File Structure
3.1	Game Logic
3.1.1	Type flags (static/typeflag.dat)
3.1.2	Usecode game script (static/eusecode.flx)
3.2	Graphics
3.2.1	Game palette (static/u8pal.pal)
3.3	Game Interface
3.3.1	Fonts (static/u8fonts.flx)
3.3.2	Mouse cursors (static/u8mouse.shp)
3.3.3	Gumps and windows (static/u8gumps.shp)
3.4	Maps
3.4.1	World graphics (static/u8shapes.flx)
3.4.2	Compressed world graphics (static/u8shapes.cmp)
3.4.3	Map templates (static/globs.dat)
3.4.4	World objects (static/fixed.dat, gamedat/nonfixed.dat)
4	References
Files[edit]

These are the files in an Ultima VIII installation, and what is known about them.

u8.exe
gamedat/ - Contains an unpacked savegame and runtime data.
gamedat/avatar.dat
gamedat/flag.dat
gamedat/gumps.dat
gamedat/itemcach.dat - A FLX archive
gamedat/kernel.dat - A FLX archive
gamedat/nonfixed.dat - A FLX archive containing world objects that can be changed.
gamedat/npcdata.dat - A FLX archive
savegame/u8save.###
sound/
sound/*.dll - Music device drivers.
sound/music.flx - A FLX archive
sound/sound.flx - A FLX archive
static/ - Contains unchanging game data.
static/anim.dat
static/ecredits.dat
static/eintro.skf - A FLX archive
static/endgame.skf - A FLX archive
static/fixed.dat - A FLX archive containing world objects that cannot be changed.
static/glob.flx - A FLX archive containing templates used in map data.
static/gumpage.dat
static/quotes.dat
static/typeflag.dat - Contains information about the types in the game.
static/u8fonts.flx - A FLX archive containing Shapes that have fonts for the game.
static/u8gumps.flx - A FLX archive containing Shapes that have game interface images and gumps.
static/u8mouse.shp - A standalone Shape file that has the mouse cursors.
static/u8pal.pal - A standalone #aPalette file that contains the game palette.
static/u8shapes.cmp - Compressed form of "static/u8shapes.flx". The first time the game is run, it is decompressed.

static/u8shapes.flx - A FLX archive containing Shapes that have world graphics and animations.
static/wpnovlav.dat - A FLX archive
static/xformpal.dat - A FLX archive
usecode/ - Contains game scripting data.
usecode/eusecode.flx - A FLX archive
usecode/unkcoff.dat
Common Formats[edit]

All multi-byte data are stored in little-endian order. For example, 1122h could be stored as the bytes (22h 11h). Signed values are stored as two's-complement. For example, the signed byte -1 is stored as FFh. These are the basic types that are referred to throughout this document:

uint8/16/24/32 - Unsigned one, two, three, and four-byte integers.
int8/16/32 - Signed one, two, and four-byte integers.
char - ASCII character stored as uint8.
zero - A single byte that is always zero.
FLX Archives[edit]

Files with a ".flx" extension, and many without, are stored in a common archive file format. The header has this format:

Offset	Size	Type	Name	Description
0	82/52h	char[82]	Comment	Always filled with 26/1Ah, which in ASCII is SUB (indicates an invalid character).
82/52h	2	zero[2]	-	Might be part of the comment.
84/54h	4	uint32	Count	The number of records in the archive.
88/58h	4	uint32	Version?	Always 1.
92/5Ch	4	uint32	TotalSize	Either the size in bytes of the archive file, including the header, or zero.
96/60h	4	uint32	CRC?	Seems to be some kind of CRC.
100/64h	100/64h	zero[100]	-	Padding or space for expansion.
144/90h	8*Count	Record[Count]	Records	Record offset and length values, as described below.
Record format:
00h	4	uint32	Offset	Offset from the beginning of the file for the record. This is zero if there is no record at this index.
04h	4	uint32	Length	Length in bytes of the record, or zero if there is no record at this index.

The interpretation of record data depends upon the file itself. All records in an archive have the same format.

Palettes[edit]

Palettes are stored as (Color[256] Colors), where Color is (uint8 Red, uint8 Green, uint8 Blue), and each component is from 0 (darkest) to 63 (brightest). Index 255 is transparent.

Shapes[edit]

Shapes are used as the record format in the FLX Archives "static/u8shapes.flx", "static/u8fonts.flx", and "static/u8gumps.flx" files, as well as a single standalone shape in the "static/u8mouse.shp" file. It has this format:

Offset	Size	Type	Name	Description
00h	2	uint16	MaximumSizeX	In u8fonts.flx and u8mouse.shp, contains the maximum width of the shape's frames. In u8shapes.flx, sometimes contains the ShapeIndex.
02h	2	uint16	MaximumSizeY	In u8fonts.flx and u8mouse.shp, contains the maximum height of the shape's frames.
04h	2	uint16	Count	The number of frames in the shape.
06h	6*Count	FrameHeader[Count]	FrameHeaders	The offsets and byte sizes of the frames.
FrameHeader has this format:
00h	3	uint24	FrameOffset	Offset from the start of the shape to the beginning of the frame in bytes.
03h	1	uint8	??	This is 128 in "static/u8shapes.flx", and 0 everywhere else. Its meaning is a mystery.
04h	2	uint16	FrameSize	The size in bytes of a frame.
At the FrameOffset for each frame is the frame data:
00h	2	uint16	ShapeIndex	Always either the zero-based index of the shape in its archive or zero. Seems meaningless either way.
02h	2	uint16	FrameIndex	Always either the zero-based index of the frame in its shape or zero. Seems meaningless either way.
04h	4	zero[4]	 	 
08h	2	uint16	Compression	Either 0 or 1; this has relevance to the RLE data below.
0Ah	2	uint16	SizeX	Horizontal size of the frame in pixels.
0Ch	2	uint16	SizeY	Vertical size of the frame in pixels.
0Eh	2	uint16	OffsetX	Horizontal offset of the hot spot of the frame in pixels.
10h	2	uint16	OffsetY	Vertical offset of the hot spot of the frame in pixels.
12h	2*SizeY	uint16[SizeY]	RowOffsets	Relative offset to the start of the row data, from the start of the offset. For example, the offset relative to the beginning of the frame data would be (16 + Offset + Index * 2), where Offset is the row offset and Index is the row index.
...	...	...	RowData[SizeY]	The rows of the frame in order.

RowData is stored as RLE data. Describing this is easiest in pseudocode:

int x = 0;

while(x < SizeX) {
    x += ReadUInt8();
    if(x >= SizeX)
        break;

    uint8 length = ReadUInt8();
    uint8 type = 0;

    if(Compression == 1) {
        type = length & 1;
        length >>= 1;
    }

    // Read length bytes to the output.
    if(type == 0)
        ReadData(x, y, length);
    else {
        // Copy value length times to the output.
        uint8 value = ReadUInt8();
        ReadRun(x, y, length, value);
    }
    
    x += length;
}
File Structure[edit]

This section discusses individual files within their subsystem.

Game Logic[edit]
Type flags (static/typeflag.dat)[edit]

This contains information about each type in the game. Each type info record is 8 bytes long; there are 2048 records in Ultima 8 (one for each entry in "static/u8shapes.shp"). Each record has this bit format:

Offset	Shift	Bits	Mask	Name	Description
00h	0	8	255	??	??
01h	0	3	7	??	??
 	3	1	1	Translucent	Whether to use translucency rendering for this shape.
 	4	4	15	QualityUse	How the Quality field is used by the type. 1 for Quality, 2 for Quantity, 6 for Container.
02h	0	4	15	??	??
 	4	4	15	SizeX	Size in the X direction for this object.
03h	0	4	15	SizeY	Size in the Y direction for this object.
 	4	4	15	SizeZ	Size in the Z direction for this object.
04h	0	4	15	AnimationType	How this shape is to be animated; see the details below.
 	4	4	15	AnimationData	Parameter to AnimationType that depends upon that value.
05h	0	4	15	??	??
 	4	1	1	HideInGame	Hide this shape in game. This is for eggs and invisible walls.
 	5	1	1	??	Only used for flaming oils.
 	6	1	1	NonBlocking	Tentative.
 	7	1	1	??	Only used for the 'blue field passage gem'.
06h	0	8	255	??	??
07h	0	8	255	??	??

The AnimationType field changes the conditions where the shape will cycle through a frame during a game update tick, and what frames are cycled through. These conditions are:

Value	AnimationData	Description
0	 	Never.
1 and 3	0	Always cycle through all frames.
 	1	Cycle through all frames 50% of the time.
 	>1	Cycle through some frames (source is unclear).
2	0	Cycle through all frames 50% of the time.
 	1	Cycle through all frames 25% of the time.
 	>1	Cycle through some frames 50% of the time (source is unclear).
4	 	Cycle through all frames with a 1 in AnimationData chance of incrementing.
5	 	Animated by Usecode Event 2.
Usecode game script (static/eusecode.flx)[edit]

This FLX archive contains the game script. Each record corresponds to a class.

Graphics[edit]

Ultima 8 uses 256-colour VGA indexed graphics at a 320x200 resolution. Most notable about the resolution for modern concerns is that the pixels were not square, but were instead vertical rectangles. To display them properly on a modern monitor, the graphics should be 20% vertically taller or they will appear crushed. Fortunately the dimetric projection hides quite a bit of this distortion for the world graphics.

Game palette (static/u8pal.pal)[edit]

This standalone Palette is used during the game.

Game Interface[edit]
Fonts (static/u8fonts.flx)[edit]
Mouse cursors (static/u8mouse.shp)[edit]

This standalone Shape file contains the mouse cursors. These are they:

Index	Description
0-7	Short movement north, northeast, east, southeast, south, southwest, west, and northwest.
8-15	Medium movement north, northeast, etc.
16-23	Long movement north, northeast, etc.
24	No movement (blue dot).
25-32	Combat movement north, northeast, etc.
33	No combat movement (red dot).
34	Use target (four blue arrows pointing inwards).
35	Pentagram.
36	Skeleton hand pointer.
37	Crosshair.
38	Quill.
39	Magnifying lens.
40	Red cross.
41	Empty image.
Gumps and windows (static/u8gumps.shp)[edit]
Maps[edit]

Ultima VIII uses a 2:1 dimetric perspective. To translate an (X, Y, Z) coordinate in world space to pixels, Xp will be ((X - Y) / S) and Yp will be ((X + Y) / (S * 2) - Z). S is 2 for objects specified in "static/globs.dat" and 4 everywhere else.

World graphics (static/u8shapes.flx)[edit]

This FLX archive contains Shapes that fill in the world graphics. The OffsetX and OffsetY fields in the individual shape frames indicate the pixels to subtract from an object's world position to the point to draw the object at.

Compressed world graphics (static/u8shapes.cmp)[edit]
Map templates (static/globs.dat)[edit]

This FLX archive contains blocks of objects that can be inserted into a map to save space. Each record has this format:

Offset	Size	Type	Name	Description
00h	2	uint16	Count	Number of objects in this glob. This is sometimes zero.
02h	6*Count	GlobObject[Count]	Objects	List of objects in the glob, documented below.
This is the format of a GlobObject:
00h	1	uint8	X	Horizontal position of the object.
01h	1	uint8	Y	Vertical position of the object.
02h	1	uint8	Z	Elevation of the object.
03h	2	uint16	ShapeIndex	A zero-based index into the "static/shapes.flx" archive for the shape to draw.
05h	1	uint8	FrameIndex	The zero-based index of the frame to draw in the shape specified by ShapeIndex above.
World objects (static/fixed.dat, gamedat/nonfixed.dat)[edit]

These FLX archives contains the world objects. "static/fixed.dat" contains those objects that cannot be modified during play. "gamedat/nonfixed.dat" contains those objects which can be changed.

Each record contains a Map, which is simply a list of objects. There are as many objects as the (RecordLength / 16). Each object has the format:

Offset	Size	Type	Name	Description
00h	2	uint16	X	Horizontal position on the map.
02h	2	uint16	Y	Vertical position on the map.
04h	1	uint8	Z	Elevation of the object.
05h	2	uint16	ShapeIndex	A zero-based index into the "static/shapes.flx" archive for the shape to draw and the type of the object.
07h	1	uint8	FrameIndex	The zero-based index of the frame to draw in the shape specified by ShapeIndex above.
08h	2	uint16	Flags	General object flags.
0Ah	2	uint16	Quality	The meaning depends upon the object type.
0Ch	1	uint8	NpcIndex	Which NPC this is an object for, if this is an NPC.
0Dh	1	uint8	MapIndex	Which map index the NPC is on (?).
0Eh	2	uint16	NextObjectId	Appears to have no use.
References[edit]
Game Extracts of Ultima VIII: Pagan on Bootstrike.com
docs/ directory on the Pentagram SVN site.
Technical Details
Game	Ultima III ☥ Ultima IV ☥ Ultima V ☥ Ultima VI ☥ Ultima VII ☥ Ultima VIII ☥ Ultima IX ☥ Ultima Underworld
Categories: Technical DetailsUltima VIII
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
This page was last edited on 30 July 2020, at 13:58.
Content is available under Attribution-ShareAlike 3.0 Unported unless otherwise noted.
Privacy policy
About Ultima Codex
Disclaimers
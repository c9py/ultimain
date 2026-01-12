# exult/tools at master · exult/exult · GitHub

**URL:** https://github.com/exult/exult/tree/master/tools

---

Skip to content
Navigation Menu
Platform
Solutions
Resources
Open Source
Enterprise
Pricing
Sign in
Sign up
exult
/
exult
Public
Notifications
Fork 87
 Star 633
Code
Issues
69
Pull requests
5
Discussions
Actions
Projects
Security
Insights
Files
 master
.github
.travis
android
audio
conf
content
data
desktop
docs
files
flic
gamemgr
gumps
headers
imagewin
ios
m4
macosx
mapedit
msvcstuff
objs
pathfinder
server
shapes
tools
aseprite_plugin
compiler
gimp_plugin
mockup
scripts
smooth
ucxt
Makefile.am
README
cmanip.cc
expack.1
expack.cc
expack.txt
exult_shp_thumbnailer.cc
exult_u7shapes.thumbnailer.in
intrins1.txt
intrins2.txt
ipack.1
ipack.cc
ipack.txt
mklink.cc
rip.cc
shp2pcx.1
shp2pcx.cc
shp2pcx.txt
splitshp.1
splitshp.cc
splitshp.txt
textpack.1
textpack.cc
textpack.txt
Breadcrumbs
exult
/tools/
Directory actions
More options
Latest commit
marzojr
Fixing embedded null detection in UCC: string literals
a6656a4
 · 
History
History
Folders and files
Name	Last commit message	Last commit date

parent directory
..


aseprite_plugin
	
Incrementing version to 1.13.1
	


compiler
	
Fixing embedded null detection in UCC: string literals
	


gimp_plugin
	
Fixes for Gimp and Aseprite plugins' Makefile.am
	


mockup
	
Fix mockup tool palette array size 255 -> 256
	


scripts
	
When using the script to update version numbers, one more case in the…
	


smooth
	
Migrate Exult to SDL 3, Commit 2 : Gather all source changes to make …
	


ucxt
	
Aligning behavior of ucc/wuc/ucxt with regards to UI_UNKNOWN_XX intri…
	


Makefile.am
	
Rename SDL2_ and SDL2_IMAGE_ to SDL_ and SDL_IMAGE_
	


README
	
Usecode compiler tools extracted and integrated into build
	


cmanip.cc
	
Cleaning .clang-format and formatting everything with it
	


expack.1
	
Fixed "Ultima 7" vs "Ultima VII" in front facing code and documentations
	


expack.cc
	
Reworking endian input
	


expack.txt
	
Fixed "Ultima 7" vs "Ultima VII" in front facing code and documentations
	


exult_shp_thumbnailer.cc
	
The SHP Thumbnailer is not Gnome dependent, rename it exult_shp_thumb…
	


exult_u7shapes.thumbnailer.in
	
The SHP Thumbnailer is not Gnome dependent, rename it exult_shp_thumb…
	


intrins1.txt
	
New files
	


intrins2.txt
	
New files
	


ipack.1
	
* tools/*.1: finally added Robert Bihlmeyer man pages. Added them
	


ipack.cc
	
Reworking endian input
	


ipack.txt
	
Added 'all' option to ipack
	


mklink.cc
	
Reworking endian input
	


rip.cc
	
Cleaning .clang-format and formatting everything with it
	


shp2pcx.1
	
* tools/*.1: finally added Robert Bihlmeyer man pages. Added them
	


shp2pcx.cc
	
Migrate Exult to SDL 3, Commit 2 : Gather all source changes to make …
	


shp2pcx.txt
	
* tools/shp2pcx.txt, splitshp.txt, textpack.txt added, expack.txt upd…
	


splitshp.1
	
* tools/*.1: finally added Robert Bihlmeyer man pages. Added them
	


splitshp.cc
	
Cleaning .clang-format and formatting everything with it
	


splitshp.txt
	
* tools/shp2pcx.txt, splitshp.txt, textpack.txt added, expack.txt upd…
	


textpack.1
	
* tools/*.1: finally added Robert Bihlmeyer man pages. Added them
	


textpack.cc
	
Cleaning up and improving textmsg reading code
	


textpack.txt
	
* tools/shp2pcx.txt, splitshp.txt, textpack.txt added, expack.txt upd…
	


u7bgflag.txt
	
New files
	


u7siflag.txt
	
New files
	


u7voice2syx.cc
	
Reworking endian input
	


ucformat.txt
	
New files
	


uctools.h
	
Getting rid of array_size
	


wuc.cc
	
Aligning behavior of ucc/wuc/ucxt with regards to UI_UNKNOWN_XX intri…
	


x-shapefile.xml
	
Permit alternate installation folders for the Shape Thumbnailer confi…
	
README
Hello,

This .ZIP contains the following files:

README		This File
B.EXE		Decompiler by Maxim & Wody
MKLINK.EXE	Creates the link-dependencies U7 uses
RIP.COM		Rips usecode to little files, and builds usecode-file by Wody
WUC.COM		Usecode assembler by Wody
INTRINS1.TXT	List of used intrinsic functions for U7BG by Wody
INTRINS2.TXT	List of used intrinsic functions for U7SI by Wody
U7BGFLAG.TXT	Flags used with PUSHF/POPF for U7BG by Wody
U7SIFLAG.TXT	Flags used with PUSHF/POPF for U7SI by Wody
UCFORMAT.TXT	Format of Usecode-file by various people
SOURCE.ZIP	ZIP file of source for all programs included

Maxim is Maxim S. Shatskih, Email: maxim__s@mtu-net.ru
Wody is Wouter Dijkslag, Email: wody@wody.demon.nl
(more credits in UCFORMAT.TXT)

If you want to do anything with it, you need the usecode file from
Ultima 7: The Black Gate or Ultima7: Serpent Isle. With this file, you can then
create a list by running B.EXE, which shows it on your screen. To redirect it
to a file, use B > FILE, where FILE is the file you want it to.

This file has a lot of functions in it, in the form of:

            Number
Function #0 (0096H), offset = 00000000, size = 00a2, data = 005d

The most interesting part is number. If you run B with that number (B 0096)
you will get the source of that function, as far as is known. Off course, this
can be redirected too.

You can then change that function, and recompile it!

If you want to change a function, you first need to create compiled files and
an index file for all functions. You could do this by hand, but there is a
program for it, called RIP. You can rip a single function (RIP 0096), take all
functions out of the usecode (RIP ALL), only create an index (RIP INDEX), or
put all files and the index back together (RIP GLUE).

The compile is done by the program WUC. You run this by WUC infile outfile.
This means, if you have function 0096 written in file 0096.TXT, you need to run
WUC 0096.TXT 0096, but you could also write FUNCTION.TXT 0096 or something to
that effect.

If you just change a single function, and didn't change the size, you can put
it back with RIP PUT. Otherwise you have to rebuild the entire file with RIP
GLUE, and run MKLINK. When the usecode file is built, copy it over the old
version, and run U7. If you ran MKLINK, also copy the LINKDEP1 and LINKDEP2
files over the old versions. Then it's testing.. Don't forget to make backups!

Read the decompiled usecode to see how to program for U7. If you find out
anything (functions, meanings of opcodes, etc) which is unknown, or have any
comments, please tell Wouter so this package can become better!



Ultima & The Serpent Isle are trademarks or registered trademarks of ORIGIN
Systems, Inc.

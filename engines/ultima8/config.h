/* config.h - Pentagram configuration for SDL3 build */
#ifndef CONFIG_H
#define CONFIG_H

/* Package info */
#define PACKAGE "pentagram"
#define PACKAGE_BUGREPORT "pentagram-devel@lists.sourceforge.net"
#define PACKAGE_NAME "Pentagram"
#define PACKAGE_STRING "Pentagram 1.0"
#define PACKAGE_TARNAME "pentagram"
#define PACKAGE_VERSION "1.0"
#define VERSION "1.0"

/* System features */
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_UNISTD_H 1
#define HAVE_DIRENT_H 1
#define HAVE_STDINT_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_MEMORY_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_STRINGS_H 1

/* SDL */
#define HAVE_SDL 1
#define USE_SDL 1

/* PNG support */
#define HAVE_PNG_H 1
#define USE_PNG 1

/* Freetype support */
#define HAVE_FREETYPE2 1
#define USE_FREETYPE2 1
#define USE_SDL_TTF 1

/* MIDI support */
#define USE_TIMIDITY_MIDI 1
#define USE_FMOPL_MIDI 1

/* Zip support */
#define HAVE_ZIP_SUPPORT 1

/* Endianness */
#define WORDS_BIGENDIAN 0

/* Size of types */
#define SIZEOF_SHORT 2
#define SIZEOF_INT 4
#define SIZEOF_LONG 8
#define SIZEOF_INTP 8

/* Console streams */
#define SAFE_CONSOLE_STREAMS 1

/* Debug features */
/* #undef DEBUG */

#endif /* CONFIG_H */

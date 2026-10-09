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

/* Freetype support. SDL_ttf is a CMake option (PENTAGRAM_USE_SDL_TTF), off by default. */
#define HAVE_FREETYPE2 1
#define USE_FREETYPE2 1

/* Home directory for pentagram.ini ($HOME/.pentagram). */
#define HAVE_HOME 1

/* MIDI support is selected by CMake (USE_TIMIDITY_MIDI / USE_FMOPL_MIDI). */

/* Zip support */
#define HAVE_ZIP_SUPPORT 1

/* Endianness */
#define WORDS_BIGENDIAN 0

/* Size of types. Windows is LLP64 (long is 4); Unix x86_64 is LP64 (long is 8). */
#define SIZEOF_SHORT 2
#define SIZEOF_INT 4
#define SIZEOF_LONG_LONG 8
#if defined(_WIN64)
#define SIZEOF_LONG 4
#define SIZEOF_INTP 8
#elif defined(_WIN32)
#define SIZEOF_LONG 4
#define SIZEOF_INTP 4
#else
#define SIZEOF_LONG 8
#define SIZEOF_INTP 8
#endif

/* Console streams */
#define SAFE_CONSOLE_STREAMS 1

/* Debug features */
/* #undef DEBUG */

#endif /* CONFIG_H */

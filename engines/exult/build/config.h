/*
 * config.h - CMake generated configuration header for Exult
 */

#ifndef CONFIG_H
#define CONFIG_H

/* Package information */
#define PACKAGE "exult"
#define PACKAGE_VERSION "1.10.0"
#define VERSION "1.10.0"
#define PACKAGE_NAME "Exult"
#define PACKAGE_STRING "Exult 1.10.0"
#define PACKAGE_BUGREPORT "exult-general@lists.sourceforge.net"

/* Build configuration */
/* #undef HAVE_CONFIG_H */

/* SDL3 support */
#define HAVE_SDL 1
#define USE_SDL3 1
#define HAVE_SDL_H 1

/* Audio support */
/* #undef HAVE_VORBIS */
/* #undef HAVE_OGG */
/* #undef HAVE_FLUIDSYNTH */
/* #undef HAVE_MT32EMU */

/* Image support */
/* #undef HAVE_SDL_IMAGE */
/* #undef HAVE_PNG */

/* System headers */
#define HAVE_STDINT_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_UNISTD_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_DIRENT_H 1
#define HAVE_FCNTL_H 1
#define HAVE_LIMITS_H 1
#define HAVE_SIGNAL_H 1

/* String functions */
#define HAVE_STRCASECMP 1
#define HAVE_STRNCASECMP 1
#define HAVE_SNPRINTF 1

/* Memory functions */
#define HAVE_MMAP 1

/* Network support */
#define HAVE_NETDB_H 1
#define HAVE_SYS_SOCKET_H 1
#define HAVE_NETINET_IN_H 1
#define HAVE_ARPA_INET_H 1

/* Data directory */
#ifndef EXULT_DATADIR
#define EXULT_DATADIR "/usr/local/share/exult"
#endif

/* Enable features */
#define HAVE_ZIP_SUPPORT 1
#define USE_EXULTSTUDIO 0
#define USE_TIMIDITY_MIDI 1

/* Platform detection */
#ifdef __linux__
#define LINUX 1
#endif

#ifdef __APPLE__
#define MACOSX 1
#endif

#ifdef _WIN32
#define WIN32 1
#endif

/* Endianness - assume little endian for now */
#define WORDS_BIGENDIAN 0

/* Size types */
#define SIZEOF_SHORT 2
#define SIZEOF_INT 4
#define SIZEOF_LONG 8
#define SIZEOF_LONG_LONG 8

#endif /* CONFIG_H */

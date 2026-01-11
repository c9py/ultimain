/*
 * config.h - Configuration header for Ultima Engines Integration
 * 
 * This file provides the necessary configuration macros for building
 * the shared components library.
 */

#ifndef CONFIG_H
#define CONFIG_H

/* Package information */
#define PACKAGE "ultima-integration"
#define PACKAGE_VERSION "1.0.0"
#define VERSION "1.0.0"

/* SDL3 support */
#define HAVE_SDL 1
#define USE_SDL3 1

/* Audio support */
#define HAVE_VORBIS 1
#define HAVE_OGG 1

/* System features */
#define HAVE_STDINT_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_UNISTD_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_SYS_STAT_H 1

/* String functions */
#define HAVE_STRCASECMP 1
#define HAVE_STRNCASECMP 1

/* Data directory */
#ifndef EXULT_DATADIR
#define EXULT_DATADIR "data"
#endif

/* Enable features */
#define HAVE_ZIP_SUPPORT 1

#endif /* CONFIG_H */

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __PRODUCT_CFG_H__
#define __PRODUCT_CFG_H__

/* product_config.h.  Generated from product_config.h.in by configure.  */
/* product_config.h.in.  Generated from configure.ac by autoheader.  */

/* basic product line */
#define BASIC_SDK /**/

/* Define wsf heartbeat interval in seconds */
#define CONFIG_HEARTBEAT_INTERVAL 3

/* Define wsf max message length in byte */
#define CONFIG_MAX_MESSAGE_LENGTH 4096

/* Define wsf request timeout interval in seconds */
#define CONFIG_REQTIMEOUT_INTERVAL 2
/* gateway product line */
/* #undef GATEWAY_SDK */

/* Define if --enable-gtest */
#define GTEST /**/

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if you have POSIX threads libraries and header files. */
#define HAVE_PTHREAD 1

/* Have PTHREAD_PRIO_INHERIT. */
/* #undef HAVE_PTHREAD_PRIO_INHERIT */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "alink-agent"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "alink-agent"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "alink-agent 0.1"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "alink-agent"

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.1"

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* router product line */
/* #undef ROUTER_SDK */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "0.1"

#endif


/* libbb_udhcp.h - busybox compatability wrapper */

#ifndef _LIBBB_UDHCP_H
#define _LIBBB_UDHCP_H

#ifdef BB_VER
#include "libbb.h"

#ifdef CONFIG_FEATURE_UDHCP_SYSLOG
#define SYSLOG
#endif

#ifdef CONFIG_FEATURE_UDHCP_DEBUG
#define DEBUG
#endif

#define COMBINED_BINARY
#define VERSION "0.9.8-pre"

#else /* ! BB_VER */

#define TRUE			1
#define FALSE			0

#define xmalloc malloc

#endif /* BB_VER */

#endif /* _LIBBB_UDHCP_H */

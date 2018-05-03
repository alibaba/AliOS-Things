
#include "porting.h"

#ifndef __IPERF_CONFIG_H
#define __IPERF_CONFIG_H
/* src/iperf_config.h.  Generated from iperf_config.h.in by configure.  */
/* src/iperf_config.h.in.  Generated from configure.ac by autoheader.  */

#define IPERF_SUPPORT_LONGLONG
#ifdef __SSV_UNIX_SIM__
#define IPERF_SUPPORT_DOUBLE
#endif

#ifdef IPERF_SUPPORT_DOUBLE
typedef double iperf_jit_t;
#else
typedef signed long iperf_jit_t;
#endif

#ifdef IPERF_SUPPORT_LONGLONG
typedef unsigned long long iperf_size_t;
typedef signed long long iperf_ssize_t;
#else
typedef unsigned long iperf_size_t;
typedef signed long iperf_ssize_t;
#endif

#define HAVE_SCTP       0
#define HAVE_TCP_MSS    0

/* Define to 1 if you have the `cpuset_setaffinity' function. */
/* #undef HAVE_CPUSET_SETAFFINITY */

/* Have CPU affinity support. */
#define HAVE_CPU_AFFINITY 0

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 0

/* Have IPv6 flowlabel support. */
#define HAVE_FLOWLABEL 0

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 0

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 0

/* Define to 1 if you have the <netinet/sctp.h> header file. */
/* #undef HAVE_NETINET_SCTP_H */

/* Define to 1 if you have the `sched_setaffinity' function. */
#define HAVE_SCHED_SETAFFINITY 0

/* Have SCTP support. */
/* #undef HAVE_SCTP */

/* Define to 1 if you have the `sendfile' function. */
#define HAVE_SENDFILE 0

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 0

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if the system has the type `struct sctp_assoc_value'. */
/* #undef HAVE_STRUCT_SCTP_ASSOC_VALUE */

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 0

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 0

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Have TCP_CONGESTION sockopt. */
#define HAVE_TCP_CONGESTION 0

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the sub-directory where libtool stores uninstalled libraries. */
//#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "iperf"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "https://github.com/esnet/iperf"

/* Define to the full name of this package. */
#define PACKAGE_NAME "iperf"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "iperf 3-CURRENT"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "iperf"

/* Define to the home page for this package. */
#define PACKAGE_URL "http://software.es.net/iperf/"

/* Define to the version of this package. */
#define PACKAGE_VERSION "3-CURRENT"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "3-CURRENT"

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */
#define HAVE_CLIENT  1
#define HAVE_SERVER  1

#endif


/* coap_config.h.  Generated from coap_config.h.in by configure.  */
/* coap_config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Define to 1 if you have the <assert.h> header file. */
#define HAVE_ASSERT_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the `getaddrinfo' function. */
#define HAVE_GETADDRINFO 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if the system has libcunit */
/* #undef HAVE_LIBCUNIT */

/* Define if the system has libgnutls28 */
/* #undef HAVE_LIBGNUTLS */

/* Define if the system has libtinydtls */
/* #undef HAVE_LIBTINYDTLS */

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the `malloc' function. */
#define HAVE_MALLOC 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* Define to 1 if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* Define if the system has libssl1.1 */
/* #undef HAVE_OPENSSL */
#define HAVE_MBEDTLS 1
#define COAP_TLS_LIBRARY_MBEDTLS 4 /**< Using Mbedtls library */

/* Define to 1 if you have the `select' function. */
#define HAVE_SELECT 1

/* Define to 1 if you have the `socket' function. */
#define HAVE_SOCKET 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strnlen' function. */
#define HAVE_STRNLEN 1

/* Define to 1 if you have the `strrchr' function. */
#define HAVE_STRRCHR 1

/* Define to 1 if you have the <syslog.h> header file. */

/*#undef HAVE_SYSLOG_H 1 */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/unistd.h> header file. */
#define HAVE_SYS_UNISTD_H 1

/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "libcoap-developers@lists.sourceforge.net"

/* Define to the full name of this package. */
#define PACKAGE_NAME "libcoap"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libcoap 4.2.0rc4"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libcoap"

/* Define to the home page for this package. */
#define PACKAGE_URL "https://libcoap.net/"

/* Define to the version of this package. */
#define PACKAGE_VERSION "4.2.0rc4"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* #define WITHOUT_ASYNC 1 SHOULD NOT defined */
#define WITHOUT_OBSERVE 1
#define WITHOUT_QUERY_FILTER 1

#if !defined(WITH_LWIP) && !defined(WITH_SAL)
/* Define to 1 if you have the <netinet/in.h> header file. */
/* #undef HAVE_NETINET_IN_H 0 x*/
#define HAVE_NETINET_IN_H 1

/* Define to 1 if you have the <arpa/inet.h> header file. */
/* #undef  HAVE_ARPA_INET_H 0 x */
#define HAVE_ARPA_INET_H 1


/* Define to 1 if you have the <sys/ioctl.h> header file. */
/*#undef #HAVE_SYS_IOCTL_H  x*/
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
/*#undef HAVE_SYS_SOCKET_H x*/
#define HAVE_SYS_SOCKET_H 1

#define IPV6_SUPPORT_LIBCOAP 1

/*#define SEND_RECV_MSG_SUPPORT_LIBCOAP 1 */
#endif

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Define this to 1 for ancillary data on MacOS */
/* #undef __APPLE_USE_RFC_3542 */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef ssize_t */

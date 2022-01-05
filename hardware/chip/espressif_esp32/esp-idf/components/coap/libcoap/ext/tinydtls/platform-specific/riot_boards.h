/************************************************************************/
/* RIOT-specific parameters                                          */
/************************************************************************/


#ifndef _RIOT_BOARDS_H_
#define _RIOT_BOARDS_H_

/*
 * SETTING FOR TINYDTLS OVER RIOT
 * In standard installation of TinyDTLS they are at dtls_config.h
 * Only those used by the main library (not test/ or test* files) are here.
 *
 * WARNING: Dynamic memory is not working very well over RIOT.
 */


/* RIOT supports  <assert.h> header file.  */
#define HAVE_ASSERT_H 1

/* RIOT supports  <inttypes.h> header file.  */
#define HAVE_INTTYPES_H 1

/* RIOT does NOT implement the member sin6_len */
#undef HAVE_SOCKADDR_IN6_SIN6_LEN

/* RIOT supports the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* RIOT supports the <time.h> header file. */
#define HAVE_TIME_H 1

/* RIOT supports Sockets  */
/* TODO NOTE: This should be enable by WITH_RIOT_SOCKETS  */

/* RIOT has partial support for the `vprintf' function. */
/* DANGER Removing bring issues with dtls_debug.h and dtls_debug.c */
#define HAVE_VPRINTF 1


/*
 * INFORMATION ABOUT TINYDTLS
 * NOTE: This is used mostly by dtls_debug
 */

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "tinydtls"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "tinydtls 0.8.6.X"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "tinydtls"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.8.6.X"

/*
 * PERFORMANCE AND RESOURCES AVALIABLE TO THE MOTES FOR DTLS SESSIONS
 *
 * They are used with peer.c, crypto.c and hmac.c
 *
 * NOTE: Those numbers are not strictly the minimum. You can adjust at will.
 *       Example: iotlab-m3 were tested with two Peers sessions.
 *
 * WARNING: Dynamic memory is not working very well over RIOT.
            Therefore, adapting static memory could improve the performance
            for RIOT nodes.
 */

#ifndef DTLS_CONTEXT_MAX
/** The maximum number of DTLS  context at the same time. */
#  define DTLS_CONTEXT_MAX 2
#endif

#ifndef DTLS_PEER_MAX
/** The maximum number DTLS peers (i.e. sessions). */
#  define DTLS_PEER_MAX 1
#endif

#ifndef DTLS_HANDSHAKE_MAX
/** The maximum number of concurrent DTLS handshakes. */
#  define DTLS_HANDSHAKE_MAX 1
#endif

#ifndef DTLS_SECURITY_MAX
/** The maximum number of concurrently used cipher keys */
#  define DTLS_SECURITY_MAX (DTLS_PEER_MAX + DTLS_HANDSHAKE_MAX)
#endif

/* TODO: Adapt this to RIOT (currently is only for Contiki) */
#ifndef DTLS_HASH_MAX
/** The maximum number of hash functions that can be used in parallel. */
#  define DTLS_HASH_MAX (3 * DTLS_PEER_MAX)
#endif

/** do not use uthash's hash tables (the tables uses malloc/free) */
#define DTLS_PEERS_NOHASH 1

/* The 802.15.4 ACK can provoke very fast re-transmissions with a value
 * higher than one. This is a temporary bad behavior for the RIOT MAC
 */
#ifndef DTLS_DEFAULT_MAX_RETRANSMIT
# define DTLS_DEFAULT_MAX_RETRANSMIT 1
#endif


/*
 * INFORMATION SHA2/ LIBRARY VARIABLES
 *
 * TODO: Clarify the way RIOT identifies BYTE_ORDER
 */

/*
 * RIOT supports the  <inttypes.h> header file.
 * NOTE: uintXX_t definitions with the ANSI C headers instead of custom typedef
 */
#define SHA2_USE_INTTYPES_H 1

/* RIOT "supports" memset()/memcpy() BUT not bzero()/mcopy(). */
#define SHA2_USE_MEMSET_MEMCPY 1


/*
 * NOTE Gcc is who define if we are big endian or little endian.
 * Because RIOT has __BYTE_ORDER__ and BYTE_ORDER it is not clear which one
 * should take preference here. Or, if the #define inside of sha2/sha2.h
 * should be removed at all.
 */
#ifndef BIG_ENDIAN
#if !defined(__BIG_ENDIAN__)
#    define BIG_ENDIAN    4321
#  else
#    define BIG_ENDIAN    __BIG_ENDIAN__
#  endif
#endif

#ifndef LITTLE_ENDIAN
#if !defined(__LITTLE_ENDIAN__)
#    define LITTLE_ENDIAN    1234
#  else
#    define LITTLE_ENDIAN    __LITTLE_ENDIAN__
#  endif
#endif

#endif /* _RIOT_BOARDS_H_ */

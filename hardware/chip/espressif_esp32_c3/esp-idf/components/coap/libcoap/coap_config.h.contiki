#ifndef COAP_CONFIG_H_
#define COAP_CONFIG_H_

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#define WITH_CONTIKI 1

#ifndef COAP_CONSTRAINED_STACK
#define COAP_CONSTRAINED_STACK 1
#endif

#ifndef COAP_DISABLE_TCP
#define COAP_DISABLE_TCP 1
#endif

#define PACKAGE_STRING "libcoap"
#define PACKAGE_NAME "libcoap"

#ifndef COAP_DEFAULT_PDU_SIZE
#define COAP_DEFAULT_PDU_SIZE 64
#endif /* COAP_DEFAULT_PDU_SIZE */
#ifndef COAP_RXBUFFER_SIZE
#define COAP_RXBUFFER_SIZE 64
#endif /* COAP_RXBUFFER_SIZE */

/** Number of resources that can be handled by a CoAP server in addition to
 * @c /.well-known/core */
#ifndef COAP_MAX_RESOURCES
#define COAP_MAX_RESOURCES 3
#endif /* COAP_MAX_RESOURCES */

/** Number of attributes that can be handled (should be at least
 * @c 2 * COAP_MAX_RESOURCES. to carry the content type and the
 * resource type. */
#ifndef COAP_MAX_ATTRIBUTES
#define COAP_MAX_ATTRIBUTES 4
#endif /* COAP_MAX_ATTRIBUTES */

/**
 * Number of PDUs that can be stored simultaneously. This number
 * includes both, the PDUs stored for retransmission as well as the
 * PDUs received. Beware that choosing a too small value can lead to
 * many retransmissions to be dealt with.
 */
#ifndef COAP_PDU_MAXCNT
#define COAP_PDU_MAXCNT 4
#endif /* COAP_PDU_MAXCNT */

/**
 * Maximum number of sessions.
 */
#ifndef COAP_MAX_SESSIONS
#define COAP_MAX_SESSIONS 2
#endif /* COAP_MAX_SESSIONS */

/**
 * Maximum number of subscriptions. Every additional subscriber costs
 * 36 B.
 */
#ifndef COAP_MAX_SUBSCRIBERS
#define COAP_MAX_SUBSCRIBERS 3
#endif /* COAP_MAX_SUBSCRIBERS */

/**
 * The maximum number of cache-key entries that allocate
 * fixed-size memory blocks.
 */
#ifndef COAP_MAX_CACHE_KEYS
#define COAP_MAX_CACHE_KEYS        (2U)
#endif /* COAP_MAX_CACHE_KEYS */

/**
 * The maximum number of cache-entry entries that allocate
 * fixed-size memory blocks.
 */
#ifndef COAP_MAX_CACHE_ENTRIES
#define COAP_MAX_CACHE_ENTRIES        (2U)
#endif /* COAP_MAX_CACHE_ENTRIES */

/**
 * Maximum number of large body transmissions.
 */
#ifndef COAP_MAX_LG_XMIT
#define COAP_MAX_LG_XMIT 2
#endif /* COAP_MAX_LG_XMIT */

/**
 * Maximum number of large body client receives.
 */
#ifndef COAP_MAX_LG_CRCV
#define COAP_MAX_LG_CRCV 2
#endif /* COAP_MAX_LG_CRCV */

/**
 * Maximum number of large body server receives.
 */
#ifndef COAP_MAX_LG_SRCV
#define COAP_MAX_LG_SRCV 2
#endif /* COAP_MAX_LG_SRCV */

/**
 * Number of notifications that may be sent non-confirmable before a
 * confirmable message is sent to detect if observers are alive. The
 * maximum allowed value here is @c 15.
 */
#ifndef COAP_OBS_MAX_NON
#define COAP_OBS_MAX_NON   5
#endif /* COAP_OBS_MAX_NON */

/**
 * Number of confirmable notifications that may fail (i.e. time out
 * without being ACKed) before an observer is removed. The maximum
 * value for COAP_OBS_MAX_FAIL is @c 3.
 */
#ifndef COAP_OBS_MAX_FAIL
#define COAP_OBS_MAX_FAIL  3
#endif /* COAP_OBS_MAX_FAIL */

#ifndef DEBUG
# define DEBUG DEBUG_PRINT
#endif

#define HAVE_STRNLEN 1
#define HAVE_SNPRINTF 1
#define HAVE_STRINGS_H 1

/* there is no file-oriented output */
#define COAP_DEBUG_FD NULL
#define COAP_ERR_FD   NULL

#include "contiki-conf.h"

#if (defined(PLATFORM) && PLATFORM == PLATFORM_MC1322X) || defined(CONTIKI_TARGET_ECONOTAG)
/* Redbee econotags get a special treatment here: endianness is set
 * explicitly, and
 */

#define BYTE_ORDER UIP_LITTLE_ENDIAN

#define HAVE_ASSERT_H
#define HAVE_UNISTD_H
#define HAVE_SYS_TYPES_H
#define HAVE_LIMITS_H
#endif /* PLATFORM_MC1322X || CONTIKI_TARGET_ECONOTAG */

#if defined(TMOTE_SKY) || defined(CONTIKI_TARGET_SKY) || defined(CONTIKI_TARGET_WISMOTE)
/* Need to set the byte order for TMote Sky explicitely */

#define BYTE_ORDER UIP_LITTLE_ENDIAN
#undef COAP_DEFAULT_PDU_SIZE
#undef COAP_RXBUFFER_SIZE
#define COAP_DEFAULT_PDU_SIZE 100
#define COAP_RXBUFFER_SIZE 100

#define COAP_MAX_BLOCK_SZX 2

typedef int ssize_t;
typedef void FILE;

#define HAVE_LIMITS_H 1
#undef HAVE_ASSERT_H
#define HAVE_VPRINTF 1
#endif /* defined(TMOTE_SKY) */

#ifdef CONTIKI_TARGET_MINIMAL_NET
#undef COAP_DEFAULT_PDU_SIZE
#undef COAP_RXBUFFER_SIZE
#define COAP_DEFAULT_PDU_SIZE 1152
#define COAP_RXBUFFER_SIZE 1472
#define HAVE_ASSERT_H 1
#define HAVE_VPRINTF 1
#define HAVE_SYS_TYPES_H 1
#endif /* CONTIKI_TARGET_MINIMAL_NET */

#ifdef CONTIKI_TARGET_CC2538DK
#define BYTE_ORDER UIP_LITTLE_ENDIAN
#undef COAP_DEFAULT_PDU_SIZE
#undef COAP_RXBUFFER_SIZE
#define COAP_DEFAULT_PDU_SIZE 100
#define COAP_RXBUFFER_SIZE 100

#undef COAP_MAX_BLOCK_SZX
#define COAP_MAX_BLOCK_SZX 2

#define HAVE_LIMITS_H 1
#endif /* CONTIKI_TARGET_CC2538DK */

#ifndef BYTE_ORDER
# ifdef UIP_CONF_BYTE_ORDER
#  define BYTE_ORDER UIP_CONF_BYTE_ORDER
# else
#  error "UIP_CONF_BYTE_ORDER not defined"
# endif /* UIP_CONF_BYTE_ORDER */
#endif /* BYTE_ORDER */

/* Define assert() as empty directive unless HAVE_ASSERT_H is given. */
#ifndef HAVE_ASSERT_H
# define assert(x)
#endif

#define ntohs uip_ntohs

#include <stdio.h>
#define coap_log(fd, ...) printf(__VA_ARGS__)

#endif /* COAP_CONFIG_H_ */


/*******************************************************************************
 *
 * Copyright (c) 2011, 2012, 2013, 2014, 2015 Olaf Bergmann (TZI) and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v. 1.0 which accompanies this distribution.
 *
 * The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 * http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Olaf Bergmann  - initial API and implementation
 *    Hauke Mehrtens - memory optimization, ECC integration
 *
 *******************************************************************************/

#ifndef _DTLS_GLOBAL_H_
#define _DTLS_GLOBAL_H_

#include <stdlib.h>
#include <sys/types.h>

#include "tinydtls.h"

#ifndef DTLSv12
/* The current version of tinyDTLS supports DTLSv1.2 only. */
#define DTLSv12 1
#endif

#ifndef WITH_SHA256
/* The current version of tinyDTLS supports DTLSv1.2 with SHA256 PRF
   only. */
#define WITH_SHA256 1
#endif

/* Define our own types as at least uint32_t does not work on my amd64. */

typedef unsigned char uint8;
typedef unsigned char uint16[2];
typedef unsigned char uint24[3];
typedef unsigned char uint32[4];
typedef unsigned char uint48[6];

#ifndef DTLS_MAX_BUF
/** Maximum size of DTLS message.
    When Peers are sending bigger messages this causes problems. Californium
    with ECDSA needs at least 220 */
#if (defined(WITH_CONTIKI) || defined(RIOT_VERSION))
#ifdef DTLS_ECC
#define DTLS_MAX_BUF 200
#else /* DTLS_ECC */
#define DTLS_MAX_BUF 100
#endif /* DTLS_ECC */
#else /* WITH_CONTIKI */
#define DTLS_MAX_BUF 1400
#endif /* WITH_CONTIKI || RIOT_VERSION */
#endif

#ifndef DTLS_DEFAULT_MAX_RETRANSMIT
/** Number of message retransmissions. */
#define DTLS_DEFAULT_MAX_RETRANSMIT 7
#endif

/** Known cipher suites.*/
typedef enum { 
  TLS_NULL_WITH_NULL_NULL = 0x0000,   /**< NULL cipher  */
  TLS_PSK_WITH_AES_128_CCM_8 = 0xC0A8, /**< see RFC 6655 */
  TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8 = 0xC0AE /**< see RFC 7251 */
} dtls_cipher_t;

/** Known compression suites.*/
typedef enum {
  TLS_COMPRESSION_NULL = 0x0000		/* NULL compression */
} dtls_compression_t;

#define TLS_EXT_ELLIPTIC_CURVES		10 /* see RFC 4492 */
#define TLS_EXT_EC_POINT_FORMATS	11 /* see RFC 4492 */
#define TLS_EXT_SIG_HASH_ALGO		13 /* see RFC 5246 */
#define TLS_EXT_CLIENT_CERTIFICATE_TYPE	19 /* see RFC 7250 */
#define TLS_EXT_SERVER_CERTIFICATE_TYPE	20 /* see RFC 7250 */
#define TLS_EXT_ENCRYPT_THEN_MAC	22 /* see RFC 7366 */

#define TLS_CERT_TYPE_RAW_PUBLIC_KEY	2 /* see RFC 7250 */

#define TLS_EXT_ELLIPTIC_CURVES_SECP256R1	23 /* see RFC 4492 */

#define TLS_EXT_EC_POINT_FORMATS_UNCOMPRESSED	0 /* see RFC 4492 */

#define TLS_EC_CURVE_TYPE_NAMED_CURVE		3 /* see RFC 4492 */

#define TLS_CLIENT_CERTIFICATE_TYPE_ECDSA_SIGN	64 /* see RFC 4492 */

#define TLS_EXT_SIG_HASH_ALGO_SHA256		4 /* see RFC 5246 */
#define TLS_EXT_SIG_HASH_ALGO_ECDSA		3 /* see RFC 5246 */

/** 
 * XORs \p n bytes byte-by-byte starting at \p y to the memory area
 * starting at \p x. */
static inline void
memxor(unsigned char *x, const unsigned char *y, size_t n) {
  while(n--) {
    *x ^= *y;
    x++; y++;
  }
}

/**
 * Compares \p len bytes from @p a with @p b in constant time. This
 * functions always traverses the entire length to prevent timing
 * attacks.
 *
 * \param a Byte sequence to compare
 * \param b Byte sequence to compare
 * \param len Number of bytes to compare.
 * \return \c 1 if \p a and \p b are equal, \c 0 otherwise.
 */
static inline int
equals(unsigned char *a, unsigned char *b, size_t len) {
  int result = 1;
  while (len--) {
    result &= (*a++ == *b++);
  }
  return result;
}

#ifdef HAVE_FLS
#define dtls_fls(i) fls(i)
#else
static inline int 
dtls_fls(unsigned int i) {
  int n;
  for (n = 0; i; n++)
    i >>= 1;
  return n;
}
#endif /* HAVE_FLS */

#undef uthash_fatal
#define uthash_fatal(msg) return(-1) /* fatal error in uthash */

#endif /* _DTLS_GLOBAL_H_ */

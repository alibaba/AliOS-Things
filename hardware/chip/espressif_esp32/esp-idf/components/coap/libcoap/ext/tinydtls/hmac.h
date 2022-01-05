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

#ifndef _DTLS_HMAC_H_
#define _DTLS_HMAC_H_

#include <sys/types.h>

#include "tinydtls.h"
#include "global.h"

#ifdef WITH_SHA256
#ifdef RIOT_VERSION
#include "hashes/sha256.h"

typedef sha256_context_t dtls_hash_ctx;
typedef sha256_context_t dtls_sha256_ctx;
#define DTLS_HASH_CTX_SIZE sizeof(sha256_context_t)
#define DTLS_SHA256_DIGEST_LENGTH (SHA256_DIGEST_LENGTH)

#define dtls_sha256_init(Ctx)             sha256_init((Ctx))
#define dtls_sha256_update(Ctx,Input,Len) sha256_update((Ctx), (Input), (Len))
#define dtls_sha256_final(Buf,Ctx)        sha256_final((Ctx), (Buf))

#else /* RIOT_VERSION */

/** Aaron D. Gifford's implementation of SHA256
 *  see http://www.aarongifford.com/ */
#include "sha2/sha2.h"

typedef dtls_sha256_ctx dtls_hash_ctx;
#define DTLS_HASH_CTX_SIZE sizeof(dtls_sha256_ctx)

#endif /* RIOT_VERSION */


typedef dtls_hash_ctx *dtls_hash_t;


static inline void
dtls_hash_init(dtls_hash_t ctx) {
  dtls_sha256_init((dtls_sha256_ctx *)ctx);
}

static inline void 
dtls_hash_update(dtls_hash_t ctx, const unsigned char *input, size_t len) {
  dtls_sha256_update((dtls_sha256_ctx *)ctx, input, len);
}

static inline size_t
dtls_hash_finalize(unsigned char *buf, dtls_hash_t ctx) {
  dtls_sha256_final(buf, (dtls_sha256_ctx *)ctx);
  return DTLS_SHA256_DIGEST_LENGTH;
}
#endif /* WITH_SHA256 */

void dtls_hmac_storage_init(void);

/**
 * \defgroup HMAC Keyed-Hash Message Authentication Code (HMAC)
 * NIST Standard FIPS 198 describes the Keyed-Hash Message Authentication 
 * Code (HMAC) which is used as hash function for the DTLS PRF.
 * @{
 */

#define DTLS_HMAC_BLOCKSIZE   64	/**< size of hmac blocks */
#define DTLS_HMAC_DIGEST_SIZE 32	/**< digest size (for SHA-256) */
#define DTLS_HMAC_MAX         64	/**< max number of bytes in digest */

/**
 * List of known hash functions for use in dtls_hmac_init(). The
 * identifiers are the same as the HashAlgorithm defined in 
 * <a href="http://tools.ietf.org/html/rfc5246#section-7.4.1.4.1"
 * >Section 7.4.1.4.1 of RFC 5246</a>.
 */
typedef enum { 
  HASH_NONE=0, HASH_MD5=1, HASH_SHA1=2, HASH_SHA224=3,
  HASH_SHA256=4, HASH_SHA384=5, HASH_SHA512=6
} dtls_hashfunc_t;

/**
 * Context for HMAC generation. This object is initialized with
 * dtls_hmac_init() and must be passed to dtls_hmac_update() and
 * dtls_hmac_finalize(). Once, finalized, the component \c H is
 * invalid and must be initialized again with dtls_hmac_init() before
 * the structure can be used again. 
 */
typedef struct {
  unsigned char pad[DTLS_HMAC_BLOCKSIZE]; /**< ipad and opad storage */
  dtls_hash_ctx data;		          /**< context for hash function */
} dtls_hmac_context_t;

/**
 * Initializes an existing HMAC context. 
 *
 * @param ctx The HMAC context to initialize.
 * @param key    The secret key.
 * @param klen   The length of @p key.
 */
void dtls_hmac_init(dtls_hmac_context_t *ctx, const unsigned char *key, size_t klen);

/**
 * Allocates a new HMAC context \p ctx with the given secret key.
 * This function returns \c 1 if \c ctx has been set correctly, or \c
 * 0 or \c -1 otherwise. Note that this function allocates new storage
 * that must be released by dtls_hmac_free().
 *
 * \param key    The secret key.
 * \param klen   The length of \p key.
 * \return A new dtls_hmac_context_t object or @c NULL on error
 */
dtls_hmac_context_t *dtls_hmac_new(const unsigned char *key, size_t klen);

/**
 * Releases the storage for @p ctx that has been allocated by
 * dtls_hmac_new().
 *
 * @param ctx The dtls_hmac_context_t to free. 
 */
void dtls_hmac_free(dtls_hmac_context_t *ctx);

/**
 * Updates the HMAC context with data from \p input. 
 * 
 * \param ctx    The HMAC context.
 * \param input  The input data.
 * \param ilen   Size of \p input.
 */
void dtls_hmac_update(dtls_hmac_context_t *ctx,
		      const unsigned char *input, size_t ilen);

/** 
 * Completes the HMAC generation and writes the result to the given
 * output parameter \c result. The buffer must be large enough to hold
 * the message digest created by the actual hash function. If in
 * doubt, use \c DTLS_HMAC_MAX. The function returns the number of
 * bytes written to \c result. 
 *
 * \param ctx    The HMAC context.
 * \param result Output parameter where the MAC is written to.
 * \return Length of the MAC written to \p result.
 */
int dtls_hmac_finalize(dtls_hmac_context_t *ctx, unsigned char *result);

/**@}*/

#endif /* _DTLS_HMAC_H_ */

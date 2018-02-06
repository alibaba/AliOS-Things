/**
 * \file hash.h
 *
 * \brief Generic message digest wrapper
 *
 * \author Adriaan de Jong <dejong@fox-it.com>
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
#ifndef MBEDTLS_HASH_H
#define MBEDTLS_HASH_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque struct defined in md_internal.h
 */
typedef struct mbedtls_md_info_t mbedtls_hash_info_t;

#define mbedtls_hash_context_t mbedtls_md_context_t

/**
 * \brief           Returns the message digest information associated with the
 *                  given digest type.
 *
 * \param md_type   type of digest to search for.
 *
 * \return          The message digest information associated with md_type or
 *                  NULL if not found.
 */
const mbedtls_hash_info_t *mbedtls_hash_info_from_type( mbedtls_md_type_t md_type );

/**
 * \brief           Initialize a md_context (as NONE)
 *                  This should always be called first.
 *                  Prepares the context for mbedtls_md_setup() or mbedtls_md_free().
 */
void mbedtls_hash_init( mbedtls_hash_context_t *ctx );

/**
 * \brief           Free and clear the internal structures of ctx.
 *                  Can be called at any time after mbedtls_hash_init().
 *                  Mandatory once mbedtls_md_setup() has been called.
 */
void mbedtls_hash_free( mbedtls_hash_context_t *ctx );

#if ! defined(MBEDTLS_DEPRECATED_REMOVED)
#if defined(MBEDTLS_DEPRECATED_WARNING)
#define MBEDTLS_DEPRECATED    __attribute__((deprecated))
#else
#define MBEDTLS_DEPRECATED
#endif
/**
 * \brief           Select MD to use and allocate internal structures.
 *                  Should be called after mbedtls_md_init() or mbedtls_md_free().
 *                  Makes it necessary to call mbedtls_md_free() later.
 *
 * \deprecated      Superseded by mbedtls_md_setup() in 2.0.0
 *
 * \param ctx       Context to set up.
 * \param md_info   Message digest to use.
 *
 * \returns         \c 0 on success,
 *                  \c MBEDTLS_ERR_MD_BAD_INPUT_DATA on parameter failure,
 *                  \c MBEDTLS_ERR_MD_ALLOC_FAILED memory allocation failure.
 */
int mbedtls_hash_init_ctx( mbedtls_hash_context_t *ctx, const mbedtls_hash_info_t *md_info ) MBEDTLS_DEPRECATED;
#undef MBEDTLS_DEPRECATED
#endif /* MBEDTLS_DEPRECATED_REMOVED */

/**
 * \brief           Select MD to use and allocate internal structures.
 *                  Should be called after mbedtls_md_init() or mbedtls_md_free().
 *                  Makes it necessary to call mbedtls_md_free() later.
 *
 * \param ctx       Context to set up.
 * \param md_info   Message digest to use.
 * \param hmac      0 to save some memory if HMAC will not be used,
 *                  non-zero is HMAC is going to be used with this context.
 *
 * \returns         \c 0 on success,
 *                  \c MBEDTLS_ERR_MD_BAD_INPUT_DATA on parameter failure,
 *                  \c MBEDTLS_ERR_MD_ALLOC_FAILED memory allocation failure.
 */
int mbedtls_hash_setup( mbedtls_hash_context_t *ctx, const mbedtls_hash_info_t *md_info, int hmac );

/**
 * \brief           Returns the size of the message digest output.
 *
 * \param md_info   message digest info
 *
 * \return          size of the message digest output in bytes.
 */
unsigned char mbedtls_hash_get_size( const mbedtls_hash_info_t *md_info );

/**
 * \brief           Returns the type of the message digest output.
 *
 * \param md_info   message digest info
 *
 * \return          type of the message digest output.
 */
mbedtls_md_type_t mbedtls_hash_get_type( const mbedtls_hash_info_t *md_info );

/**
 * \brief           Prepare the context to digest a new message.
 *                  Generally called after mbedtls_md_setup() or mbedtls_hash_finish().
 *                  Followed by mbedtls_hash_update().
 *
 * \param ctx       generic message digest context.
 *
 * \returns         0 on success, MBEDTLS_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int mbedtls_hash_starts( mbedtls_hash_context_t *ctx );

/**
 * \brief           Generic message digest process buffer
 *                  Called between mbedtls_hash_starts() and mbedtls_hash_finish().
 *                  May be called repeatedly.
 *
 * \param ctx       Generic message digest context
 * \param input     buffer holding the  datal
 * \param ilen      length of the input data
 *
 * \returns         0 on success, MBEDTLS_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int mbedtls_hash_update( mbedtls_hash_context_t *ctx, const unsigned char *input, size_t ilen );

/**
 * \brief           Generic message digest final digest
 *                  Called after mbedtls_hash_update().
 *                  Usually followed by mbedtls_md_free() or mbedtls_hash_starts().
 *
 * \param ctx       Generic message digest context
 * \param output    Generic message digest checksum result
 *
 * \returns         0 on success, MBEDTLS_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int mbedtls_hash_finish( mbedtls_hash_context_t *ctx, unsigned char *output );

/**
 * \brief          Output = message_digest( input buffer )
 *
 * \param md_info  message digest info
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   Generic message digest checksum result
 *
 * \returns        0 on success, MBEDTLS_ERR_MD_BAD_INPUT_DATA if parameter
 *                 verification fails.
 */
int mbedtls_hash( const mbedtls_hash_info_t *md_info, const unsigned char *input, size_t ilen,
        unsigned char *output );

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_HASH_H */

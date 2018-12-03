/**
* Copyright (C) 2017  Alibaba Group Holding Limited.
**/

#ifndef ALI_ALGO_HASH_H
#define ALI_ALGO_HASH_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque struct defined in md_internal.h
 */
typedef struct ali_algo_md_info_t ali_algo_hash_info_t;

#define ali_algo_hash_context_t ali_algo_md_context_t

/**
 * \brief           Returns the message digest information associated with the
 *                  given digest type.
 *
 * \param md_type   type of digest to search for.
 *
 * \return          The message digest information associated with md_type or
 *                  NULL if not found.
 */
const ali_algo_hash_info_t *ali_algo_hash_info_from_type( ali_algo_md_type_t md_type );

/**
 * \brief           Initialize a md_context (as NONE)
 *                  This should always be called first.
 *                  Prepares the context for ali_algo_md_setup() or ali_algo_md_free().
 */
void ali_algo_hash_init( ali_algo_hash_context_t *ctx );

/**
 * \brief           Free and clear the internal structures of ctx.
 *                  Can be called at any time after ali_algo_hash_init().
 *                  Mandatory once ali_algo_md_setup() has been called.
 */
void ali_algo_hash_free( ali_algo_hash_context_t *ctx );

#if ! defined(ALI_ALGO_DEPRECATED_REMOVED)
#if defined(ALI_ALGO_DEPRECATED_WARNING)
#define ALI_ALGO_DEPRECATED    __attribute__((deprecated))
#else
#define ALI_ALGO_DEPRECATED
#endif
/**
 * \brief           Select MD to use and allocate internal structures.
 *                  Should be called after ali_algo_md_init() or ali_algo_md_free().
 *                  Makes it necessary to call ali_algo_md_free() later.
 *
 * \deprecated      Superseded by ali_algo_md_setup() in 2.0.0
 *
 * \param ctx       Context to set up.
 * \param md_info   Message digest to use.
 *
 * \returns         \c 0 on success,
 *                  \c ALI_ALGO_ERR_MD_BAD_INPUT_DATA on parameter failure,
 *                  \c ALI_ALGO_ERR_MD_ALLOC_FAILED memory allocation failure.
 */
int ali_algo_hash_init_ctx( ali_algo_hash_context_t *ctx, const ali_algo_hash_info_t *md_info ) ALI_ALGO_DEPRECATED;
#undef ALI_ALGO_DEPRECATED
#endif /* ALI_ALGO_DEPRECATED_REMOVED */

/**
 * \brief           Select MD to use and allocate internal structures.
 *                  Should be called after ali_algo_md_init() or ali_algo_md_free().
 *                  Makes it necessary to call ali_algo_md_free() later.
 *
 * \param ctx       Context to set up.
 * \param md_info   Message digest to use.
 * \param hmac      0 to save some memory if HMAC will not be used,
 *                  non-zero is HMAC is going to be used with this context.
 *
 * \returns         \c 0 on success,
 *                  \c ALI_ALGO_ERR_MD_BAD_INPUT_DATA on parameter failure,
 *                  \c ALI_ALGO_ERR_MD_ALLOC_FAILED memory allocation failure.
 */
int ali_algo_hash_setup( ali_algo_hash_context_t *ctx, const ali_algo_hash_info_t *md_info, int hmac );

/**
 * \brief           Returns the size of the message digest output.
 *
 * \param md_info   message digest info
 *
 * \return          size of the message digest output in bytes.
 */
unsigned char ali_algo_hash_get_size( const ali_algo_hash_info_t *md_info );

/**
 * \brief           Returns the type of the message digest output.
 *
 * \param md_info   message digest info
 *
 * \return          type of the message digest output.
 */
ali_algo_md_type_t ali_algo_hash_get_type( const ali_algo_hash_info_t *md_info );

/**
 * \brief           Prepare the context to digest a new message.
 *                  Generally called after ali_algo_md_setup() or ali_algo_hash_finish().
 *                  Followed by ali_algo_hash_update().
 *
 * \param ctx       generic message digest context.
 *
 * \returns         0 on success, ALI_ALGO_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int ali_algo_hash_starts( ali_algo_hash_context_t *ctx );

/**
 * \brief           Generic message digest process buffer
 *                  Called between ali_algo_hash_starts() and ali_algo_hash_finish().
 *                  May be called repeatedly.
 *
 * \param ctx       Generic message digest context
 * \param input     buffer holding the  datal
 * \param ilen      length of the input data
 *
 * \returns         0 on success, ALI_ALGO_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int ali_algo_hash_update( ali_algo_hash_context_t *ctx, const unsigned char *input, size_t ilen );

/**
 * \brief           Generic message digest final digest
 *                  Called after ali_algo_hash_update().
 *                  Usually followed by ali_algo_md_free() or ali_algo_hash_starts().
 *
 * \param ctx       Generic message digest context
 * \param output    Generic message digest checksum result
 *
 * \returns         0 on success, ALI_ALGO_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int ali_algo_hash_finish( ali_algo_hash_context_t *ctx, unsigned char *output );

/**
 * \brief          Output = message_digest( input buffer )
 *
 * \param md_info  message digest info
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   Generic message digest checksum result
 *
 * \returns        0 on success, ALI_ALGO_ERR_MD_BAD_INPUT_DATA if parameter
 *                 verification fails.
 */
int ali_algo_hash( const ali_algo_hash_info_t *md_info, const unsigned char *input, size_t ilen,
        unsigned char *output );

#ifdef __cplusplus
}
#endif

#endif /* ALI_ALGO_HASH_H */

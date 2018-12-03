/**
* Copyright (C) 2017  Alibaba Group Holding Limited.
**/

#ifndef ALI_ALGO_MD_H
#define ALI_ALGO_MD_H

#include <stddef.h>

#define ALI_ALGO_ERR_MD_FEATURE_UNAVAILABLE                -0x5080  /**< The selected feature is not available. */
#define ALI_ALGO_ERR_MD_BAD_INPUT_DATA                     -0x5100  /**< Bad input parameters to function. */
#define ALI_ALGO_ERR_MD_ALLOC_FAILED                       -0x5180  /**< Failed to allocate memory. */
#define ALI_ALGO_ERR_MD_FILE_IO_ERROR                      -0x5200  /**< Opening or reading of file failed. */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ALI_ALGO_MD_NONE=0,
    ALI_ALGO_MD_MD2,
    ALI_ALGO_MD_MD4,
    ALI_ALGO_MD_MD5,
    ALI_ALGO_MD_SHA1,
    ALI_ALGO_MD_SHA224,
    ALI_ALGO_MD_SHA256,
    ALI_ALGO_MD_SHA384,
    ALI_ALGO_MD_SHA512,
    ALI_ALGO_MD_RIPEMD160,
} ali_algo_md_type_t;

#if defined(ALI_ALGO_SHA512_C)
#define ALI_ALGO_MD_MAX_SIZE         64  /* longest known is SHA512 */
#else
#define ALI_ALGO_MD_MAX_SIZE         32  /* longest known is SHA256 or less */
#endif

/**
 * Opaque struct defined in md_internal.h
 */
typedef struct ali_algo_md_info_t ali_algo_md_info_t;

/**
 * Generic message digest context.
 */
typedef struct {
    /** Information about the associated message digest */
    const ali_algo_md_info_t *md_info;

    /** Digest-specific context */
    void *md_ctx;

    /** HMAC part of the context */
    void *hmac_ctx;
} ali_algo_md_context_t;

/**
 * \brief Returns the list of digests supported by the generic digest module.
 *
 * \return          a statically allocated array of digests, the last entry
 *                  is 0.
 */
const int *ali_algo_md_list( void );

/**
 * \brief           Returns the message digest information associated with the
 *                  given digest name.
 *
 * \param md_name   Name of the digest to search for.
 *
 * \return          The message digest information associated with md_name or
 *                  NULL if not found.
 */
const ali_algo_md_info_t *ali_algo_md_info_from_string( const char *md_name );

/**
 * \brief           Returns the message digest information associated with the
 *                  given digest type.
 *
 * \param md_type   type of digest to search for.
 *
 * \return          The message digest information associated with md_type or
 *                  NULL if not found.
 */
const ali_algo_md_info_t *ali_algo_md_info_from_type( ali_algo_md_type_t md_type );

/**
 * \brief           Initialize a md_context (as NONE)
 *                  This should always be called first.
 *                  Prepares the context for ali_algo_md_setup() or ali_algo_md_free().
 */
void ali_algo_md_init( ali_algo_md_context_t *ctx );

/**
 * \brief           Free and clear the internal structures of ctx.
 *                  Can be called at any time after ali_algo_md_init().
 *                  Mandatory once ali_algo_md_setup() has been called.
 */
void ali_algo_md_free( ali_algo_md_context_t *ctx );

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
int ali_algo_md_init_ctx( ali_algo_md_context_t *ctx, const ali_algo_md_info_t *md_info ) ALI_ALGO_DEPRECATED;
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
int ali_algo_md_setup( ali_algo_md_context_t *ctx, const ali_algo_md_info_t *md_info, int hmac );

/**
 * \brief           Clone the state of an MD context
 *
 * \note            The two contexts must have been setup to the same type
 *                  (cloning from SHA-256 to SHA-512 make no sense).
 *
 * \warning         Only clones the MD state, not the HMAC state! (for now)
 *
 * \param dst       The destination context
 * \param src       The context to be cloned
 *
 * \return          \c 0 on success,
 *                  \c ALI_ALGO_ERR_MD_BAD_INPUT_DATA on parameter failure.
 */
int ali_algo_md_clone( ali_algo_md_context_t *dst,
                      const ali_algo_md_context_t *src );

/**
 * \brief           Returns the size of the message digest output.
 *
 * \param md_info   message digest info
 *
 * \return          size of the message digest output in bytes.
 */
unsigned char ali_algo_md_get_size( const ali_algo_md_info_t *md_info );

/**
 * \brief           Returns the type of the message digest output.
 *
 * \param md_info   message digest info
 *
 * \return          type of the message digest output.
 */
ali_algo_md_type_t ali_algo_md_get_type( const ali_algo_md_info_t *md_info );

/**
 * \brief           Returns the name of the message digest output.
 *
 * \param md_info   message digest info
 *
 * \return          name of the message digest output.
 */
const char *ali_algo_md_get_name( const ali_algo_md_info_t *md_info );

/**
 * \brief           Prepare the context to digest a new message.
 *                  Generally called after ali_algo_md_setup() or ali_algo_md_finish().
 *                  Followed by ali_algo_md_update().
 *
 * \param ctx       generic message digest context.
 *
 * \returns         0 on success, ALI_ALGO_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int ali_algo_md_starts( ali_algo_md_context_t *ctx );

/**
 * \brief           Generic message digest process buffer
 *                  Called between ali_algo_md_starts() and ali_algo_md_finish().
 *                  May be called repeatedly.
 *
 * \param ctx       Generic message digest context
 * \param input     buffer holding the  datal
 * \param ilen      length of the input data
 *
 * \returns         0 on success, ALI_ALGO_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int ali_algo_md_update( ali_algo_md_context_t *ctx, const unsigned char *input, size_t ilen );

/**
 * \brief           Generic message digest final digest
 *                  Called after ali_algo_md_update().
 *                  Usually followed by ali_algo_md_free() or ali_algo_md_starts().
 *
 * \param ctx       Generic message digest context
 * \param output    Generic message digest checksum result
 *
 * \returns         0 on success, ALI_ALGO_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int ali_algo_md_finish( ali_algo_md_context_t *ctx, unsigned char *output );

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
int ali_algo_md( const ali_algo_md_info_t *md_info, const unsigned char *input, size_t ilen,
        unsigned char *output );

#if defined(ALI_ALGO_FS_IO)
/**
 * \brief          Output = message_digest( file contents )
 *
 * \param md_info  message digest info
 * \param path     input file name
 * \param output   generic message digest checksum result
 *
 * \return         0 if successful,
 *                 ALI_ALGO_ERR_MD_FILE_IO_ERROR if file input failed,
 *                 ALI_ALGO_ERR_MD_BAD_INPUT_DATA if md_info was NULL.
 */
int ali_algo_md_file( const ali_algo_md_info_t *md_info, const char *path,
                     unsigned char *output );
#endif /* ALI_ALGO_FS_IO */

/**
 * \brief           Set HMAC key and prepare to authenticate a new message.
 *                  Usually called after ali_algo_md_setup() or ali_algo_md_hmac_finish().
 *
 * \param ctx       HMAC context
 * \param key       HMAC secret key
 * \param keylen    length of the HMAC key in bytes
 *
 * \returns         0 on success, ALI_ALGO_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int ali_algo_md_hmac_starts( ali_algo_md_context_t *ctx, const unsigned char *key,
                    size_t keylen );

/**
 * \brief           Generic HMAC process buffer.
 *                  Called between ali_algo_md_hmac_starts() or ali_algo_md_hmac_reset()
 *                  and ali_algo_md_hmac_finish().
 *                  May be called repeatedly.
 *
 * \param ctx       HMAC context
 * \param input     buffer holding the  data
 * \param ilen      length of the input data
 *
 * \returns         0 on success, ALI_ALGO_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int ali_algo_md_hmac_update( ali_algo_md_context_t *ctx, const unsigned char *input,
                    size_t ilen );

/**
 * \brief           Output HMAC.
 *                  Called after ali_algo_md_hmac_update().
 *                  Usually followed by ali_algo_md_hmac_reset(),
 *                  ali_algo_md_hmac_starts(), or ali_algo_md_free().
 *
 * \param ctx       HMAC context
 * \param output    Generic HMAC checksum result
 *
 * \returns         0 on success, ALI_ALGO_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int ali_algo_md_hmac_finish( ali_algo_md_context_t *ctx, unsigned char *output);

/**
 * \brief           Prepare to authenticate a new message with the same key.
 *                  Called after ali_algo_md_hmac_finish() and before
 *                  ali_algo_md_hmac_update().
 *
 * \param ctx       HMAC context to be reset
 *
 * \returns         0 on success, ALI_ALGO_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int ali_algo_md_hmac_reset( ali_algo_md_context_t *ctx );

/**
 * \brief          Output = Generic_HMAC( hmac key, input buffer )
 *
 * \param md_info  message digest info
 * \param key      HMAC secret key
 * \param keylen   length of the HMAC key in bytes
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   Generic HMAC-result
 *
 * \returns        0 on success, ALI_ALGO_ERR_MD_BAD_INPUT_DATA if parameter
 *                 verification fails.
 */
int ali_algo_md_hmac( const ali_algo_md_info_t *md_info, const unsigned char *key, size_t keylen,
                const unsigned char *input, size_t ilen,
                unsigned char *output );

/* Internal use */
int ali_algo_md_process( ali_algo_md_context_t *ctx, const unsigned char *data );

#ifdef __cplusplus
}
#endif

#endif /* ALI_ALGO_MD_H */

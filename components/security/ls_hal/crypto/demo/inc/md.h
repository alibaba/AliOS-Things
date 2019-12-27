/**
* Copyright (C) 2017  Alibaba Group Holding Limited.
**/

#ifndef __MD_H
#define __MD_H

#include <stddef.h>

#define HAL_ERR_MD_FEATURE_UNAVAILABLE                -0x5080  /**< The selected feature is not available. */
#define HAL_ERR_MD_BAD_INPUT_DATA                     -0x5100  /**< Bad input parameters to function. */
#define HAL_ERR_MD_ALLOC_FAILED                       -0x5180  /**< Failed to allocate memory. */
#define HAL_ERR_MD_FILE_IO_ERROR                      -0x5200  /**< Opening or reading of file failed. */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_MD_NONE=0,
    HAL_MD_MD2,
    HAL_MD_MD4,
    HAL_MD_MD5,
    HAL_MD_SHA1,
    HAL_MD_SHA224,
    HAL_MD_SHA256,
    HAL_MD_SHA384,
    HAL_MD_SHA512,
    HAL_MD_RIPEMD160,
} impl_md_type_t;

#if defined(HAL_SHA512_C)
#define HAL_MD_MAX_SIZE         64  /* longest known is SHA512 */
#else
#define HAL_MD_MAX_SIZE         32  /* longest known is SHA256 or less */
#endif

/**
 * Opaque struct defined in md_internal.h
 */
typedef struct impl_md_info_t impl_md_info_t;

/**
 * Generic message digest context.
 */
typedef struct {
    /** Information about the associated message digest */
    const impl_md_info_t *md_info;

    /** Digest-specific context */
    void *md_ctx;

    /** HMAC part of the context */
    void *hmac_ctx;
} impl_md_context_t;

/**
 * \brief Returns the list of digests supported by the generic digest module.
 *
 * \return          a statically allocated array of digests, the last entry
 *                  is 0.
 */
const int *impl_md_list( void );

/**
 * \brief           Returns the message digest information associated with the
 *                  given digest name.
 *
 * \param md_name   Name of the digest to search for.
 *
 * \return          The message digest information associated with md_name or
 *                  NULL if not found.
 */
const impl_md_info_t *impl_md_info_from_string( const char *md_name );

/**
 * \brief           Returns the message digest information associated with the
 *                  given digest type.
 *
 * \param md_type   type of digest to search for.
 *
 * \return          The message digest information associated with md_type or
 *                  NULL if not found.
 */
const impl_md_info_t *impl_md_info_from_type( impl_md_type_t md_type );

/**
 * \brief           Initialize a md_context (as NONE)
 *                  This should always be called first.
 *                  Prepares the context for impl_md_setup() or impl_md_free().
 */
void impl_md_init( impl_md_context_t *ctx );

/**
 * \brief           Free and clear the internal structures of ctx.
 *                  Can be called at any time after impl_md_init().
 *                  Mandatory once impl_md_setup() has been called.
 */
void impl_md_free( impl_md_context_t *ctx );

#if ! defined(HAL_DEPRECATED_REMOVED)
#if defined(HAL_DEPRECATED_WARNING)
#define HAL_DEPRECATED    __attribute__((deprecated))
#else
#define HAL_DEPRECATED
#endif
/**
 * \brief           Select MD to use and allocate internal structures.
 *                  Should be called after impl_md_init() or impl_md_free().
 *                  Makes it necessary to call impl_md_free() later.
 *
 * \deprecated      Superseded by impl_md_setup() in 2.0.0
 *
 * \param ctx       Context to set up.
 * \param md_info   Message digest to use.
 *
 * \returns         \c 0 on success,
 *                  \c HAL_ERR_MD_BAD_INPUT_DATA on parameter failure,
 *                  \c HAL_ERR_MD_ALLOC_FAILED memory allocation failure.
 */
int impl_md_init_ctx( impl_md_context_t *ctx, const impl_md_info_t *md_info ) HAL_DEPRECATED;
#undef HAL_DEPRECATED
#endif /* HAL_DEPRECATED_REMOVED */

/**
 * \brief           Select MD to use and allocate internal structures.
 *                  Should be called after impl_md_init() or impl_md_free().
 *                  Makes it necessary to call impl_md_free() later.
 *
 * \param ctx       Context to set up.
 * \param md_info   Message digest to use.
 * \param hmac      0 to save some memory if HMAC will not be used,
 *                  non-zero is HMAC is going to be used with this context.
 *
 * \returns         \c 0 on success,
 *                  \c HAL_ERR_MD_BAD_INPUT_DATA on parameter failure,
 *                  \c HAL_ERR_MD_ALLOC_FAILED memory allocation failure.
 */
int impl_md_setup( impl_md_context_t *ctx, const impl_md_info_t *md_info, int hmac );

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
 *                  \c HAL_ERR_MD_BAD_INPUT_DATA on parameter failure.
 */
int impl_md_clone( impl_md_context_t *dst,
                      const impl_md_context_t *src );

/**
 * \brief           Returns the size of the message digest output.
 *
 * \param md_info   message digest info
 *
 * \return          size of the message digest output in bytes.
 */
unsigned char impl_md_get_size( const impl_md_info_t *md_info );

/**
 * \brief           Returns the type of the message digest output.
 *
 * \param md_info   message digest info
 *
 * \return          type of the message digest output.
 */
impl_md_type_t impl_md_get_type( const impl_md_info_t *md_info );

/**
 * \brief           Returns the name of the message digest output.
 *
 * \param md_info   message digest info
 *
 * \return          name of the message digest output.
 */
const char *impl_md_get_name( const impl_md_info_t *md_info );

/**
 * \brief           Prepare the context to digest a new message.
 *                  Generally called after impl_md_setup() or impl_md_finish().
 *                  Followed by impl_md_update().
 *
 * \param ctx       generic message digest context.
 *
 * \returns         0 on success, HAL_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int impl_md_starts( impl_md_context_t *ctx );

/**
 * \brief           Generic message digest process buffer
 *                  Called between impl_md_starts() and impl_md_finish().
 *                  May be called repeatedly.
 *
 * \param ctx       Generic message digest context
 * \param input     buffer holding the  datal
 * \param ilen      length of the input data
 *
 * \returns         0 on success, HAL_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int impl_md_update( impl_md_context_t *ctx, const unsigned char *input, size_t ilen );

/**
 * \brief           Generic message digest final digest
 *                  Called after impl_md_update().
 *                  Usually followed by impl_md_free() or impl_md_starts().
 *
 * \param ctx       Generic message digest context
 * \param output    Generic message digest checksum result
 *
 * \returns         0 on success, HAL_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int impl_md_finish( impl_md_context_t *ctx, unsigned char *output );

/**
 * \brief          Output = message_digest( input buffer )
 *
 * \param md_info  message digest info
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   Generic message digest checksum result
 *
 * \returns        0 on success, HAL_ERR_MD_BAD_INPUT_DATA if parameter
 *                 verification fails.
 */
int impl_md( const impl_md_info_t *md_info, const unsigned char *input, size_t ilen,
        unsigned char *output );

#if defined(HAL_FS_IO)
/**
 * \brief          Output = message_digest( file contents )
 *
 * \param md_info  message digest info
 * \param path     input file name
 * \param output   generic message digest checksum result
 *
 * \return         0 if successful,
 *                 HAL_ERR_MD_FILE_IO_ERROR if file input failed,
 *                 HAL_ERR_MD_BAD_INPUT_DATA if md_info was NULL.
 */
int impl_md_file( const impl_md_info_t *md_info, const char *path,
                     unsigned char *output );
#endif /* HAL_FS_IO */

/**
 * \brief           Set HMAC key and prepare to authenticate a new message.
 *                  Usually called after impl_md_setup() or impl_md_hmac_finish().
 *
 * \param ctx       HMAC context
 * \param key       HMAC secret key
 * \param keylen    length of the HMAC key in bytes
 *
 * \returns         0 on success, HAL_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int impl_md_hmac_starts( impl_md_context_t *ctx, const unsigned char *key,
                    size_t keylen );

/**
 * \brief           Generic HMAC process buffer.
 *                  Called between impl_md_hmac_starts() or impl_md_hmac_reset()
 *                  and impl_md_hmac_finish().
 *                  May be called repeatedly.
 *
 * \param ctx       HMAC context
 * \param input     buffer holding the  data
 * \param ilen      length of the input data
 *
 * \returns         0 on success, HAL_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int impl_md_hmac_update( impl_md_context_t *ctx, const unsigned char *input,
                    size_t ilen );

/**
 * \brief           Output HMAC.
 *                  Called after impl_md_hmac_update().
 *                  Usually followed by impl_md_hmac_reset(),
 *                  impl_md_hmac_starts(), or impl_md_free().
 *
 * \param ctx       HMAC context
 * \param output    Generic HMAC checksum result
 *
 * \returns         0 on success, HAL_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int impl_md_hmac_finish( impl_md_context_t *ctx, unsigned char *output);

/**
 * \brief           Prepare to authenticate a new message with the same key.
 *                  Called after impl_md_hmac_finish() and before
 *                  impl_md_hmac_update().
 *
 * \param ctx       HMAC context to be reset
 *
 * \returns         0 on success, HAL_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int impl_md_hmac_reset( impl_md_context_t *ctx );

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
 * \returns        0 on success, HAL_ERR_MD_BAD_INPUT_DATA if parameter
 *                 verification fails.
 */
int impl_md_hmac( const impl_md_info_t *md_info, const unsigned char *key, size_t keylen,
                const unsigned char *input, size_t ilen,
                unsigned char *output );

/* Internal use */
int impl_md_process( impl_md_context_t *ctx, const unsigned char *data );

#ifdef __cplusplus
}
#endif

#endif /* HAL_MD_H */

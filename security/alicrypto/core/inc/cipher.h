/**
* Copyright (C) 2017  Alibaba Group Holding Limited.
**/


#ifndef ALI_ALGO_CIPHER_H
#define ALI_ALGO_CIPHER_H

#include "../../config/config.h"
#include <stddef.h>

#if defined(ALI_ALGO_CIPHER_MODE_CBC)
#define ALI_ALGO_CIPHER_MODE_WITH_PADDING
#endif


#define ALI_ALGO_ERR_CIPHER_FEATURE_UNAVAILABLE            -0x6080  /**< The selected feature is not available. */
#define ALI_ALGO_ERR_CIPHER_BAD_INPUT_DATA                 -0x6100  /**< Bad input parameters to function. */
#define ALI_ALGO_ERR_CIPHER_ALLOC_FAILED                   -0x6180  /**< Failed to allocate memory. */
#define ALI_ALGO_ERR_CIPHER_INVALID_PADDING                -0x6200  /**< Input data contains invalid padding and is rejected. */
#define ALI_ALGO_ERR_CIPHER_FULL_BLOCK_EXPECTED            -0x6280  /**< Decryption of block requires a full block. */
#define ALI_ALGO_ERR_CIPHER_AUTH_FAILED                    -0x6300  /**< Authentication failed (for AEAD modes). */
#define ALI_ALGO_ERR_CIPHER_INVALID_CONTEXT              -0x6380  /**< The context is invalid, eg because it was free()ed. */

#define ALI_ALGO_CIPHER_VARIABLE_IV_LEN     0x01    /**< Cipher accepts IVs of variable length */
#define ALI_ALGO_CIPHER_VARIABLE_KEY_LEN    0x02    /**< Cipher accepts keys of variable length */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ALI_ALGO_CIPHER_ID_NONE = 0,
    ALI_ALGO_CIPHER_ID_NULL,
    ALI_ALGO_CIPHER_ID_AES,
    ALI_ALGO_CIPHER_ID_DES,
    ALI_ALGO_CIPHER_ID_3DES,
    ALI_ALGO_CIPHER_ID_CAMELLIA,
    ALI_ALGO_CIPHER_ID_BLOWFISH,
    ALI_ALGO_CIPHER_ID_ARC4,
} ali_algo_cipher_id_t;

typedef enum {
    ALI_ALGO_CIPHER_NONE = 0,
    ALI_ALGO_CIPHER_NULL,
    ALI_ALGO_CIPHER_AES_128_ECB,
    ALI_ALGO_CIPHER_AES_192_ECB,
    ALI_ALGO_CIPHER_AES_256_ECB,
    ALI_ALGO_CIPHER_AES_128_CBC,
    ALI_ALGO_CIPHER_AES_192_CBC,
    ALI_ALGO_CIPHER_AES_256_CBC,
    ALI_ALGO_CIPHER_AES_128_CFB128,
    ALI_ALGO_CIPHER_AES_192_CFB128,
    ALI_ALGO_CIPHER_AES_256_CFB128,
    ALI_ALGO_CIPHER_AES_128_CTR,
    ALI_ALGO_CIPHER_AES_192_CTR,
    ALI_ALGO_CIPHER_AES_256_CTR,
    ALI_ALGO_CIPHER_AES_128_GCM,
    ALI_ALGO_CIPHER_AES_192_GCM,
    ALI_ALGO_CIPHER_AES_256_GCM,
    ALI_ALGO_CIPHER_CAMELLIA_128_ECB,
    ALI_ALGO_CIPHER_CAMELLIA_192_ECB,
    ALI_ALGO_CIPHER_CAMELLIA_256_ECB,
    ALI_ALGO_CIPHER_CAMELLIA_128_CBC,
    ALI_ALGO_CIPHER_CAMELLIA_192_CBC,
    ALI_ALGO_CIPHER_CAMELLIA_256_CBC,
    ALI_ALGO_CIPHER_CAMELLIA_128_CFB128,
    ALI_ALGO_CIPHER_CAMELLIA_192_CFB128,
    ALI_ALGO_CIPHER_CAMELLIA_256_CFB128,
    ALI_ALGO_CIPHER_CAMELLIA_128_CTR,
    ALI_ALGO_CIPHER_CAMELLIA_192_CTR,
    ALI_ALGO_CIPHER_CAMELLIA_256_CTR,
    ALI_ALGO_CIPHER_CAMELLIA_128_GCM,
    ALI_ALGO_CIPHER_CAMELLIA_192_GCM,
    ALI_ALGO_CIPHER_CAMELLIA_256_GCM,
    ALI_ALGO_CIPHER_DES_ECB,
    ALI_ALGO_CIPHER_DES_CBC,
    ALI_ALGO_CIPHER_DES_EDE_ECB,
    ALI_ALGO_CIPHER_DES_EDE_CBC,
    ALI_ALGO_CIPHER_DES_EDE3_ECB,
    ALI_ALGO_CIPHER_DES_EDE3_CBC,
    ALI_ALGO_CIPHER_BLOWFISH_ECB,
    ALI_ALGO_CIPHER_BLOWFISH_CBC,
    ALI_ALGO_CIPHER_BLOWFISH_CFB64,
    ALI_ALGO_CIPHER_BLOWFISH_CTR,
    ALI_ALGO_CIPHER_ARC4_128,
    ALI_ALGO_CIPHER_AES_128_CCM,
    ALI_ALGO_CIPHER_AES_192_CCM,
    ALI_ALGO_CIPHER_AES_256_CCM,
    ALI_ALGO_CIPHER_CAMELLIA_128_CCM,
    ALI_ALGO_CIPHER_CAMELLIA_192_CCM,
    ALI_ALGO_CIPHER_CAMELLIA_256_CCM,
} ali_algo_cipher_type_t;

typedef enum {
    ALI_ALGO_MODE_NONE = 0,
    ALI_ALGO_MODE_ECB,
    ALI_ALGO_MODE_CBC,
    ALI_ALGO_MODE_CFB,
    ALI_ALGO_MODE_OFB, /* Unused! */
    ALI_ALGO_MODE_CTR,
    ALI_ALGO_MODE_GCM,
    ALI_ALGO_MODE_STREAM,
    ALI_ALGO_MODE_CCM,
} ali_algo_cipher_mode_t;

typedef enum {
    ALI_ALGO_PADDING_PKCS7 = 0,     /**< PKCS7 padding (default)        */
    ALI_ALGO_PADDING_ONE_AND_ZEROS, /**< ISO/IEC 7816-4 padding         */
    ALI_ALGO_PADDING_ZEROS_AND_LEN, /**< ANSI X.923 padding             */
    ALI_ALGO_PADDING_ZEROS,         /**< zero padding (not reversible!) */
    ALI_ALGO_PADDING_NONE,          /**< never pad (full blocks only)   */
} ali_algo_cipher_padding_t;

typedef enum {
    ALI_ALGO_OPERATION_NONE = -1,
    ALI_ALGO_DECRYPT = 0,
    ALI_ALGO_ENCRYPT,
} ali_algo_operation_t;

enum {
    /** Undefined key length */
    ALI_ALGO_KEY_LENGTH_NONE = 0,
    /** Key length, in bits (including parity), for DES keys */
    ALI_ALGO_KEY_LENGTH_DES  = 64,
    /** Key length, in bits (including parity), for DES in two key EDE */
    ALI_ALGO_KEY_LENGTH_DES_EDE = 128,
    /** Key length, in bits (including parity), for DES in three-key EDE */
    ALI_ALGO_KEY_LENGTH_DES_EDE3 = 192,
};

/** Maximum length of any IV, in bytes */
#define ALI_ALGO_MAX_IV_LENGTH      16
/** Maximum block size of any cipher, in bytes */
#define ALI_ALGO_MAX_BLOCK_LENGTH   16

/**
 * Base cipher information (opaque struct).
 */
typedef struct ali_algo_cipher_base_t ali_algo_cipher_base_t;

/**
 * CMAC context (opaque struct).
 */
typedef struct ali_algo_cmac_context_t ali_algo_cmac_context_t;

/**
 * Cipher information. Allows cipher functions to be called in a generic way.
 */
typedef struct {
    /** Full cipher identifier (e.g. ALI_ALGO_CIPHER_AES_256_CBC) */
    ali_algo_cipher_type_t type;

    /** Cipher mode (e.g. ALI_ALGO_MODE_CBC) */
    ali_algo_cipher_mode_t mode;

    /** Cipher key length, in bits (default length for variable sized ciphers)
     *  (Includes parity bits for ciphers like DES) */
    unsigned int key_bitlen;

    /** Name of the cipher */
    const char * name;

    /** IV/NONCE size, in bytes.
     *  For cipher that accept many sizes: recommended size */
    unsigned int iv_size;

    /** Flags for variable IV size, variable key size, etc. */
    int flags;

    /** block size, in bytes */
    unsigned int block_size;

    /** Base cipher information and functions */
    const ali_algo_cipher_base_t *base;

} ali_algo_cipher_info_t;

/**
 * Generic cipher context.
 */
typedef struct {
    /** Information about the associated cipher */
    const ali_algo_cipher_info_t *cipher_info;

    /** Key length to use */
    int key_bitlen;

    /** Operation that the context's key has been initialised for */
    ali_algo_operation_t operation;

#if defined(ALI_ALGO_CIPHER_MODE_WITH_PADDING)
    /** Padding functions to use, if relevant for cipher mode */
    void (*add_padding)( unsigned char *output, size_t olen, size_t data_len );
    int (*get_padding)( unsigned char *input, size_t ilen, size_t *data_len );
#endif

    /** Buffer for data that hasn't been encrypted yet */
    unsigned char unprocessed_data[ALI_ALGO_MAX_BLOCK_LENGTH];

    /** Number of bytes that still need processing */
    size_t unprocessed_len;

    /** Current IV or NONCE_COUNTER for CTR-mode */
    unsigned char iv[ALI_ALGO_MAX_IV_LENGTH];

    /** IV size in bytes (for ciphers with variable-length IVs) */
    size_t iv_size;

    /** Cipher-specific context */
    void *cipher_ctx;
} ali_algo_cipher_context_t;

/**
 * \brief Returns the list of ciphers supported by the generic cipher module.
 *
 * \return              a statically allocated array of ciphers, the last entry
 *                      is 0.
 */
const int *ali_algo_cipher_list( void );

/**
 * \brief               Returns the cipher information structure associated
 *                      with the given cipher name.
 *
 * \param cipher_name   Name of the cipher to search for.
 *
 * \return              the cipher information structure associated with the
 *                      given cipher_name, or NULL if not found.
 */
const ali_algo_cipher_info_t *ali_algo_cipher_info_from_string( const char *cipher_name );

/**
 * \brief               Returns the cipher information structure associated
 *                      with the given cipher type.
 *
 * \param cipher_type   Type of the cipher to search for.
 *
 * \return              the cipher information structure associated with the
 *                      given cipher_type, or NULL if not found.
 */
const ali_algo_cipher_info_t *ali_algo_cipher_info_from_type( const ali_algo_cipher_type_t cipher_type );

/**
 * \brief               Returns the cipher information structure associated
 *                      with the given cipher id, key size and mode.
 *
 * \param cipher_id     Id of the cipher to search for
 *                      (e.g. ALI_ALGO_CIPHER_ID_AES)
 * \param key_bitlen    Length of the key in bits
 * \param mode          Cipher mode (e.g. ALI_ALGO_MODE_CBC)
 *
 * \return              the cipher information structure associated with the
 *                      given cipher_type, or NULL if not found.
 */
const ali_algo_cipher_info_t *ali_algo_cipher_info_from_values( const ali_algo_cipher_id_t cipher_id,
                                              int key_bitlen,
                                              const ali_algo_cipher_mode_t mode );

/**
 * \brief               Initialize a cipher_context (as NONE)
 */
void ali_algo_cipher_init( ali_algo_cipher_context_t *ctx );

/**
 * \brief               Free and clear the cipher-specific context of ctx.
 *                      Freeing ctx itself remains the responsibility of the
 *                      caller.
 */
void ali_algo_cipher_free( ali_algo_cipher_context_t *ctx );

/**
 * \brief               Initialises and fills the cipher context structure with
 *                      the appropriate values.
 *
 * \note                Currently also clears structure. In future versions you
 *                      will be required to call ali_algo_cipher_init() on the structure
 *                      first.
 *
 * \param ctx           context to initialise. May not be NULL.
 * \param cipher_info   cipher to use.
 *
 * \return              0 on success,
 *                      ALI_ALGO_ERR_CIPHER_BAD_INPUT_DATA on parameter failure,
 *                      ALI_ALGO_ERR_CIPHER_ALLOC_FAILED if allocation of the
 *                      cipher-specific context failed.
 */
int ali_algo_cipher_setup( ali_algo_cipher_context_t *ctx, const ali_algo_cipher_info_t *cipher_info );

/**
 * \brief               Returns the block size of the given cipher.
 *
 * \param ctx           cipher's context. Must have been initialised.
 *
 * \return              size of the cipher's blocks, or 0 if ctx has not been
 *                      initialised.
 */
static inline unsigned int ali_algo_cipher_get_block_size( const ali_algo_cipher_context_t *ctx )
{
    if( NULL == ctx || NULL == ctx->cipher_info )
        return 0;

    return ctx->cipher_info->block_size;
}

/**
 * \brief               Returns the mode of operation for the cipher.
 *                      (e.g. ALI_ALGO_MODE_CBC)
 *
 * \param ctx           cipher's context. Must have been initialised.
 *
 * \return              mode of operation, or ALI_ALGO_MODE_NONE if ctx
 *                      has not been initialised.
 */
static inline ali_algo_cipher_mode_t ali_algo_cipher_get_cipher_mode( const ali_algo_cipher_context_t *ctx )
{
    if( NULL == ctx || NULL == ctx->cipher_info )
        return ALI_ALGO_MODE_NONE;

    return ctx->cipher_info->mode;
}

/**
 * \brief               Returns the size of the cipher's IV/NONCE in bytes.
 *
 * \param ctx           cipher's context. Must have been initialised.
 *
 * \return              If IV has not been set yet: (recommended) IV size
 *                      (0 for ciphers not using IV/NONCE).
 *                      If IV has already been set: actual size.
 */
static inline int ali_algo_cipher_get_iv_size( const ali_algo_cipher_context_t *ctx )
{
    if( NULL == ctx || NULL == ctx->cipher_info )
        return 0;

    if( ctx->iv_size != 0 )
        return (int) ctx->iv_size;

    return (int) ctx->cipher_info->iv_size;
}

/**
 * \brief               Returns the type of the given cipher.
 *
 * \param ctx           cipher's context. Must have been initialised.
 *
 * \return              type of the cipher, or ALI_ALGO_CIPHER_NONE if ctx has
 *                      not been initialised.
 */
static inline ali_algo_cipher_type_t ali_algo_cipher_get_type( const ali_algo_cipher_context_t *ctx )
{
    if( NULL == ctx || NULL == ctx->cipher_info )
        return ALI_ALGO_CIPHER_NONE;

    return ctx->cipher_info->type;
}

/**
 * \brief               Returns the name of the given cipher, as a string.
 *
 * \param ctx           cipher's context. Must have been initialised.
 *
 * \return              name of the cipher, or NULL if ctx was not initialised.
 */
static inline const char *ali_algo_cipher_get_name( const ali_algo_cipher_context_t *ctx )
{
    if( NULL == ctx || NULL == ctx->cipher_info )
        return 0;

    return ctx->cipher_info->name;
}

/**
 * \brief               Returns the key length of the cipher.
 *
 * \param ctx           cipher's context. Must have been initialised.
 *
 * \return              cipher's key length, in bits, or
 *                      ALI_ALGO_KEY_LENGTH_NONE if ctx has not been
 *                      initialised.
 */
static inline int ali_algo_cipher_get_key_bitlen( const ali_algo_cipher_context_t *ctx )
{
    if( NULL == ctx || NULL == ctx->cipher_info )
        return ALI_ALGO_KEY_LENGTH_NONE;

    return (int) ctx->cipher_info->key_bitlen;
}

/**
 * \brief               Returns the operation of the given cipher.
 *
 * \param ctx           cipher's context. Must have been initialised.
 *
 * \return              operation (ALI_ALGO_ENCRYPT or ALI_ALGO_DECRYPT),
 *                      or ALI_ALGO_OPERATION_NONE if ctx has not been
 *                      initialised.
 */
static inline ali_algo_operation_t ali_algo_cipher_get_operation( const ali_algo_cipher_context_t *ctx )
{
    if( NULL == ctx || NULL == ctx->cipher_info )
        return ALI_ALGO_OPERATION_NONE;

    return ctx->operation;
}

/**
 * \brief               Set the key to use with the given context.
 *
 * \param ctx           generic cipher context. May not be NULL. Must have been
 *                      initialised using cipher_context_from_type or
 *                      cipher_context_from_string.
 * \param key           The key to use.
 * \param key_bitlen    key length to use, in bits.
 * \param operation     Operation that the key will be used for, either
 *                      ALI_ALGO_ENCRYPT or ALI_ALGO_DECRYPT.
 *
 * \returns             0 on success, ALI_ALGO_ERR_CIPHER_BAD_INPUT_DATA if
 *                      parameter verification fails or a cipher specific
 *                      error code.
 */
int ali_algo_cipher_setkey( ali_algo_cipher_context_t *ctx, const unsigned char *key,
                   int key_bitlen, const ali_algo_operation_t operation );

#if defined(ALI_ALGO_CIPHER_MODE_WITH_PADDING)
/**
 * \brief               Set padding mode, for cipher modes that use padding.
 *                      (Default: PKCS7 padding.)
 *
 * \param ctx           generic cipher context
 * \param mode          padding mode
 *
 * \returns             0 on success, ALI_ALGO_ERR_CIPHER_FEATURE_UNAVAILABLE
 *                      if selected padding mode is not supported, or
 *                      ALI_ALGO_ERR_CIPHER_BAD_INPUT_DATA if the cipher mode
 *                      does not support padding.
 */
int ali_algo_cipher_set_padding_mode( ali_algo_cipher_context_t *ctx, ali_algo_cipher_padding_t mode );
#endif /* ALI_ALGO_CIPHER_MODE_WITH_PADDING */

/**
 * \brief               Set the initialization vector (IV) or nonce
 *
 * \param ctx           generic cipher context
 * \param iv            IV to use (or NONCE_COUNTER for CTR-mode ciphers)
 * \param iv_len        IV length for ciphers with variable-size IV;
 *                      discarded by ciphers with fixed-size IV.
 *
 * \returns             0 on success, or ALI_ALGO_ERR_CIPHER_BAD_INPUT_DATA
 *
 * \note                Some ciphers don't use IVs nor NONCE. For these
 *                      ciphers, this function has no effect.
 */
int ali_algo_cipher_set_iv( ali_algo_cipher_context_t *ctx,
                   const unsigned char *iv, size_t iv_len );

/**
 * \brief               Finish preparation of the given context
 *
 * \param ctx           generic cipher context
 *
 * \returns             0 on success, ALI_ALGO_ERR_CIPHER_BAD_INPUT_DATA
 *                      if parameter verification fails.
 */
int ali_algo_cipher_reset( ali_algo_cipher_context_t *ctx );


/**
 * \brief               Generic cipher update function. Encrypts/decrypts
 *                      using the given cipher context. Writes as many block
 *                      size'd blocks of data as possible to output. Any data
 *                      that cannot be written immediately will either be added
 *                      to the next block, or flushed when cipher_final is
 *                      called.
 *                      Exception: for ALI_ALGO_MODE_ECB, expects single block
 *                                 in size (e.g. 16 bytes for AES)
 *
 * \param ctx           generic cipher context
 * \param input         buffer holding the input data
 * \param ilen          length of the input data
 * \param output        buffer for the output data. Should be able to hold at
 *                      least ilen + block_size. Cannot be the same buffer as
 *                      input!
 * \param olen          length of the output data, will be filled with the
 *                      actual number of bytes written.
 *
 * \returns             0 on success, ALI_ALGO_ERR_CIPHER_BAD_INPUT_DATA if
 *                      parameter verification fails,
 *                      ALI_ALGO_ERR_CIPHER_FEATURE_UNAVAILABLE on an
 *                      unsupported mode for a cipher or a cipher specific
 *                      error code.
 *
 * \note                If the underlying cipher is GCM, all calls to this
 *                      function, except the last one before ali_algo_cipher_finish(),
 *                      must have ilen a multiple of the block size.
 */
int ali_algo_cipher_update( ali_algo_cipher_context_t *ctx, const unsigned char *input,
                   size_t ilen, unsigned char *output, size_t *olen );

/**
 * \brief               Generic cipher finalisation function. If data still
 *                      needs to be flushed from an incomplete block, data
 *                      contained within it will be padded with the size of
 *                      the last block, and written to the output buffer.
 *
 * \param ctx           Generic cipher context
 * \param output        buffer to write data to. Needs block_size available.
 * \param olen          length of the data written to the output buffer.
 *
 * \returns             0 on success, ALI_ALGO_ERR_CIPHER_BAD_INPUT_DATA if
 *                      parameter verification fails,
 *                      ALI_ALGO_ERR_CIPHER_FULL_BLOCK_EXPECTED if decryption
 *                      expected a full block but was not provided one,
 *                      ALI_ALGO_ERR_CIPHER_INVALID_PADDING on invalid padding
 *                      while decrypting or a cipher specific error code.
 */
int ali_algo_cipher_finish( ali_algo_cipher_context_t *ctx,
                   unsigned char *output, size_t *olen );


/**
 * \brief               Generic all-in-one encryption/decryption
 *                      (for all ciphers except AEAD constructs).
 *
 * \param ctx           generic cipher context
 * \param iv            IV to use (or NONCE_COUNTER for CTR-mode ciphers)
 * \param iv_len        IV length for ciphers with variable-size IV;
 *                      discarded by ciphers with fixed-size IV.
 * \param input         buffer holding the input data
 * \param ilen          length of the input data
 * \param output        buffer for the output data. Should be able to hold at
 *                      least ilen + block_size. Cannot be the same buffer as
 *                      input!
 * \param olen          length of the output data, will be filled with the
 *                      actual number of bytes written.
 *
 * \note                Some ciphers don't use IVs nor NONCE. For these
 *                      ciphers, use iv = NULL and iv_len = 0.
 *
 * \returns             0 on success, or
 *                      ALI_ALGO_ERR_CIPHER_BAD_INPUT_DATA, or
 *                      ALI_ALGO_ERR_CIPHER_FULL_BLOCK_EXPECTED if decryption
 *                      expected a full block but was not provided one, or
 *                      ALI_ALGO_ERR_CIPHER_INVALID_PADDING on invalid padding
 *                      while decrypting, or
 *                      a cipher specific error code.
 */
int ali_algo_cipher_crypt( ali_algo_cipher_context_t *ctx,
                  const unsigned char *iv, size_t iv_len,
                  const unsigned char *input, size_t ilen,
                  unsigned char *output, size_t *olen );


#ifdef __cplusplus
}
#endif

#endif /* ALI_ALGO_CIPHER_H */

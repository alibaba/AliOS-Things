/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/


#ifndef ALI_ALGO_PK_H
#define ALI_ALGO_PK_H

#include "md.h"

#if defined(ALI_ALGO_RSA_C)
#include "rsa.h"
#endif


#define ALI_ALGO_ERR_PK_ALLOC_FAILED        -0x3F80  /**< Memory allocation failed. */
#define ALI_ALGO_ERR_PK_TYPE_MISMATCH       -0x3F00  /**< Type mismatch, eg attempt to encrypt with an ECDSA key */
#define ALI_ALGO_ERR_PK_BAD_INPUT_DATA      -0x3E80  /**< Bad input parameters to function. */
#define ALI_ALGO_ERR_PK_FILE_IO_ERROR       -0x3E00  /**< Read/write of file failed. */
#define ALI_ALGO_ERR_PK_KEY_INVALID_VERSION -0x3D80  /**< Unsupported key version */
#define ALI_ALGO_ERR_PK_KEY_INVALID_FORMAT  -0x3D00  /**< Invalid key tag or value. */
#define ALI_ALGO_ERR_PK_UNKNOWN_PK_ALG      -0x3C80  /**< Key algorithm is unsupported (only RSA and EC are supported). */
#define ALI_ALGO_ERR_PK_PASSWORD_REQUIRED   -0x3C00  /**< Private key password can't be empty. */
#define ALI_ALGO_ERR_PK_PASSWORD_MISMATCH   -0x3B80  /**< Given private key password does not allow for correct decryption. */
#define ALI_ALGO_ERR_PK_INVALID_PUBKEY      -0x3B00  /**< The pubkey tag or value is invalid (only RSA and EC are supported). */
#define ALI_ALGO_ERR_PK_INVALID_ALG         -0x3A80  /**< The algorithm tag or value is invalid. */
#define ALI_ALGO_ERR_PK_UNKNOWN_NAMED_CURVE -0x3A00  /**< Elliptic curve is unsupported (only NIST curves are supported). */
#define ALI_ALGO_ERR_PK_FEATURE_UNAVAILABLE -0x3980  /**< Unavailable feature, e.g. RSA disabled for RSA key. */
#define ALI_ALGO_ERR_PK_SIG_LEN_MISMATCH    -0x3900  /**< The signature is valid but its length is less than expected. */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          Public key types
 */
typedef enum {
    ALI_ALGO_PK_NONE=0,
    ALI_ALGO_PK_RSA,
    ALI_ALGO_PK_ECKEY,
    ALI_ALGO_PK_ECKEY_DH,
    ALI_ALGO_PK_ECDSA,
    ALI_ALGO_PK_RSA_ALT,
    ALI_ALGO_PK_RSASSA_PSS,
} ali_algo_pk_type_t;

/**
 * \brief           Options for RSASSA-PSS signature verification.
 *                  See \c ali_algo_rsa_rsassa_pss_verify_ext()
 */
typedef struct
{
    ali_algo_md_type_t mgf1_hash_id;
    int expected_salt_len;

} ali_algo_pk_rsassa_pss_options;

/**
 * \brief           Types for interfacing with the debug module
 */
typedef enum
{
    ALI_ALGO_PK_DEBUG_NONE = 0,
    ALI_ALGO_PK_DEBUG_MPI,
    ALI_ALGO_PK_DEBUG_ECP,
} ali_algo_pk_debug_type;

/**
 * \brief           Item to send to the debug module
 */
typedef struct
{
    ali_algo_pk_debug_type type;
    const char *name;
    void *value;
} ali_algo_pk_debug_item;

/** Maximum number of item send for debugging, plus 1 */
#define ALI_ALGO_PK_DEBUG_MAX_ITEMS 3

/**
 * \brief           Public key information and operations
 */
typedef struct ali_algo_pk_info_t ali_algo_pk_info_t;

/**
 * \brief           Public key container
 */
typedef struct
{
    const ali_algo_pk_info_t *   pk_info; /**< Public key informations        */
    void *                      pk_ctx;  /**< Underlying public key context  */
} ali_algo_pk_context;

#if defined(ALI_ALGO_RSA_C)
/**
 * Quick access to an RSA context inside a PK context.
 *
 * \warning You must make sure the PK context actually holds an RSA context
 * before using this function!
 */
static inline ali_algo_rsa_context *ali_algo_pk_rsa( const ali_algo_pk_context pk )
{
    return( (ali_algo_rsa_context *) (pk).pk_ctx );
}
#endif /* ALI_ALGO_RSA_C */


/**
 * \brief           Return information associated with the given PK type
 *
 * \param pk_type   PK type to search for.
 *
 * \return          The PK info associated with the type or NULL if not found.
 */
const ali_algo_pk_info_t *ali_algo_pk_info_from_type( ali_algo_pk_type_t pk_type );

/**
 * \brief           Initialize a ali_algo_pk_context (as NONE)
 */
void ali_algo_pk_init( ali_algo_pk_context *ctx );

/**
 * \brief           Free a ali_algo_pk_context
 */
void ali_algo_pk_free( ali_algo_pk_context *ctx );

/**
 * \brief           Initialize a PK context with the information given
 *                  and allocates the type-specific PK subcontext.
 *
 * \param ctx       Context to initialize. Must be empty (type NONE).
 * \param info      Information to use
 *
 * \return          0 on success,
 *                  ALI_ALGO_ERR_PK_BAD_INPUT_DATA on invalid input,
 *                  ALI_ALGO_ERR_PK_ALLOC_FAILED on allocation failure.
 *
 * \note            For contexts holding an RSA-alt key, use
 *                  \c ali_algo_pk_setup_rsa_alt() instead.
 */
int ali_algo_pk_setup( ali_algo_pk_context *ctx, const ali_algo_pk_info_t *info );

/**
 * \brief           Get the size in bits of the underlying key
 *
 * \param ctx       Context to use
 *
 * \return          Key size in bits, or 0 on error
 */
size_t ali_algo_pk_get_bitlen( const ali_algo_pk_context *ctx );

/**
 * \brief           Get the length in bytes of the underlying key
 * \param ctx       Context to use
 *
 * \return          Key length in bytes, or 0 on error
 */
static inline size_t ali_algo_pk_get_len( const ali_algo_pk_context *ctx )
{
    return( ( ali_algo_pk_get_bitlen( ctx ) + 7 ) / 8 );
}

/**
 * \brief           Tell if a context can do the operation given by type
 *
 * \param ctx       Context to test
 * \param type      Target type
 *
 * \return          0 if context can't do the operations,
 *                  1 otherwise.
 */
int ali_algo_pk_can_do( const ali_algo_pk_context *ctx, ali_algo_pk_type_t type );

/**
 * \brief           Verify signature (including padding if relevant).
 *
 * \param ctx       PK context to use
 * \param md_alg    Hash algorithm used (see notes)
 * \param hash      Hash of the message to sign
 * \param hash_len  Hash length or 0 (see notes)
 * \param sig       Signature to verify
 * \param sig_len   Signature length
 *
 * \return          0 on success (signature is valid),
 *                  ALI_ALGO_ERR_PK_SIG_LEN_MISMATCH if the signature is
 *                  valid but its actual length is less than sig_len,
 *                  or a specific error code.
 *
 * \note            For RSA keys, the default padding type is PKCS#1 v1.5.
 *                  Use \c ali_algo_pk_verify_ext( ALI_ALGO_PK_RSASSA_PSS, ... )
 *                  to verify RSASSA_PSS signatures.
 *
 * \note            If hash_len is 0, then the length associated with md_alg
 *                  is used instead, or an error returned if it is invalid.
 *
 * \note            md_alg may be ALI_ALGO_MD_NONE, only if hash_len != 0
 */
int ali_algo_pk_verify( ali_algo_pk_context *ctx, ali_algo_md_type_t md_alg,
               const unsigned char *hash, size_t hash_len,
               const unsigned char *sig, size_t sig_len );

/**
 * \brief           Verify signature, with options.
 *                  (Includes verification of the padding depending on type.)
 *
 * \param type      Signature type (inc. possible padding type) to verify
 * \param options   Pointer to type-specific options, or NULL
 * \param ctx       PK context to use
 * \param md_alg    Hash algorithm used (see notes)
 * \param hash      Hash of the message to sign
 * \param hash_len  Hash length or 0 (see notes)
 * \param sig       Signature to verify
 * \param sig_len   Signature length
 *
 * \return          0 on success (signature is valid),
 *                  ALI_ALGO_ERR_PK_TYPE_MISMATCH if the PK context can't be
 *                  used for this type of signatures,
 *                  ALI_ALGO_ERR_PK_SIG_LEN_MISMATCH if the signature is
 *                  valid but its actual length is less than sig_len,
 *                  or a specific error code.
 *
 * \note            If hash_len is 0, then the length associated with md_alg
 *                  is used instead, or an error returned if it is invalid.
 *
 * \note            md_alg may be ALI_ALGO_MD_NONE, only if hash_len != 0
 *
 * \note            If type is ALI_ALGO_PK_RSASSA_PSS, then options must point
 *                  to a ali_algo_pk_rsassa_pss_options structure,
 *                  otherwise it must be NULL.
 */
int ali_algo_pk_verify_ext( ali_algo_pk_type_t type, const void *options,
                   ali_algo_pk_context *ctx, ali_algo_md_type_t md_alg,
                   const unsigned char *hash, size_t hash_len,
                   const unsigned char *sig, size_t sig_len );

/**
 * \brief           Make signature, including padding if relevant.
 *
 * \param ctx       PK context to use - must hold a private key
 * \param md_alg    Hash algorithm used (see notes)
 * \param hash      Hash of the message to sign
 * \param hash_len  Hash length or 0 (see notes)
 * \param sig       Place to write the signature
 * \param sig_len   Number of bytes written
 * \param f_rng     RNG function
 * \param p_rng     RNG parameter
 *
 * \return          0 on success, or a specific error code.
 *
 * \note            For RSA keys, the default padding type is PKCS#1 v1.5.
 *                  There is no interface in the PK module to make RSASSA-PSS
 *                  signatures yet.
 *
 * \note            If hash_len is 0, then the length associated with md_alg
 *                  is used instead, or an error returned if it is invalid.
 *
 * \note            For RSA, md_alg may be ALI_ALGO_MD_NONE if hash_len != 0.
 *                  For ECDSA, md_alg may never be ALI_ALGO_MD_NONE.
 */
int ali_algo_pk_sign( ali_algo_pk_context *ctx, ali_algo_md_type_t md_alg,
             const unsigned char *hash, size_t hash_len,
             unsigned char *sig, size_t *sig_len,
             int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );

/**
 * \brief           Decrypt message (including padding if relevant).
 *
 * \param ctx       PK context to use - must hold a private key
 * \param input     Input to decrypt
 * \param ilen      Input size
 * \param output    Decrypted output
 * \param olen      Decrypted message length
 * \param osize     Size of the output buffer
 * \param f_rng     RNG function
 * \param p_rng     RNG parameter
 *
 * \note            For RSA keys, the default padding type is PKCS#1 v1.5.
 *
 * \return          0 on success, or a specific error code.
 */
int ali_algo_pk_decrypt( ali_algo_pk_context *ctx,
                const unsigned char *input, size_t ilen,
                unsigned char *output, size_t *olen, size_t osize,
                int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );

/**
 * \brief           Encrypt message (including padding if relevant).
 *
 * \param ctx       PK context to use
 * \param input     Message to encrypt
 * \param ilen      Message size
 * \param output    Encrypted output
 * \param olen      Encrypted output length
 * \param osize     Size of the output buffer
 * \param f_rng     RNG function
 * \param p_rng     RNG parameter
 *
 * \note            For RSA keys, the default padding type is PKCS#1 v1.5.
 *
 * \return          0 on success, or a specific error code.
 */
int ali_algo_pk_encrypt( ali_algo_pk_context *ctx,
                const unsigned char *input, size_t ilen,
                unsigned char *output, size_t *olen, size_t osize,
                int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );

/**
 * \brief           Check if a public-private pair of keys matches.
 *
 * \param pub       Context holding a public key.
 * \param prv       Context holding a private (and public) key.
 *
 * \return          0 on success or ALI_ALGO_ERR_PK_BAD_INPUT_DATA
 */
int ali_algo_pk_check_pair( const ali_algo_pk_context *pub, const ali_algo_pk_context *prv );

/**
 * \brief           Export debug information
 *
 * \param ctx       Context to use
 * \param items     Place to write debug items
 *
 * \return          0 on success or ALI_ALGO_ERR_PK_BAD_INPUT_DATA
 */
int ali_algo_pk_debug( const ali_algo_pk_context *ctx, ali_algo_pk_debug_item *items );

/**
 * \brief           Access the type name
 *
 * \param ctx       Context to use
 *
 * \return          Type name on success, or "invalid PK"
 */
const char * ali_algo_pk_get_name( const ali_algo_pk_context *ctx );

/**
 * \brief           Get the key type
 *
 * \param ctx       Context to use
 *
 * \return          Type on success, or ALI_ALGO_PK_NONE
 */
ali_algo_pk_type_t ali_algo_pk_get_type( const ali_algo_pk_context *ctx );

#if defined(ALI_ALGO_PK_PARSE_C)
/** \ingroup pk_module */
/**
 * \brief           Parse a private key in PEM or DER format
 *
 * \param ctx       key to be initialized
 * \param key       input buffer
 * \param keylen    size of the buffer
 *                  (including the terminating null byte for PEM data)
 * \param pwd       password for decryption (optional)
 * \param pwdlen    size of the password
 *
 * \note            On entry, ctx must be empty, either freshly initialised
 *                  with ali_algo_pk_init() or reset with ali_algo_pk_free(). If you need a
 *                  specific key type, check the result with ali_algo_pk_can_do().
 *
 * \note            The key is also checked for correctness.
 *
 * \return          0 if successful, or a specific PK or PEM error code
 */
int ali_algo_pk_parse_key( ali_algo_pk_context *ctx,
                  const unsigned char *key, size_t keylen,
                  const unsigned char *pwd, size_t pwdlen );

/** \ingroup pk_module */
/**
 * \brief           Parse a public key in PEM or DER format
 *
 * \param ctx       key to be initialized
 * \param key       input buffer
 * \param keylen    size of the buffer
 *                  (including the terminating null byte for PEM data)
 *
 * \note            On entry, ctx must be empty, either freshly initialised
 *                  with ali_algo_pk_init() or reset with ali_algo_pk_free(). If you need a
 *                  specific key type, check the result with ali_algo_pk_can_do().
 *
 * \note            The key is also checked for correctness.
 *
 * \return          0 if successful, or a specific PK or PEM error code
 */
int ali_algo_pk_parse_public_key( ali_algo_pk_context *ctx,
                         const unsigned char *key, size_t keylen );

#endif /* ALI_ALGO_PK_PARSE_C */


/*
 * WARNING: Low-level functions. You probably do not want to use these unless
 *          you are certain you do ;)
 */

#if defined(ALI_ALGO_PK_PARSE_C)
/**
 * \brief           Parse a SubjectPublicKeyInfo DER structure
 *
 * \param p         the position in the ASN.1 data
 * \param end       end of the buffer
 * \param pk        the key to fill
 *
 * \return          0 if successful, or a specific PK error code
 */
int ali_algo_pk_parse_subpubkey( unsigned char **p, const unsigned char *end,
                        ali_algo_pk_context *pk );
#endif /* ALI_ALGO_PK_PARSE_C */


#ifdef __cplusplus
}
#endif

#endif /* ALI_ALGO_PK_H */

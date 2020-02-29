/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/


#ifndef __RSA_H
#define __RSA_H

#include "ls_osa.h"

#include "bignum.h"
#include "md.h"

#if defined(HAL_THREADING_C)
#include "threading.h"
#endif

/*
 * RSA Error codes
 */
#define HAL_ERR_RSA_BAD_INPUT_DATA                    -0x4080  /**< Bad input parameters to function. */
#define HAL_ERR_RSA_INVALID_PADDING                   -0x4100  /**< Input data contains invalid padding and is rejected. */
#define HAL_ERR_RSA_KEY_GEN_FAILED                    -0x4180  /**< Something failed during generation of a key. */
#define HAL_ERR_RSA_KEY_CHECK_FAILED                  -0x4200  /**< Key failed to pass the library's validity check. */
#define HAL_ERR_RSA_PUBLIC_FAILED                     -0x4280  /**< The public key operation failed. */
#define HAL_ERR_RSA_PRIVATE_FAILED                    -0x4300  /**< The private key operation failed. */
#define HAL_ERR_RSA_VERIFY_FAILED                     -0x4380  /**< The PKCS#1 verification failed. */
#define HAL_ERR_RSA_OUTPUT_TOO_LARGE                  -0x4400  /**< The output buffer for decryption is not large enough. */
#define HAL_ERR_RSA_RNG_FAILED                        -0x4480  /**< The random generator failed to generate non-zeros. */

/*
 * RSA constants
 */
#define HAL_RSA_PUBLIC      0
#define HAL_RSA_PRIVATE     1

#define HAL_RSA_PKCS_V15    0
#define HAL_RSA_PKCS_V21    1

#define HAL_RSA_SIGN        1
#define HAL_RSA_CRYPT       2

#define HAL_RSA_SALT_LEN_ANY    -1

/*
 * The above constants may be used even if the RSA module is compile out,
 * eg for alternative (PKCS#11) RSA implemenations in the PK layers.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          RSA context structure
 */
typedef struct
{
    int ver;                        /*!<  always 0          */
    size_t len;                     /*!<  size(N) in chars  */
    const uint8_t *n;               /*!<  public modulus    */
    size_t n_size;
    const uint8_t *e;               /*!<  public exponent   */
    size_t e_size;
    const uint8_t *d;               /*!<  private exponent  */
    size_t d_size;
    const uint8_t *p;               /*!<  1st prime factor  */
    size_t p_size;
    const uint8_t *q;               /*!<  2nd prime factor  */
    size_t q_size;
    const uint8_t *dp;              /*!<  D % (P - 1)       */
    size_t dp_size;
    const uint8_t *dq;              /*!<  D % (Q - 1)       */
    size_t dq_size;
    const uint8_t *qp;              /*!<  1 / (Q % P)       */
    size_t qp_size;
    const uint8_t *rn;              /*!<  cached R^2 mod N  */
    size_t rn_size;
    const uint8_t *rp;              /*!<  cached R^2 mod P  */
    size_t rp_size;
    const uint8_t *rq;              /*!<  cached R^2 mod Q  */
    size_t rq_size;
    const uint8_t *vi;              /*!<  cached blinding value     */
    size_t vi_size;
    const uint8_t *vf;              /*!<  cached un-blinding value  */
    size_t vf_size;
    int padding;                    /*!<  HAL_RSA_PKCS_V15 for 1.5 padding and
                                         HAL_RSA_PKCS_v21 for OAEP/PSS */
    int hash_id;                    /*!<  Hash identifier of impl_md_type_t as
                                      specified in the impl_md.h header file
                                      for the EME-OAEP and EMSA-PSS
                                      encoding */
#if (CONFIG_MULTH_SUPPORT)
    void  *mutex;              /*!<  Thread-safety mutex       */
#endif
}
impl_rsa_context;

/**
 * \brief          Initialize an RSA context
 *
 *                 Note: Set padding to HAL_RSA_PKCS_V21 for the RSAES-OAEP
 *                 encryption scheme and the RSASSA-PSS signature scheme.
 *
 * \param ctx      RSA context to be initialized
 * \param padding  HAL_RSA_PKCS_V15 or HAL_RSA_PKCS_V21
 * \param hash_id  HAL_RSA_PKCS_V21 hash identifier
 *
 * \note           The hash_id parameter is actually ignored
 *                 when using HAL_RSA_PKCS_V15 padding.
 *
 * \note           Choice of padding mode is strictly enforced for private key
 *                 operations, since there might be security concerns in
 *                 mixing padding modes. For public key operations it's merely
 *                 a default value, which can be overriden by calling specific
 *                 rsa_rsaes_xxx or rsa_rsassa_xxx functions.
 *
 * \note           The chosen hash is always used for OEAP encryption.
 *                 For PSS signatures, it's always used for making signatures,
 *                 but can be overriden (and always is, if set to
 *                 HAL_MD_NONE) for verifying them.
 */
void impl_rsa_init( impl_rsa_context *ctx,
               int padding,
               int hash_id);

/**
 * \brief          Set padding for an already initialized RSA context
 *                 See \c impl_rsa_init() for details.
 *
 * \param ctx      RSA context to be set
 * \param padding  HAL_RSA_PKCS_V15 or HAL_RSA_PKCS_V21
 * \param hash_id  HAL_RSA_PKCS_V21 hash identifier
 */
void impl_rsa_set_padding( impl_rsa_context *ctx, int padding, int hash_id);

/**
 * \brief          Generate an RSA keypair
 *
 * \param ctx      RSA context that will hold the key
 * \param f_rng    RNG function
 * \param p_rng    RNG parameter
 * \param nbits    size of the public key in bits
 * \param exponent public exponent (e.g., 65537)
 *
 * \note           impl_rsa_init() must be called beforehand to setup
 *                 the RSA context.
 *
 * \return         0 if successful, or an HAL_ERR_RSA_XXX error code
 */
int impl_rsa_gen_key( impl_rsa_context *ctx,
                 int (*f_rng)(void *, unsigned char *, size_t),
                 void *p_rng,
                 unsigned int nbits, int exponent );

/**
 * \brief          Check a public RSA key
 *
 * \param ctx      RSA context to be checked
 *
 * \return         0 if successful, or an HAL_ERR_RSA_XXX error code
 */
int impl_rsa_check_pubkey( const impl_rsa_context *ctx );

/**
 * \brief          Check a private RSA key
 *
 * \param ctx      RSA context to be checked
 *
 * \return         0 if successful, or an HAL_ERR_RSA_XXX error code
 */
int impl_rsa_check_privkey( const impl_rsa_context *ctx );

/**
 * \brief          Check a public-private RSA key pair.
 *                 Check each of the contexts, and make sure they match.
 *
 * \param pub      RSA context holding the public key
 * \param prv      RSA context holding the private key
 *
 * \return         0 if successful, or an HAL_ERR_RSA_XXX error code
 */
int impl_rsa_check_pub_priv( const impl_rsa_context *pub, const impl_rsa_context *prv );

/**
 * \brief          Do an RSA public key operation
 *
 * \param ctx      RSA context
 * \param input    input buffer
 * \param output   output buffer
 *
 * \return         0 if successful, or an HAL_ERR_RSA_XXX error code
 *
 * \note           This function does NOT take care of message
 *                 padding. Also, be sure to set input[0] = 0 or ensure that
 *                 input is smaller than N.
 *
 * \note           The input and output buffers must be large
 *                 enough (eg. 128 bytes if RSA-1024 is used).
 */
int impl_rsa_public( impl_rsa_context *ctx,
                const unsigned char *input,
                unsigned char *output );

/**
 * \brief          Do an RSA private key operation
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Needed for blinding)
 * \param p_rng    RNG parameter
 * \param input    input buffer
 * \param output   output buffer
 *
 * \return         0 if successful, or an HAL_ERR_RSA_XXX error code
 *
 * \note           The input and output buffers must be large
 *                 enough (eg. 128 bytes if RSA-1024 is used).
 */
int impl_rsa_private( impl_rsa_context *ctx,
                 int (*f_rng)(void *, unsigned char *, size_t),
                 void *p_rng,
                 const unsigned char *input,
                 unsigned char *output );

/**
 * \brief          Generic wrapper to perform a PKCS#1 encryption using the
 *                 mode from the context. Add the message padding, then do an
 *                 RSA operation.
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Needed for padding and PKCS#1 v2.1 encoding
 *                               and HAL_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     HAL_RSA_PUBLIC or HAL_RSA_PRIVATE
 * \param ilen     contains the plaintext length
 * \param input    buffer holding the data to be encrypted
 * \param output   buffer that will hold the ciphertext
 *
 * \return         0 if successful, or an HAL_ERR_RSA_XXX error code
 *
 * \note           The output buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 */
int impl_rsa_pkcs1_encrypt( impl_rsa_context *ctx,
                       int (*f_rng)(void *, unsigned char *, size_t),
                       void *p_rng,
                       int mode, size_t ilen,
                       const unsigned char *input,
                       unsigned char *output );

/**
 * \brief          Perform a PKCS#1 v1.5 encryption (RSAES-PKCS1-v1_5-ENCRYPT)
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Needed for padding and HAL_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     HAL_RSA_PUBLIC or HAL_RSA_PRIVATE
 * \param ilen     contains the plaintext length
 * \param input    buffer holding the data to be encrypted
 * \param output   buffer that will hold the ciphertext
 *
 * \return         0 if successful, or an HAL_ERR_RSA_XXX error code
 *
 * \note           The output buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 */
int impl_rsa_rsaes_pkcs1_v15_encrypt( impl_rsa_context *ctx,
                                 int (*f_rng)(void *, unsigned char *, size_t),
                                 void *p_rng,
                                 int mode, size_t ilen,
                                 const unsigned char *input,
                                 unsigned char *output );

/**
 * \brief          Perform a PKCS#1 v2.1 OAEP encryption (RSAES-OAEP-ENCRYPT)
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Needed for padding and PKCS#1 v2.1 encoding
 *                               and HAL_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     HAL_RSA_PUBLIC or HAL_RSA_PRIVATE
 * \param label    buffer holding the custom label to use
 * \param label_len contains the label length
 * \param ilen     contains the plaintext length
 * \param input    buffer holding the data to be encrypted
 * \param output   buffer that will hold the ciphertext
 *
 * \return         0 if successful, or an HAL_ERR_RSA_XXX error code
 *
 * \note           The output buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 */
int impl_rsa_rsaes_oaep_encrypt( impl_rsa_context *ctx,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng,
                            int mode,
                            const unsigned char *label, size_t label_len,
                            size_t ilen,
                            const unsigned char *input,
                            unsigned char *output );

/**
 * \brief          Generic wrapper to perform a PKCS#1 decryption using the
 *                 mode from the context. Do an RSA operation, then remove
 *                 the message padding
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Only needed for HAL_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     HAL_RSA_PUBLIC or HAL_RSA_PRIVATE
 * \param olen     will contain the plaintext length
 * \param input    buffer holding the encrypted data
 * \param output   buffer that will hold the plaintext
 * \param output_max_len    maximum length of the output buffer
 *
 * \return         0 if successful, or an HAL_ERR_RSA_XXX error code
 *
 * \note           The output buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used) otherwise
 *                 an error is thrown.
 */
int impl_rsa_pkcs1_decrypt( impl_rsa_context *ctx,
                       int (*f_rng)(void *, unsigned char *, size_t),
                       void *p_rng,
                       int mode, size_t *olen,
                       const unsigned char *input,
                       unsigned char *output,
                       size_t output_max_len );

/**
 * \brief          Perform a PKCS#1 v1.5 decryption (RSAES-PKCS1-v1_5-DECRYPT)
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Only needed for HAL_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     HAL_RSA_PUBLIC or HAL_RSA_PRIVATE
 * \param olen     will contain the plaintext length
 * \param input    buffer holding the encrypted data
 * \param output   buffer that will hold the plaintext
 * \param output_max_len    maximum length of the output buffer
 *
 * \return         0 if successful, or an HAL_ERR_RSA_XXX error code
 *
 * \note           The output buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used) otherwise
 *                 an error is thrown.
 */
int impl_rsa_rsaes_pkcs1_v15_decrypt( impl_rsa_context *ctx,
                                 int (*f_rng)(void *, unsigned char *, size_t),
                                 void *p_rng,
                                 int mode, size_t *olen,
                                 const unsigned char *input,
                                 unsigned char *output,
                                 size_t output_max_len );

/**
 * \brief          Perform a PKCS#1 v2.1 OAEP decryption (RSAES-OAEP-DECRYPT)
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Only needed for HAL_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     HAL_RSA_PUBLIC or HAL_RSA_PRIVATE
 * \param label    buffer holding the custom label to use
 * \param label_len contains the label length
 * \param olen     will contain the plaintext length
 * \param input    buffer holding the encrypted data
 * \param output   buffer that will hold the plaintext
 * \param output_max_len    maximum length of the output buffer
 *
 * \return         0 if successful, or an HAL_ERR_RSA_XXX error code
 *
 * \note           The output buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used) otherwise
 *                 an error is thrown.
 */
int impl_rsa_rsaes_oaep_decrypt( impl_rsa_context *ctx,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng,
                            int mode,
                            const unsigned char *label, size_t label_len,
                            size_t *olen,
                            const unsigned char *input,
                            unsigned char *output,
                            size_t output_max_len );

/**
 * \brief          Generic wrapper to perform a PKCS#1 signature using the
 *                 mode from the context. Do a private RSA operation to sign
 *                 a message digest
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Needed for PKCS#1 v2.1 encoding and for
 *                               HAL_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     HAL_RSA_PUBLIC or HAL_RSA_PRIVATE
 * \param md_alg   a HAL_MD_XXX (use HAL_MD_NONE for signing raw data)
 * \param hashlen  message digest length (for HAL_MD_NONE only)
 * \param hash     buffer holding the message digest
 * \param sig      buffer that will hold the ciphertext
 *
 * \return         0 if the signing operation was successful,
 *                 or an HAL_ERR_RSA_XXX error code
 *
 * \note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 *
 * \note           In case of PKCS#1 v2.1 encoding, see comments on
 * \note           \c impl_rsa_rsassa_pss_sign() for details on md_alg and hash_id.
 */
int impl_rsa_pkcs1_sign( impl_rsa_context *ctx,
                    int (*f_rng)(void *, unsigned char *, size_t),
                    void *p_rng,
                    int mode,
                    impl_md_type_t md_alg,
                    unsigned int hashlen,
                    const unsigned char *hash,
                    unsigned char *sig );

/**
 * \brief          Perform a PKCS#1 v1.5 signature (RSASSA-PKCS1-v1_5-SIGN)
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Only needed for HAL_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     HAL_RSA_PUBLIC or HAL_RSA_PRIVATE
 * \param md_alg   a HAL_MD_XXX (use HAL_MD_NONE for signing raw data)
 * \param hashlen  message digest length (for HAL_MD_NONE only)
 * \param hash     buffer holding the message digest
 * \param sig      buffer that will hold the ciphertext
 *
 * \return         0 if the signing operation was successful,
 *                 or an HAL_ERR_RSA_XXX error code
 *
 * \note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 */
int impl_rsa_rsassa_pkcs1_v15_sign( impl_rsa_context *ctx,
                               int (*f_rng)(void *, unsigned char *, size_t),
                               void *p_rng,
                               int mode,
                               impl_md_type_t md_alg,
                               unsigned int hashlen,
                               const unsigned char *hash,
                               unsigned char *sig );

/**
 * \brief          Perform a PKCS#1 v2.1 PSS signature (RSASSA-PSS-SIGN)
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Needed for PKCS#1 v2.1 encoding and for
 *                               HAL_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     HAL_RSA_PUBLIC or HAL_RSA_PRIVATE
 * \param md_alg   a HAL_MD_XXX (use HAL_MD_NONE for signing raw data)
 * \param hashlen  message digest length (for HAL_MD_NONE only)
 * \param hash     buffer holding the message digest
 * \param sig      buffer that will hold the ciphertext
 *
 * \return         0 if the signing operation was successful,
 *                 or an HAL_ERR_RSA_XXX error code
 *
 * \note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 *
 * \note           The hash_id in the RSA context is the one used for the
 *                 encoding. md_alg in the function call is the type of hash
 *                 that is encoded. According to RFC 3447 it is advised to
 *                 keep both hashes the same.
 */
int impl_rsa_rsassa_pss_sign( impl_rsa_context *ctx,
                         int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng,
                         int mode,
                         impl_md_type_t md_alg,
                         unsigned int hashlen,
                         const unsigned char *hash,
                         unsigned char *sig );

/**
 * \brief          Generic wrapper to perform a PKCS#1 verification using the
 *                 mode from the context. Do a public RSA operation and check
 *                 the message digest
 *
 * \param ctx      points to an RSA public key
 * \param f_rng    RNG function (Only needed for HAL_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     HAL_RSA_PUBLIC or HAL_RSA_PRIVATE
 * \param md_alg   a HAL_MD_XXX (use HAL_MD_NONE for signing raw data)
 * \param hashlen  message digest length (for HAL_MD_NONE only)
 * \param hash     buffer holding the message digest
 * \param sig      buffer holding the ciphertext
 *
 * \return         0 if the verify operation was successful,
 *                 or an HAL_ERR_RSA_XXX error code
 *
 * \note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 *
 * \note           In case of PKCS#1 v2.1 encoding, see comments on
 *                 \c impl_rsa_rsassa_pss_verify() about md_alg and hash_id.
 */
int impl_rsa_pkcs1_verify( impl_rsa_context *ctx,
                      int (*f_rng)(void *, unsigned char *, size_t),
                      void *p_rng,
                      int mode,
                      impl_md_type_t md_alg,
                      unsigned int hashlen,
                      const unsigned char *hash,
                      const unsigned char *sig );

/**
 * \brief          Perform a PKCS#1 v1.5 verification (RSASSA-PKCS1-v1_5-VERIFY)
 *
 * \param ctx      points to an RSA public key
 * \param f_rng    RNG function (Only needed for HAL_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     HAL_RSA_PUBLIC or HAL_RSA_PRIVATE
 * \param md_alg   a HAL_MD_XXX (use HAL_MD_NONE for signing raw data)
 * \param hashlen  message digest length (for HAL_MD_NONE only)
 * \param hash     buffer holding the message digest
 * \param sig      buffer holding the ciphertext
 *
 * \return         0 if the verify operation was successful,
 *                 or an HAL_ERR_RSA_XXX error code
 *
 * \note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 */
int impl_rsa_rsassa_pkcs1_v15_verify( impl_rsa_context *ctx,
                                 int (*f_rng)(void *, unsigned char *, size_t),
                                 void *p_rng,
                                 int mode,
                                 impl_md_type_t md_alg,
                                 unsigned int hashlen,
                                 const unsigned char *hash,
                                 const unsigned char *sig );

/**
 * \brief          Perform a PKCS#1 v2.1 PSS verification (RSASSA-PSS-VERIFY)
 *                 (This is the "simple" version.)
 *
 * \param ctx      points to an RSA public key
 * \param f_rng    RNG function (Only needed for HAL_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     HAL_RSA_PUBLIC or HAL_RSA_PRIVATE
 * \param md_alg   a HAL_MD_XXX (use HAL_MD_NONE for signing raw data)
 * \param hashlen  message digest length (for HAL_MD_NONE only)
 * \param hash     buffer holding the message digest
 * \param sig      buffer holding the ciphertext
 *
 * \return         0 if the verify operation was successful,
 *                 or an HAL_ERR_RSA_XXX error code
 *
 * \note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 *
 * \note           The hash_id in the RSA context is the one used for the
 *                 verification. md_alg in the function call is the type of
 *                 hash that is verified. According to RFC 3447 it is advised to
 *                 keep both hashes the same. If hash_id in the RSA context is
 *                 unset, the md_alg from the function call is used.
 */
int impl_rsa_rsassa_pss_verify( impl_rsa_context *ctx,
                           int (*f_rng)(void *, unsigned char *, size_t),
                           void *p_rng,
                           int mode,
                           impl_md_type_t md_alg,
                           unsigned int hashlen,
                           const unsigned char *hash,
                           const unsigned char *sig );

/**
 * \brief          Perform a PKCS#1 v2.1 PSS verification (RSASSA-PSS-VERIFY)
 *                 (This is the version with "full" options.)
 *
 * \param ctx      points to an RSA public key
 * \param f_rng    RNG function (Only needed for HAL_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     HAL_RSA_PUBLIC or HAL_RSA_PRIVATE
 * \param md_alg   a HAL_MD_XXX (use HAL_MD_NONE for signing raw data)
 * \param hashlen  message digest length (for HAL_MD_NONE only)
 * \param hash     buffer holding the message digest
 * \param mgf1_hash_id message digest used for mask generation
 * \param expected_salt_len Length of the salt used in padding, use
 *                 HAL_RSA_SALT_LEN_ANY to accept any salt length
 * \param sig      buffer holding the ciphertext
 *
 * \return         0 if the verify operation was successful,
 *                 or an HAL_ERR_RSA_XXX error code
 *
 * \note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 *
 * \note           The hash_id in the RSA context is ignored.
 */
int impl_rsa_rsassa_pss_verify_ext( impl_rsa_context *ctx,
                               int (*f_rng)(void *, unsigned char *, size_t),
                               void *p_rng,
                               int mode,
                               impl_md_type_t md_alg,
                               unsigned int hashlen,
                               const unsigned char *hash,
                               impl_md_type_t mgf1_hash_id,
                               int expected_salt_len,
                               const unsigned char *sig );

/**
 * \brief          Copy the components of an RSA context
 *
 * \param dst      Destination context
 * \param src      Source context
 *
 * \return         0 on success,
 *                 HAL_ERR_MPI_ALLOC_FAILED on memory allocation failure
 */
int impl_rsa_copy( impl_rsa_context *dst, const impl_rsa_context *src );

/**
 * \brief          Free the components of an RSA key
 *
 * \param ctx      RSA Context to free
 */
void impl_rsa_free( impl_rsa_context *ctx );

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int impl_rsa_self_test( int verbose );

#ifdef __cplusplus
}
#endif

#endif /* rsa.h */

/**
 * \file rsa.h
 *
 * \brief The RSA public-key cryptosystem
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
#ifndef MBEDTLS_RSA_ALT_H
#define MBEDTLS_RSA_ALT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sec_crypto_rsa.h"

/**
 * \brief          RSA context structure
 */
typedef struct {
    int ver;                    /*!<  always 0          */
    size_t len;                 /*!<  size(N) in chars  */

    mbedtls_mpi N;                      /*!<  public modulus    */
    mbedtls_mpi E;                      /*!<  public exponent   */

    mbedtls_mpi D;                      /*!<  private exponent  */
    mbedtls_mpi P;                      /*!<  1st prime factor  */
    mbedtls_mpi Q;                      /*!<  2nd prime factor  */
    mbedtls_mpi DP;                     /*!<  D % (P - 1)       */
    mbedtls_mpi DQ;                     /*!<  D % (Q - 1)       */
    mbedtls_mpi QP;                     /*!<  1 / (Q % P)       */
    int padding;                /*!<  MBEDTLS_RSA_PKCS_V15 for 1.5 padding and
                                      MBEDTLS_RSA_PKCS_v21 for OAEP/PSS         */
    int hash_id;                /*!<  Hash identifier of mbedtls_md_type_t as
                                      specified in the mbedtls_md.h header file
                                      for the EME-OAEP and EMSA-PSS
                                      encoding                          */
#if defined(CONFIG_TEE_RSA)
    void *ctx;
#endif

    sc_rsa_t sc_rsa;
    sc_rsa_context_t sc_ctx;
} mbedtls_rsa_context;

/**
 * \brief          Initialize an RSA context
 *
 *                 Note: Set padding to MBEDTLS_RSA_PKCS_V21 for the RSAES-OAEP
 *                 encryption scheme and the RSASSA-PSS signature scheme.
 *
 * \param ctx      RSA context to be initialized
 * \param padding  MBEDTLS_RSA_PKCS_V15 or MBEDTLS_RSA_PKCS_V21
 * \param hash_id  MBEDTLS_RSA_PKCS_V21 hash identifier
 *
 * \note           The hash_id parameter is actually ignored
 *                 when using MBEDTLS_RSA_PKCS_V15 padding.
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
 *                 MBEDTLS_MD_NONE) for verifying them.
 */
void mbedtls_rsa_init(mbedtls_rsa_context *ctx,
                      int padding,
                      int hash_id);

/**
 * \brief          Check a public RSA key
 *
 * \param ctx      RSA context to be checked
 *
 * \return         0 if successful, or an MBEDTLS_ERR_RSA_XXX error code
 */
int mbedtls_rsa_check_pubkey(const mbedtls_rsa_context *ctx);

/**
 * \brief          Check a private RSA key
 *
 * \param ctx      RSA context to be checked
 *
 * \return         0 if successful, or an MBEDTLS_ERR_RSA_XXX error code
 */
int mbedtls_rsa_check_privkey(const mbedtls_rsa_context *ctx);

/**
 * \brief          Check a public-private RSA key pair.
 *                 Check each of the contexts, and make sure they match.
 *
 * \param pub      RSA context holding the public key
 * \param prv      RSA context holding the private key
 *
 * \return         0 if successful, or an MBEDTLS_ERR_RSA_XXX error code
 */
int mbedtls_rsa_check_pub_priv(const mbedtls_rsa_context *pub, const mbedtls_rsa_context *prv);

/**
 * \brief          Generic wrapper to perform a PKCS#1 encryption using the
 *                 mode from the context. Add the message padding, then do an
 *                 RSA operation.
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Needed for padding and PKCS#1 v2.1 encoding
 *                               and MBEDTLS_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     MBEDTLS_RSA_PUBLIC or MBEDTLS_RSA_PRIVATE
 * \param ilen     contains the plaintext length
 * \param input    buffer holding the data to be encrypted
 * \param output   buffer that will hold the ciphertext
 *
 * \return         0 if successful, or an MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           The output buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 */
int mbedtls_rsa_pkcs1_encrypt(mbedtls_rsa_context *ctx,
                              int (*f_rng)(void *, unsigned char *, size_t),
                              void *p_rng,
                              int mode, size_t ilen,
                              const unsigned char *input,
                              unsigned char *output);

/**
 * \brief          Generic wrapper to perform a PKCS#1 decryption using the
 *                 mode from the context. Do an RSA operation, then remove
 *                 the message padding
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Only needed for MBEDTLS_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     MBEDTLS_RSA_PUBLIC or MBEDTLS_RSA_PRIVATE
 * \param olen     will contain the plaintext length
 * \param input    buffer holding the encrypted data
 * \param output   buffer that will hold the plaintext
 * \param output_max_len    maximum length of the output buffer
 *
 * \return         0 if successful, or an MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           The output buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used) otherwise
 *                 an error is thrown.
 */
int mbedtls_rsa_pkcs1_decrypt(mbedtls_rsa_context *ctx,
                              int (*f_rng)(void *, unsigned char *, size_t),
                              void *p_rng,
                              int mode, size_t *olen,
                              const unsigned char *input,
                              unsigned char *output,
                              size_t output_max_len);

/**
 * \brief          Generic wrapper to perform a PKCS#1 signature using the
 *                 mode from the context. Do a private RSA operation to sign
 *                 a message digest
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Needed for PKCS#1 v2.1 encoding and for
 *                               MBEDTLS_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     MBEDTLS_RSA_PUBLIC or MBEDTLS_RSA_PRIVATE
 * \param md_alg   a MBEDTLS_MD_XXX (use MBEDTLS_MD_NONE for signing raw data)
 * \param hashlen  message digest length (for MBEDTLS_MD_NONE only)
 * \param hash     buffer holding the message digest
 * \param sig      buffer that will hold the ciphertext
 *
 * \return         0 if the signing operation was successful,
 *                 or an MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 *
 * \note           In case of PKCS#1 v2.1 encoding, see comments on
 * \note           \c mbedtls_rsa_rsassa_pss_sign() for details on md_alg and hash_id.
 */
int mbedtls_rsa_pkcs1_sign(mbedtls_rsa_context *ctx,
                           int (*f_rng)(void *, unsigned char *, size_t),
                           void *p_rng,
                           int mode,
                           mbedtls_md_type_t md_alg,
                           unsigned int hashlen,
                           const unsigned char *hash,
                           unsigned char *sig);
/**
 * \brief          Generic wrapper to perform a PKCS#1 verification using the
 *                 mode from the context. Do a public RSA operation and check
 *                 the message digest
 *
 * \param ctx      points to an RSA public key
 * \param f_rng    RNG function (Only needed for MBEDTLS_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     MBEDTLS_RSA_PUBLIC or MBEDTLS_RSA_PRIVATE
 * \param md_alg   a MBEDTLS_MD_XXX (use MBEDTLS_MD_NONE for signing raw data)
 * \param hashlen  message digest length (for MBEDTLS_MD_NONE only)
 * \param hash     buffer holding the message digest
 * \param sig      buffer holding the ciphertext
 *
 * \return         0 if the verify operation was successful,
 *                 or an MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 *
 * \note           In case of PKCS#1 v2.1 encoding, see comments on
 *                 \c mbedtls_rsa_rsassa_pss_verify() about md_alg and hash_id.
 */
int mbedtls_rsa_pkcs1_verify(mbedtls_rsa_context *ctx,
                             int (*f_rng)(void *, unsigned char *, size_t),
                             void *p_rng,
                             int mode,
                             mbedtls_md_type_t md_alg,
                             unsigned int hashlen,
                             const unsigned char *hash,
                             const unsigned char *sig);

/**
 * \brief          Free the components of an RSA key
 *
 * \param ctx      RSA Context to free
 */
void mbedtls_rsa_free(mbedtls_rsa_context *ctx);

/**
 * \brief          This function retrieves the length of RSA modulus in Bytes.
 *
 * \param ctx      The initialized RSA context.
 *
 * \return         The length of the RSA modulus in Bytes.
 *
 */
size_t mbedtls_rsa_get_len( const mbedtls_rsa_context *ctx );


/**
 * \brief          This function imports core RSA parameters, in raw big-endian
 *                 binary format, into an RSA context.
 *
 * \note           This function can be called multiple times for successive
 *                 imports, if the parameters are not simultaneously present.
 *
 *                 Any sequence of calls to this function should be followed
 *                 by a call to mbedtls_rsa_complete(), which checks and
 *                 completes the provided information to a ready-for-use
 *                 public or private RSA key.
 *
 * \note           See mbedtls_rsa_complete() for more information on which
 *                 parameters are necessary to set up a private or public
 *                 RSA key.
 *
 * \note           The imported parameters are copied and need not be preserved
 *                 for the lifetime of the RSA context being set up.
 *
 * \param ctx      The initialized RSA context to store the parameters in.
 * \param N        The RSA modulus. This may be \c NULL.
 * \param N_len    The Byte length of \p N; it is ignored if \p N == NULL.
 * \param P        The first prime factor of \p N. This may be \c NULL.
 * \param P_len    The Byte length of \p P; it ns ignored if \p P == NULL.
 * \param Q        The second prime factor of \p N. This may be \c NULL.
 * \param Q_len    The Byte length of \p Q; it is ignored if \p Q == NULL.
 * \param D        The private exponent. This may be \c NULL.
 * \param D_len    The Byte length of \p D; it is ignored if \p D == NULL.
 * \param E        The public exponent. This may be \c NULL.
 * \param E_len    The Byte length of \p E; it is ignored if \p E == NULL.
 *
 * \return         \c 0 on success.
 * \return         A non-zero error code on failure.
 */
int mbedtls_rsa_import_raw( mbedtls_rsa_context *ctx,
                            unsigned char const *N, size_t N_len,
                            unsigned char const *P, size_t P_len,
                            unsigned char const *Q, size_t Q_len,
                            unsigned char const *D, size_t D_len,
                            unsigned char const *E, size_t E_len );

                            
/**
 * \brief          This function completes an RSA context from
 *                 a set of imported core parameters.
 *
 *                 To setup an RSA public key, precisely \p N and \p E
 *                 must have been imported.
 *
 *                 To setup an RSA private key, sufficient information must
 *                 be present for the other parameters to be derivable.
 *
 *                 The default implementation supports the following:
 *                 <ul><li>Derive \p P, \p Q from \p N, \p D, \p E.</li>
 *                 <li>Derive \p N, \p D from \p P, \p Q, \p E.</li></ul>
 *                 Alternative implementations need not support these.
 *
 *                 If this function runs successfully, it guarantees that
 *                 the RSA context can be used for RSA operations without
 *                 the risk of failure or crash.
 *
 * \warning        This function need not perform consistency checks
 *                 for the imported parameters. In particular, parameters that
 *                 are not needed by the implementation might be silently
 *                 discarded and left unchecked. To check the consistency
 *                 of the key material, see mbedtls_rsa_check_privkey().
 *
 * \param ctx      The initialized RSA context holding imported parameters.
 *
 * \return         \c 0 on success.
 * \return         #MBEDTLS_ERR_RSA_BAD_INPUT_DATA if the attempted derivations
 *                 failed.
 *
 */
int mbedtls_rsa_complete( mbedtls_rsa_context *ctx );

#ifdef __cplusplus
}
#endif

#endif /* rsa.h */

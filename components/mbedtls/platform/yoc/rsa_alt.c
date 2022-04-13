/*
 *  The RSA public-key cryptosystem
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
/*
 *  The following sources were referenced in the design of this implementation
 *  of the RSA algorithm:
 *
 *  [1] A method for obtaining digital signatures and public-key cryptosystems
 *      R Rivest, A Shamir, and L Adleman
 *      http://people.csail.mit.edu/rivest/pubs.html#RSA78
 *
 *  [2] Handbook of Applied Cryptography - 1997, Chapter 8
 *      Menezes, van Oorschot and Vanstone
 *
 *  [3] Malware Guard Extension: Using SGX to Conceal Cache Attacks
 *      Michael Schwarz, Samuel Weiser, Daniel Gruss, Clémentine Maurice and
 *      Stefan Mangard
 *      https://arxiv.org/abs/1702.08719v2
 *
 */

#include "mbedtls/rsa.h"

#include <string.h>
#include "mbedtls/platform_util.h"
#include "mbedtls/rsa_internal.h"

#if defined(MBEDTLS_PKCS1_V21)
#include "mbedtls/md.h"
#endif

#if defined(MBEDTLS_PKCS1_V15) && !defined(__OpenBSD__)
#include <stdlib.h>
#endif

#ifdef MBEDTLS_RSA_ALT

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf printf
#define mbedtls_calloc calloc
#define mbedtls_free free
#endif

#if defined(CONFIG_TEE_RSA)
#include "drv/tee.h"
#endif

#define RSA_VALIDATE_RET( cond )                                       \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_RSA_BAD_INPUT_DATA )

/*
 * Initialize an RSA context
 */
void mbedtls_rsa_init(mbedtls_rsa_context *ctx, int padding, int hash_id)
{
    memset(ctx, 0, sizeof(mbedtls_rsa_context));

    ctx->padding      = padding;
    ctx->hash_id      = hash_id;
    /* NOTE: set invalid bitwidth to 0 */
    sc_rsa_init(&ctx->sc_rsa, 0, 0);
    memset(&ctx->sc_ctx, 0, sizeof(sc_rsa_context_t));
}

/*
 * Checks whether the context fields are set in such a way
 * that the RSA primitives will be able to execute without error.
 * It does *not* make guarantees for consistency of the parameters.
 */
static int rsa_check_context( mbedtls_rsa_context const *ctx, int is_priv,
                              int blinding_needed )
{
#if !defined(MBEDTLS_RSA_NO_CRT)
    /* blinding_needed is only used for NO_CRT to decide whether
     * P,Q need to be present or not. */
    ((void) blinding_needed);
#endif

    if( ctx->len != mbedtls_mpi_size( &ctx->N ) ||
        ctx->len > MBEDTLS_MPI_MAX_SIZE )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }

    /*
     * 1. Modular exponentiation needs positive, odd moduli.
     */

    /* Modular exponentiation wrt. N is always used for
     * RSA public key operations. */
    if( mbedtls_mpi_cmp_int( &ctx->N, 0 ) <= 0 ||
        mbedtls_mpi_get_bit( &ctx->N, 0 ) == 0  )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }

#if !defined(MBEDTLS_RSA_NO_CRT)
    /* Modular exponentiation for P and Q is only
     * used for private key operations and if CRT
     * is used. */
    if( is_priv &&
        ( mbedtls_mpi_cmp_int( &ctx->P, 0 ) <= 0 ||
          mbedtls_mpi_get_bit( &ctx->P, 0 ) == 0 ||
          mbedtls_mpi_cmp_int( &ctx->Q, 0 ) <= 0 ||
          mbedtls_mpi_get_bit( &ctx->Q, 0 ) == 0  ) )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }
#endif /* !MBEDTLS_RSA_NO_CRT */

    /*
     * 2. Exponents must be positive
     */

    /* Always need E for public key operations */
    if( mbedtls_mpi_cmp_int( &ctx->E, 0 ) <= 0 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

#if defined(MBEDTLS_RSA_NO_CRT)
    /* For private key operations, use D or DP & DQ
     * as (unblinded) exponents. */
    if( is_priv && mbedtls_mpi_cmp_int( &ctx->D, 0 ) <= 0 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
#else
    if( is_priv &&
        ( mbedtls_mpi_cmp_int( &ctx->DP, 0 ) <= 0 ||
          mbedtls_mpi_cmp_int( &ctx->DQ, 0 ) <= 0  ) )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }
#endif /* MBEDTLS_RSA_NO_CRT */

    /* Blinding shouldn't make exponents negative either,
     * so check that P, Q >= 1 if that hasn't yet been
     * done as part of 1. */
#if defined(MBEDTLS_RSA_NO_CRT)
    if( is_priv && blinding_needed &&
        ( mbedtls_mpi_cmp_int( &ctx->P, 0 ) <= 0 ||
          mbedtls_mpi_cmp_int( &ctx->Q, 0 ) <= 0 ) )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }
#endif

    /* It wouldn't lead to an error if it wasn't satisfied,
     * but check for QP >= 1 nonetheless. */
#if !defined(MBEDTLS_RSA_NO_CRT)
    if( is_priv &&
        mbedtls_mpi_cmp_int( &ctx->QP, 0 ) <= 0 )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }
#endif

    return( 0 );
}

/*
 * Check a public RSA key
 */
int mbedtls_rsa_check_pubkey(const mbedtls_rsa_context *ctx)
{
    RSA_VALIDATE_RET( ctx != NULL );

    if( rsa_check_context( ctx, 0 /* public */, 0 /* no blinding */ ) != 0 ) {
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    }

    if( mbedtls_mpi_bitlen( &ctx->N ) < 128 )
    {
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    }

    if( mbedtls_mpi_get_bit( &ctx->E, 0 ) == 0 ||
        mbedtls_mpi_bitlen( &ctx->E )     < 2  ||
        mbedtls_mpi_cmp_mpi( &ctx->E, &ctx->N ) >= 0 )
    {
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    }

    return( 0 );
}


/*
 * Check a private RSA key
 */
int mbedtls_rsa_check_privkey(const mbedtls_rsa_context *ctx)
{
    RSA_VALIDATE_RET( ctx != NULL );

    if( mbedtls_rsa_check_pubkey( ctx ) != 0 ||
        rsa_check_context( ctx, 1 /* private */, 1 /* blinding */ ) != 0 )
    {
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    }

    if( mbedtls_rsa_validate_params( &ctx->N, &ctx->P, &ctx->Q,
                                     &ctx->D, &ctx->E, NULL, NULL ) != 0 )
    {
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    }

#if !defined(MBEDTLS_RSA_NO_CRT)
    else if( mbedtls_rsa_validate_crt( &ctx->P, &ctx->Q, &ctx->D,
                                       &ctx->DP, &ctx->DQ, &ctx->QP ) != 0 )
    {
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    }
#endif

    return( 0 );
}

/*
 * Check if contexts holding a public and private key match
 */
int mbedtls_rsa_check_pub_priv(const mbedtls_rsa_context *pub, const mbedtls_rsa_context *prv)
{
    if (mbedtls_rsa_check_pubkey(pub) != 0 || mbedtls_rsa_check_privkey(prv) != 0) {
        return (MBEDTLS_ERR_RSA_KEY_CHECK_FAILED);
    }

    if (mbedtls_mpi_cmp_mpi(&pub->N, &prv->N) != 0 || mbedtls_mpi_cmp_mpi(&pub->E, &prv->E) != 0) {
        return (MBEDTLS_ERR_RSA_KEY_CHECK_FAILED);
    }

    return (0);
}

/*
 * Add the message padding, then do an RSA operation
 */
int mbedtls_rsa_pkcs1_encrypt(mbedtls_rsa_context *ctx,
                              int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, int mode,
                              size_t ilen, const unsigned char *input, unsigned char *output)
{
    int            ret = 0;
    unsigned int   output_len = 128;
    unsigned char *key;
    unsigned int   key_size = 0;
    (void)f_rng;
    (void)p_rng;
    (void)mode;
    (void)output_len;

    /*for now Only support RSA1024 and RSA2048*/
    if (ctx->len != 128 && ctx->len != 256) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    switch (ctx->padding) {
#if defined(MBEDTLS_PKCS1_V15)

        case MBEDTLS_RSA_PKCS_V15:
            key_size = mbedtls_mpi_size(&ctx->N);
            key      = (unsigned char *)mbedtls_calloc(1, key_size * 2);

            if (!key) {
                return MBEDTLS_ERR_RSA_PUBLIC_FAILED;
            }

            mbedtls_mpi_write_binary(&ctx->N, key, key_size);
            mbedtls_mpi_write_binary(&ctx->E, key + key_size, key_size);

            unsigned char *new_output = mbedtls_calloc(1, 256);
            (void)ret;
#if defined(CONFIG_TEE_RSA)
            ret = csi_tee_rsa_encrypt(input, ilen,
                                      key, key_size * 2,
                                      new_output, &output_len,
                                      TEE_RSA_PKCS1_PADDING);
#endif
            ctx->sc_ctx.key_bits     = ctx->len * 8;
            ctx->sc_ctx.padding_type = SC_RSA_PADDING_MODE_PKCS1;
            ctx->sc_ctx.n            = key;
            ctx->sc_ctx.e            = key + key_size;
            ret = sc_rsa_encrypt(&ctx->sc_rsa, &ctx->sc_ctx, (void *)input, ilen, new_output);
            memcpy(output, new_output, ctx->len);

            mbedtls_free(key);
            mbedtls_free(new_output);

            if (ret) {
                return MBEDTLS_ERR_RSA_PUBLIC_FAILED;
            }
            return 0;

        default:
            return (MBEDTLS_ERR_RSA_INVALID_PADDING);
    }
#endif
}

/*
 * Do an RSA operation, then remove the message padding
 */
int mbedtls_rsa_pkcs1_decrypt(mbedtls_rsa_context *ctx,
                              int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, int mode,
                              size_t *olen, const unsigned char *input, unsigned char *output,
                              size_t output_max_len)
{
    unsigned char *key;
    unsigned int   key_size = 0;
    int            ret = 0;

    (void)f_rng;
    (void)p_rng;
    (void)mode;
    (void)output_max_len;
    (void)ret;

    /*for now Only support RSA1024 and RSA2048*/
    if (ctx->len != 128 && ctx->len != 256) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    switch (ctx->padding) {
#if defined(MBEDTLS_PKCS1_V15)

        case MBEDTLS_RSA_PKCS_V15:
            key_size = mbedtls_mpi_size(&ctx->N);
            key      = (unsigned char *)mbedtls_calloc(1, key_size * 3);

            if (!key) {
                return MBEDTLS_ERR_RSA_PUBLIC_FAILED;
            }

            mbedtls_mpi_write_binary(&ctx->N, key, key_size);
            mbedtls_mpi_write_binary(&ctx->E, key + key_size, key_size);
            mbedtls_mpi_write_binary(&ctx->D, key + key_size + key_size, key_size);

#if defined(CONFIG_TEE_RSA)
            ret = csi_tee_rsa_decrypt(input, ctx->len,
                                      key, key_size * 3,
                                      output, olen,
                                      TEE_RSA_PKCS1_PADDING);
#endif

            ctx->sc_ctx.key_bits     = ctx->len * 8;
            ctx->sc_ctx.padding_type = SC_RSA_PADDING_MODE_PKCS1;
            ctx->sc_ctx.n            = key;
            ctx->sc_ctx.e            = key + key_size;
            ctx->sc_ctx.d            = key + key_size + key_size;
            ret = sc_rsa_decrypt(&ctx->sc_rsa, &ctx->sc_ctx, (void *)input, ctx->len, output, olen);
            mbedtls_free(key);
            if (ret) {
                return MBEDTLS_ERR_RSA_PRIVATE_FAILED;
            }
            break;
        default:
            return (MBEDTLS_ERR_RSA_INVALID_PADDING);
    }

    return 0;
}

/*
 * Do an RSA operation to sign the message digest
 */
int mbedtls_rsa_pkcs1_sign(mbedtls_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t),
                           void *p_rng, int mode, mbedtls_md_type_t md_alg, unsigned int hashlen,
                           const unsigned char *hash, unsigned char *sig)
{
    unsigned char  type;
    int            ret = 0;
    unsigned int   sig_len = 0;
    unsigned char *key;
    unsigned int   key_size = 0;

    (void)f_rng;
    (void)p_rng;
    (void)mode;
    (void)sig_len;

    /*for now Only support RSA1024 and RSA2048*/
    if (ctx->len != 128 && ctx->len != 256) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    if (md_alg != MBEDTLS_MD_NONE) {
        const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(md_alg);
        hashlen                          = mbedtls_md_get_size(md_info);
    }

#if defined(CONFIG_TEE_RSA)
    if (md_alg == MBEDTLS_MD_MD5) {
        type = TEE_RSA_MD5;
    } else if (md_alg == MBEDTLS_MD_SHA1) {
        type = TEE_RSA_SHA1;
    } else if (md_alg == MBEDTLS_MD_SHA256) {
        type = TEE_RSA_SHA256;
    } else {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }
#endif

    if (md_alg == MBEDTLS_MD_MD5) {
        type = SC_RSA_HASH_TYPE_MD5;
    } else if (md_alg == MBEDTLS_MD_SHA1) {
        type = SC_RSA_HASH_TYPE_SHA1;
    } else if (md_alg == MBEDTLS_MD_SHA256) {
        type = SC_RSA_HASH_TYPE_SHA256;
    } else {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }
    (void)type;

    switch (ctx->padding) {
#if defined(MBEDTLS_PKCS1_V15)

        case MBEDTLS_RSA_PKCS_V15:
            key_size = mbedtls_mpi_size(&ctx->N);
            key      = (unsigned char *)mbedtls_calloc(1, key_size * 3);

            if (!key) {
                return MBEDTLS_ERR_RSA_PUBLIC_FAILED;
            }

            mbedtls_mpi_write_binary(&ctx->N, key, key_size);
            mbedtls_mpi_write_binary(&ctx->E, key + key_size, key_size);
            mbedtls_mpi_write_binary(&ctx->D, key + key_size + key_size, key_size);
            (void)ret;
#if defined(CONFIG_TEE_RSA)
            ret = csi_tee_rsa_sign(hash, hashlen, key, key_size * 3, sig, &sig_len, type);
#endif

            ctx->sc_ctx.key_bits     = ctx->len * 8;
            ctx->sc_ctx.padding_type = SC_RSA_PADDING_MODE_PKCS1;
            ctx->sc_ctx.n            = key;
            ctx->sc_ctx.e            = key + key_size;
            ctx->sc_ctx.d            = key + key_size + key_size;
            ret = sc_rsa_sign(&ctx->sc_rsa, &ctx->sc_ctx, (void *)hash, hashlen, sig, type);
            mbedtls_free(key);
            if (ret) {
                return MBEDTLS_ERR_RSA_PRIVATE_FAILED;
            }

            break;
#endif
        default:
            return (MBEDTLS_ERR_RSA_INVALID_PADDING);
    }

    return 0;
}

/*
 * Do an RSA operation and check the message digest
 */
int mbedtls_rsa_pkcs1_verify(mbedtls_rsa_context *ctx,
                             int (*f_rng)(void *, unsigned char *, size_t), void *p_rng, int mode,
                             mbedtls_md_type_t md_alg, unsigned int hashlen,
                             const unsigned char *hash, const unsigned char *sig)
{
    unsigned char  type;
    int            ret = 0;
    unsigned char *key;
    unsigned int   key_size = 0;

    (void)f_rng;
    (void)p_rng;
    (void)mode;

    /*for now Only support RSA1024 and RSA2048*/
    if (ctx->len != 128 && ctx->len != 256) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    if (md_alg != MBEDTLS_MD_NONE) {
        const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(md_alg);
        hashlen                          = mbedtls_md_get_size(md_info);
    }

#if defined(CONFIG_TEE_RSA)
    if (md_alg == MBEDTLS_MD_MD5) {
        type = TEE_RSA_MD5;
    } else if (md_alg == MBEDTLS_MD_SHA1) {
        type = TEE_RSA_SHA1;
    } else if (md_alg == MBEDTLS_MD_SHA256) {
        type = TEE_RSA_SHA256;
    } else {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }
#endif

    if (md_alg == MBEDTLS_MD_MD5) {
        type = SC_RSA_HASH_TYPE_MD5;
    } else if (md_alg == MBEDTLS_MD_SHA1) {
        type = SC_RSA_HASH_TYPE_SHA1;
    } else if (md_alg == MBEDTLS_MD_SHA256) {
        type = SC_RSA_HASH_TYPE_SHA256;
    } else {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    switch (ctx->padding) {
#if defined(MBEDTLS_PKCS1_V15)

        case MBEDTLS_RSA_PKCS_V15:
            key_size = mbedtls_mpi_size(&ctx->N);
            key      = (unsigned char *)mbedtls_calloc(1, key_size * 2);

            if (!key) {
                return MBEDTLS_ERR_RSA_PUBLIC_FAILED;
            }

            mbedtls_mpi_write_binary(&ctx->N, key, key_size);
            mbedtls_mpi_write_binary(&ctx->E, key + key_size, key_size);
            unsigned char *new_sig = mbedtls_calloc(1, 256);
            memcpy(new_sig, sig, 256);

            (void)type;
#if defined(CONFIG_TEE_RSA)
            ret = csi_tee_rsa_verify((uint8_t *)hash, hashlen, key, key_size * 2,
                                     (uint8_t *)new_sig, ctx->len, type);
#endif
            ctx->sc_ctx.key_bits     = ctx->len * 8;
            ctx->sc_ctx.padding_type = SC_RSA_PADDING_MODE_PKCS1;
            ctx->sc_ctx.n            = key;
            ctx->sc_ctx.e            = key + key_size;
            ret = sc_rsa_verify(&ctx->sc_rsa, &ctx->sc_ctx, (void *)hash, hashlen, new_sig,
                                 ctx->len, type);
            mbedtls_free(new_sig);

            mbedtls_free(key);
            if (ret) {
                /* success */
                return 0;
            } else {
                return MBEDTLS_ERR_RSA_PRIVATE_FAILED;
            }
#endif

        default:
            ret = (MBEDTLS_ERR_RSA_INVALID_PADDING);
    }
    return ret;
}

size_t mbedtls_rsa_get_len( const mbedtls_rsa_context *ctx )
{
    return( ctx->len );
}

int mbedtls_rsa_complete( mbedtls_rsa_context *ctx )
{
    return 0;
}


int mbedtls_rsa_import( mbedtls_rsa_context *ctx,
                        const mbedtls_mpi *N,
                        const mbedtls_mpi *P, const mbedtls_mpi *Q,
                        const mbedtls_mpi *D, const mbedtls_mpi *E )
{
    int ret;
    RSA_VALIDATE_RET( ctx != NULL );

    if( ( N != NULL && ( ret = mbedtls_mpi_copy( &ctx->N, N ) ) != 0 ) ||
        ( P != NULL && ( ret = mbedtls_mpi_copy( &ctx->P, P ) ) != 0 ) ||
        ( Q != NULL && ( ret = mbedtls_mpi_copy( &ctx->Q, Q ) ) != 0 ) ||
        ( D != NULL && ( ret = mbedtls_mpi_copy( &ctx->D, D ) ) != 0 ) ||
        ( E != NULL && ( ret = mbedtls_mpi_copy( &ctx->E, E ) ) != 0 ) )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA + ret );
    }

    if( N != NULL )
        ctx->len = mbedtls_mpi_size( &ctx->N );

    return( 0 );
}

int mbedtls_rsa_import_raw( mbedtls_rsa_context *ctx,
                            unsigned char const *N, size_t N_len,
                            unsigned char const *P, size_t P_len,
                            unsigned char const *Q, size_t Q_len,
                            unsigned char const *D, size_t D_len,
                            unsigned char const *E, size_t E_len )
{
    int ret = 0;

    if( N != NULL )
    {
        MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &ctx->N, N, N_len ) );
        ctx->len = mbedtls_mpi_size( &ctx->N );
    }

    if( P != NULL )
        MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &ctx->P, P, P_len ) );

    if( Q != NULL )
        MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &ctx->Q, Q, Q_len ) );

    if( D != NULL )
        MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &ctx->D, D, D_len ) );

    if( E != NULL )
        MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &ctx->E, E, E_len ) );

cleanup:

    if( ret != 0 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA + ret );

    return( 0 );
}

/*
 * Free the components of an RSA key
 */
void mbedtls_rsa_free(mbedtls_rsa_context *ctx)
{
    mbedtls_mpi_free(&ctx->D);
    mbedtls_mpi_free(&ctx->E);
    mbedtls_mpi_free(&ctx->N);

#if defined(MBEDTLS_THREADING_C)
    mbedtls_mutex_free(&ctx->mutex);
#endif
    sc_rsa_uninit(&ctx->sc_rsa);
    memset(&ctx->sc_ctx, 0, sizeof(sc_rsa_context_t));
}
#endif

#endif /* MBEDTLS_RSA_ALT */
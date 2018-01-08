/**
 * Copyright (C) 2017 The YunOS IoT Project. All rights reserved.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbedtls/md.h"

#if defined(MBEDTLS_DEBUG_C)
#include "mbedtls/debug.h"
#endif

#if defined(MBEDTLS_THREADING_ALT)
#include "mbedtls/threading.h"
#endif

#if defined(MBEDTLS_AES_ALT)
#include "mbedtls/aes.h"
#include "mbedtls/cipher.h"
#endif

#if defined(MBEDTLS_PK_ALT)
#include "mbedtls/rsa.h"
#include "ali_crypto.h"
#else
#include "ali_crypto.h"
#endif

#define MBEDTLS_ALT_PRINT(_f, _a ...)  \
        printf("%s %d: "_f,  __FUNCTION__, __LINE__, ##_a)

#if defined(MBEDTLS_AES_ALT)
/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}
#endif

#if defined(MBEDTLS_THREADING_ALT)
void threading_mutex_init(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || mutex->is_valid)
        return;

    mutex->is_valid = aos_mutex_new(&mutex->mutex) == 0;
}

void threading_mutex_free(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid)
        return;

    aos_mutex_free(&mutex->mutex);
    mutex->is_valid = 0;
}

int threading_mutex_lock(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid)
        return( MBEDTLS_ERR_THREADING_BAD_INPUT_DATA );

    if (aos_mutex_lock( &mutex->mutex, AOS_WAIT_FOREVER) != 0 )
        return(MBEDTLS_ERR_THREADING_MUTEX_ERROR);

    return 0;
}

int threading_mutex_unlock( mbedtls_threading_mutex_t *mutex )
{
    if( mutex == NULL || ! mutex->is_valid )
        return( MBEDTLS_ERR_THREADING_BAD_INPUT_DATA );

    if( aos_mutex_unlock( &mutex->mutex ) != 0 )
        return( MBEDTLS_ERR_THREADING_MUTEX_ERROR );

    return( 0 );
}

#endif /* MBEDTLS_THREADING_ALT */

#if defined(MBEDTLS_AES_ALT)
void mbedtls_aes_init_alt( mbedtls_aes_context *ctx )
{
    memset( ctx, 0, sizeof( mbedtls_aes_context ) );
}

void mbedtls_aes_free_alt( mbedtls_aes_context *ctx )
{
    if (ctx == NULL) {
        return;
    }

    if (ctx->ali_ctx) {
        free(ctx->ali_ctx);
    }

    mbedtls_zeroize(ctx, sizeof(mbedtls_aes_context));
}

int mbedtls_aes_setkey_enc_alt( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits )
{
    if (ctx == NULL || key == NULL || keybits == 0) {
        MBEDTLS_ALT_PRINT("invalid input args\n");
        return -1;
    }

    if ((keybits >> 3) > 32) {
        MBEDTLS_ALT_PRINT("invalid key length - %d\n", keybits >> 3);
        return -1;
    } else {
        ctx->key_len = keybits >> 3;
        memcpy(ctx->key, key, ctx->key_len);
    }

    return 0;
}

int mbedtls_aes_setkey_dec_alt( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits )
{
    if (ctx == NULL || key == NULL || keybits == 0) {
        MBEDTLS_ALT_PRINT("invalid input args\n");
        return -1;
    }

    if ((keybits >> 3) > 32) {
        MBEDTLS_ALT_PRINT("invalid key length - %d\n", keybits >> 3);
        return -1;
    } else {
        ctx->key_len = keybits >> 3;
        memcpy(ctx->key, key, ctx->key_len);
    }

    return 0;
}

int mbedtls_aes_crypt_ecb_alt( mbedtls_aes_context *ctx,
                    int mode,
                    const unsigned char input[16],
                    unsigned char output[16] )
{
    (void)ctx;
    (void)mode;
    (void)input;
    (void)output;

    MBEDTLS_ALT_PRINT("not support!\n");

    return -1;
}

int mbedtls_aes_crypt_cbc_alt( mbedtls_aes_context *ctx,
                    int mode,
                    size_t length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output )
{
    bool is_enc;
    size_t ctx_size;
    ali_crypto_result result;

    if (ctx == NULL ||
        length == 0 || input == NULL || output == NULL) {
        MBEDTLS_ALT_PRINT("invalid input args\n");
        return -1;
    }

    if (length % 16) {
        MBEDTLS_ALT_PRINT("invalid input length - %d\n", length);
        return -1;
    }

    if (ctx->ali_ctx == NULL) {
        result = ali_aes_get_ctx_size(AES_CBC, &ctx_size);
        if (result != ALI_CRYPTO_SUCCESS) {
            MBEDTLS_ALT_PRINT("get ctx size fail - 0x%x\n", result);
            return -1;
        }

        ctx->ali_ctx = malloc(ctx_size);
        if (ctx->ali_ctx == NULL) {
            MBEDTLS_ALT_PRINT("malloc(%d) fail\n", ctx_size);
            return -1;
        } else {
            memset(ctx->ali_ctx, 0, ctx_size);
        }
    }

    if (ctx->status == AES_STATUS_INIT) {
        if (mode == MBEDTLS_AES_ENCRYPT) {
            is_enc = true;
        } else {
            is_enc = false;
        }

        if (ctx->reset) {
            result = ali_aes_reset(ctx->ali_ctx);
            if (result != ALI_CRYPTO_SUCCESS) {
                MBEDTLS_ALT_PRINT("aes reset fail - 0x%x\n", result);
                return -1;
            }
        }

        result = ali_aes_init(AES_CBC, is_enc,
                     ctx->key, NULL, ctx->key_len, iv, ctx->ali_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            MBEDTLS_ALT_PRINT("aes init fail - 0x%x\n", result);
            return -1;
        } else {
            ctx->reset = 1;
            ctx->status = AES_STATUS_PROCESS;
        }
    }

    result = ali_aes_process(input,
                 output, length, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("aes process fail - 0x%x\n", result);
        return -1;
    }

    return 0;
}

#endif /* MBEDTLS_AES_ALT */

#if defined(MBEDTLS_PK_ALT)
int rsa_verify_alt(void *ctx, size_t hash_id,
                   const unsigned char *hash, size_t hash_len,
                   const unsigned char *sig, size_t sig_len)
{
    int ret = 0;
    bool result1 = false;
    ali_crypto_result result;
    mbedtls_rsa_context *rsa_ctx;
    hash_type_t hash_type;
    rsa_padding_t rsa_padding;
    size_t pub_key_len;
    uint8_t *pub_key = NULL;

    if (ctx == NULL || hash == NULL || sig == NULL) {
        MBEDTLS_ALT_PRINT("invalid input args\n");
        return -1;
    }

    rsa_ctx = (mbedtls_rsa_context *)ctx;
    if (sig_len != rsa_ctx->n_len) {
        MBEDTLS_ALT_PRINT("invalid sig len - %d\n", sig_len);
        return -1;
    }

    if (rsa_ctx->padding != MBEDTLS_RSA_ALT_PKCS_V15) {
        MBEDTLS_ALT_PRINT("invalid rsa padding - %d\n", rsa_ctx->padding);
        return -1;
    }

    switch(hash_id) {
        case MBEDTLS_MD_MD5: {
            if (hash_len != MD5_HASH_SIZE) {
                MBEDTLS_ALT_PRINT("invalid hash len - %d\n", hash_len);
                return -1;
            } else {
                hash_type = MD5;
            }

            break;
        }
        case MBEDTLS_MD_SHA1: {
            if (hash_len != SHA1_HASH_SIZE) {
                MBEDTLS_ALT_PRINT("invalid hash len - %d\n", hash_len);
                return -1;
            } else {
                hash_type = SHA1;
            }

            break;
        }
        case MBEDTLS_MD_SHA256: {
            if (hash_len != SHA256_HASH_SIZE) {
                MBEDTLS_ALT_PRINT("invalid hash len - %d\n", hash_len);
                return -1;
            } else {
                hash_type = SHA256;
            }

            break;
        }

        default:
            MBEDTLS_ALT_PRINT("not support hash_id - %d\n", hash_id);
            return -1;
    }

    result = ali_rsa_get_pubkey_size((size_t)rsa_ctx->n_len, &pub_key_len);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("get pubkey size fail - 0x%x\n", result);
        return -1;
    }

    pub_key = malloc(pub_key_len);
    if (pub_key == NULL) {
        MBEDTLS_ALT_PRINT("malloc(%d) fail\n", pub_key_len);
        ret = -1;
        goto _out;
    } else {
        memset(pub_key, 0, pub_key_len);
    }

    result = ali_rsa_init_pubkey(rsa_ctx->n_len << 3,
                 rsa_ctx->rsa_n, rsa_ctx->n_len,
                 rsa_ctx->rsa_e, rsa_ctx->e_len, (rsa_pubkey_t *)pub_key);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("init pub_key fail - 0x%x\n", result);
        ret = -1;
        goto _out;
    }

    rsa_padding.type = RSASSA_PKCS1_V1_5;
    rsa_padding.pad.rsassa_v1_5.type = hash_type;
    result = ali_rsa_verify((rsa_pubkey_t *)pub_key,
                            hash, hash_len,
                            sig, sig_len, rsa_padding, &result1);
    if (result != ALI_CRYPTO_SUCCESS || result1 == false) {
        MBEDTLS_ALT_PRINT("rsa verify fail - 0x%x\n", result);
        ret = -1;
        goto _out;
    }

_out:
    if (pub_key) {
        free(pub_key);
    }

    return ret;
}

int rsa_sign_alt(void *ctx, size_t hash_id,
                 const unsigned char *hash, size_t hash_len,
                 unsigned char *sig, size_t *sig_len)
{
    (void)ctx;
    (void)hash_id;

    (void)hash;
    (void)hash_len;
    (void)sig;
    (void)sig_len;

    MBEDTLS_ALT_PRINT("not support!\n");

    return -1;
}

int rsa_decrypt_alt(void *ctx, 
                    const unsigned char *input, size_t ilen,
                    unsigned char *output, size_t *olen)
{
    (void)ctx;
    (void)input;
    (void)ilen;

    (void)output;
    (void)olen;

    MBEDTLS_ALT_PRINT("not support!\n");

    return -1;
}

int rsa_encrypt_alt(void *ctx,
                    const unsigned char *input, size_t ilen,
                    unsigned char *output, size_t *olen)
{
    int ret = 0;
    ali_crypto_result result;
    mbedtls_rsa_context *rsa_ctx;
    rsa_padding_t rsa_padding;
    size_t pub_key_len;
    uint8_t *pub_key = NULL;

    if (ctx == NULL || input == NULL || ilen == 0 ||
        output == NULL || olen == NULL) {
        MBEDTLS_ALT_PRINT("invalid input args\n");
        return -1;
    }

    rsa_ctx = (mbedtls_rsa_context *)ctx;
    if (rsa_ctx->padding != MBEDTLS_RSA_ALT_PKCS_V15) {
        MBEDTLS_ALT_PRINT("invalid rsa padding - %d\n", rsa_ctx->padding);
        return -1;
    }

    result = ali_rsa_get_pubkey_size((size_t)rsa_ctx->n_len, &pub_key_len);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("get pubkey size fail - 0x%x\n", result);
        return -1;
    }

    pub_key = malloc(pub_key_len);
    if (pub_key == NULL) {
        MBEDTLS_ALT_PRINT("malloc(%d) fail\n", pub_key_len);
        ret = -1;
        goto _out;
    } else {
        memset(pub_key, 0, pub_key_len);
    }

    result = ali_rsa_init_pubkey(rsa_ctx->n_len << 3,
                 rsa_ctx->rsa_n, rsa_ctx->n_len,
                 rsa_ctx->rsa_e, rsa_ctx->e_len, (rsa_pubkey_t *)pub_key);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("init pub_key fail - 0x%x\n", result);
        ret = -1;
        goto _out;
    }

    rsa_padding.type = RSAES_PKCS1_V1_5;
    result = ali_rsa_public_encrypt((rsa_pubkey_t *)pub_key,
                            input, ilen, output, olen, rsa_padding);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("rsa encrypt fail - 0x%x\n", result);
        ret = -1;
        goto _out;
    }

_out:
    if (pub_key) {
        free(pub_key);
    }

    return ret;
}

void mbedtls_rsa_init_alt(mbedtls_rsa_context *ctx,
                           int padding, int hash_id)
{
    memset( ctx, 0, sizeof(mbedtls_rsa_context));

    ctx->padding = padding;
    ctx->hash_id = hash_id;
}


void mbedtls_rsa_free_alt(mbedtls_rsa_context *ctx)
{
    memset(ctx, 0, sizeof(mbedtls_rsa_context));
}

#endif /* MBEDTLS_PK_ALT */


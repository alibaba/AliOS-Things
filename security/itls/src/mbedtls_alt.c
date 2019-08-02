/**
 * Copyright (C) 2017 The YunOS IoT Project. All rights reserved.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "itls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "itls/debug.h"
#include "itls/aes.h"
#include "itls/sha1.h"
#include "itls/sha256.h"
#include "itls/timing.h"
#include "itls/platform.h"

#if defined(MBEDTLS_AES_ALT)
#include "ali_crypto.h"
#endif

#define MBEDTLS_ALT_ASSERT(_x)                          \
    do {                                                \
        if (!(_x)) {                                    \
            ls_osa_print("ASSERT FAILURE:\n");          \
            ls_osa_print("%s (%d): %s\n",               \
                    __FILE__, __LINE__, __FUNCTION__);  \
            while (1) /* loop */;                       \
        }                                               \
    } while (0)

#if defined(MBEDTLS_AES_ALT)
/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n )
{
    volatile unsigned char *p = v;
    while ( n-- ) {
        *p++ = 0;
    }
}
#endif

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
        mbedtls_free(ctx->ali_ctx);
    }

    mbedtls_zeroize(ctx, sizeof(mbedtls_aes_context));
}

int mbedtls_aes_setkey_enc_alt( mbedtls_aes_context *ctx, const unsigned char *key,
                                unsigned int keybits )
{
    if (ctx == NULL || key == NULL || keybits == 0) {
        SSL_DBG_LOG("invalid input args\n");
        return -1;
    }

    if ((keybits >> 3) > 32) {
        SSL_DBG_LOG("invalid key length - %d\n", keybits >> 3);
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
        SSL_DBG_LOG("invalid input args\n");
        return -1;
    }

    if ((keybits >> 3) > 32) {
        SSL_DBG_LOG("invalid key length - %d\n", keybits >> 3);
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

    SSL_DBG_LOG("not support!\n");

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
        SSL_DBG_LOG("invalid input args\n");
        return -1;
    }

    if (length % 16) {
        SSL_DBG_LOG("invalid input length - %d\n", (int)length);
        return -1;
    }

    if (ctx->ali_ctx == NULL) {
        result = ali_aes_get_ctx_size(AES_CBC, &ctx_size);
        if (result != ALI_CRYPTO_SUCCESS) {
            SSL_DBG_LOG("get ctx size fail - 0x%x\n", result);
            return -1;
        }

        ctx->ali_ctx = mbedtls_calloc(1, ctx_size);
        if (ctx->ali_ctx == NULL) {
            SSL_DBG_LOG("mbedtls_calloc(%d) fail\n", (int)ctx_size);
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
                SSL_DBG_LOG("aes reset fail - 0x%x\n", result);
                return -1;
            }
        }

        result = ali_aes_init(AES_CBC, is_enc,
                              ctx->key, NULL, ctx->key_len, iv, ctx->ali_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            SSL_DBG_LOG("aes init fail - 0x%x\n", result);
            return -1;
        } else {
            ctx->reset = 1;
            ctx->status = AES_STATUS_PROCESS;
        }
    }

    result = ali_aes_process(input,
                             output, length, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        SSL_DBG_LOG("aes process fail - 0x%x\n", result);
        return -1;
    }

    return 0;
}

#endif /* MBEDTLS_AES_ALT */

#if defined(MBEDTLS_SHA1_ALT)
void mbedtls_sha1_init_alt(mbedtls_sha1_context *ctx)
{
    memset(ctx, 0, sizeof(mbedtls_sha1_context));
}

void mbedtls_sha1_free_alt(mbedtls_sha1_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    if (ctx->ali_ctx) {
        mbedtls_free(ctx->ali_ctx);
    }

    mbedtls_zeroize(ctx, sizeof(mbedtls_sha1_context));
}

void mbedtls_sha1_clone_alt(mbedtls_sha1_context *dst,
                            const mbedtls_sha1_context *src)
{
    dst->ali_ctx = mbedtls_calloc(1, src->size);
    if (dst->ali_ctx == NULL) {
        SSL_DBG_LOG("mbedtls_calloc(%d) fail\n", (int)src->size);
        MBEDTLS_ALT_ASSERT(0);
    }

    dst->size = src->size;
    memcpy(dst->ali_ctx, src->ali_ctx, dst->size);
}

void mbedtls_sha1_starts_alt(mbedtls_sha1_context *ctx)
{
    size_t ctx_size;
    ali_crypto_result result;

    if (NULL == ctx->ali_ctx) {
        result = ali_sha1_get_ctx_size(&ctx_size);
        if (result != ALI_CRYPTO_SUCCESS) {
            SSL_DBG_LOG("get ctx size fail - 0x%x\n", result);
            MBEDTLS_ALT_ASSERT(0);
        }

        ctx->size = ctx_size;
        ctx->ali_ctx = mbedtls_calloc(1, ctx_size);
        if (ctx->ali_ctx == NULL) {
            SSL_DBG_LOG("mbedtls_calloc(%d) fail\n", (int)ctx_size);
            MBEDTLS_ALT_ASSERT(0);
        } else {
            memset(ctx->ali_ctx, 0, ctx_size);
        }
    }

    result = ali_sha1_init(ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        SSL_DBG_LOG("sha1 init fail - 0x%x\n", result);
        MBEDTLS_ALT_ASSERT(0);
    }

    return;
}

void mbedtls_sha1_update_alt(mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen)
{
    ali_crypto_result result;

    result = ali_sha1_update(input, ilen, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        SSL_DBG_LOG("sha1 update fail - 0x%x\n", result);
        MBEDTLS_ALT_ASSERT(0);
    }

    return;
}

void mbedtls_sha1_finish_alt(mbedtls_sha1_context *ctx, unsigned char output[20])
{
    ali_crypto_result result;

    result = ali_sha1_final(output, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        SSL_DBG_LOG("sha1 final fail - 0x%x\n", result);
        MBEDTLS_ALT_ASSERT(0);
    }

    return;
}

void mbedtls_sha1_alt(const unsigned char *input, size_t ilen, unsigned char output[20])
{
    mbedtls_sha1_context ctx;

    mbedtls_sha1_init_alt( &ctx );
    mbedtls_sha1_starts_alt( &ctx );
    mbedtls_sha1_update_alt( &ctx, input, ilen );
    mbedtls_sha1_finish_alt( &ctx, output );
    mbedtls_sha1_free_alt( &ctx );
}
#endif /* MBEDTLS_SHA1_ALT */

#if defined(MBEDTLS_SHA256_ALT)
void mbedtls_sha256_init_alt(mbedtls_sha256_context *ctx)
{
    memset(ctx, 0, sizeof(mbedtls_sha256_context));
}

void mbedtls_sha256_free_alt(mbedtls_sha256_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    if (ctx->ali_ctx) {
        mbedtls_free(ctx->ali_ctx);
    }

    mbedtls_zeroize(ctx, sizeof(mbedtls_sha256_context));
}

void mbedtls_sha256_clone_alt(mbedtls_sha256_context *dst,
                              const mbedtls_sha256_context *src)
{
    dst->ali_ctx = mbedtls_calloc(1, src->size);
    if (dst->ali_ctx == NULL) {
        SSL_DBG_LOG("mbedtls_calloc(%d) fail\n", (int)src->size);
        MBEDTLS_ALT_ASSERT(0);
    }

    dst->size = src->size;
    memcpy(dst->ali_ctx, src->ali_ctx, dst->size);
}

void mbedtls_sha256_starts_alt(mbedtls_sha256_context *ctx, int is224)
{
    size_t ctx_size;
    ali_crypto_result result;

    if (is224 == 1) {
        SSL_DBG_LOG("sha224 is not surpported!\n");
        return;
    }

    if (NULL == ctx->ali_ctx) {
        result = ali_sha256_get_ctx_size(&ctx_size);
        if (result != ALI_CRYPTO_SUCCESS) {
            SSL_DBG_LOG("get ctx size fail - 0x%x\n", result);
            MBEDTLS_ALT_ASSERT(0);
        }

        ctx->size = ctx_size;
        ctx->ali_ctx = mbedtls_calloc(1, ctx_size);
        if (ctx->ali_ctx == NULL) {
            SSL_DBG_LOG("mbedtls_calloc(%d) fail\n", (int)ctx_size);
            MBEDTLS_ALT_ASSERT(0);
        } else {
            memset(ctx->ali_ctx, 0, ctx_size);
        }
    }

    result = ali_sha256_init(ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        SSL_DBG_LOG("sha256 init fail - 0x%x\n", result);
        MBEDTLS_ALT_ASSERT(0);
    }

    return;
}

void mbedtls_sha256_update_alt(mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen)
{
    ali_crypto_result result;

    result = ali_sha256_update(input, ilen, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        SSL_DBG_LOG("sha256 update fail - 0x%x\n", result);
        MBEDTLS_ALT_ASSERT(0);
    }

    return;
}

void mbedtls_sha256_finish_alt(mbedtls_sha256_context *ctx, unsigned char output[32])
{
    ali_crypto_result result;

    result = ali_sha256_final(output, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        SSL_DBG_LOG("sha256 final fail - 0x%x\n", result);
        MBEDTLS_ALT_ASSERT(0);
    }

    return;
}

void mbedtls_sha256_alt(const unsigned char *input, size_t ilen, unsigned char output[32], int is224)
{
    mbedtls_sha256_context ctx;

    mbedtls_sha256_init_alt( &ctx );
    mbedtls_sha256_starts_alt( &ctx, is224 );
    mbedtls_sha256_update_alt( &ctx, input, ilen );
    mbedtls_sha256_finish_alt( &ctx, output );
    mbedtls_sha256_free_alt( &ctx );
}
#endif /* MBEDTLS_SHA256_ALT */

#if defined(MBEDTLS_TIMING_ALT)
struct _hr_time
{
    unsigned long long start;
};

unsigned long mbedtls_timing_get_timer( struct mbedtls_timing_hr_time *val, int reset )
{
    unsigned long delta;
    unsigned long long offset;
    struct _hr_time *t = (struct _hr_time *) val;

    offset = ls_osa_get_time_ms();

    if( reset )
    {
        t->start = ls_osa_get_time_ms();
        return( 0 );
    }

    delta = offset - t->start;

    return( delta );
}

void mbedtls_timing_set_delay( void *data, uint32_t int_ms, uint32_t fin_ms )
{
    mbedtls_timing_delay_context *ctx = (mbedtls_timing_delay_context *) data;

    ctx->int_ms = int_ms;
    ctx->fin_ms = fin_ms;

    if( fin_ms != 0 )
        (void) mbedtls_timing_get_timer( &ctx->timer, 1 );
}

int mbedtls_timing_get_delay( void *data )
{
    mbedtls_timing_delay_context *ctx = (mbedtls_timing_delay_context *) data;
    unsigned long elapsed_ms;

    if( ctx->fin_ms == 0 )
        return( -1 );

    elapsed_ms = mbedtls_timing_get_timer( &ctx->timer, 0 );

    if( elapsed_ms >= ctx->fin_ms )
        return( 2 );

    if( elapsed_ms >= ctx->int_ms )
        return( 1 );

    return( 0 );
}
#endif /* MBEDTLS_TIMING_ALT */

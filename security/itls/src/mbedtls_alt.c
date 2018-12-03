/**
 * Copyright (C) 2017 The YunOS IoT Project. All rights reserved.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "itls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "itls/md.h"
#include "itls/debug.h"
#include "itls/threading.h"
#include "itls/aes.h"
#include "itls/sha1.h"
#include "itls/sha256.h"
#include "itls/platform.h"

#if defined(CONFIG_PLAT_AOS)
#include <aos/kernel.h>
#endif

#if defined(MBEDTLS_AES_ALT) || defined(MBEDTLS_MD_ALT)
#include "ali_crypto.h"
#endif

#ifndef PLATFORM_ANDROID
#define MBEDTLS_ALT_PRINT(_f, _a ...)  \
        printf("%s %d: "_f,  __FUNCTION__, __LINE__, ##_a)
#else  /* PLATFORM_ANDROID */

#include <android/log.h>

#define __LOG_TAG__             "ITLS_LOG"

#define LOG_INF(...)            __android_log_print(        \
                                        ANDROID_LOG_INFO,   \
                                        __LOG_TAG__,        \
                                        __VA_ARGS__)

#define MBEDTLS_ALT_PRINT(_f, _a ...)  \
            LOG_INF("%s %d: "_f,  __FUNCTION__, __LINE__, ##_a)

#endif /* PLATFORM_ANDROID */

#define MBEDTLS_ALT_ASSERT(_x)                          \
    do {                                                \
        if (!(_x)) {                                    \
            printf("ASSERT FAILURE:\n");                \
            printf("%s (%d): %s\n",                     \
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

#if defined(MBEDTLS_PLATFORM_ALT)

#if defined(CONFIG_MEM_TEST_EN)

#define MBEDTLS_MEM_INFO_MAGIC   0x12345678

static unsigned int mbedtls_mem_used = 0;
static unsigned int mbedtls_max_mem_used = 0;

typedef struct {
    int magic;
    int size;
} mbedtls_mem_info_t;
#endif

void *mbedtls_calloc( size_t n, size_t size )
{
    void *buf;
#if defined(CONFIG_MEM_TEST_EN)
    mbedtls_mem_info_t *mem_info;
#endif

    if (n == 0 || size == 0) {
        return NULL;
    }

#if defined(CONFIG_MEM_TEST_EN)
    buf = malloc(n * size + sizeof(mbedtls_mem_info_t));
    if (NULL == buf) {
        return NULL;
    } else {
        memset(buf, 0, n * size + sizeof(mbedtls_mem_info_t));
    }

    mem_info = (mbedtls_mem_info_t *)buf;
    mem_info->magic = MBEDTLS_MEM_INFO_MAGIC;
    mem_info->size = n * size;
    buf += sizeof(mbedtls_mem_info_t);

    mbedtls_mem_used += mem_info->size;
    if (mbedtls_mem_used > mbedtls_max_mem_used) {
        mbedtls_max_mem_used = mbedtls_mem_used;
    }

    MBEDTLS_ALT_PRINT("INFO -- itls malloc: 0x%x %d  total used: %d  max used: %d\n",
                       (uint32_t)buf, size, mbedtls_mem_used, mbedtls_max_mem_used);
#else
#if defined(CONFIG_PLAT_AOS)
    buf = aos_malloc(n * size);
#else
    buf = malloc(n * size);
#endif
    if (NULL == buf) {
        return NULL;
    } else {
        memset(buf, 0, n * size);
    }
#endif

    return buf;
}

void mbedtls_free( void *ptr )
{
#if defined(CONFIG_MEM_TEST_EN)
    mbedtls_mem_info_t *mem_info;
#endif

    if (NULL == ptr) {
        return;
    }

#if defined(CONFIG_MEM_TEST_EN)
    mem_info = ptr - sizeof(mbedtls_mem_info_t);
    if (mem_info->magic != MBEDTLS_MEM_INFO_MAGIC) {
        MBEDTLS_ALT_PRINT("bad mem magic: 0x%x\n", mem_info->magic);
        return;
    }

    mbedtls_mem_used -= mem_info->size;
    MBEDTLS_ALT_PRINT("INFO -- itls free: 0x%x %d  total used: %d  max used: %d\n",
                       (uint32_t)ptr, mem_info->size, mbedtls_mem_used, mbedtls_max_mem_used);

    free(mem_info);
#else
#if defined(CONFIG_PLAT_AOS)
    aos_free(ptr);
#else
    free(ptr);
#endif
#endif
}

#endif

#if defined(MBEDTLS_THREADING_ALT)
void threading_mutex_init(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || mutex->is_valid) {
        return;
    }

    mutex->is_valid = aos_mutex_new(&mutex->mutex) == 0;
}

void threading_mutex_free(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid) {
        return;
    }

    aos_mutex_free(&mutex->mutex);
    mutex->is_valid = 0;
}

int threading_mutex_lock(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid) {
        return ( MBEDTLS_ERR_THREADING_BAD_INPUT_DATA );
    }

    if (aos_mutex_lock( &mutex->mutex, AOS_WAIT_FOREVER) != 0 ) {
        return (MBEDTLS_ERR_THREADING_MUTEX_ERROR);
    }

    return 0;
}

int threading_mutex_unlock( mbedtls_threading_mutex_t *mutex )
{
    if ( mutex == NULL || ! mutex->is_valid ) {
        return ( MBEDTLS_ERR_THREADING_BAD_INPUT_DATA );
    }

    if ( aos_mutex_unlock( &mutex->mutex ) != 0 ) {
        return ( MBEDTLS_ERR_THREADING_MUTEX_ERROR );
    }

    return ( 0 );
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
        mbedtls_free(ctx->ali_ctx);
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
        MBEDTLS_ALT_PRINT("invalid input length - %d\n", (uint32_t)length);
        return -1;
    }

    if (ctx->ali_ctx == NULL) {
        result = ali_aes_get_ctx_size(AES_CBC, &ctx_size);
        if (result != ALI_CRYPTO_SUCCESS) {
            MBEDTLS_ALT_PRINT("get ctx size fail - 0x%x\n", result);
            return -1;
        }

        ctx->ali_ctx = mbedtls_calloc(1, ctx_size);
        if (ctx->ali_ctx == NULL) {
            MBEDTLS_ALT_PRINT("mbedtls_calloc(%d) fail\n", (uint32_t)ctx_size);
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
        MBEDTLS_ALT_PRINT("mbedtls_calloc(%d) fail\n", (uint32_t)src->size);
        MBEDTLS_ALT_ASSERT(0);
    }

    dst->size = src->size;
    memcpy(dst->ali_ctx, src->ali_ctx, dst->size);
}

void mbedtls_sha1_starts_alt(mbedtls_sha1_context *ctx)
{
    size_t ctx_size;
    hash_type_t type = SHA1;
    ali_crypto_result result;

    if (NULL == ctx->ali_ctx) {
        result = ali_hash_get_ctx_size(type, &ctx_size);
        if (result != ALI_CRYPTO_SUCCESS) {
            MBEDTLS_ALT_PRINT("get ctx size fail - 0x%x\n", result);
            MBEDTLS_ALT_ASSERT(0);
        }

        ctx->size = ctx_size;
        ctx->ali_ctx = mbedtls_calloc(1, ctx_size);
        if (ctx->ali_ctx == NULL) {
            MBEDTLS_ALT_PRINT("mbedtls_calloc(%d) fail\n", (uint32_t)ctx_size);
            MBEDTLS_ALT_ASSERT(0);
        } else {
            memset(ctx->ali_ctx, 0, ctx_size);
        }
    }

    result = ali_hash_init(type, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("sha1 init fail - 0x%x\n", result);
        MBEDTLS_ALT_ASSERT(0);
    }

    return;
}

void mbedtls_sha1_update_alt(mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen)
{
    ali_crypto_result result;

    result = ali_hash_update(input, ilen, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("sha1 update fail - 0x%x\n", result);
        MBEDTLS_ALT_ASSERT(0);
    }

    return;
}

void mbedtls_sha1_finish_alt(mbedtls_sha1_context *ctx, unsigned char output[20])
{
    ali_crypto_result result;

    result = ali_hash_final(output, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("sha1 final fail - 0x%x\n", result);
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
        MBEDTLS_ALT_PRINT("mbedtls_calloc(%d) fail\n", (uint32_t)src->size);
        MBEDTLS_ALT_ASSERT(0);
    }

    dst->size = src->size;
    memcpy(dst->ali_ctx, src->ali_ctx, dst->size);
}

void mbedtls_sha256_starts_alt(mbedtls_sha256_context *ctx, int is224)
{
    size_t ctx_size;
    hash_type_t type;
    ali_crypto_result result;

    if (is224 == 0) {
        type = SHA256;
    } else {
        type = SHA224;
    }

    if (NULL == ctx->ali_ctx) {
        result = ali_hash_get_ctx_size(type, &ctx_size);
        if (result != ALI_CRYPTO_SUCCESS) {
            MBEDTLS_ALT_PRINT("get ctx size fail - 0x%x\n", result);
            MBEDTLS_ALT_ASSERT(0);
        }

        ctx->size = ctx_size;
        ctx->ali_ctx = mbedtls_calloc(1, ctx_size);
        if (ctx->ali_ctx == NULL) {
            MBEDTLS_ALT_PRINT("mbedtls_calloc(%d) fail\n", (uint32_t)ctx_size);
            MBEDTLS_ALT_ASSERT(0);
        } else {
            memset(ctx->ali_ctx, 0, ctx_size);
        }
    }

    result = ali_hash_init(type, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("sha256 init fail - 0x%x\n", result);
        MBEDTLS_ALT_ASSERT(0);
    }

    return;
}

void mbedtls_sha256_update_alt(mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen)
{
    ali_crypto_result result;

    result = ali_hash_update(input, ilen, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("sha256 update fail - 0x%x\n", result);
        MBEDTLS_ALT_ASSERT(0);
    }

    return;
}

void mbedtls_sha256_finish_alt(mbedtls_sha256_context *ctx, unsigned char output[32])
{
    ali_crypto_result result;

    result = ali_hash_final(output, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("sha256 final fail - 0x%x\n", result);
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

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
#endif

#if defined(MBEDTLS_MD5_ALT)
#include "mbedtls/md5.h"
#endif
#if defined(MBEDTLS_SHA1_ALT)
#include "mbedtls/sha1.h"
#endif
#if defined(MBEDTLS_SHA256_ALT)
#include "mbedtls/sha256.h"
#endif

#if defined(MBEDTLS_IOT_PLAT_AOS)
#include <aos/kernel.h>
#endif

#if defined(MBEDTLS_PLATFORM_ALT)
#include "mbedtls/platform.h"
#else
#define mbedtls_calloc calloc
#define mbedtls_free free
#endif

#if defined(MBEDTLS_AES_ALT) || defined(MBEDTLS_PK_ALT) || \
  defined(MBEDTLS_MD_ALT)
#include "ali_crypto.h"
#endif

#if defined(LOG_SIMPLE)
#define MBEDTLS_ALT_PRINT(_f, ...) \
    printf("Mbedtls Error: %s %d\n", __FUNCTION__, __LINE__)

#define MBEDTLS_ALT_ASSERT(_x)                                         \
    do {                                                               \
        if (!(_x)) {                                                   \
            printf("Mbedtls ASSERT: %s %d\n", __FUNCTION__, __LINE__); \
            while (1) /* loop */                                       \
                ;                                                      \
        }                                                              \
    } while (0)
#else
#define MBEDTLS_ALT_PRINT(_f, ...) \
    printf("%s %d: "_f, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define MBEDTLS_ALT_ASSERT(_x)                                         \
    do {                                                               \
        if (!(_x)) {                                                   \
            printf("ASSERT FAILURE:\n");                               \
            printf("%s (%d): %s\n", __FILE__, __LINE__, __FUNCTION__); \
            while (1) /* loop */                                       \
                ;                                                      \
        }                                                              \
    } while (0)
#endif

#if defined(MBEDTLS_AES_ALT)
/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize(void *v, size_t n)
{
    volatile unsigned char *p = v;
    while (n--) {
        *p++ = 0;
    }
}
#endif

#if defined(MBEDTLS_PLATFORM_ALT)
#if defined(XTENSA_MALLOC_IRAM)
extern void *iram_heap_malloc(size_t xWantedSize);
extern void  iram_heap_free(void *pv);
extern int   iram_heap_check_addr(void *addr);
void *       mbedtls_calloc(size_t n, size_t size)
{
    void *buf = NULL;

    if (n == 0 || size == 0) {
        return NULL;
    }
    buf = iram_heap_malloc(n * size);
    if (!buf) {
        buf = malloc(n * size);
    }

    if (NULL == buf) {
        return NULL;
    } else {
        memset(buf, 0, n * size);
    }

    return buf;
}

void mbedtls_free(void *ptr)
{
    if (NULL == ptr) {
        return;
    }

    if (iram_heap_check_addr(ptr) == 1) {
        iram_heap_free(ptr);
    } else {
        free(ptr);
    }
}
#else
void *mbedtls_calloc(size_t n, size_t size)
{
    void *buf;

    if (n == 0 || size == 0) {
        return NULL;
    }

#if defined(MBEDTLS_IOT_PLAT_AOS)
    buf = aos_malloc(n * size);
#else
    buf = malloc(n * size);
#endif
    if (NULL == buf) {
        return NULL;
    } else {
        memset(buf, 0, n * size);
    }

    return buf;
}

void mbedtls_free(void *ptr)
{
    if (NULL == ptr) {
        return;
    }

#if defined(MBEDTLS_IOT_PLAT_AOS)
    aos_free(ptr);
#else
    free(ptr);
#endif
}
#endif
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
        return (MBEDTLS_ERR_THREADING_BAD_INPUT_DATA);
    }

    if (aos_mutex_lock(&mutex->mutex, AOS_WAIT_FOREVER) != 0) {
        return (MBEDTLS_ERR_THREADING_MUTEX_ERROR);
    }

    return 0;
}

int threading_mutex_unlock(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid) {
        return (MBEDTLS_ERR_THREADING_BAD_INPUT_DATA);
    }

    if (aos_mutex_unlock(&mutex->mutex) != 0) {
        return (MBEDTLS_ERR_THREADING_MUTEX_ERROR);
    }

    return (0);
}

#endif /* MBEDTLS_THREADING_ALT */

#if defined(MBEDTLS_AES_ALT)
void mbedtls_aes_init_alt(mbedtls_aes_context *ctx)
{
    memset(ctx, 0, sizeof(mbedtls_aes_context));
}

void mbedtls_aes_free_alt(mbedtls_aes_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    if (ctx->ali_ctx) {
        mbedtls_free(ctx->ali_ctx);
    }

    mbedtls_zeroize(ctx, sizeof(mbedtls_aes_context));
}

int mbedtls_aes_setkey_enc_alt(mbedtls_aes_context *ctx,
                               const unsigned char *key, unsigned int keybits)
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

int mbedtls_aes_setkey_dec_alt(mbedtls_aes_context *ctx,
                               const unsigned char *key, unsigned int keybits)
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

int mbedtls_aes_crypt_ecb_alt(mbedtls_aes_context *ctx, int mode,
                              const unsigned char input[16],
                              unsigned char       output[16])
{
    (void)ctx;
    (void)mode;
    (void)input;
    (void)output;

    MBEDTLS_ALT_PRINT("not support!\n");

    return -1;
}

int mbedtls_aes_crypt_cbc_alt(mbedtls_aes_context *ctx, int mode, size_t length,
                              unsigned char iv[16], const unsigned char *input,
                              unsigned char *output)
{
    bool              is_enc;
    size_t            ctx_size;
    ali_crypto_result result;

    if (ctx == NULL || length == 0 || input == NULL || output == NULL) {
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

        ctx->ali_ctx = mbedtls_calloc(1, ctx_size);
        if (ctx->ali_ctx == NULL) {
            MBEDTLS_ALT_PRINT("mbedtls_calloc(%d) fail\n", ctx_size);
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

        result = ali_aes_init(AES_CBC, is_enc, ctx->key, NULL, ctx->key_len, iv,
                              ctx->ali_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            MBEDTLS_ALT_PRINT("aes init fail - 0x%x\n", result);
            return -1;
        } else {
            ctx->reset  = 1;
            ctx->status = AES_STATUS_PROCESS;
        }
    }

    result = ali_aes_process(input, output, length, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("aes process fail - 0x%x\n", result);
        return -1;
    }

    return 0;
}

#endif /* MBEDTLS_AES_ALT */

#if defined(MBEDTLS_PK_ALT)
int rsa_verify_alt(void *ctx, size_t hash_id, const unsigned char *hash,
                   size_t hash_len, const unsigned char *sig, size_t sig_len)
{
    int                  ret     = 0;
    bool                 result1 = false;
    ali_crypto_result    result;
    mbedtls_rsa_context *rsa_ctx;
    hash_type_t          hash_type;
    rsa_padding_t        rsa_padding;
    size_t               pub_key_len;
    uint8_t *            pub_key = NULL;

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

    switch (hash_id) {
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

    pub_key = mbedtls_calloc(1, pub_key_len);
    if (pub_key == NULL) {
        MBEDTLS_ALT_PRINT("mbedtls_calloc(%d) fail\n", pub_key_len);
        ret = -1;
        goto _out;
    } else {
        memset(pub_key, 0, pub_key_len);
    }

    result = ali_rsa_init_pubkey(rsa_ctx->n_len << 3, rsa_ctx->rsa_n,
                                 rsa_ctx->n_len, rsa_ctx->rsa_e, rsa_ctx->e_len,
                                 (rsa_pubkey_t *)pub_key);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("init pub_key fail - 0x%x\n", result);
        ret = -1;
        goto _out;
    }

    rsa_padding.type                 = RSASSA_PKCS1_V1_5;
    rsa_padding.pad.rsassa_v1_5.type = hash_type;
    result = ali_rsa_verify((rsa_pubkey_t *)pub_key, hash, hash_len, sig,
                            sig_len, rsa_padding, &result1);
    if (result != ALI_CRYPTO_SUCCESS || result1 == false) {
        MBEDTLS_ALT_PRINT("rsa verify fail - 0x%x\n", result);
        ret = -1;
        goto _out;
    }

_out:
    if (pub_key) {
        mbedtls_free(pub_key);
    }

    return ret;
}

int rsa_sign_alt(void *ctx, size_t hash_id, const unsigned char *hash,
                 size_t hash_len, unsigned char *sig, size_t *sig_len)
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

int rsa_decrypt_alt(void *ctx, const unsigned char *input, size_t ilen,
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

int rsa_encrypt_alt(void *ctx, const unsigned char *input, size_t ilen,
                    unsigned char *output, size_t *olen)
{
    int                  ret = 0;
    ali_crypto_result    result;
    mbedtls_rsa_context *rsa_ctx;
    rsa_padding_t        rsa_padding;
    size_t               pub_key_len;
    uint8_t *            pub_key = NULL;

    if (ctx == NULL || input == NULL || ilen == 0 || output == NULL ||
        olen == NULL) {
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

    pub_key = mbedtls_calloc(1, pub_key_len);
    if (pub_key == NULL) {
        MBEDTLS_ALT_PRINT("mbedtls_calloc(%d) fail\n", pub_key_len);
        ret = -1;
        goto _out;
    } else {
        memset(pub_key, 0, pub_key_len);
    }

    result = ali_rsa_init_pubkey(rsa_ctx->n_len << 3, rsa_ctx->rsa_n,
                                 rsa_ctx->n_len, rsa_ctx->rsa_e, rsa_ctx->e_len,
                                 (rsa_pubkey_t *)pub_key);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("init pub_key fail - 0x%x\n", result);
        ret = -1;
        goto _out;
    }

    rsa_padding.type = RSAES_PKCS1_V1_5;
    result = ali_rsa_public_encrypt((rsa_pubkey_t *)pub_key, input, ilen,
                                    output, olen, rsa_padding);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("rsa encrypt fail - 0x%x\n", result);
        ret = -1;
        goto _out;
    }

_out:
    if (pub_key) {
        mbedtls_free(pub_key);
    }

    return ret;
}

void mbedtls_rsa_init_alt(mbedtls_rsa_context *ctx, int padding, int hash_id)
{
    memset(ctx, 0, sizeof(mbedtls_rsa_context));

    ctx->padding = padding;
    ctx->hash_id = hash_id;
}


void mbedtls_rsa_free_alt(mbedtls_rsa_context *ctx)
{
    memset(ctx, 0, sizeof(mbedtls_rsa_context));
}

#endif /* MBEDTLS_PK_ALT */

#if defined(MBEDTLS_MD5_ALT)
void mbedtls_md5_init_alt(mbedtls_md5_context *ctx)
{
    memset(ctx, 0, sizeof(mbedtls_md5_context));
}

void mbedtls_md5_free_alt(mbedtls_md5_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    if (ctx->ali_ctx) {
        mbedtls_free(ctx->ali_ctx);
    }

    mbedtls_zeroize(ctx, sizeof(mbedtls_md5_context));
}

void mbedtls_md5_clone_alt(mbedtls_md5_context *      dst,
                           const mbedtls_md5_context *src)
{
    dst->ali_ctx = mbedtls_calloc(1, src->size);
    if (dst->ali_ctx == NULL) {
        MBEDTLS_ALT_PRINT("mbedtls_calloc(%d) fail\n", src->size);
        MBEDTLS_ALT_ASSERT(0);
    }

    dst->size = src->size;
    memcpy(dst->ali_ctx, src->ali_ctx, dst->size);
}

void mbedtls_md5_starts_alt(mbedtls_md5_context *ctx)
{
    size_t            ctx_size;
    hash_type_t       type = MD5;
    ali_crypto_result result;

    if (NULL == ctx->ali_ctx) {
        result = ali_hash_get_ctx_size(type, &ctx_size);
        if (result != ALI_CRYPTO_SUCCESS) {
            MBEDTLS_ALT_PRINT("get ctx size fail - 0x%x\n", result);
            MBEDTLS_ALT_ASSERT(0);
        }

        ctx->size    = ctx_size;
        ctx->ali_ctx = mbedtls_calloc(1, ctx_size);
        if (ctx->ali_ctx == NULL) {
            MBEDTLS_ALT_PRINT("mbedtls_calloc(%d) fail\n", ctx_size);
            MBEDTLS_ALT_ASSERT(0);
        } else {
            memset(ctx->ali_ctx, 0, ctx_size);
        }
    }

    result = ali_hash_init(type, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("md5 init fail - 0x%x\n", result);
        MBEDTLS_ALT_ASSERT(0);
    }

    return;
}

void mbedtls_md5_update_alt(mbedtls_md5_context *ctx,
                            const unsigned char *input, size_t ilen)
{
    ali_crypto_result result;

    result = ali_hash_update(input, ilen, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("md5 update fail - 0x%x\n", result);
        MBEDTLS_ALT_ASSERT(0);
    }

    return;
}

void mbedtls_md5_finish_alt(mbedtls_md5_context *ctx, unsigned char output[16])
{
    ali_crypto_result result;

    result = ali_hash_final(output, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("md5 final fail - 0x%x\n", result);
        MBEDTLS_ALT_ASSERT(0);
    }

    return;
}

void mbedtls_md5_alt(const unsigned char *input, size_t ilen,
                     unsigned char output[16])
{
    mbedtls_md5_context ctx;

    mbedtls_md5_init_alt(&ctx);
    mbedtls_md5_starts_alt(&ctx);
    mbedtls_md5_update_alt(&ctx, input, ilen);
    mbedtls_md5_finish_alt(&ctx, output);
    mbedtls_md5_free_alt(&ctx);
}
#endif /* MBEDTLS_MD5_ALT */

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

void mbedtls_sha1_clone_alt(mbedtls_sha1_context *      dst,
                            const mbedtls_sha1_context *src)
{
    dst->ali_ctx = mbedtls_calloc(1, src->size);
    if (dst->ali_ctx == NULL) {
        MBEDTLS_ALT_PRINT("mbedtls_calloc(%d) fail\n", src->size);
        MBEDTLS_ALT_ASSERT(0);
    }

    dst->size = src->size;
    memcpy(dst->ali_ctx, src->ali_ctx, dst->size);
}

void mbedtls_sha1_starts_alt(mbedtls_sha1_context *ctx)
{
    size_t            ctx_size;
    hash_type_t       type = SHA1;
    ali_crypto_result result;

    if (NULL == ctx->ali_ctx) {
        result = ali_hash_get_ctx_size(type, &ctx_size);
        if (result != ALI_CRYPTO_SUCCESS) {
            MBEDTLS_ALT_PRINT("get ctx size fail - 0x%x\n", result);
            MBEDTLS_ALT_ASSERT(0);
        }

        ctx->size    = ctx_size;
        ctx->ali_ctx = mbedtls_calloc(1, ctx_size);
        if (ctx->ali_ctx == NULL) {
            MBEDTLS_ALT_PRINT("mbedtls_calloc(%d) fail\n", ctx_size);
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

void mbedtls_sha1_update_alt(mbedtls_sha1_context *ctx,
                             const unsigned char *input, size_t ilen)
{
    ali_crypto_result result;

    result = ali_hash_update(input, ilen, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("sha1 update fail - 0x%x\n", result);
        MBEDTLS_ALT_ASSERT(0);
    }

    return;
}

void mbedtls_sha1_finish_alt(mbedtls_sha1_context *ctx,
                             unsigned char         output[20])
{
    ali_crypto_result result;

    result = ali_hash_final(output, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("sha1 final fail - 0x%x\n", result);
        MBEDTLS_ALT_ASSERT(0);
    }

    return;
}

void mbedtls_sha1_alt(const unsigned char *input, size_t ilen,
                      unsigned char output[20])
{
    mbedtls_sha1_context ctx;

    mbedtls_sha1_init_alt(&ctx);
    mbedtls_sha1_starts_alt(&ctx);
    mbedtls_sha1_update_alt(&ctx, input, ilen);
    mbedtls_sha1_finish_alt(&ctx, output);
    mbedtls_sha1_free_alt(&ctx);
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

void mbedtls_sha256_clone_alt(mbedtls_sha256_context *      dst,
                              const mbedtls_sha256_context *src)
{
    dst->ali_ctx = mbedtls_calloc(1, src->size);
    if (dst->ali_ctx == NULL) {
        MBEDTLS_ALT_PRINT("mbedtls_calloc(%d) fail\n", src->size);
        MBEDTLS_ALT_ASSERT(0);
    }

    dst->size = src->size;
    memcpy(dst->ali_ctx, src->ali_ctx, dst->size);
}

void mbedtls_sha256_starts_alt(mbedtls_sha256_context *ctx, int is224)
{
    size_t            ctx_size;
    hash_type_t       type;
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

        ctx->size    = ctx_size;
        ctx->ali_ctx = mbedtls_calloc(1, ctx_size);
        if (ctx->ali_ctx == NULL) {
            MBEDTLS_ALT_PRINT("mbedtls_calloc(%d) fail\n", ctx_size);
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

void mbedtls_sha256_update_alt(mbedtls_sha256_context *ctx,
                               const unsigned char *input, size_t ilen)
{
    ali_crypto_result result;

    result = ali_hash_update(input, ilen, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("sha256 update fail - 0x%x\n", result);
        MBEDTLS_ALT_ASSERT(0);
    }

    return;
}

void mbedtls_sha256_finish_alt(mbedtls_sha256_context *ctx,
                               unsigned char           output[32])
{
    ali_crypto_result result;

    result = ali_hash_final(output, ctx->ali_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        MBEDTLS_ALT_PRINT("sha256 final fail - 0x%x\n", result);
        MBEDTLS_ALT_ASSERT(0);
    }

    return;
}

void mbedtls_sha256_alt(const unsigned char *input, size_t ilen,
                        unsigned char output[32], int is224)
{
    mbedtls_sha256_context ctx;

    mbedtls_sha256_init_alt(&ctx);
    mbedtls_sha256_starts_alt(&ctx, is224);
    mbedtls_sha256_update_alt(&ctx, input, ilen);
    mbedtls_sha256_finish_alt(&ctx, output);
    mbedtls_sha256_free_alt(&ctx);
}
#endif /* MBEDTLS_SHA256_ALT */

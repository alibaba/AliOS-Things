/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <breeze_hal_sec.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"

#include "ali_crypto.h"

#define KEY_LEN 16

void *sec_aes128_init(const uint8_t *key, const uint8_t *iv)
{
    ali_crypto_result result;
    void             *aes_ctx;
    size_t            aes_ctx_size, alloc_siz;
    uint8_t          *p;

    result = breeze_aes_get_ctx_size(AES_CBC, &aes_ctx_size);
    if (result != ALI_CRYPTO_SUCCESS) {
        LOGE("aos_awss", "get ctx size fail(%08x)", result);
        return NULL;
    }

    alloc_siz = aes_ctx_size + KEY_LEN * 2 + sizeof(bool);
    aes_ctx   = aos_malloc(alloc_siz);
    if (aes_ctx == NULL) {
        LOGE("aos_awss", "kmalloc(%d) fail", (int)aes_ctx_size);
        return NULL;
    }
    memset(aes_ctx, 0, alloc_siz);

    p = (uint8_t *)aes_ctx + aes_ctx_size;
    memcpy(p, key, KEY_LEN);
    p += KEY_LEN;
    memcpy(p, iv, KEY_LEN);

    return aes_ctx;
}

int sec_aes128_destroy(void *aes)
{
    if (aes) {
        aos_free(aes);
    }

    return 0;
}

static int platform_aes128_encrypt_decrypt(void *aes_ctx, const void *src,
                                           size_t siz, void *dst, bool is_enc)
{
    ali_crypto_result result;
    size_t            dlen, in_len = siz, ctx_siz;
    uint8_t          *p, *key, *iv;
    if (aes_ctx == NULL) {
        LOGE("aos_awss", "platform_aes128_encrypt_decrypt aes_ctx is NULL");
        return -1;
    }
    result = breeze_aes_get_ctx_size(AES_CBC, &ctx_siz);
    if (result != ALI_CRYPTO_SUCCESS) {
        LOGE("aos_awss", "get ctx size fail(%08x)", result);
        return 0;
    }

    p   = (uint8_t *)aes_ctx + ctx_siz;
    key = p;
    p += KEY_LEN;
    iv = p;

    in_len <<= 4;
    dlen = in_len;

    extern ali_crypto_result breeze_aes_init(aes_type_t type, bool is_enc,
                               const uint8_t *key1, const uint8_t *key2,
                               size_t keybytes, const uint8_t *iv,
                               void *context);

    result = breeze_aes_init(AES_CBC, is_enc, key, NULL, KEY_LEN, iv, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        LOGE("aos_awss", "breeze_aes_init fail(%08x)", result);
        return 0;
    }

    extern ali_crypto_result breeze_aes_finish(const uint8_t *src, size_t src_size,
                                 uint8_t *dst, size_t *dst_size,
                                 sym_padding_t padding, void *context);
    result = breeze_aes_finish(src, in_len, dst, &dlen, SYM_NOPAD, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        LOGE("aos_awss", "aes_finish fail(%08x)", result);
        return -1;
    }

    return 0;
}

int sec_aes128_cbc_encrypt(void *aes, const void *src, size_t block_num,
                           void *dst)
{
    return platform_aes128_encrypt_decrypt(aes, src, block_num, dst, true);
}

int sec_aes128_cbc_decrypt(void *aes, const void *src, size_t block_num,
                           void *dst)
{
    return platform_aes128_encrypt_decrypt(aes, src, block_num, dst, false);
}

void sec_sha256_init(SHA256_CTX *ctx)
{
#ifdef USE_EXTERNAL_MEBDTLS
    mbedtls_sha256_init(ctx);
    mbedtls_sha256_starts_ret(ctx, 0);
#else
    extern void __sha256_init(SHA256_CTX *ctx);
    __sha256_init(ctx);
#endif
}

void sec_sha256_update(SHA256_CTX *ctx, const BYTE data[], size_t len)
{
#ifdef USE_EXTERNAL_MEBDTLS
    mbedtls_sha256_update_ret(ctx, data, len);
#else
    extern void __sha256_update(SHA256_CTX *ctx, const BYTE data[], size_t len);
    __sha256_update(ctx, data, len);
#endif
}

void sec_sha256_final(SHA256_CTX *ctx, BYTE hash[])
{
#ifdef USE_EXTERNAL_MEBDTLS
    mbedtls_sha256_finish_ret(ctx, hash);
#else
   extern void __sha256_final(SHA256_CTX *ctx, BYTE hash[]);
   __sha256_final(ctx, hash);
#endif
}

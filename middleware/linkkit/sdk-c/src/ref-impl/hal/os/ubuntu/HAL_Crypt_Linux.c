/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#if defined(ALCS_ENABLED) || defined(COAP_COMM_ENABLED) || defined(DEV_BIND_ENABLED)

#include <string.h>
#include "iot_import.h"
#include "mbedtls/aes.h"

#define AES_BLOCK_SIZE 16

typedef struct {
    mbedtls_aes_context ctx;
    uint8_t iv[16];
    uint8_t key[16];
} platform_aes_t;

p_HAL_Aes128_t HAL_Aes128_Init(
            _IN_ const uint8_t *key,
            _IN_ const uint8_t *iv,
            _IN_ AES_DIR_t dir)
{
    int ret = 0;
    platform_aes_t *p_aes128 = NULL;

    if (!key || !iv) return p_aes128;

    p_aes128 = (platform_aes_t *)calloc(1, sizeof(platform_aes_t));
    if (!p_aes128) return p_aes128;

    mbedtls_aes_init(&p_aes128->ctx);

    if (dir == HAL_AES_ENCRYPTION) {
        ret = mbedtls_aes_setkey_enc(&p_aes128->ctx, key, 128);
    } else {
        ret = mbedtls_aes_setkey_dec(&p_aes128->ctx, key, 128);
    }

    if (ret == 0) {
        memcpy(p_aes128->iv, iv, 16);
        memcpy(p_aes128->key, key, 16);
    } else {
        free(p_aes128);
        p_aes128 = NULL;
    }

    return (p_HAL_Aes128_t *)p_aes128;
}

int HAL_Aes128_Destroy(_IN_ p_HAL_Aes128_t aes)
{
    if (!aes) return -1;

    mbedtls_aes_free(&((platform_aes_t *)aes)->ctx);
    free(aes);

    return 0;
}

int HAL_Aes128_Cbc_Encrypt(
            _IN_ p_HAL_Aes128_t aes,
            _IN_ const void *src,
            _IN_ size_t blockNum,
            _OU_ void *dst)
{
    int i   = 0;
    int ret = ret;
    platform_aes_t *p_aes128 = (platform_aes_t *)aes;

    if (!aes || !src || !dst) return -1;

    for (i = 0; i < blockNum; ++i) {
        ret = mbedtls_aes_crypt_cbc(&p_aes128->ctx, MBEDTLS_AES_ENCRYPT, AES_BLOCK_SIZE,
                                    p_aes128->iv, src, dst);
        src += 16;
        dst += 16;
    }

    return ret;
}

int HAL_Aes128_Cbc_Decrypt(
            _IN_ p_HAL_Aes128_t aes,
            _IN_ const void *src,
            _IN_ size_t blockNum,
            _OU_ void *dst)
{
    int i   = 0;
    int ret = -1;
    platform_aes_t *p_aes128 = (platform_aes_t *)aes;

    if (!aes || !src || !dst) return ret;

    for (i = 0; i < blockNum; ++i) {
        ret = mbedtls_aes_crypt_cbc(&p_aes128->ctx, MBEDTLS_AES_DECRYPT, AES_BLOCK_SIZE,
                                    p_aes128->iv, src, dst);
        src += 16;
        dst += 16;
    }

    return ret;
}
#if defined(MBEDTLS_CIPHER_MODE_CFB)
int HAL_Aes128_Cfb_Encrypt(
            _IN_ p_HAL_Aes128_t aes,
            _IN_ const void *src,
            _IN_ size_t length,
            _OU_ void *dst)
{
    size_t offset = 0;
    int ret = -1;
    platform_aes_t *p_aes128 = (platform_aes_t *)aes;

    if (!aes || !src || !dst) return ret;

    ret = mbedtls_aes_crypt_cfb128(&p_aes128->ctx, MBEDTLS_AES_ENCRYPT, length,
                                   &offset, p_aes128->iv, src, dst);
    return ret;
}
#endif

#if defined(MBEDTLS_CIPHER_MODE_CFB)
int HAL_Aes128_Cfb_Decrypt(
            _IN_ p_HAL_Aes128_t aes,
            _IN_ const void *src,
            _IN_ size_t length,
            _OU_ void *dst)
{
    size_t offset = 0;
    int ret = -1;
    platform_aes_t *p_aes128 = (platform_aes_t *)aes;

    if (!aes || !src || !dst) return ret;

    ret = mbedtls_aes_setkey_enc(&p_aes128->ctx, p_aes128->key, 128);
    ret = mbedtls_aes_crypt_cfb128(&p_aes128->ctx, MBEDTLS_AES_DECRYPT, length,
                                   &offset, p_aes128->iv, src, dst);
    return ret;
}
#endif
#endif
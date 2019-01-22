/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "kv_conf.h"

#if (KV_SECURE_SUPPORT)

#include <stdint.h>
#include <string.h>

#include "kv_adapt.h"
#include "kv_err.h"

#if (KV_SECURE_CRYPT_IMPL == 1)

#include "mbedtls/aes.h"

static mbedtls_aes_context aes_context;

static uint8_t aes_key[32] = {
    0x86, 0xf6, 0xd2, 0xbe, 0x45, 0xb5, 0xab, 0x9c,
    0xc7, 0xd5, 0x96, 0xf7, 0xaf, 0x45, 0xfa, 0xf7,
    0xbe, 0x6a, 0x5d, 0xb0, 0x04, 0xc4, 0xde, 0xb5,
    0xf5, 0x0c, 0x4f, 0xc3, 0x71, 0x19, 0x3e, 0xe8
};

static uint8_t aes_iv[16] = {
    0xef, 0x80, 0x18, 0xdc, 0xa3, 0x72, 0x72, 0x31,
    0x99, 0x2e, 0x3a, 0xba, 0x60, 0xf5, 0x0b, 0xd4
};

static int32_t _ofb_encrypt_wrap(uint8_t *input, uint8_t *output, uint32_t len)
{
    int32_t ret;

    uint32_t offset = 0;
    uint8_t  iv[16] = {0};

    memcpy(iv, aes_iv, sizeof(aes_iv));

    ret = mbedtls_aes_setkey_enc(&aes_context, aes_key, sizeof(aes_key) * 8);
    if (ret != KV_OK) {
        return ret;
    }

    ret = mbedtls_aes_crypt_ofb(&aes_context, len, &offset, iv, input, output);
    if (ret != KV_OK) {
        return ret;
    }

    return KV_OK;
}

int32_t kv_secure_encrypt(uint8_t *input, uint8_t *output, uint32_t input_len)
{
    if (_ofb_encrypt_wrap(input, output, input_len) != KV_OK) {
        return KV_ERR_ENCRYPT;
    }

    return KV_OK;
}

int32_t kv_secure_decrypt(uint8_t *input, uint8_t *output, uint32_t input_len)
{
    if (_ofb_encrypt_wrap(input, output, input_len) != KV_OK) {
        return KV_ERR_ENCRYPT;
    }

    return KV_OK;
}

#else /* User defined encrypt/decrypt implement here */

int32_t kv_secure_encrypt(uint8_t *input, uint8_t *output, uint32_t input_len)
{
    return KV_ERR_NOT_SUPPORT;
}

int32_t kv_secure_decrypt(uint8_t *input, uint8_t *output, uint32_t input_len)
{
    return KV_ERR_NOT_SUPPORT;
}

#endif /* KV_SECURE_CRYPT_IMPL */

#endif /* KV_SECURE_SUPPORT */


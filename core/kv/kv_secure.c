/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "kv_conf.h"

#if (KV_SECURE_SUPPORT)

#include <stdint.h>
#include <string.h>

#include "kv_api.h"

#include "kv_adapt.h"

#if (KV_SECURE_CRYPT_IMPL == 1)

#include "mbedtls/aes.h"

#define OFB_KEY_LEN 32
#define OFB_IV_LEN  16

static mbedtls_aes_context aes_context;

static int32_t _ofb_encrypt_wrap(uint8_t *ofb_key, uint8_t *ofb_iv,
                                 uint8_t *input, uint8_t *output, uint32_t len)
{
    int32_t ret;

    uint32_t offset = 0;
    uint8_t  iv[OFB_IV_LEN] = {0};

    if ((input == NULL) || (output == NULL)) {
        return KV_ERR_INVALID_PARAM;
    }

    memcpy(iv, ofb_iv, sizeof(iv));

    ret = mbedtls_aes_setkey_enc(&aes_context, ofb_key, OFB_KEY_LEN * 8);
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
    uint8_t *ofb_key = NULL;
    uint8_t *ofb_iv  = NULL;

    ofb_key = kv_secure_getkey(OFB_KEY_LEN);
    ofb_iv  = kv_secure_getiv(OFB_IV_LEN);

    if ((ofb_key == NULL) || (ofb_iv == NULL)) {
        return KV_ERR_ENCRYPT;
    }

    if (_ofb_encrypt_wrap(ofb_key, ofb_iv, input, output, input_len) != KV_OK) {
        return KV_ERR_ENCRYPT;
    }

    return KV_OK;
}

int32_t kv_secure_decrypt(uint8_t *input, uint8_t *output, uint32_t input_len)
{
    uint8_t *ofb_key = NULL;
    uint8_t *ofb_iv  = NULL;

    ofb_key = kv_secure_get_key(OFB_KEY_LEN);
    ofb_iv  = kv_secure_get_iv(OFB_IV_LEN);

    if ((ofb_key == NULL) || (ofb_iv == NULL)) {
        return KV_ERR_ENCRYPT;
    }

    if (_ofb_encrypt_wrap(ofb_key, ofb_iv, input, output, input_len) != KV_OK) {
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


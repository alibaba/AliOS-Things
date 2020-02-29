/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */

#include "ali_crypto.h"
#include "ls_hal_crypt.h"

static uint32_t randseed = 12345;

static uint32_t _crypt_rand_word(void)
{
    return (randseed = randseed * 1664525 + 1013904223);
}

static void _rand_gen(uint8_t *buf, size_t len)
{
    uint32_t i;
    uint32_t tmp;

    tmp = _crypt_rand_word();
    for (i = 0; i < len; i++) {
        if ((i & 3) == 0) {
            tmp = _crypt_rand_word();
        }
        buf[i] = ((tmp >> ((i & 3) << 3)) & 0xff);
    }
}

ali_crypto_result ali_rand_gen(uint8_t *buf, size_t len)
{
    ls_hal_crypt_result ret = 0;
    if (buf == NULL || len == 0) {
        CRYPTO_DBG_LOG("invalid input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }
    
    ret = ls_hal_get_random(buf, len);
    if (ret != HAL_CRYPT_SUCCESS) {
        CRYPTO_DBG_LOG("hal get random failed(0x%08x)\n", ret);
        return ALI_CRYPTO_ERROR;
    }

    // gen rand
    _rand_gen(buf, len);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_seed(uint8_t *seed, size_t seed_len)
{
    uint32_t i, tmp = 0;

    if (seed == NULL || seed_len == 0) {
        CRYPTO_DBG_LOG("invalid input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    for (i = 0; i < (seed_len - seed_len % 4); i += 4) {
        tmp ^= seed[i];
        tmp ^= seed[i + 1] << 8;
        tmp ^= seed[i + 2] << 16;
        tmp ^= seed[i + 3] << 24;
    }

    while (i < seed_len) {
        tmp ^= seed[i++];
    }

    randseed = tmp;
    	
    return ALI_CRYPTO_SUCCESS;
}

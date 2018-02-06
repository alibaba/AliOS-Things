/**
 * Copyright (C) 2017 The YunOS Project. All rights reserved.
 **/

#include "ali_crypto.h"
#include "mbed_crypto.h"

//static uint32_t next = 1;
static uint32_t randseed = 12345;

uint32_t ali_crypt_rand_word(void)
{
    return (randseed = randseed * 1664525 + 1013904223);
}

ali_crypto_result ali_rand_gen(uint8_t *buf, size_t len)
{
    uint32_t i;
    uint32_t tmp;

    if (buf == NULL || len == 0) {
        MBED_DBG_E("ali_rand_gen: invalid input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    tmp = ali_crypt_rand_word();
    for (i = 0; i < len; i++) {
        if ((i & 3) == 0) {
            tmp = ali_crypt_rand_word();
        }
        buf[i] = ((tmp >> ((i & 3) << 3)) & 0xff);
    }

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_seed(uint8_t *seed, size_t seed_len)
{
    uint32_t i, tmp = 0;

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

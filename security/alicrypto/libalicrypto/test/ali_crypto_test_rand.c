/**
 * Copyright (C) 2017 The YunOS Project. All rights reserved.
 */

#include "ali_crypto_test.h"

int ali_crypto_rand_test(void)
{
    uint32_t i        = 0;
    uint8_t  seed[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    size_t   seed_len = 16;
    uint8_t  tmp_buf[32];
    uint8_t  rand_buf[32];
    size_t   rand_len = 32;
    ali_crypto_result result;

    result = ali_seed(seed, seed_len);
    if (result != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "ali_seed fail(%08x)\n", result);
    }

    /* for gcov coverage */
    result = ali_rand_gen(NULL, rand_len);
    if (result == ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "gen rand fail(%08x)\n", result);
    }

    result = ali_rand_gen(rand_buf, rand_len);
    if (result != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "gen rand fail(%08x)\n", result);
    }

    while (i++ < 10) {
        CRYPT_MEMCPY(tmp_buf, rand_buf, rand_len);

        result = ali_rand_gen(rand_buf, rand_len);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "gen rand fail(%08x)\n", result);
        }

        if (!CRYPT_MEMCMP(tmp_buf, rand_buf, rand_len)) {
            ali_crypto_print_data("tmp_buf", tmp_buf, rand_len);
            ali_crypto_print_data("rand_buf", rand_buf, rand_len);
            PRINT_RET(-1, "RAND test fail!\n");
        }
    }

    CRYPT_INF("RAND test success!\n");

    return 0;
}

/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/

#include "ali_crypto.h"
#include "drv_tee.h"
#include "../../libalicrypto/mbed/inc/mbed_crypto.h"

ali_crypto_result ali_rand_gen(uint8_t *buf, size_t len)
{
    if (buf == NULL || len == 0) {
        MBED_DBG_E("ali_rand_gen: invalid input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    if (csi_tee_rand_generate(buf, len)) {
        MBED_DBG_E("ali_rand_gen: fail!\n");
        return ALI_CRYPTO_ERR_STATE;
    }

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_seed(uint8_t *seed, size_t seed_len)
{
    (void)seed_len;
    csi_tee_rand_seed(*(uint32_t *)seed);

    return ALI_CRYPTO_SUCCESS;
}

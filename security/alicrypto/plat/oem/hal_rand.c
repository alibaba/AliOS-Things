/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/

#include "ali_crypto.h"
#include "impl_crypto.h"
#include "hal_crypto.h"
#include "sal_crypto.h"



ali_crypto_result hal_rand_gen(uint8_t *buf, size_t len)
{
    UNUSED(buf);
    UNUSED(len);
    ALI_CRYPTO_DBG_I("hal_rand_gen\n");

    return ALI_CRYPTO_SUCCESS;
}


ali_crypto_result hal_seed(uint8_t *seed, size_t seed_len)
{
    UNUSED(seed);
    UNUSED(seed_len);
    ALI_CRYPTO_DBG_I("sal_seed\n");

    return ALI_CRYPTO_SUCCESS;
}


/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */

#include "ali_crypto.h"
#include "ls_hal_crypt.h"

ali_crypto_result ali_rand_gen(uint8_t *buf, size_t len)
{
    if (buf == NULL || len == 0) {
        CRYPTO_DBG_LOG("invalid input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }
    
    return ls_hal_get_random(buf, len);
}

ali_crypto_result ali_seed(uint8_t *seed, size_t seed_len)
{
    if (seed == NULL || seed_len == 0) {
        CRYPTO_DBG_LOG("invalid input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }
    	
    return ALI_CRYPTO_SUCCESS;
}

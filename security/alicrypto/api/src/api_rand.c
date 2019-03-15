/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/
#include "ali_crypto.h"
#include "sal_crypto.h"  // use sal as for now, later merge to ls_hal_get_random

ali_crypto_result ali_rand_gen(uint8_t *buf, size_t len)
{
	ali_crypto_result result = 0;

    if (buf == NULL || len == 0) {
        ALI_CRYPTO_DBG_E("invalid input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }
    
    result = sal_rand_gen(buf, len);

    return result;
}

ali_crypto_result ali_seed(uint8_t *seed, size_t seed_len)
{
    ali_crypto_result result = 0;
    
    if (seed == NULL || seed_len == 0) {
        ALI_CRYPTO_DBG_E("invalid input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }
    	
    result = sal_seed(seed, seed_len);
	
    return result;
}

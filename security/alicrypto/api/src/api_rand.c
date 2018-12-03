/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/
#include "ali_crypto.h"
#include "impl_crypto.h"


ali_crypto_result ali_rand_gen(uint8_t *buf, size_t len)
{
    ali_crypto_result result = 0;
    rand_impl_t *p_impl = NULL;

    if (buf == NULL || len == 0) {
        ALI_CRYPTO_DBG_E("invalid input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }
    
    p_impl = ali_crypto_rand_get_impl();
    if(NULL == p_impl) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid impl\n");
    }

    if(NULL == p_impl->ops.rand_gen) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid aes ops\n");
    }
	
    result = p_impl->ops.rand_gen(buf, len);
    return result;
}

ali_crypto_result ali_seed(uint8_t *seed, size_t seed_len)
{
    ali_crypto_result result = 0;
    rand_impl_t *p_impl = NULL;
    
    if (seed == NULL || seed_len == 0) {
        ALI_CRYPTO_DBG_E("invalid input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }
    
    p_impl = ali_crypto_rand_get_impl();
    if(NULL == p_impl) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid impl\n");
    }

    if(NULL == p_impl->ops.seed) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid aes ops\n");
    }
	
    result = p_impl->ops.seed(seed, seed_len);
    return result;
}

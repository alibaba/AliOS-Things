/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include "ls_osa.h"
#include "crypto.h"
#include "irot_pal.h"

void* irot_pal_memory_malloc(int size)
{
    return ls_osa_malloc((size_t)size);
}

void irot_pal_memory_free(void* ptr)
{
    ls_osa_free(ptr);
}

irot_result_t irot_pal_get_random(uint8_t* buf, uint32_t len)
{
    ali_crypto_result result;
    uint32_t seed;

    seed = (uint32_t)ls_osa_get_time_ms();
    result = crypto_seed((uint8_t*)&seed, sizeof(seed));
    if (result != ALI_CRYPTO_SUCCESS) {
        return IROT_ERROR_GENERIC;
    }

#if (ID2_LOCAL_TEST)
    memset(buf, 0xAB, len);
#else
    result = crypto_rand_gen(buf, len);
    if (result != ALI_CRYPTO_SUCCESS) {
        return IROT_ERROR_GENERIC;
    }
#endif

    return IROT_SUCCESS;
}

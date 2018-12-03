/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#ifdef PLATFORM_ALIOS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "config.h"
#include "irot_pal.h"
#include "log/log.h"
#include "ali_crypto.h"
#include "crypto.h"

#include <aos/kernel.h>

void* irot_pal_memory_malloc(int size)
{
    void* ptr;
    ptr = aos_malloc(size);
    return ptr;
}

void irot_pal_memory_free(void* ptr)
{
    aos_free(ptr);
}

irot_result_t irot_pal_get_random(uint8_t* buf, uint32_t len)
{
    ali_crypto_result result;
    static int flag = 0;
    uint32_t seed;

    if (flag == 0)
    {
        seed = (uint32_t)aos_now_ms();
        result = crypto_seed((uint8_t*)&seed, sizeof(seed));
        if (result != ALI_CRYPTO_SUCCESS)
        {
            return IROT_ERROR_GENERIC;
        }
        flag = 1;
    }
    result = crypto_rand_gen(buf, len);
#if (ID2_LOCAL_TEST)
    memset(buf, 0xAB, len);
#endif
    if (result != ALI_CRYPTO_SUCCESS)
    {
        return IROT_ERROR_GENERIC;
    }
    return IROT_SUCCESS;
}

void irot_pal_log(const char* fmt, ...)
{
    va_list args;

    printf(" ID2=> ");
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    //fflush(stdout);
}

#endif

/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "config.h"
#include "aos.h"
#include "irot_pal.h"
#include "log/log.h"

#include "ali_crypto.h"
#include "crypto.h"

void *irot_pal_memory_malloc(int size)
{
    void *ptr;
    ptr = aos_malloc(size);
    return ptr;
}

void irot_pal_memory_free(void *ptr)
{
    aos_free(ptr);
}

irot_result_t irot_pal_base64_encode(const uint8_t *in, uint32_t in_len, uint8_t *out, uint32_t *out_len)
{
    unsigned char *p = NULL;
    int base64_out_len = *out_len;
    id2_log_hex_data("base64 input:", in, in_len);
    p = base64_encode(in, (int)in_len, out, &base64_out_len);
    if (!p) {
        return IROT_ERROR_GENERIC;
    }
    *out_len = base64_out_len;
    id2_log_hex_data("base64 output:", out, *out_len);
    return IROT_SUCCESS;
}

irot_result_t irot_pal_hash_sum(const uint8_t *in, uint32_t in_len, uint8_t *out, uint32_t *out_len, digest_t type)
{
    irot_result_t ret = IROT_SUCCESS;
    size_t ctx_size;
    ali_crypto_result result;
    void *ctx = NULL;

    if (type == DIGEST_TYPE_SHA256) {
        if (*out_len < 32) {
            ret = IROT_ERROR_SHORT_BUFFER;
        } else {
            result = crypto_hash_get_ctx_size(SHA256, &ctx_size);
            if (result != ALI_CRYPTO_SUCCESS) {
                ret = IROT_ERROR_GENERIC;
                goto EXIT;
            }
            ctx = irot_pal_memory_malloc(ctx_size);
            if (ctx == NULL) {
                ret = IROT_ERROR_GENERIC;
                goto EXIT;
            }
            result = crypto_hash_init(SHA256, ctx);
            if (result != ALI_CRYPTO_SUCCESS) {
                ret = IROT_ERROR_GENERIC;
                goto EXIT;
            }
            result = crypto_hash_update(in, in_len, ctx);
            if (result != ALI_CRYPTO_SUCCESS) {
                ret = IROT_ERROR_GENERIC;
                goto EXIT;
            }
            result = crypto_hash_final(out, ctx);
            if (result != ALI_CRYPTO_SUCCESS) {
                ret = IROT_ERROR_GENERIC;
                goto EXIT;
            }
            *out_len = 32;
        }
    } else {
        ret = IROT_ERROR_NOT_SUPPORTED;
    }
EXIT:
    irot_pal_memory_free(ctx);
    return ret;
}


irot_result_t irot_pal_get_random(uint8_t *buf, uint32_t len)
{
    ali_crypto_result result;
    static int flag = 0;
    uint32_t seed;

    if (flag == 0) {
        seed = (uint32_t)aos_now_ms();
        result = crypto_seed((uint8_t *)&seed, sizeof(seed));
        if (result != ALI_CRYPTO_SUCCESS) {
            return IROT_ERROR_GENERIC;
        }
        flag = 1;
    }
    result = crypto_rand_gen(buf, len);
#if (ID2_LOCAL_TEST)
    memset(buf, 0xAB, len);
#endif
    if (result != ALI_CRYPTO_SUCCESS) {
        return IROT_ERROR_GENERIC;
    }
    return IROT_SUCCESS;
}

void irot_pal_log(const char *fmt, ...)
{
    va_list args;

    printf(" ID2=> ");
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    //fflush(stdout);
}

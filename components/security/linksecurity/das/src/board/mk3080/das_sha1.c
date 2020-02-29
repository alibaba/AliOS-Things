/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <das/platform.h>
#include <das/sha1.h>
#include <aos/kernel.h>

#include "ali_crypto.h"

#define DAS_SHA1_ASSERT(_x)                             \
    do {                                                \
        if (!(_x)) {                                    \
            printf("ASSERT FAILURE:\n");                \
            printf("%s (%d): %s\n",                     \
                    __FILE__, __LINE__, __FUNCTION__);  \
            while (1) /* loop */;                       \
        }                                               \
    } while (0)

void das_sha1_start(das_sha1_context_t *ctx)
{
    size_t size;
    ali_crypto_result ret = 0;

    ret = ali_hash_get_ctx_size(SHA1, &size);
    if (ret) {
        DAS_SHA1_ASSERT(0);
    }

    memset(ctx, 0, sizeof(das_sha1_context_t));
    ctx->size = (uint32_t)size;

    ctx->context = aos_malloc(ctx->size);
    if (!ctx->context) {
        printf("malloc %d fail\n", (int)ctx->size);
        DAS_SHA1_ASSERT(0);
    }

    ret = ali_hash_init(SHA1, ctx->context);
    if (ret) {
        printf("sha1 init fail, 0x%x\n", ret);
        DAS_SHA1_ASSERT(0);
    }

    return;
}

void das_sha1_update(das_sha1_context_t *ctx, const unsigned char *input, size_t ilen)
{
    ali_crypto_result ret = 0;

    ret = ali_hash_update(input, ilen, ctx->context);
    if (ret) {
        printf("sha1 update fail, 0x%x\n", ret);
        DAS_SHA1_ASSERT(0);
    }

    return;
}

void das_sha1_finish(das_sha1_context_t *ctx, unsigned char output[20])
{
    ali_crypto_result ret = 0;

    ret = ali_hash_final(output, ctx->context);
    if (ret) {
        DAS_LOG("sha1 finish fail, 0x%x\n", ret);
        DAS_SHA1_ASSERT(0);
    }

    aos_free(ctx->context);
    memset(ctx, 0, sizeof(das_sha1_context_t));

    return;
}

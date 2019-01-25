/*
 * Copyright (C) 2016-2019 Alibaba Group Holding Limited
 */

#include "ls_hal.h"

#ifdef CONFIG_HAL_DEMO
#include "md5.h"
#include "sha1.h"
#include "sha256.h"

typedef struct {
    hal_md5_context context;
} hal_md5_ctx_t;

typedef struct {
    hal_sha1_context context;
} hal_sha1_ctx_t;

typedef struct {
    hal_sha256_context context;
} hal_sha256_ctx_t;
#endif

int ls_hal_md5_get_size(void)
{
#ifdef CONFIG_HAL_DEMO
    int size = sizeof(hal_md5_ctx_t);

    return size;
#else
    HAL_DBG_LOG("not supported!\n");

    return -1;
#endif
}

int ls_hal_md5_init(void *ctx)
{
#ifdef CONFIG_HAL_DEMO
    hal_md5_ctx_t *hal_ctx = (hal_md5_ctx_t *)ctx;

    if (ctx == NULL) {
        return HAL_CRYPT_ERROR_BAD_PARAMETERS;
    }

    hal_md5_init(&hal_ctx->context);
    hal_md5_starts(&hal_ctx->context);

    return HAL_CRYPT_SUCCESS;

#else /* CONFIG_HAL_DEMO */

    (void)ctx;

    HAL_DBG_LOG("not supported!\n");

    return HAL_CRYPT_ERROR_NOT_SUPPORTED;

#endif /* CONFIG_HAL_DEMO */
}

int ls_hal_md5_update(void *ctx, const uint8_t *src, size_t size)
{
#ifdef CONFIG_HAL_DEMO
    hal_md5_ctx_t *hal_ctx = (hal_md5_ctx_t *)ctx;

    if (ctx == NULL || src == NULL) {
        return HAL_CRYPT_ERROR_BAD_PARAMETERS;
    }

    hal_md5_update(&hal_ctx->context, src, size);

    return HAL_CRYPT_SUCCESS;
#else /* CONFIG_HAL_DEMO */

    (void)ctx;
    (void)src;
    (void)size;

    HAL_DBG_LOG("not supported!\n");

    return HAL_CRYPT_ERROR_NOT_SUPPORTED;

#endif /* CONFIG_HAL_DEMO */
}

int ls_hal_md5_finish(void *ctx, uint8_t digest[16])
{
#ifdef CONFIG_HAL_DEMO
    hal_md5_ctx_t *hal_ctx = (hal_md5_ctx_t *)ctx;

    if (ctx == NULL) {
        return HAL_CRYPT_ERROR_BAD_PARAMETERS;
    }

    hal_md5_finish(&hal_ctx->context, digest);

    return HAL_CRYPT_SUCCESS;

#else /* CONFIG_HAL_DEMO */

    (void)ctx;
    (void)digest;

    HAL_DBG_LOG("not supported!\n");

    return HAL_CRYPT_ERROR_NOT_SUPPORTED;

#endif /* CONFIG_HAL_DEMO */
}

int ls_hal_sha1_get_size(void)
{
    HAL_DBG_LOG("not supported!\n");

    return -1;
}

int ls_hal_sha1_init(void *ctx)
{
    (void)ctx;

    HAL_DBG_LOG("not supported!\n");

    return HAL_CRYPT_ERROR_NOT_SUPPORTED;
}

int ls_hal_sha1_update(void *ctx, const uint8_t *src, size_t size)
{
    (void)ctx;
    (void)src;
    (void)size;

    HAL_DBG_LOG("not supported!\n");

    return HAL_CRYPT_ERROR_NOT_SUPPORTED;
}

int ls_hal_sha1_finish(void *ctx, uint8_t digest[20])
{
    (void)ctx;
    (void)digest;

    HAL_DBG_LOG("not supported!\n");

    return HAL_CRYPT_ERROR_NOT_SUPPORTED;
}

int ls_hal_sha256_get_size(void)
{
    HAL_DBG_LOG("not supported!\n");

    return -1;
}

int ls_hal_sha256_init(void *ctx)
{
    (void)ctx;

    HAL_DBG_LOG("not supported!\n");

    return HAL_CRYPT_ERROR_NOT_SUPPORTED;
}

int ls_hal_sha256_update(void *ctx, const uint8_t *src, size_t size)
{
    (void)ctx;
    (void)src;
    (void)size;

    HAL_DBG_LOG("not supported!\n");

    return HAL_CRYPT_ERROR_NOT_SUPPORTED;
}

int ls_hal_sha256_finish(void *ctx, uint8_t digest[32])
{
    (void)ctx;
    (void)digest;

    HAL_DBG_LOG("not supported!\n");

    return HAL_CRYPT_ERROR_NOT_SUPPORTED;
}


/*
 * Copyright (C) 2016-2019 Alibaba Group Holding Limited
 */

#include "ls_hal.h"
#include "md5.h"
#include "sha1.h"
#include "sha256.h"

int ls_hal_md5_get_size(void)
{
    int size = sizeof(impl_md5_ctx_t);
    return size;
}

int ls_hal_md5_init(void *ctx)
{
    impl_md5_ctx_t *impl_ctx;

    if (ctx == NULL) {
        LS_HAL_LOG("invalid ctx\n");
        return HAL_CRYPT_INVALID_CONTEXT;
    }

    impl_ctx = (impl_md5_ctx_t *)ctx;

    impl_md5_init(&impl_ctx->context);
    impl_md5_starts(&impl_ctx->context);

    return HAL_CRYPT_SUCCESS;
}

int ls_hal_md5_update(void *ctx, const uint8_t *src, size_t size)
{
    impl_md5_ctx_t *impl_ctx;

    if (ctx == NULL) {
        LS_HAL_LOG("invalid ctx\n");
        return HAL_CRYPT_INVALID_CONTEXT;
    }

    if (src == NULL && size != 0) {
        LS_HAL_LOG("src is NULL and non-zero size\n");
        return HAL_CRYPT_INVALID_ARG;
    }

    impl_ctx = (impl_md5_ctx_t *)ctx;

    impl_md5_update(&impl_ctx->context, src, size);

    return HAL_CRYPT_SUCCESS;
}

int ls_hal_md5_finish(void *ctx, uint8_t digest[16])
{
    impl_md5_ctx_t *impl_ctx;

    if (ctx == NULL) {
        LS_HAL_LOG("invalid ctx\n");
        return HAL_CRYPT_INVALID_CONTEXT;
    }

    impl_ctx = (impl_md5_ctx_t *)ctx;

    impl_md5_finish(&impl_ctx->context, digest);

    return HAL_CRYPT_SUCCESS;
}

int ls_hal_sha1_get_size(void)
{
    int size = sizeof(impl_sha1_ctx_t);
    return size;
}

int ls_hal_sha1_init(void *ctx)
{
    impl_sha1_ctx_t *impl_ctx;

    if (ctx == NULL) {
        LS_HAL_LOG("invalid ctx\n");
        return HAL_CRYPT_INVALID_CONTEXT;
    }

    impl_ctx = (impl_sha1_ctx_t *)ctx;

    impl_sha1_init(&impl_ctx->context);
    impl_sha1_starts(&impl_ctx->context);

    return HAL_CRYPT_SUCCESS;
}

int ls_hal_sha1_update(void *ctx, const uint8_t *src, size_t size)
{
    impl_sha1_ctx_t *impl_ctx;

    if (ctx == NULL) {
        LS_HAL_LOG("invalid ctx\n");
        return HAL_CRYPT_INVALID_CONTEXT;
    }

    if (src == NULL && size != 0) {
        LS_HAL_LOG("NULL src and non-zero size\n");
        return HAL_CRYPT_INVALID_ARG;
    }

    impl_ctx = (impl_sha1_ctx_t *)ctx;
    impl_sha1_update(&impl_ctx->context, src, size);

    return HAL_CRYPT_SUCCESS;
}

int ls_hal_sha1_finish(void *ctx, uint8_t digest[20])
{
    impl_sha1_ctx_t * impl_ctx;

    if (ctx == NULL) {
        LS_HAL_LOG("invalid ctx\n");
        return HAL_CRYPT_INVALID_CONTEXT;
    }

    impl_ctx = (impl_sha1_ctx_t *)ctx;

    impl_sha1_finish(&impl_ctx->context, digest);

    return HAL_CRYPT_SUCCESS;
}

int ls_hal_sha256_get_size(void)
{
    return sizeof(impl_sha256_ctx_t);
}

int ls_hal_sha256_init(void *ctx)
{
    impl_sha256_ctx_t *impl_ctx;

    if (ctx == NULL) {
        LS_HAL_LOG("invalid ctx\n");
        return HAL_CRYPT_INVALID_CONTEXT;
    }

    impl_ctx = (impl_sha256_ctx_t *)ctx;

    impl_sha256_init(&impl_ctx->context);
    impl_sha256_starts(&impl_ctx->context, 0);

    return HAL_CRYPT_SUCCESS;
}

int ls_hal_sha256_update(void *ctx, const uint8_t *src, size_t size)
{
    impl_sha256_ctx_t *impl_ctx;

    if (ctx == NULL) {
        LS_HAL_LOG("invalid ctx\n");
        return HAL_CRYPT_INVALID_CONTEXT;
    }

    impl_ctx = (impl_sha256_ctx_t *)ctx;

    impl_sha256_update(&impl_ctx->context, src, size);

    return HAL_CRYPT_SUCCESS;
}

int ls_hal_sha256_finish(void *ctx, uint8_t digest[32])
{
    impl_sha256_ctx_t *impl_ctx;

    if (ctx == NULL) {
        LS_HAL_LOG("invalid ctx\n");
        return HAL_CRYPT_INVALID_CONTEXT;
    }

    impl_ctx = (impl_sha256_ctx_t *)ctx;

    impl_sha256_finish(&impl_ctx->context, digest);

    return HAL_CRYPT_SUCCESS;
}

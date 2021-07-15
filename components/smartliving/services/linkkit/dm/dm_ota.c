/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#if defined(OTA_ENABLED) && !defined(BUILD_AOS)
#include "iotx_dm_internal.h"

static dm_ota_ctx_t g_dm_ota_ctx;

static dm_ota_ctx_t *_dm_ota_get_ctx(void)
{
    return &g_dm_ota_ctx;
}

int dm_ota_init(void)
{
    dm_ota_ctx_t *ctx = _dm_ota_get_ctx();
    memset(ctx, 0, sizeof(dm_ota_ctx_t));

    HAL_GetProductKey(ctx->product_key);
    HAL_GetDeviceName(ctx->device_name);

    return SUCCESS_RETURN;
}

int dm_ota_sub(void)
{
    dm_ota_ctx_t *ctx = _dm_ota_get_ctx();
    void *handle = NULL;

    /* Init OTA Handle */
    handle = IOT_OTA_Init(ctx->product_key, ctx->device_name, NULL);
    if (handle == NULL) {
        return FAIL_RETURN;
    }

    ctx->ota_handle = handle;

    return SUCCESS_RETURN;
}

int dm_ota_deinit(void)
{
    dm_ota_ctx_t *ctx = _dm_ota_get_ctx();

    if (ctx->ota_handle) {
        IOT_OTA_Deinit(ctx->ota_handle);
        ctx->ota_handle = NULL;
    }

    return SUCCESS_RETURN;
}

int dm_ota_get_ota_handle(void **handle)
{
    dm_ota_ctx_t *ctx = _dm_ota_get_ctx();

    if (handle == NULL || *handle != NULL) {
        return FAIL_RETURN;
    }

    if (ctx->ota_handle == NULL) {
        return FAIL_RETURN;
    }

    *handle = ctx->ota_handle;

    return SUCCESS_RETURN;
}
#endif
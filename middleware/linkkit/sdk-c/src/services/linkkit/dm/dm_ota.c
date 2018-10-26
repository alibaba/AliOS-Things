/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include "iotx_dm_internal.h"

#ifdef OTA_ENABLED
static dm_ota_ctx_t g_dm_ota_ctx;

static dm_ota_ctx_t *_dm_ota_get_ctx(void)
{
    return &g_dm_ota_ctx;
}
#endif

int dm_ota_init(void)
{
#ifdef OTA_ENABLED
    dm_ota_ctx_t *ctx = _dm_ota_get_ctx();

    memset(ctx, 0, sizeof(dm_ota_ctx_t));

    HAL_GetProductKey(ctx->product_key);
    HAL_GetDeviceName(ctx->device_name);

    /* Init OTA Handle */
    ctx->ota_handle = IOT_OTA_Init(ctx->product_key, ctx->device_name, NULL);
    if (ctx->ota_handle == NULL) {
        return FAIL_RETURN;
    }
#endif
    return SUCCESS_RETURN;
}

int dm_ota_deinit(void)
{
#ifdef OTA_ENABLED
    dm_ota_ctx_t *ctx = _dm_ota_get_ctx();

    if (ctx->ota_handle) {
        IOT_OTA_Deinit(ctx->ota_handle);
        ctx->ota_handle = NULL;
    }
#endif
    return SUCCESS_RETURN;
}

int dm_ota_get_ota_handle(void **handle)
{
#ifdef OTA_ENABLED
    dm_ota_ctx_t *ctx = _dm_ota_get_ctx();

    if (handle == NULL || *handle != NULL) {
        return FAIL_RETURN;
    }

    if (ctx->ota_handle == NULL) {
        return FAIL_RETURN;
    }

    *handle = ctx->ota_handle;
#endif
    return SUCCESS_RETURN;
}
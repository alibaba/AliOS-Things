/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "iotx_dm_internal.h"

#if defined(OTA_ENABLED) && !defined(BUILD_AOS)

static dm_ota_ctx_t g_dm_ota_ctx;

static dm_ota_ctx_t *_dm_ota_get_ctx(void)
{
    return &g_dm_ota_ctx;
}

static int _dm_ota_clean_state(void *context)
{
    dm_cota_ctx_t *cota = dm_cota_get_ctx();
    dm_fota_ctx_t *fota = dm_fota_get_ctx();
    if (cota != NULL) {
        cota->is_report_new_config = 0;
    }

    if (fota != NULL) {
        fota->is_report_new_config = 0;
    }
    return 0;
}

int dm_ota_init(void)
{
    dm_ota_ctx_t *ctx = _dm_ota_get_ctx();
    memset(ctx, 0, sizeof(dm_ota_ctx_t));

    IOT_Ioctl(IOTX_IOCTL_GET_PRODUCT_KEY, ctx->product_key);
    IOT_Ioctl(IOTX_IOCTL_GET_DEVICE_NAME, ctx->device_name);

    return SUCCESS_RETURN;
}

int dm_ota_sub(void)
{
    dm_ota_ctx_t *ctx = _dm_ota_get_ctx();
    void *handle = NULL;

    /* Init OTA Handle */
    handle = IOT_OTA_Init(ctx->product_key, ctx->device_name, NULL);
    if (handle == NULL) {
        return STATE_DEV_MODEL_OTA_INIT_FAILED;
    }
    IOT_OTA_SetOnPushedCallback(handle, _dm_ota_clean_state);
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
#ifdef DEVICE_MODEL_GATEWAY
#ifdef DEVICE_MODEL_SUBDEV_OTA
int dm_ota_switch_device(int devid)
{
    char pk[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char dn[IOTX_DEVICE_NAME_LEN + 1] = {0};
    char ds[IOTX_DEVICE_SECRET_LEN + 1] = {0};
    void *ota_handle = NULL;
    int res = -1;
    dm_ota_ctx_t *ctx = NULL;

    res = dm_mgr_search_device_by_devid(devid, pk, dn, ds);
    if (SUCCESS_RETURN != res) {
        return res;
    }

    res = dm_ota_get_ota_handle(&ota_handle);
    if (res != SUCCESS_RETURN) {
        return res;
    }

    /* if currently a device is doing OTA, do not interrupt */
    if (IOT_OTA_IsFetching(ota_handle)) {
        return STATE_DEV_MODEL_OTA_STILL_IN_PROGRESS;
    }

    dm_ota_deinit();
    ctx = _dm_ota_get_ctx();
    memset(ctx, 0, sizeof(dm_ota_ctx_t));

    memcpy(ctx->product_key, pk, strlen(pk) + 1);
    memcpy(ctx->device_name, dn, strlen(dn) + 1);
    res = dm_ota_sub();
    return res;
}
#endif
#endif

int dm_ota_get_ota_handle(void **handle)
{
    dm_ota_ctx_t *ctx = _dm_ota_get_ctx();

    if (handle == NULL || *handle != NULL) {
        return STATE_DEV_MODEL_INTERNAL_ERROR;
    }

    if (ctx->ota_handle == NULL) {
        return STATE_DEV_MODEL_OTA_NOT_INITED;
    }

    *handle = ctx->ota_handle;
    return SUCCESS_RETURN;
}
#endif

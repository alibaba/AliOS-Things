/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include "iotx_dm_internal.h"
#include "iot_export_ota.h"
#include "dm_ota.h"
#include "dm_conn.h"
#include "dm_cm_wrapper.h"
#include "dm_message.h"

#ifdef OTA_ENABLED
static dm_ota_ctx_t g_dm_ota_ctx;

static dm_ota_ctx_t* _dm_ota_get_ctx(void)
{
    return &g_dm_ota_ctx;
}
#endif

int dm_ota_init(void)
{
#ifdef OTA_ENABLED
    int res = 0;
    dm_ota_ctx_t *ctx = _dm_ota_get_ctx();
    void *cloud_connectivity = NULL;
    void *mqtt_handle = NULL;

    memset(ctx,0,sizeof(dm_ota_ctx_t));

    /* Get Connectivity Initialized By CM */
    cloud_connectivity = dm_conn_get_cloud_conn();
    if (cloud_connectivity == NULL) {
        dm_log_err(DM_UTILS_LOG_CM_CLOUD_CONNECTIVITY_NOT_EXIST);
        return FAIL_RETURN;
    }

    /* Get MQTT Protocol Handle By Connectivity */
    res = dm_cmw_conn_get_prototol_handle(cloud_connectivity,&mqtt_handle);
    if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

    HAL_GetProductKey(ctx->product_key);
    HAL_GetDeviceName(ctx->device_name);

    /* Init OTA Handle */
    ctx->ota_handle = IOT_OTA_Init(ctx->product_key,ctx->device_name,mqtt_handle);
    if (ctx->ota_handle == NULL) {
        dm_log_err(DM_UTILS_LOG_COTA_INIT_FAILED);
        return FAIL_RETURN;
    }
#endif
    return SUCCESS_RETURN;
}

int dm_ota_deinit(void)
{
#ifdef OTA_ENABLED
     dm_ota_ctx_t *ctx = _dm_ota_get_ctx();

     if (ctx->ota_handle) {IOT_OTA_Deinit(ctx->ota_handle);}
#endif
     return SUCCESS_RETURN;
}

int dm_ota_get_ota_handle(void** handle)
{
#ifdef OTA_ENABLED
    dm_ota_ctx_t *ctx = _dm_ota_get_ctx();

    if (handle == NULL || *handle != NULL) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
        return FAIL_RETURN;
    }

    if (ctx->ota_handle == NULL) {return FAIL_RETURN;}

    *handle = ctx->ota_handle;
#endif
    return SUCCESS_RETURN;
}
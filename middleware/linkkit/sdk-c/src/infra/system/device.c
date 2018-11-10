/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#include <string.h>

#include "iotx_system_internal.h"

static iotx_conn_info_t     *iotx_conn_info = NULL;

int iotx_device_info_get(iotx_device_info_t *device_info)
{
    if (device_info == NULL) {
        return -1;
    }
    memset(device_info, 0x0, sizeof(iotx_device_info_t));
    HAL_GetProductKey(device_info->product_key);
    HAL_GetDeviceName(device_info->device_name);
    HAL_GetDeviceSecret(device_info->device_secret);
    HAL_GetDeviceID(device_info->device_id);

    return 0;
}

iotx_conn_info_pt iotx_conn_info_get(void)
{
    if (iotx_conn_info == NULL) {
        iotx_conn_info = LITE_malloc(sizeof(iotx_conn_info_t), MEM_MAGIC, "system");
        memset(iotx_conn_info, 0, sizeof(iotx_conn_info_t));
    }
    return iotx_conn_info;
}

void iotx_conn_info_delete(void)
{
    if (iotx_conn_info == NULL) {
        return;
    }
    LITE_free(iotx_conn_info);
    iotx_conn_info = NULL;
}

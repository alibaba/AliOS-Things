/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#include <string.h>

#include "iotx_system_internal.h"

static iotx_conn_info_t     iotx_conn_info;

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
    memset(&iotx_conn_info, 0, sizeof(iotx_conn_info));
    return &iotx_conn_info;
}

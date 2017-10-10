/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>

#include "lite-log.h"
#include "device.h"

static iotx_conn_info_t     iotx_conn_info;
static iotx_device_info_t   iotx_device_info;
static int                  iotx_devinfo_inited = 0;

int iotx_device_info_init(void)
{
    if (iotx_devinfo_inited) {
        log_debug("device_info already created, return!");
        return 0;
    }

    memset(&iotx_device_info, 0x0, sizeof(iotx_device_info_t));
    memset(&iotx_conn_info, 0x0, sizeof(iotx_conn_info_t));
    iotx_devinfo_inited = 1;

    log_info("device_info created successfully!");
    return SUCCESS_RETURN;
}

int iotx_device_info_set(
            const char *product_key,
            const char *device_name,
            const char *device_secret)
{
    int ret;
    log_debug("start to set device info!");

    memset(&iotx_device_info, 0x0, sizeof(iotx_device_info));
    strncpy(iotx_device_info.product_key, product_key, IOTX_PRODUCT_KEY_LEN);
    strncpy(iotx_device_info.device_name, device_name, IOTX_DEVICE_NAME_LEN);
    strncpy(iotx_device_info.device_secret, device_secret, IOTX_DEVICE_SECRET_LEN);

    //construct device-id(@product_key+@device_name)
    ret = snprintf(iotx_device_info.device_id, IOTX_DEVICE_ID_LEN, "%s.%s", product_key, device_name);
    if ((ret < 0) || (ret >= IOTX_DEVICE_ID_LEN)) {
        log_err("set device info failed");
        return FAIL_RETURN;
    }

    log_debug("device_info set successfully!");
    return SUCCESS_RETURN;
}

iotx_device_info_pt iotx_device_info_get(void)
{
    return &iotx_device_info;
}

iotx_conn_info_pt iotx_conn_info_get(void)
{
    return &iotx_conn_info;
}


/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "amp_platform.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "aos_network.h"
#include "aos/kv.h"
#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"
#include "aiot_devinfo_api.h"
#include "module_aiot.h"

#define MOD_STR "AIOT_ACTINFO"

int32_t amp_app_devinfo_report(void *mqtt_handle)
{
    int32_t res = STATE_SUCCESS;
    void *devinfo_handle = NULL;
    aiot_devinfo_msg_t *devinfo = NULL;
    char *msg = NULL;
    int32_t msg_len = 0;
    char product_key[IOTX_PRODUCT_KEY_LEN] = {0};
    char device_name[IOTX_DEVICE_NAME_LEN] = {0};
    int productkey_len = IOTX_PRODUCT_KEY_LEN;
    int devicename_len = IOTX_DEVICE_NAME_LEN;

    devinfo_handle = aiot_devinfo_init();
    if (devinfo_handle == NULL) {
        amp_debug(MOD_STR, "ntp service init failed");
        return -1;
    }

    res = aiot_devinfo_setopt(devinfo_handle, AIOT_DEVINFOOPT_MQTT_HANDLE, (void *)mqtt_handle);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "devinfo set mqtt handle failed");
        aiot_devinfo_deinit(&devinfo_handle);
        return -1;
    }

    aos_kv_get(AMP_CUSTOMER_PRODUCTKEY, product_key, &productkey_len);
    aos_kv_get(AMP_CUSTOMER_DEVICENAME, device_name, &devicename_len);

    msg_len = strlen(DEVICE_INFO_UPDATE_FMT) + 32;
    msg = (char *)aos_malloc(msg_len);
    if (msg == NULL) {
        amp_debug(MOD_STR, "malloc msg err");
        return -1;
    }
    memset(msg, 0, msg_len);

    /* devinfo update message */
    res = snprintf(msg,
                       msg_len,
                       DEVICE_INFO_UPDATE_FMT,
                       APPLICATION,
                       MODULE_NAME
                      );
    if (res <= 0) {
        amp_debug(MOD_STR, "topic msg generate err");
        aos_free(msg);
        return -1;
    }

    devinfo = aos_malloc(sizeof(aiot_devinfo_msg_t));
    if (devinfo == NULL) {
        amp_debug(MOD_STR, "device update info malloc failed");
        aos_free(msg);
        return -1;
    }
    memset(devinfo, 0, sizeof(aiot_devinfo_msg_t));

    devinfo->product_key = aos_malloc(IOTX_PRODUCT_KEY_LEN);
    if (devinfo->product_key == NULL) {
        amp_debug(MOD_STR, "device update info malloc failed");
        aos_free(msg);
        aos_free(devinfo);
        return -1;
    }
    memset(devinfo->product_key, 0, IOTX_PRODUCT_KEY_LEN);

    devinfo->device_name = aos_malloc(IOTX_DEVICE_NAME_LEN);
    if (devinfo->device_name == NULL) {
        amp_debug(MOD_STR, "device update info malloc failed");
        aos_free(msg);
        aos_free(devinfo);
        return -1;
    }
    memset(devinfo->device_name, 0, IOTX_DEVICE_NAME_LEN);

    devinfo->data.update.params = aos_malloc(msg_len);
    if (devinfo == NULL) {
        amp_debug(MOD_STR, "device update info malloc failed");
        aos_free(msg);
        aos_free(devinfo);
        return -1;
    }
    memset(devinfo->data.update.params, 0, msg_len);

    devinfo->type = AIOT_DEVINFO_MSG_UPDATE;
    memcpy(devinfo->product_key, product_key, strlen(product_key));
    memcpy(devinfo->device_name, device_name, strlen(device_name));
    memcpy(devinfo->data.update.params, msg, msg_len);

    res = aiot_devinfo_send(devinfo_handle, devinfo);
    if (res < STATE_SUCCESS) {
        amp_debug(MOD_STR, "das stepping failed");
        aos_free(msg);
        aos_free(devinfo->product_key);
        aos_free(devinfo->device_name);
        aos_free(devinfo->data.update.params);
        aos_free(devinfo);
        aiot_devinfo_deinit(&devinfo_handle);
        return -1;
    }

    aos_free(msg);
    aos_free(devinfo->product_key);
    aos_free(devinfo->device_name);
    aos_free(devinfo->data.update.params);
    aos_free(devinfo);

    return res;
}
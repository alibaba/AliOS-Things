/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#define CONFIG_LOGMACRO_DETAILS

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board_info.h"
#include "board_mgr.h"
#ifdef AMP_KV_ENABLE
#include "aos/kv.h"
#endif

int8_t board_setDeviceInfo(char *deviceKey, char *deviceName,
                           char *deviceSecret)
{
#ifdef AMP_KV_ENABLE    
    if (NULL != deviceKey) {
        aos_kv_set(DEVICE_KEY_TAG, (void *)deviceKey, strlen(deviceKey),
                          1);
    }

    if (NULL != deviceName) {
        aos_kv_set(DEVICE_NAME_TAG, (void *)deviceName,
                          strlen(deviceName), 1);
    }

    if (NULL != deviceSecret) {
        aos_kv_set(DEVICE_SECRET_TAG, (void *)deviceSecret,
                          strlen(deviceSecret), 1);
    }
#endif
    return (0);
}

int8_t board_getDeviceInfo(char **productKey, char **deviceName,
                           char **deviceSecret)
{
#ifdef AMP_KV_ENABLE
    char tmp[64] = {0x00};
    uint32_t len      = 0;
    int8_t ret   = -1;
    if (NULL != productKey) {
        len = 64;
        ret = aos_kv_get(DEVICE_KEY_TAG, tmp, &len);
        if (0 == ret) {
            tmp[len]    = 0x00;
            *productKey = strdup(tmp);
        } else {
            *productKey = NULL;
        }
    }

    if (NULL != deviceName) {
        len = 64;
        ret = aos_kv_get(DEVICE_NAME_TAG, tmp, &len);
        if (0 == ret) {
            tmp[len]    = 0x00;
            *deviceName = strdup(tmp);
        } else {
            *deviceName = NULL;
        }
    }
    if (NULL != deviceSecret) {
        len = 64;
        ret = aos_kv_get(DEVICE_SECRET_TAG, tmp, &len);
        if (0 == ret) {
            tmp[len]      = 0x00;
            *deviceSecret = strdup(tmp);
        } else {
            *deviceSecret = NULL;
        }
    }
#endif    
    return (0);
}

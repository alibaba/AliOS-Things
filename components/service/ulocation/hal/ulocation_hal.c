/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "ulocation_hal.h"

int ulocation_hal_init()
{
    int ret = -1;
    /* init GPS sensor */
    ret = ulocation_hal_gps_init();
    if (ret != 0) {
        LOGE("uLocation", "ulocation hal gps init failed!");
        return ret;
    }
    /* gprs and wifi is communication model, can not init here*/
    return ret;
}

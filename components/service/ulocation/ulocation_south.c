/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "ulocation/ulocation.h"
#include "ulocation_hal.h"

#ifdef ULOCATION_GPS

int ulocation_update_gpsinfo(location_t *gps) /* GPS */
{
    int ret = -1;
    if (gps == NULL) {
        LOGE("uLocation", "ulocation hal gps info is NULL!");
        return ret;
    }
    ret = ulocation_hal_get_gps(gps);
    if (ret != 0) {
        LOGE("uLocation", "ulocation hal get gps info failed!");
        return ret;
    }
    return ret;
}

#endif

#ifdef ULOCATION_WIFI

int ulocation_update_wifiinfo(char *mmac, char *macs) /* wifi */
{
    int ret = -1;

    /* get wifi ssid or (gps & cell info) */
    if (mmac == NULL || macs == NULL) {
        LOGE("uLocation", "ulocation hal wifi info is NULL!");
        return ret;
    }
    ret = ulocation_hal_get_wifi(mmac, macs);
    if (ret != 0) {
        LOGE("uLocation", "ulocation hal get wifi info failed!");
        return ret;
    }

    return ret;
}

#endif

#ifdef ULOCATION_GPRS

int ulocation_update_gprsinfo(char *cdma, char *bts, char *nearbts) /* gprs */
{
    int ret = -1;

    if (cdma == NULL || bts == NULL || nearbts == NULL) {
        LOGE("uLocation", "ulocation hal gprs info is NULL!");
        return ret;
    }
    ret = ulocation_hal_get_gprs(cdma, bts, nearbts);
    if (ret != 0) {
        LOGE("uLocation", "ulocation hal get gprs info failed!");
        return ret;
    }
    return ret;
}

#endif


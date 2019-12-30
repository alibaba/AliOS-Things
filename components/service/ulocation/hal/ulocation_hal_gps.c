/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "ulocation/ulocation.h"
#include "ulocation_hal.h"

int ulocation_hal_get_gps(location_t *gps)
{
    int   ret = -1;
    float tmp;
    gps_data_t gps_data;
    location_t location;

    location.outdoor.latitude = NORTHARCTIC_LAT; /* north pole */

    /* Read the gps sensor data */
    ret = sensor_hal_read(TAG_DEV_GPS, 0, &gps_data, sizeof(gps_data));
    if(ret <= 0) {
        LOGE("uLocation", "gps sensor device read failed!");
        return -1;
    }

    location.outdoor.longitude = gps_data.lon;
    location.outdoor.latitude = gps_data.lat;
    location.outdoor.altitude = gps_data.elv;
    location.outdoor.coordinatesys = ULOCATION_COORDINATESYS_GCJ02;

    tmp = (location.outdoor.latitude - NORTHARCTIC_LAT);

    if (tmp * tmp < LATDIFF_ARCTIC) { /* are you inside north arctic ? */
        ret = -1;
    } else {
        *gps = location;
        ret = 0;
    }

    return ret;
}

int ulocation_hal_gps_init()
{
    int ret = -1;

    /* Sensor Hal start */
    ret = sensor_hal_init();
    if (ret != 0) {
        LOGE("uLocation", "sensor hal init failed!");
        return ret;
    }

    /* Open the gps sensor device */
    ret = sensor_hal_open(TAG_DEV_GPS, 0);
    if (ret != 0) {
        LOGE("uLocation", "gps sensor open failed!");
        return ret;
    }
    LOGD("uLocation", "gps sensor device open success!");

    return ret;
}


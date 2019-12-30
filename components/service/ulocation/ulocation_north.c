/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <cJSON.h>
#include "hal/wifi.h"
#include "ulocation/ulocation.h"
#include "ulocation_hal.h"

void parse_positioning_info(char *msg_from_cloud)
{
    char *tmp;

    location_t location;

    cJSON *root         = NULL;
    cJSON *geo_location = NULL;
    cJSON *latitude     = NULL;
    cJSON *altitude     = NULL;
    cJSON *longitude    = NULL;

    if (msg_from_cloud == NULL) {
        return;
    }

    /* Parse Root */
    root = cJSON_Parse(msg_from_cloud);

    if (root == NULL || !cJSON_IsObject(root)) {
        LOGE("uLocation","JSON Parse Error");
        return;
    }

    tmp = cJSON_Print(root);
    LOGD("uLocation", "parse_positioning_info: root = %s", tmp);
    HAL_Free(tmp);

    geo_location = cJSON_GetObjectItem(root, "GeoLocation");

    if (geo_location == NULL || !cJSON_IsObject(geo_location)) {
        cJSON_Delete(root);
        LOGE("uLocation", "JSON Parse Error");
        return;
    }

    tmp = cJSON_Print(geo_location);
    LOGD("uLocation", "parse_positioning_info: geo_location = %s", tmp);
    HAL_Free(tmp);

    latitude = cJSON_GetObjectItem(geo_location, "latitude");

    if (latitude == NULL || !cJSON_IsNumber(latitude)) {
        cJSON_Delete(root);
        LOGE("uLocation", "JSON Parse Error");
        return;
    }

    location.outdoor.latitude = (float)latitude->valuedouble;

    tmp = cJSON_Print(latitude);
    LOGD("uLocation", "parse_positioning_info: latitude = %s\n", tmp);
    HAL_Free(tmp);

    altitude = cJSON_GetObjectItem(geo_location, "altitude");

    if (altitude == NULL || !cJSON_IsNumber(altitude)){
        cJSON_Delete(root);
        LOGE("uLocation", "JSON Parse Error");
        return;
    }

    tmp = cJSON_Print(altitude);
    LOGD("uLocation", "parse_positioning_info: altitude = %s\n", tmp);
    HAL_Free(tmp);

    longitude = cJSON_GetObjectItem(geo_location, "longitude");

    if (longitude == NULL || !cJSON_IsNumber(longitude)){
        cJSON_Delete(root);
        LOGE("uLocation", "JSON Parse Error");
        return;
    }

    tmp = cJSON_Print(longitude);
    LOGD("uLocation", "parse_positioning_info: longitude = %s\n", tmp);
    HAL_Free(tmp);

    location.outdoor.longitude = (float)longitude->valuedouble;

    LOGD("uLocation", "parse_positioning_info: latitude: %f %f longitude: %f\n",
                    location.outdoor.latitude, location.outdoor.latitude, location.outdoor.longitude);

    set_location(&location);

    cJSON_Delete(root);
}

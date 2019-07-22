/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <cJSON.h>

#include "hal/wifi.h"
#include "ulocation/ulocation.h"
#include "ulocation_common.h"

#define ULOCATION_TRACE(...)                                    \
    do {                                                        \
        HAL_Printf("\033[1;32;40m%s.%d: ", __func__, __LINE__); \
        HAL_Printf(__VA_ARGS__);                                \
        HAL_Printf("\033[0m\r\n");                              \
    } while (0)

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
        ULOCATION_TRACE("JSON Parse Error");
        return;
    }

    tmp = cJSON_Print(root);
    ULOCATION_TRACE("parse_positioning_info: root = %s\n", tmp);
    HAL_Free(tmp);

    geo_location = cJSON_GetObjectItem(root, "GeoLocation");

    if (geo_location == NULL || !cJSON_IsObject(geo_location)) {
        cJSON_Delete(root);
        ULOCATION_TRACE("JSON Parse Error");
        return;
    }

    tmp = cJSON_Print(geo_location);
    ULOCATION_TRACE("parse_positioning_info: geo_location = %s\n", tmp);
    HAL_Free(tmp);

    latitude = cJSON_GetObjectItem(geo_location, "latitude");

    if (latitude == NULL || !cJSON_IsNumber(latitude)) {
        cJSON_Delete(root);
        ULOCATION_TRACE("JSON Parse Error");
        return;
    }

    location.outdoor.latitude = (float)latitude->valuedouble;

    tmp = cJSON_Print(latitude);
    ULOCATION_TRACE("parse_positioning_info: latitude = %s\n", tmp);
    HAL_Free(tmp);

    altitude = cJSON_GetObjectItem(geo_location, "altitude");

    if (altitude == NULL || !cJSON_IsNumber(altitude)){
        cJSON_Delete(root);
        ULOCATION_TRACE("JSON Parse Error");
        return;
    }

    tmp = cJSON_Print(altitude);
    ULOCATION_TRACE("parse_positioning_info: altitude = %s\n", tmp);
    HAL_Free(tmp);

    longitude = cJSON_GetObjectItem(geo_location, "longitude");

    if (longitude == NULL || !cJSON_IsNumber(longitude)){
        cJSON_Delete(root);

        ULOCATION_TRACE("JSON Parse Error");
        return;
    }

    tmp = cJSON_Print(longitude);
    ULOCATION_TRACE("parse_positioning_info: longitude = %s\n", tmp);
    HAL_Free(tmp);

    location.outdoor.longitude = (float)longitude->valuedouble;

    ULOCATION_TRACE("parse_positioning_info: latitude: %f %f longitude: %f\n",
                    location.outdoor.latitude, location.outdoor.latitude, location.outdoor.longitude);

    set_location(&location);

    cJSON_Delete(root);
}


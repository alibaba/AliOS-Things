/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef APP_MESSAGE_H
#define APP_MESSAGE_H

#include <stdint.h>

/* device active info report */
#define APPLICATION "soundbox"                  // product of application
#define MODULE_NAME aos_get_platform_type()     // module type

/* device info report format */
#define DEVICE_INFO_UPDATE_FMT \
        "[" \
            "{\"attrKey\":\"SYS_SDK_LANGUAGE\",\"attrValue\":\"C\",\"domain\":\"SYSTEM\"}" \
            "{\"attrKey\":\"SYS_LP_SDK_VERSION\",\"attrValue\":\"aos-r-3.0.0\",\"domain\":\"SYSTEM\"}" \
            "{\"attrKey\":\"SYS_PARTNER_ID\",\"attrValue\":\"AliOS Things Team\",\"domain\":\"SYSTEM\"}" \
            "{\"attrKey\":\"SYS_MODULE_ID\",\"attrValue\":\"haas-amp-%s@%s\",\"domain\":\"SYSTEM\"}" \
        "]"

/* device location info format */
#define LBS_INFO_FMT "{\"params\":\"{\"cdma\":\"0\",\"bts\":\"%s\",\"nearbts\":\"%s\"}\"}"

int32_t pyamp_internal_service_subscribe(void *mqtt_handle);

int32_t pyamp_internal_service_publish(void *mqtt_handle);

int32_t amp_das_service(void *mqtt_handle);

int32_t pyamp_ntp_service(void *mqtt_handle);

int32_t pyamp_location_service(void *mqtt_handle);

int32_t pyamp_devinfo_report_service(void *mqtt_handle);

#endif /* APP_MESSAGE_H */

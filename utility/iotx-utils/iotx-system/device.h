/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _IOTX_DEVICE_H_
#define _IOTX_DEVICE_H_
#if defined(__cplusplus)
extern "C" {
#endif

#include "iot_import.h"
#include "iot_export.h"

#define MQTT_SDK_VERSION    "2.0"

int     iotx_device_info_init(void);

int     iotx_device_info_set(
            const char *product_key,
            const char *device_name,
            const char *device_secret);

iotx_device_info_pt iotx_device_info_get(void);
iotx_conn_info_pt iotx_conn_info_get(void);

#if defined(__cplusplus)
}
#endif
#endif  /* #ifndef _IOTX_DEVICE_H_ */

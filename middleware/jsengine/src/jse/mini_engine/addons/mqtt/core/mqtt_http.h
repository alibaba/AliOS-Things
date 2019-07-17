/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_MQTT_HTTP_H
#define BE_MQTT_HTTP_H

#include <be_jse_module.h>
#include <be_port_osal.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
    char productKey[16];
    char deviceName[32];
    char productSecret[48];
    char deviceSecret[48];
    be_jse_symbol_t *func;
} IOT_DEVICESECRET_s;

void *mqtt_http_get_instance();

void mqtt_http_start(void *iotDeviceSecret);

#if defined(__cplusplus)
}
#endif

#endif /* BE_MQTT_HTTP_H */

/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef __IOTX_MQTT_INTERNAL_H__
#define __IOTX_MQTT_INTERNAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iot_import.h"
#include "iot_export_mqtt.h"
#include "iotx_mqtt.h"

#define mqtt_emerg(...)             log_emerg("MQTT", __VA_ARGS__)
#define mqtt_crit(...)              log_crit("MQTT", __VA_ARGS__)
#define mqtt_err(...)               log_err("MQTT", __VA_ARGS__)
#define mqtt_warning(...)           log_warning("MQTT", __VA_ARGS__)
#define mqtt_info(...)              log_info("MQTT", __VA_ARGS__)
#define mqtt_debug(...)             log_debug("MQTT", __VA_ARGS__)


#define mqtt_malloc(size)            LITE_malloc(size, MEM_MAGIC, "mqtt")
#define mqtt_free                    LITE_free

#define MQTT_DYNBUF_SEND_MARGIN                      (64)

#define MQTT_DYNBUF_RECV_MARGIN                      (8)

#define MQTT_CONNECT_REQUIRED_BUFLEN                 (256)

/* MQTT send publish packet */

#endif  /* __IOTX_MQTT_INTERNAL_H__ */

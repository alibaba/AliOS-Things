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

#define mqtt_malloc(...)            LITE_malloc(__VA_ARGS__, MEM_MAGIC, "mqtt")
#define mqtt_free                   LITE_free

#define MQTT_DYNBUF_MARGIN                      (64)
#define MQTT_CONNECT_REQUIRED_BUFLEN            (256)

#if WITH_MQTT_DYNBUF
#define RESET_SERIALIZE_BUF(cli, b, s)          do { \
        LITE_free(cli->b); \
        cli->b = NULL; \
        cli->s = 0; \
        mqtt_debug("FREED: curr buf = %p, curr buf_size = %d", cli->b, cli->s); \
    } while (0)

#define ALLOC_SERIALIZE_BUF(cli, b, s, l, n)    do { \
        int     tmpbuf_len = 0; \
        \
        tmpbuf_len = l + MQTT_DYNBUF_MARGIN; \
        if (cli->b) { \
            mqtt_warning("NOT USING pre-malloced buf %p, malloc per packet", cli->b); \
        } \
        cli->b = LITE_malloc(tmpbuf_len, MEM_MAGIC, "mqtt"); \
        if (NULL == cli->b) { \
            mqtt_err("Unable to allocate %d bytes for '%s', abort!", tmpbuf_len, (n)?(n):""); \
            break;  \
        } \
        cli->s = tmpbuf_len; \
        mqtt_debug("ALLOC: curr buf = %p, curr buf_size = %d, required payload_len = %d", cli->b, cli->s, l); \
    } while (0)

#else
#define ALLOC_SERIALIZE_BUF(cli, b, s, l, n)
#define RESET_SERIALIZE_BUF(cli, b, s)
#endif

/* MQTT send publish packet */

#endif  /* __IOTX_MQTT_INTERNAL_H__ */

/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UDEV_MQTT_CLIENT_H
#define UDEV_MQTT_CLIENT_H

#include "mqtt_internal.h"

#define MQTT_RBUF_SIZE          512
#define MQTT_WBUF_SIZE          512
#define MAX_MESSAGE_HANDLERS    5

typedef MQTTString mc_string_t;

typedef enum {
    QOS0,
    QOS1,
    QOS2,
    SUBFAIL=0x80
}mc_qos_e;

typedef struct {
    mc_qos_e qos;
    unsigned char retained;
    unsigned char dup;
    unsigned short id;
    void *payload;
    size_t payloadlen;
} mc_message_t;

typedef void (*mc_handler_cb)(mc_string_t* topic_name, mc_message_t* message);

typedef struct {
    const char* topic_filter;
    mc_handler_cb handler;
} mc_message_handler_t;


int udev_mqtt_publish(int fd, const char* topic_name, mc_message_t* message);
int udev_mqtt_subscript(int fd, const char* topic_filter, mc_qos_e qos, mc_handler_cb handler);
int udev_mqtt_connect(const char *host, unsigned short port, const char *client_id, const char *token, unsigned int request_timeout);

#endif /* UDEV_MQTT_CLIENT_H */

/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef _UDEV_MQTT_CLIENT_H_
#define _UDEV_MQTT_CLIENT_H_

#include "iotx_mqtt.h"

#define MQTT_RBUF_SIZE   512
#define MQTT_WBUF_SIZE   512
#define MAX_MESSAGE_HANDLERS 5

/**
 * @brief The structure of network connection(TCP or SSL).
 *   The user has to allocate memory for this structure.
 */

enum QoS { QOS0, QOS1, QOS2, SUBFAIL=0x80 };
typedef struct MQTTMessage
{
    enum QoS qos;
    unsigned char retained;
    unsigned char dup;
    unsigned short id;
    void *payload;
    size_t payloadlen;
} MQTTMessage;

typedef void (*handler_fun)(MQTTString* topicName, MQTTMessage* message);

typedef struct
{
    const char* topicFilter;
    handler_fun handler;
} MessageHandlers;


int udev_mqtt_publish(int fd, const char* topicName, MQTTMessage* message);

int udev_mqtt_subscript(int fd, const char* topicFilter, enum QoS qos, handler_fun handler);

int udev_mqtt_connect(const char *host, unsigned short port, const char *client_id, unsigned int request_timeout);

#endif /* _udev_MQTT_CLIENT_H_ */

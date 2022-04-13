/** @defgroup ota_hal_transport_api
 * @{
 *
 * This is an include file of OTA transport interface.
 *
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef OTA_HAL_TRANSPORT_H
#define OTA_HAL_TRANSPORT_H

#ifdef __cplusplus
extern "C" {
#endif

/*MQTT*/
typedef struct {
    unsigned short  packet_id;
    unsigned char   qos;
    unsigned char   dup;
    unsigned char   retain;
    unsigned short  topic_len;
    unsigned int    payload_len;
    const char     *ptopic;
    const char     *payload;
} ota_mqtt_topic_t;

typedef enum {
    OTA_MQTT_EVENT_SUB_SUCCESS  = 3,
    OTA_MQTT_EVENT_PUB_RECEIVED = 12,
    OTA_MQTT_EVENT_BUF_OVERFLOW = 13,
} ota_mqtt_event_t;

typedef struct {
    ota_mqtt_event_t  event;
    ota_mqtt_topic_t *topic;
} ota_mqtt_msg_t;

int ota_hal_mqtt_subscribe(void *mqtt_client, char *topic, void* cb, void *ctx);
int ota_hal_mqtt_publish(void *mqtt_client, char *topic, int qos, void *data, int len);
/**
 * @}
 */
#ifdef __cplusplus
}
#endif
#endif /*OTA_HAL_TRANSPORT_H*/


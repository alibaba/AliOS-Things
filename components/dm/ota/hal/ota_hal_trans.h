/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef OTA_HAL_TRANSPORT_H
#define OTA_HAL_TRANSPORT_H

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

int ota_hal_socket(void);
int ota_hal_close(int fd);
int ota_hal_mqtt_init(void);
int ota_hal_mqtt_deinit(void);
int ota_hal_mqtt_subscribe(char *topic, void* cb, void *ctx);
int ota_hal_mqtt_publish(char *topic, int qos, void *data, int len);

/*COAP*/
typedef enum {
    COAP_CONTENT_TYPE_JSON,
    COAP_CONTENT_TYPE_CBOR,
} coap_content_type_e;

typedef enum {
    COAP_MESSAGE_CON = 0, /* confirmable message */
    COAP_MESSAGE_NON = 1, /* non-confirmable message */
} coap_msg_type_e;

typedef void (*coap_cb_t)(void *p_arg, void *p_message);

typedef struct {
    unsigned char       *p_payload;
    unsigned short       payload_len;
    coap_content_type_e  content_type;
    coap_msg_type_e      msg_type;
    void                *user_data;
    coap_cb_t            resp_callback;
} ota_coap_message_t;

int ota_coap_init(void);
int ota_coap_deinit(void);
int ota_ble_disconnect(void);
int ota_coap_send(char *topic, void *message);
int ota_coap_get_code(void *message, void *code);
int ota_coap_get_payload(void *message, const char **payload, int *len);
int ota_ble_transport(unsigned char cmd, unsigned char* buf, unsigned int len);
int ota_coap_parse_block(void *message, int type, int *num,int *more, int *size);
int ota_coap_send_block(char *topic, void *message, int block_type, int num, int more, int size);
#endif /*OTA_HAL_TRANSPORT_H*/


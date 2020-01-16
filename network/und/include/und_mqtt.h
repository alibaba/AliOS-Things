/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UND_MQTT_H
#define UND_MQTT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    /* Undefined event */
    UND_MQTT_EVT_UNDEF = 0,
    /* MQTT disconnect event */
    UND_MQTT_EVT_DISCONNECT = 1,
    /* MQTT reconnect event */
    UND_MQTT_EVT_RECONNECT = 2,
    /* A ACK to the specific subscribe which specify by packet-id be received */
    UND_MQTT_EVT_SUBCRIBE_SUCCESS = 3,
    /* No ACK to the specific subscribe which specify by packet-id be received in timeout period */
    UND_MQTT_EVT_SUBCRIBE_TIMEOUT = 4,
    /* A failed ACK to the specific subscribe which specify by packet-id be received*/
    UND_MQTT_EVT_SUBCRIBE_NACK = 5,
    /* A ACK to the specific unsubscribe which specify by packet-id be received */
    UND_MQTT_EVT_UNSUBCRIBE_SUCCESS = 6,
    /* No ACK to the specific unsubscribe which specify by packet-id be received in timeout period */
    UND_MQTT_EVT_UNSUBCRIBE_TIMEOUT = 7,
    /* A failed ACK to the specific unsubscribe which specify by packet-id be received*/
    UND_MQTT_EVT_UNSUBCRIBE_NACK = 8,
    /* A ACK to the specific publish which specify by packet-id be received */
    UND_MQTT_EVT_PUBLISH_SUCCESS = 9,
    /* No ACK to the specific publish which specify by packet-id be received in timeout period */
    UND_MQTT_EVT_PUBLISH_TIMEOUT = 10,
    /* A failed ACK to the specific publish which specify by packet-id be received*/
    UND_MQTT_EVT_PUBLISH_NACK = 11,
    /* MQTT packet published from MQTT remote broker be received */
    UND_MQTT_EVT_PUBLISH_RECEIVED = 12,
    /* MQTT packet buffer overflow which the remaining space less than to receive byte */
    UND_MQTT_EVT_BUFFER_OVERFLOW = 13,
} und_mqtt_event_type_t;

typedef struct {
    /* Specify the event type */
    und_mqtt_event_type_t  event_type;

    /*
     * Specify the detail event information. @msg means different to different event types:
     *
     * 1) UND_MQTT_EVT_UNKNOWN,
     *    UND_MQTT_EVT_DISCONNECT,
     *    UND_MQTT_EVT_RECONNECT :
     *      Its data type is string and the value is detail information.
     *
     * 2) UND_MQTT_EVT_SUBCRIBE_SUCCESS,
     *    UND_MQTT_EVT_SUBCRIBE_TIMEOUT,
     *    UND_MQTT_EVT_SUBCRIBE_NACK,
     *    UND_MQTT_EVT_UNSUBCRIBE_SUCCESS,
     *    UND_MQTT_EVT_UNSUBCRIBE_TIMEOUT,
     *    UND_MQTT_EVT_UNSUBCRIBE_NACK
     *    UND_MQTT_EVT_PUBLISH_SUCCESS,
     *    UND_MQTT_EVT_PUBLISH_TIMEOUT,
     *    UND_MQTT_EVT_PUBLISH_NACK :
     *      Its data type is @uint32_t and the value is MQTT packet identifier.
     *
     * 3) UND_MQTT_EVT_PUBLISH_RECEIVED:
     *      Its data type is @und_mqtt_topic_info_pt and see detail at the declare of this type.
     *
     * */
    void *msg;
} und_mqtt_event_msg_t;

#ifdef __cplusplus
}
#endif

#endif  /* end of UND_MQTT_H */

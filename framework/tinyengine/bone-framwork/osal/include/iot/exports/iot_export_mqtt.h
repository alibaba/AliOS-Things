/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _IOT_EXPORT_MQTT_H_
#define _IOT_EXPORT_MQTT_H_

#include <stdint.h>

/* From mqtt_client.h */
typedef enum {
    IOTX_MQTT_QOS0 = 0,
    IOTX_MQTT_QOS1,
    IOTX_MQTT_QOS2
} iotx_mqtt_qos_t;

typedef enum {

    /* Undefined event */
    IOTX_MQTT_EVENT_UNDEF = 0,

    /* MQTT disconnect event */
    IOTX_MQTT_EVENT_DISCONNECT = 1,

    /* MQTT reconnect event */
    IOTX_MQTT_EVENT_RECONNECT = 2,

    /* A ACK to the specific subscribe which specify by packet-id be received */
    IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS = 3,

    /* No ACK to the specific subscribe which specify by packet-id be received in timeout period */
    IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT = 4,

    /* A failed ACK to the specific subscribe which specify by packet-id be received*/
    IOTX_MQTT_EVENT_SUBCRIBE_NACK = 5,

    /* A ACK to the specific unsubscribe which specify by packet-id be received */
    IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS = 6,

    /* No ACK to the specific unsubscribe which specify by packet-id be received in timeout period */
    IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT = 7,

    /* A failed ACK to the specific unsubscribe which specify by packet-id be received*/
    IOTX_MQTT_EVENT_UNSUBCRIBE_NACK = 8,

    /* A ACK to the specific publish which specify by packet-id be received */
    IOTX_MQTT_EVENT_PUBLISH_SUCCESS = 9,

    /* No ACK to the specific publish which specify by packet-id be received in timeout period */
    IOTX_MQTT_EVENT_PUBLISH_TIMEOUT = 10,

    /* A failed ACK to the specific publish which specify by packet-id be received*/
    IOTX_MQTT_EVENT_PUBLISH_NACK = 11,

    /* MQTT packet published from MQTT remote broker be received */
    IOTX_MQTT_EVENT_PUBLISH_RECVEIVED = 12,

} iotx_mqtt_event_type_t;

/* topic information */
typedef struct {
    uint16_t        packet_id;
    uint8_t         qos;
    uint8_t         dup;
    uint8_t         retain;
    uint16_t        topic_len;
    uint16_t        payload_len;
    const char     *ptopic;
    const char     *payload;
} iotx_mqtt_topic_info_t, *iotx_mqtt_topic_info_pt;

typedef struct {

    /* Specify the event type */
    iotx_mqtt_event_type_t  event_type;

    /*
     * Specify the detail event information. @msg means different to different event types:
     *
     * 1) IOTX_MQTT_EVENT_UNKNOWN,
     *    IOTX_MQTT_EVENT_DISCONNECT,
     *    IOTX_MQTT_EVENT_RECONNECT :
     *      Its data type is string and the value is detail information.
     *
     * 2) IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS,
     *    IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT,
     *    IOTX_MQTT_EVENT_SUBCRIBE_NACK,
     *    IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS,
     *    IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT,
     *    IOTX_MQTT_EVENT_UNSUBCRIBE_NACK
     *    IOTX_MQTT_EVENT_PUBLISH_SUCCESS,
     *    IOTX_MQTT_EVENT_PUBLISH_TIMEOUT,
     *    IOTX_MQTT_EVENT_PUBLISH_NACK :
     *      Its data type is @uint32_t and the value is MQTT packet identifier.
     *
     * 3) IOTX_MQTT_EVENT_PUBLISH_RECVEIVED:
     *      Its data type is @iotx_mqtt_packet_info_t and see detail at the declare of this type.
     *
     * */
    void *msg;
} iotx_mqtt_event_msg_t, *iotx_mqtt_event_msg_pt;


/**
 * @brief It define a datatype of function pointer.
 *        This type of function will be called when a related event occur.
 *
 * @param pcontext, the program context
 * @param pclient, the MQTT client
 * @param msg, the event message.
 *
 * @return none
 */
typedef void (*iotx_mqtt_event_handle_func_fpt)(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg);


/* The structure of MQTT event handle */
typedef struct {
    iotx_mqtt_event_handle_func_fpt     h_fp;
    void                               *pcontext;
} iotx_mqtt_event_handle_t, *iotx_mqtt_event_handle_pt;


/* The structure of MQTT initial parameter */
typedef struct {

    uint16_t                    port;                   /* Specify MQTT broker port */
    const char                 *host;                   /* Specify MQTT broker host */
    const char                 *client_id;              /* Specify MQTT connection client id*/
    const char                 *username;               /* Specify MQTT user name */
    const char                 *password;               /* Specify MQTT password */

    /* Specify MQTT transport channel and key.
     * If the value is NULL, it means that use TCP channel,
     * If the value is NOT NULL, it means that use SSL/TLS channel and
     *   @pub_key point to the CA certification */
    const char                 *pub_key;

    uint8_t                     clean_session;            /* Specify MQTT clean session or not*/
    uint32_t                    request_timeout_ms;       /* Specify timeout of a MQTT request in millisecond */
    uint32_t                    keepalive_interval_ms;    /* Specify MQTT keep-alive interval in millisecond */

    char                       *pwrite_buf;               /* Specify write-buffer */
    uint32_t                    write_buf_size;           /* Specify size of write-buffer in byte */
    char                       *pread_buf;                /* Specify read-buffer */
    uint32_t                    read_buf_size;            /* Specify size of read-buffer in byte */

    iotx_mqtt_event_handle_t    handle_event;             /* Specify MQTT event handle */

} iotx_mqtt_param_t, *iotx_mqtt_param_pt;


/**
 * @brief Construct the MQTT client
 *        This function initialize the data structures, establish MQTT connection.
 *
 * @param pInitParams, specify the MQTT client parameter.
 *
 * @return NULL, construct failed; NOT NULL, the handle of MQTT client.
 */
void *IOT_MQTT_Construct(iotx_mqtt_param_t *pInitParams);


/**
 * @brief Deconstruct the MQTT client
 *        This function disconnect MQTT connection and release the related resource.
 *
 * @param pointer of handle, specify the MQTT client.
 *
 * @return 0, deconstruct success; -1, deconstruct failed.
 */
int IOT_MQTT_Destroy(void **phandle);


/**
 * @brief Handle MQTT packet from remote server and process timeout request
 *        which include the MQTT subscribe, unsubscribe, publish(QOS >= 1), reconnect, etc..
 *
 * @param handle, specify the MQTT client.
 * @param timeout, specify the timeout in millisecond in this loop.
 *
 * @return status.
 */
int IOT_MQTT_Yield(void *handle, int timeout_ms);


/**
 * @brief check whether MQTT connection is established or not.
 *
 * @param handle, specify the MQTT client.
 *
 * @return true, MQTT in normal state; false, MQTT in abnormal state.
 */
int IOT_MQTT_CheckStateNormal(void *handle);


/**
 * @brief Subscribe MQTT topic.
 *
 * @param handle, specify the MQTT client.
 * @param topic_filter, specify the topic filter.
 * @param qos, specify the MQTT Requested QoS.
 * @param topic_handle_func, specify the topic handle callback-function.
 * @param pcontext, specify context. When call @topic_handle_func, it will be passed back.
 *
 * @return
 * @verbatim
      -1, subscribe failed.
     >=0, subscribe successful.
          The value is a unique ID of this request.
          The ID will be passed back when callback @iotx_mqtt_param_t:handle_event.
   @endverbatim
 */
int IOT_MQTT_Subscribe(void *handle,
                       const char *topic_filter,
                       iotx_mqtt_qos_t qos,
                       iotx_mqtt_event_handle_func_fpt topic_handle_func,
                       void *pcontext);


/**
 * @brief Unsubscribe MQTT topic.
 *
 * @param handle, specify the MQTT client.
 * @param topic_filter, specify the topic filter.
 *
 * @return
 * @verbatim
      -1, unsubscribe failed.
     >=0, unsubscribe successful.
          The value is a unique ID of this request.
          The ID will be passed back when callback @iotx_mqtt_param_t:handle_event.
   @endverbatim
 */
int IOT_MQTT_Unsubscribe(void *handle, const char *topic_filter);


/**
 * @brief Publish message to specific topic.
 *
 * @param handle, specify the MQTT client.
 * @param topic_name, specify the topic name.
 * @param topic_msg, specify the topic message.
 *
 * @return
 * @verbatim
    -1, publish failed.
     0, publish successful, where QoS is 0.
    >0, publish successful, where QoS is >= 0.
        The value is a unique ID of this request.
        The ID will be passed back when callback @iotx_mqtt_param_t:handle_event.
 * @endverbatim
 *
 */
int IOT_MQTT_Publish(void *handle, const char *topic_name, iotx_mqtt_topic_info_pt topic_msg);
/* From mqtt_client.h */

#endif

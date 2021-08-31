/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "stdint.h"
#include "quickjs.h"
#include "aiot_mqtt_api.h"

typedef enum {
    AOS_MQTT_CONNECT,
    AOS_MQTT_RECONNECT,
    AOS_MQTT_DISCONNECT,
    AOS_MQTT_MESSAGE
} aos_mqtt_res_type_t;

/**
 * @brief subdev模块内部发生值得用户关注的状态变化时, 通知用户的事件类型
 */
typedef enum {
    /**
     * @brief 非法的应答报文
     */
    MQTT_JSCALLBACK_INVALID_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    MQTT_JSCALLBACK_START_CLIENT_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    MQTT_JSCALLBACK_SCRIBE_TOPIC_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    MQTT_JSCALLBACK_UNSCRIBE_TOPIC_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    MQTT_JSCALLBACK_PUBLISH_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    MQTT_JSCALLBACK_CLIENT_STOP_REF,
    /**
     * @brief 应答报文的code字段非法
     */
    MQTT_JSCALLBACK_COMMON_REF,
    /**
     * @brief 应答报文的code字段非法
     */
    MQTT_JSCALLBACK_INVALID_CODE
} amp_mqtt_jscallback_type_t;

typedef struct {
    aiot_mqtt_recv_type_t type;
    int code;
    int topic_len;
    int payload_len;
    char *topic;
    char *payload;
} aos_mqtt_recv_t;

typedef struct {
    aiot_mqtt_event_type_t type;
    int code;
} aos_mqtt_event_t;

typedef struct {
    aiot_mqtt_option_t option;
    aos_mqtt_recv_t recv;
    aos_mqtt_event_t event;
} aos_mqtt_message_t;

typedef struct {
    void (*callback)(aos_mqtt_message_t *message, void *userdata);
    void *handle;
} aos_mqtt_userdata_t;

typedef struct aos_mqtt_handle {
    char *host;
    uint16_t port;
    char *clientid;
    char *username;
    char *password;
    uint16_t keepaliveSec;
    void *mqtt_handle;
    JSValue js_cb_ref[MQTT_JSCALLBACK_INVALID_CODE];
    int res;
} aos_mqtt_handle_t;

/* create mqtt client */
int32_t mqtt_client_start(void **handle, aos_mqtt_userdata_t *userdata);

/* destroy mqtt client */
int32_t mqtt_client_stop(void **handle);

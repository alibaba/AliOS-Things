/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "stdint.h"

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
    MQTT_JSCALLBACK_INVALID_CODE
} amp_mqtt_jscallback_type_t;

typedef struct amp_mqtt_params{
    char *host;
    uint16_t port;
    char *clientid;
    char *username;
    char *password;
    uint8_t keepaliveSec;
    int js_cb_ref[MQTT_JSCALLBACK_INVALID_CODE];
    int res;
}amp_mqtt_params_t;

typedef struct amp_mqtt_handle{
    void *mqtt_handle;
    int js_cb_ref[MQTT_JSCALLBACK_INVALID_CODE];
    int res;
}amp_mqtt_handle_t;

/* create mqtt client */
int32_t mqtt_client_start(void **handle, amp_mqtt_params_t *mqtt_params);

/* destroy mqtt client */
int32_t mqtt_client_stop(void **handle);
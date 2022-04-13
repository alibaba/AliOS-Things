/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "stdint.h"

typedef enum {
    AIOT_MQTT_CONNECT,
    AIOT_MQTT_RECONNECT,
    AIOT_MQTT_DISCONNECT,
    AIOT_MQTT_MESSAGE
}aiot_mqtt_res_type_t;

/**
 * @brief dev模块内部发生值得用户关注的状态变化时, 通知用户的事件类型
 */
typedef enum {
    /**
     * @brief 非法的应答报文
     */
    AIOT_DEV_JSCALLBACK_INVALID_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    AIOT_DEV_JSCALLBACK_CREATE_DEV_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    AIOT_DEV_JSCALLBACK_SUBSCRIBE_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    AIOT_DEV_JSCALLBACK_UNSUBSCRIBE_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    AIOT_DEV_JSCALLBACK_PUBLISH_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    AIOT_DEV_JSCALLBACK_POST_PROPS_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    AIOT_DEV_JSCALLBACK_POST_EVENT_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    AIOT_DEV_JSCALLBACK_ONPROPS_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    AIOT_DEV_JSCALLBACK_ONSERVICE_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    AIOT_DEV_JSCALLBACK_REGISTER_DEV_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    AIOT_DEV_JSCALLBACK_END_CLIENT_REF,
    /**
     * @brief 应答报文的code字段非法
     */
    AIOT_DEV_JSCALLBACK_INVALID_CODE
} aiot_dev_jscallback_type_t;

/**
 * @brief subdev模块内部发生值得用户关注的状态变化时, 通知用户的事件类型
 */
typedef enum {
    /**
     * @brief 非法的应答报文
     */
    AIOT_SUBDEV_JSCALLBACK_INVALID_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    AIOT_SUBDEV_JSCALLBACK_CREATE_GATEWAY_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    AIOT_SUBDEV_JSCALLBACK_ADD_TOPO_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    AIOT_SUBDEV_JSCALLBACK_GET_TOPO_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    AIOT_SUBDEV_JSCALLBACK_REMOVE_TOPO_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    AIOT_SUBDEV_JSCALLBACK_LOGIN_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    AIOT_SUBDEV_JSCALLBACK_LOGOUT_REF,
    /**
     * @brief 应答报文的code字段非法
     */
    AIOT_SUBDEV_JSCALLBACK_REGISTER_SUBDEV_REF,
    /**
     * @brief 应答报文的code字段非法
     */
    AIOT_SUBDEV_JSCALLBACK_INVALID_CODE
} aiot_subdev_jscallback_type_t;

typedef struct iot_device_hanlde{
    void *mqtt_handle;
    void *dm_handle;
    char *region;
    int js_cb_ref[AIOT_DEV_JSCALLBACK_INVALID_CODE];
    uint16_t keepaliveSec;
    int res;
}iot_device_handle_t;

typedef struct iot_gateway_handle{
    void *mqtt_handle;
    void *subdev_handle;
    aiot_subdev_jscallback_type_t js_cb_ref[AIOT_SUBDEV_JSCALLBACK_INVALID_CODE];
    uint16_t keepaliveSec;
    int res;
}iot_gateway_handle_t;

typedef struct iot_gateway_response{
    int js_cb_ref;
    int msg_id;
    int code;
    char productkey[IOTX_PRODUCT_KEY_LEN];
    char devicename[IOTX_DEVICE_NAME_LEN];
    char message[128];
}iot_gateway_response_t;

/* create mqtt client */
int32_t aiot_mqtt_client_start(void **handle, int keepaliveSec, int js_cb_ref);

/* destroy mqtt client */
int32_t aiot_mqtt_client_stop(void **handle);

/* app mqtt process thread */
void *aiot_app_mqtt_process_thread(void *args);

/* app mqtt recv thread */
void *aiot_app_mqtt_recv_thread(void *args);

/* property post */
int32_t aiot_app_send_property_post(void *dm_handle, char *params);

/* event post */
int32_t aiot_app_send_event_post(void *dm_handle, char *event_id, char *params);

/* device dynmic register */
int32_t aiot_dynreg_http(int js_cb_ref);
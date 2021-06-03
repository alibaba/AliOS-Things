/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "stdint.h"
#include "aiot_mqtt_api.h"

/* device active info report */
#define APPLICATION "soundbox"                  // product of application
#define MODULE_NAME aos_get_platform_type()     // module type

/* device info report format */
#define DEVICE_INFO_UPDATE_FMT \
        "[" \
            "{\"attrKey\":\"SYS_SDK_LANGUAGE\",\"attrValue\":\"C\",\"domain\":\"SYSTEM\"}" \
            "{\"attrKey\":\"SYS_LP_SDK_VERSION\",\"attrValue\":\"aos-r-3.0.0\",\"domain\":\"SYSTEM\"}" \
            "{\"attrKey\":\"SYS_PARTNER_ID\",\"attrValue\":\"AliOS Things Team\",\"domain\":\"SYSTEM\"}" \
            "{\"attrKey\":\"SYS_MODULE_ID\",\"attrValue\":\"haas-amp-%s@%s\",\"domain\":\"SYSTEM\"}" \
        "]"

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
     * @brief 应答报文的id字段非法
     */
    AIOT_SUBDEV_JSCALLBACK_ON_MQTT_MESSAGE_REF,
    /**
     * @brief 应答报文的code字段非法
     */
    AIOT_SUBDEV_JSCALLBACK_REGISTER_SUBDEV_REF,
    /**
     * @brief 应答报文的id字段非法
     */
    AIOT_SUBDEV_JSCALLBACK_SUBSCRIBE_REF,

    /**
     * @brief 应答报文的id字段非法
     */
    AIOT_SUBDEV_JSCALLBACK_UNSUBSCRIBE_REF,

    /**
     * @brief 应答报文的id字段非法
     */
    AIOT_SUBDEV_JSCALLBACK_PUBLISH_REF,

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
    void *dm_handle;
    void *subdev_handle;
    int js_cb_ref[AIOT_SUBDEV_JSCALLBACK_INVALID_CODE];
    uint16_t keepaliveSec;
}iot_gateway_handle_t;

typedef struct iot_gateway_response{
    int js_cb_ref;
    int msg_id;
    int code;
    char productkey[IOTX_PRODUCT_KEY_LEN];
    char devicename[IOTX_DEVICE_NAME_LEN];
    char message[128];
}iot_gateway_response_t;

typedef struct {
    aiot_mqtt_recv_type_t type;
    int code;
    int topic_len;
    int payload_len;
    char *topic;
    char *payload;
} iot_mqtt_recv_t;

typedef struct {
    aiot_mqtt_event_type_t type;
    int code;
} iot_mqtt_event_t;

typedef struct {
    aiot_mqtt_option_t option;
    iot_mqtt_recv_t recv;
    iot_mqtt_event_t event;
}iot_mqtt_message_t;

typedef struct {
    void (*callback)(iot_mqtt_message_t *message, void *userdata);
    void *handle;
}iot_mqtt_userdata_t;

/* create mqtt client */
int32_t aiot_mqtt_client_start(void **handle, int keepaliveSec, iot_mqtt_userdata_t *userdata);

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

/* device active info report */
int32_t amp_app_devinfo_report(void *mqtt_handle);

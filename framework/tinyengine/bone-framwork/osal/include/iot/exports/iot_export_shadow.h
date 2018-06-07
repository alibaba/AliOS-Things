/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stddef.h>
/* From shadow.h */
typedef enum {
    IOTX_SHADOW_ACK_TIMEOUT = -1,
    IOTX_SHADOW_ACK_NONE = 0,
    IOTX_SHADOW_ACK_SUCCESS = 200,
    IOTX_SHADOW_ACK_ERR_JSON_FMT_IS_INVALID = 400,
    IOTX_SHADOW_ACK_ERR_METHOD_IS_NULL = 401,
    IOTX_SHADOW_ACK_ERR_STATE_IS_NULL = 402,
    IOTX_SHADOW_ACK_ERR_VERSION_IS_INVALID = 403,
    IOTX_SHADOW_ACK_ERR_REPORTED_IS_NULL = 404,
    IOTX_SHADOW_ACK_ERR_REPORTED_ATTRIBUTE_IS_NULL = 405,
    IOTX_SHADOW_ACK_ERR_METHOD_IS_INVALID = 406,
    IOTX_SHADOW_ACK_ERR_SHADOW_DOCUMENT_IS_NULL = 407,
    IOTX_SHADOW_ACK_ERR_ATTRIBUTE_EXCEEDED = 408,
    IOTX_SHADOW_ACK_ERR_SERVER_FAILED = 500,
} iotx_shadow_ack_code_t;

typedef enum {
    IOTX_SHADOW_READONLY,
    IOTX_SHADOW_WRITEONLY,
    IOTX_SHADOW_RW
} iotx_shadow_datamode_t;

typedef enum {
    IOTX_SHADOW_NULL,
    IOTX_SHADOW_INT32,
    IOTX_SHADOW_STRING,
} iotx_shadow_attr_datatype_t;

typedef struct {
    bool flag_new;
    uint32_t buf_size;
    uint32_t offset;
    char *buf;
} format_data_t, *format_data_pt;

typedef struct {
    uint32_t base_system_time; //in millisecond
    uint32_t epoch_time;
} iotx_shadow_time_t, *iotx_shadow_time_pt;

typedef void (*iotx_push_cb_fpt)(
            void *pcontext,
            iotx_shadow_ack_code_t ack_code,
            const char *ack_msg, // NOTE: NOT a string.
            uint32_t ack_msg_len);

struct iotx_shadow_attr_st;

typedef void (*iotx_shadow_attr_cb_t)(struct iotx_shadow_attr_st *pattr);
typedef struct iotx_shadow_attr_st {
    iotx_shadow_datamode_t mode;       ///< data mode
    const char *pattr_name;             ///< attribute name
    void *pattr_data;                   ///< pointer to the attribute data
    iotx_shadow_attr_datatype_t attr_type;    ///< data type
    uint32_t timestamp;                 ///timestamp in Epoch(Unix) format
    iotx_shadow_attr_cb_t callback;    ///< callback when related control message come.
} iotx_shadow_attr_t, *iotx_shadow_attr_pt;

typedef struct {
    iotx_mqtt_param_t mqtt;
} iotx_shadow_para_t, *iotx_shadow_para_pt;

/**
 * @brief Construct the Device Shadow
 *        This function initialize the data structures, establish MQTT connection
 *        and subscribe the topic: "/shadow/get/${product_key}/${device_name}"
 *
 * @param pClient, A device shadow client data structure.
 * @param pParams, The specific initial parameter.
 *
 * @return NULL, construct shadow failed; NOT NULL, deconstruct failed.
 */
void *IOT_Shadow_Construct(iotx_shadow_para_pt pparam);


/* Deconstruct the specific device shadow */
iotx_err_t IOT_Shadow_Destroy(void *handle);


/* Handle MQTT packet from cloud and wait list */
void IOT_Shadow_Yield(void *handle, uint32_t timeout);


/* Register the specific attribute */
iotx_err_t IOT_Shadow_RegisterAttribute(void *handle, iotx_shadow_attr_pt pattr);


/* Delete the specific attribute */
iotx_err_t IOT_Shadow_DeleteAttribute(void *handle, iotx_shadow_attr_pt pattr);


/* Format the attribute name and value for update */
iotx_err_t IOT_Shadow_PushFormat_Init(
            void *handle,
            format_data_pt pformat,
            char *buf,
            uint16_t size);

iotx_err_t IOT_Shadow_PushFormat_Add(
            void *handle,
            format_data_pt pformat,
            iotx_shadow_attr_pt pattr);

iotx_err_t IOT_Shadow_PushFormat_Finalize(void *handle, format_data_pt pformat);


/* Update data to Cloud. It is a synchronous interface. */
iotx_err_t IOT_Shadow_Push(
            void *handle,
            char *data,
            uint32_t data_len,
            uint16_t timeout_s);


/* Update data to Cloud. It is a asynchronous interface.
 * The result of this update will be informed by calling the callback function @cb_fpt */
iotx_err_t IOT_Shadow_Push_Async(
            void *handle,
            char *data,
            size_t data_len,
            uint16_t timeout_s,
            iotx_push_cb_fpt cb_fpt,
            void *pcontext);


/* Synchronize device shadow data from cloud. It is a synchronous interface. */
iotx_err_t IOT_Shadow_Pull(void *handle);
/* From shadow.h */

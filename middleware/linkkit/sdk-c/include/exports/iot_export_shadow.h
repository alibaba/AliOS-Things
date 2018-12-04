/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __SHADOW_EXPORT_H__
#define __SHADOW_EXPORT_H__

#if defined(__cplusplus)
extern "C" {
#endif

/* From shadow.h */
#include <sys/types.h>
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
    int         flag_new;
    uint32_t    buf_size;
    uint32_t    offset;
    char       *buf;
} format_data_t, *format_data_pt;

typedef struct {
    uint32_t base_system_time; /* in millisecond */
    uint32_t epoch_time;
} iotx_shadow_time_t, *iotx_shadow_time_pt;

typedef void (*iotx_push_cb_fpt)(
            void *pcontext,
            int ack_code,
            const char *ack_msg, /* NOTE: NOT a string. */
            uint32_t ack_msg_len);

struct iotx_shadow_attr_st;

typedef void (*iotx_shadow_attr_cb_t)(struct iotx_shadow_attr_st *pattr);
typedef struct iotx_shadow_attr_st {
    iotx_shadow_datamode_t mode;            /* data mode */
    const char *pattr_name;                 /* attribute name */
    void *pattr_data;                       /* pointer to the attribute data */
    iotx_shadow_attr_datatype_t attr_type;  /* data type */
    uint32_t timestamp;                     /* timestamp in Epoch(Unix) format */
    iotx_shadow_attr_cb_t callback;         /* callback when related control message come. */
} iotx_shadow_attr_t, *iotx_shadow_attr_pt;

typedef struct {
    iotx_mqtt_param_t mqtt;
} iotx_shadow_para_t, *iotx_shadow_para_pt;

/** @defgroup group_api api
 *  @{
 */

/** @defgroup group_shadow shadow
 *  @{
 */

/**
 * @brief Construct the Device Shadow.
 *        This function initialize the data structures, establish MQTT connection.
 *        and subscribe the topic: "/shadow/get/${product_key}/${device_name}".
 *
 * @param [in] pparam: The specific initial parameter.
 * @retval     NULL : Construct shadow failed.
 * @retval NOT_NULL : Construct success.
 * @see None.
 */
DLL_IOT_API void *IOT_Shadow_Construct(iotx_shadow_para_pt pparam);

/**
 * @brief Deconstruct the specific device shadow.
 *
 * @param [in] handle: The handle of device shaodw.
 * @retval SUCCESS_RETURN : Success.
 * @retval          other : See iotx_err_t.
 * @see None.
 */
iotx_err_t IOT_Shadow_Destroy(void *handle);

/**
 * @brief Handle MQTT packet from cloud and wait list.
 *
 * @param [in] handle: The handle of device shaodw.
 * @param [in] timeout_ms: Specify the timeout value in millisecond. In other words, the API block 'timeout'_ms millisecond maximumly.
 * @return None.
 * @see None.
 */
DLL_IOT_API void IOT_Shadow_Yield(void *handle, uint32_t timeout_ms);

/**
 * @brief Create a data type registered to the server.
 *
 * @param [in] handle: The handle of device shaodw.
 * @param [in] pattr: The parameter which registered to the server.
 * @retval SUCCESS_RETURN : Success.
 * @retval          other : See iotx_err_t.
 * @see None.
 */
iotx_err_t IOT_Shadow_RegisterAttribute(void *handle, iotx_shadow_attr_pt pattr);

/**
 * @brief Delete the specific attribute.
 *
 * @param [in] handle: The handle of device shaodw.
 * @param [in] pattr: The parameter to be deleted from server.
 * @retval SUCCESS_RETURN : Success.
 * @retval          other : See iotx_err_t.
 * @see None.
 */
iotx_err_t IOT_Shadow_DeleteAttribute(void *handle, iotx_shadow_attr_pt pattr);

/**
 * @brief Start a process the structure of the data type format.
 *
 * @param [in] handle: The handle of device shaodw.
 * @param [out] pformat: The format struct of device shadow.
 * @param [in] buf: The buf which store device shadow.
 * @param [in] size: Maximum length of device shadow attribute.
 * @retval SUCCESS_RETURN : Success.
 * @retval          other : See iotx_err_t.
 * @see None.
 */
iotx_err_t IOT_Shadow_PushFormat_Init(
            void *handle,
            format_data_pt pformat,
            char *buf,
            uint16_t size);

/**
 * @brief Format the attribute name and value for update.
 *
 * @param [in] handle: The handle of device shaodw.
 * @param [in] pformat: The format struct of device shadow.
 * @param [in] pattr: To have created the data type of the format in the add member attributes.
 * @retval SUCCESS_RETURN : Success.
 * @retval          other : See iotx_err_t.
 * @see None.
 */
iotx_err_t IOT_Shadow_PushFormat_Add(
            void *handle,
            format_data_pt pformat,
            iotx_shadow_attr_pt pattr);

/**
 * @brief Complete a process the structure of the data type format.
 *
 * @param [in] handle: The handle of device shaodw.
 * @param [in] pformat: The format struct of device shadow.
 * @retval SUCCESS_RETURN : Success.
 * @retval          other : See iotx_err_t.
 * @see None.
 */
iotx_err_t IOT_Shadow_PushFormat_Finalize(void *handle, format_data_pt pformat);

/**
 * @brief Update data to Cloud. It is a synchronous interface.
 *
 * @param [in] handle: The handle of device shaodw.
 * @param [in] data: The buf which synchronization with the server.
 * @param [in] data_len: The length, in bytes, of the data pointed to by the data parameter.
 * @param [in] timeout_s: The timeout_s in second.In other word,the API will block timeout_s second.
 * @retval SUCCESS_RETURN : Success.
 * @retval          other : See iotx_err_t.
 * @see None.
 */
iotx_err_t IOT_Shadow_Push(
            void *handle,
            char *data,
            uint32_t data_len,
            uint16_t timeout_s);

/**
 * @brief Update data to Cloud. It is a asynchronous interface.
 *        The result of this update will be informed by calling the callback function cb_fpt.
 * @param [in] handle: The handle of device shadow.
 * @param [in] data: The buf which synchronization with the server.
 * @param [in] data_len: The length, in bytes, of the data pointed to by the data parameter.
 * @param [in] timeout_s: Specify the timeout value in second. Shadow will timeout after 'timeout_s' second if did not receive push response.
 * @param [in] cb_fpt: Specify the callback function which recieve ack_code from server after push device shadow.
 * @param [in] pcontext: Specify the context which passed to the callback function.
 * @retval SUCCESS_RETURN : Success.
 * @retval          other : See iotx_err_t.
 * @see None.
 */
DLL_IOT_API int IOT_Shadow_Push_Async(
            void *handle,
            char *data,
            unsigned int data_len,
            uint16_t timeout_s,
            iotx_push_cb_fpt cb_fpt,
            void *pcontext);

/**
 * @brief Synchronize device shadow data from cloud.
 *        It is a synchronous interface.
 * @param [in] handle: The handle of device shaodw.
 * @retval SUCCESS_RETURN : Success.
 * @retval          other : See iotx_err_t.
 * @see None.
 */
iotx_err_t IOT_Shadow_Pull(void *handle);

/* From shadow.h */

/** @} */ /* end of api_shadow */
/** @} */ /* end of api */

#if defined(__cplusplus)
}
#endif
#endif /* __SHADOW_EXPORT_H__ */

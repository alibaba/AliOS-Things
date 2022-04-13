/**
 * @file aiot_http_api.h
 * @brief HTTP模块头文件, 提供用HTTP协议向阿里云物联网平台上报数据的能力
 * @date 2019-12-27
 *
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 *
 */

#ifndef _AIOT_HTTP_API_H_
#define _AIOT_HTTP_API_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include "core_stdinc.h"

/**
 * @brief 服务器返回的业务错误码
 *
 * @details
 *
 * 从云平台对上报消息回应的报文JSON中解析
 */
typedef enum {
    /**
     * @brief 0, 服务端成功接收到上报的消息
     */
    AIOT_HTTP_RSPCODE_SUCCESS                   = 0,
    /**
     * @brief 10000, 服务端返回未知错误
     */
    AIOT_HTTP_RSPCODE_COMMON_ERROR              = 10000,
    /**
     * @brief 10001, 请求参数错误
     */
    AIOT_HTTP_RSPCODE_PARAM_ERROR               = 10001,
    /**
     * @brief 20001, token过期, 请调用 @ref aiot_http_auth 进行鉴权, 获取新token
     */
    AIOT_HTTP_RSPCODE_TOKEN_EXPIRED             = 20001,
    /**
     * @brief 20002, 请求的header中无token可表明设备端合法, 请调用 @ref aiot_http_auth 进行鉴权, 获取新token
     */
    AIOT_HTTP_RSPCODE_TOKEN_NULL                = 20002,
    /**
     * @brief 20003, token错误, 请调用 @ref aiot_http_auth 进行鉴权, 获取新token
     */
    AIOT_HTTP_RSPCODE_TOKEN_CHECK_ERROR         = 20003,
    /**
     * @brief 30001, 消息上报失败
     */
    AIOT_HTTP_RSPCODE_PUBLISH_MESSAGE_ERROR     = 30001,
    /**
     * @brief 40000, 设备端上报过于频繁, 触发服务端限流
     */
    AIOT_HTTP_RSPCODE_REQUEST_TOO_MANY          = 40000,
} aiot_http_response_code_t;

/**
 * @brief @ref aiot_http_setopt 函数的 option 参数, 对于下文每一个选项中的数据类型, 指的是 @ref aiot_mqtt_setopt 中的data参数的数据类型
 *
 */
typedef enum {
    /**
     * @brief HTTP 服务器的域名地址或者ip地址
     *
     * @details
     *
     * 阿里云物联网平台域名地址列表: (tcp使用80端口, tls使用443端口)
     *
     * |                域名地址               |  区域   | 端口号 |
     * |---------------------------------------|---------|--------|
     * | iot-as-http.cn-shanghai.aliyuncs.com  |  上海   |  443   |
     *
     * 数据类型: (char *)
     */
    AIOT_HTTPOPT_HOST,
    /**
     * @brief HTTP 服务器的端口号
     *
     * @details
     *
     * 连接阿里云物联网平台时:
     *
     * 1.如果使用的是tcp, 端口号设置为80
     *
     * 2. 如果使用的是tls, 端口号设置为443
     *
     * 数据类型: (uint16_t *)
     */
    AIOT_HTTPOPT_PORT,
    /**
     * @brief HTTP建联时, 网络使用的安全凭据
     *
     * @details
     *
     * 该配置项用于为底层网络配置 @ref aiot_sysdep_network_cred_t 安全凭据数据
     *
     * 1. 若该选项不配置, 那么MQTT将以tcp方式直接建联
     *
     * 2. 若 @ref aiot_sysdep_network_cred_t 中option配置为 @ref AIOT_SYSDEP_NETWORK_CRED_NONE , HTTP将以tcp方式直接建联
     *
     * 3. 若 @ref aiot_sysdep_network_cred_t 中option配置为 @ref AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA , HTTP将以tls方式建联
     *
     * 数据类型: (aiot_sysdep_network_cred_t *)
     */
    AIOT_HTTPOPT_NETWORK_CRED,
    /**
     * @brief HTTP建联时, 建立网络连接的超时时间
     *
     * @details
     *
     * 指建立socket连接的超时时间
     *
     * 数据类型: (uint32_t *) 默认值: (5 *1000) ms
     *
     */
    AIOT_HTTPOPT_CONNECT_TIMEOUT_MS,
    /**
     * @brief HTTP发送数据时, 在协议栈花费的最长时间
     *
     * @details
     *
     * 数据类型: (uint32_t *) 默认值: (5 * 1000) ms
     */
    AIOT_HTTPOPT_SEND_TIMEOUT_MS,
    /**
     * @brief HTTP接收数据时, 在协议栈花费的最长时间
     *
     * @details
     *
     * 数据类型: (uint32_t *) 默认值: (5 * 1000) ms
     */
    AIOT_HTTPOPT_RECV_TIMEOUT_MS,
    /**
     * @brief 销毁HTTP实例时, 等待其他api执行完毕的时间
     *
     * @details
     *
     * 当调用 @ref aiot_http_deinit 销毁HTTP实例时, 若继续调用其他 aiot_http_xxx API, API会返回STATE_USER_INPUT_EXEC_DISABLED 错误
     *
     * 此时, 用户应该停止调用其他 aiot_http_xxx API
     *
     * 数据类型: (uint32_t *) 默认值: (2 * 1000) ms
     */
    AIOT_HTTPOPT_DEINIT_TIMEOUT_MS,
    /**
     * @brief 当接收服务器返回的http报文时, 单行http header的最大长度
     *
     * @details
     *
     * 当单行http header设置过短时, @ref aiot_http_recv 会返回 @ref STATE_HTTP_HEADER_BUFFER_TOO_SHORT 状态码
     *
     * 数据类型: (uint32_t *) 默认值: 128
     */
    AIOT_HTTPOPT_HEADER_BUFFER_LEN,
    /**
     * @brief 当接收服务器返回的http报文时, 每次从 @ref aiot_http_recv_handler_t 回调函数中给出的body最大长度
     *
     * @details
     *
     * 数据类型: (uint32_t *) 默认值: 128
     */
    AIOT_HTTPOPT_BODY_BUFFER_LEN,
    /**
     * @brief HTTP 内部事件回调函数
     *
     * @details
     *
     * 数据类型: (aiot_http_event_handler_t)
     */
    AIOT_HTTPOPT_EVENT_HANDLER,

    /* 以上选项配置的数据与 CORE_HTTPOPT_XXX 共用 */

    /**
     * @brief 用户需要SDK暂存的上下文
     *
     * @details
     *
     * 1. 当接收到HTTP数据时, 该上下文会从 @ref aiot_http_recv_handler_t 的 userdata 参数给出
     *
     * 2. 当HTTP内部有事件发生时, 该上下文会从 @ref aiot_http_event_handler_t 的 userdata 参数给出
     *
     * 数据类型: (void *)
     */
    AIOT_HTTPOPT_USERDATA,
    /**
     * @brief HTTP 数据接收回调函数
     *
     * @details
     *
     * 数据类型: (aiot_http_recv_handler_t)
     */
    AIOT_HTTPOPT_RECV_HANDLER,
    /**
     * @brief 设备的product key, 可从<a href="http://iot.console.aliyun.com/">阿里云物联网平台控制台</a>获取
     *
     * @details
     *
     * 数据类型: (char *)
     */
    AIOT_HTTPOPT_PRODUCT_KEY,
    /**
     * @brief 设备的device name, 可从<a href="http://iot.console.aliyun.com/">阿里云物联网平台控制台</a>获取
     *
     * @details
     *
     * 数据类型: (char *)
     */
    AIOT_HTTPOPT_DEVICE_NAME,
    /**
     * @brief 设备的device secret, 可从<a href="http://iot.console.aliyun.com/">阿里云物联网平台控制台</a>获取
     *
     * @details
     *
     * 数据类型: (char *)
     */
    AIOT_HTTPOPT_DEVICE_SECRET,
    /**
     * @brief 上报设备的扩展信息, 比如模组商ID和模组ID
     *
     * @details
     *
     * 1. 模组商ID: 格式为pid=xxx
     *
     * 2. 模组ID: 格式为mid=xxx
     *
     * 如果需要同时上报多个信息, 那么它们之间用&连接, 例如: pid=xxx&mid=xxx
     *
     * 数据类型: (char *)
     */
    AIOT_HTTPOPT_EXTEND_DEVINFO,
    /**
     * @brief 使用 @ref aiot_http_auth 进行认证并获取token的超时时间
     *
     * @details
     *
     * 数据类型: (uint32_t *) 默认值: (5 * 1000) ms
     */
    AIOT_HTTPOPT_AUTH_TIMEOUT_MS,
    /**
     * @brief 是否使用http长连接
     *
     * @details
     *
     * 若该配置的值为0, 则每次使用 @ref aiot_http_auth 和 @ref aiot_http_send 时, SDK会重新与 HTTP 服务器建立简介
     *
     * 数据类型: (uint8_t *) 默认值: (5 * 1000) ms
     */
    AIOT_HTTPOPT_LONG_CONNECTION,

    AIOT_HTTPOPT_MAX
} aiot_http_option_t;

/**
 * @brief SDK收到HTTP报文, 传递给用户数据回调函数时, 对报文类型的描述
 */
typedef enum {
    /**
     * @brief 获取到HTTP Status Code
     */
    AIOT_HTTPRECV_STATUS_CODE,
    /**
     * @brief 获取到HTTP Header, 每次返回Header中的一组键值对
     */
    AIOT_HTTPRECV_HEADER,
    /**
     * @brief 获取到HTTP Body, 返回完整的Body内容
     */
    AIOT_HTTPRECV_BODY
} aiot_http_recv_type_t;

/**
 * @brief SDK收到HTTP报文, 传递给用户数据回调函数时, 对报文内容的描述
 */
typedef struct {
    /**
     * @brief HTTP 消息类型, 更多信息请参考 @ref aiot_http_recv_type_t
     */
    aiot_http_recv_type_t type;
    union {
        /**
         * @brief HTTP 消息类型为 @ref AIOT_HTTPRECV_STATUS_CODE 时的数据
         */
        struct {
            /**
             * @brief HTTP Status Code
             */
            uint32_t code;
        } status_code;
        /**
         * @brief HTTP 消息类型为 @ref AIOT_HTTPRECV_HEADER 时的数据
         */
        struct {
            /**
             * @brief 单行 HTTP Header 的 key
             */
            char *key;
            /**
             * @brief 单行 HTTP Header 的 value
             */
            char *value;
        } header;
        /**
         * @brief HTTP 消息类型为 @ref AIOT_HTTPRECV_BODY 时的数据
         */
        struct {
            /**
             * @brief HTTP Body 的内容
             */
            uint8_t *buffer;
            /**
             * @brief HTTP Body 的长度
             */
            uint32_t len;
        } body;
    } data;
} aiot_http_recv_t;

/**
 * @brief HTTP 消息接收回调函数原型, 可以通过 @ref aiot_http_setopt 接口的 @ref AIOT_HTTPOPT_RECV_HANDLER 参数指定
 *
 * @details
 *
 * 当SDK收到 HTTP 服务器的应答数据时, 通过此回调函数输出
 *
 * @param[out] handle HTTP 句柄
 * @param[out] packet 从 HTTP 服务器接收到的数据
 * @param[out] userdata 用户通过 @ref AIOT_HTTPOPT_USERDATA 交由SDK暂存的上下文
 *
 * @return void
 */
typedef void (*aiot_http_recv_handler_t)(void *handle, const aiot_http_recv_t *packet, void *userdata);

/**
 * @brief SDK内部发生状态变化, 通过用户事件回调函数通知用户时, 对事件类型的描述
 */
typedef enum {
    /**
     * @brief token无效事件, 此时用户应该调用 @ref aiot_http_auth 获取新的token
     */
    AIOT_HTTPEVT_TOKEN_INVALID
} aiot_http_event_type_t;

/**
 * @brief SDK内部发生状态变化, 通过用户事件回调函数通知用户时, 对事件内容的描述
 */
typedef struct {
    aiot_http_event_type_t type;
} aiot_http_event_t;

/**
 * @brief HTTP 事件回调函数原型, 可以通过 @ref aiot_http_setopt 接口的 @ref AIOT_HTTPOPT_EVENT_HANDLER 参数指定
 *
 * @param[out] handle HTTP句柄
 * @param[out] event 事件结构体
 * @param[out] user_data 指向用户上下文数据的指针, 由 @ref aiot_http_setopt 的 @ref AIOT_HTTPOPT_USERDATA 选项设置
 */
typedef void (* aiot_http_event_handler_t)(void *handle, const aiot_http_event_t *event, void *userdata);

/**
 * @brief 创建一个HTTP上云实例
 *
 * @return void*
 *
 * @retval 非NULL, HTTP 实例句柄
 * @retval NULL,初始化 HTTP 实例失败
 */
void *aiot_http_init(void);

/**
 * @brief 设置HTTP实例参数
 *
 * @param[in] handle HTTP句柄
 * @param[in] option 配置选项, 更多信息请参考 @ref aiot_http_option_t
 * @param[in] data 配置数据, 更多信息请参考 @ref aiot_http_option_t
 *
 * @return int32_t
 *
 * @retval STATE_SUCCESS, 成功
 * @retval STATE_HTTP_HANDLE_IS_NULL, HTTP句柄为NULL
 * @retval STATE_USER_INPUT_OUT_RANGE, 用户输入参数无效
 * @retval STATE_SYS_DEPEND_MALLOC_FAILED, 内存分配失败
 */
int32_t aiot_http_setopt(void *handle, aiot_http_option_t option, void *data);

/**
 * @brief 向服务器发送认证请求, 获取token
 *
 * @param[in] handle HTTP句柄
 *
 * @return int32_t
 *
 * @retval STATE_SUCCESS, 认证成功
 * @retval STATE_HTTP_HANDLE_IS_NULL, HTTP句柄为NULL
 * @retval STATE_USER_INPUT_MISSING_PRODUCT_KEY, 未设置必要选项ProductKey
 * @retval STATE_USER_INPUT_MISSING_DEVICE_NAME, 未设置必要选项DeviceName
 * @retval STATE_USER_INPUT_MISSING_DEVICE_SECRET, 未设置必要选项DeviceSecret
 * @retval STATE_HTTP_TOKEN_LEN_ERROR, token长度错误
 * @retval STATE_HTTP_GET_TOKEN_FAILED, 获取token失败
 */
int32_t aiot_http_auth(void *handle);

/**
 * @brief 上报数据到物联网平台
 *
 * @param[in] handle HTTP句柄
 * @param[in] topic 上报的目标topic, 在物联网平台控制的产品详情页面有设备的完整topic列表
 * @param[in] payload 指向上报数据的指针
 * @param[in] payload_len 上报数据的长度
 *
 * @return int32_t
 *
 * @retval STATE_SUCCESS, 上报成功
 * @retval STATE_HTTP_HANDLE_IS_NULL, HTTP句柄为NULL
 * @retval STATE_USER_INPUT_OUT_RANGE, 用户输入参数无效
 * @retval STATE_HTTP_NOT_AUTH, 设备未认证
 */
int32_t aiot_http_send(void *handle, char *topic, uint8_t *payload, uint32_t payload_len);

/**
 * 服务器响应数据格式为
 * {
 *     "code": 0,              // 业务状态码
 *     "message": "success",   // 业务信息
 *     "info": {
 *         "messageId": 892687627916247040,
 *     }
 * }
 */

/**
 * @brief 接受HTTP应答数据, 数据会从用户设置的 @ref aiot_http_event_handler_t 回调函数输出
 *
 * @param[in] handle HTTP句柄
 *
 * @return int32_t
 *
 * @retval >= 0, 接受到的HTTP body数据长度
 * @retval STATE_HTTP_HANDLE_IS_NULL, HTTP句柄为NULL
 * @retval STATE_USER_INPUT_NULL_POINTER, 用户输入参数为NULL
 * @retval STATE_USER_INPUT_OUT_RANGE, buffer_len为0
 * @retval STATE_HTTP_RSP_MSG_ERROR, 服务器应答消息错误
 * @retval STATE_SYS_DEPEND_NWK_CLOSED, 网络连接已关闭
 * @retval STATE_SYS_DEPEND_NWK_READ_OVERTIME, 网络接收超时
 * @retval STATE_HTTP_RECV_LINE_TOO_LONG, HTTP单行数据过长, 内部无法解析
 * @retval STATE_HTTP_PARSE_STATUS_LINE_FAILED, 无法解析状态码
 * @retval STATE_HTTP_GET_CONTENT_LEN_FAILED, 获取Content-Length失败
 */
int32_t aiot_http_recv(void *handle);

/**
 * @brief 销毁参数p_handle所指定的HTTP实例
 *
 * @param[in] p_handle 指向HTTP句柄的指针
 *
 * @return int32_t
 *
 * @retval STATE_SUCCESS 成功
 * @retval STATE_USER_INPUT_NULL_POINTER 参数p_handle为NULL或者p_handle指向的句柄为NULL
 */
int32_t aiot_http_deinit(void **p_handle);


#if defined(__cplusplus)
}
#endif

#endif /* #ifndef _AIOT_HTTP_API_H_ */


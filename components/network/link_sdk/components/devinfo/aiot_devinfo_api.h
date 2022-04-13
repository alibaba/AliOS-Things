/**
 * @file aiot_devinfo_api.h
 * @brief devinfo模块头文件, 提供更新和删除设备标签的能力
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 * @details
 *
 * Devinfo模块用于向阿里云物联网平台更新或删除设备的标签, API的使用流程如下:
 *
 * 1. 首先参考 @ref aiot_mqtt_api.h 的说明, 保证成功建立与物联网平台的`MQTT`连接
 *
 * 2. 调用 @ref aiot_devinfo_init 初始化devinfo会话, 获取会话句柄
 *
 * 3. 调用 @ref aiot_devinfo_setopt 配置devinfo会话的参数, 常用配置项见 @ref aiot_devinfo_setopt 的说明
 *
 * 4. 调用 @ref aiot_devinfo_send 发送标签变更的请求, 比如更新或删除
 *
 * 5. 收到的应答经SDK处理后会调用由 @ref aiot_devinfo_setopt 配置的 @ref AIOT_DEVINFOOPT_RECV_HANDLER 回调函数, 通知用户云端的应答
 *
 */
#ifndef __AIOT_DEVINFO_API_H__
#define __AIOT_DEVINFO_API_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief -0x1200~-0x12FF表达SDK在devinfo模块内的状态码
 */
#define STATE_DEVINFO_BASE                                             (-0x1200)

/**
 * @brief MQTT会话句柄未设置, 请通过 @ref aiot_devinfo_setopt 设置MQTT会话句柄
 */
#define STATE_DEVINFO_MISSING_MQTT_HANDLE                              (-0x1201)

/**
 * @brief devinfo模块收到从网络上来的报文时, 通知用户的报文类型
 */
typedef enum {
    AIOT_DEVINFORECV_GENERIC_REPLY,
} aiot_devinfo_recv_type_t;

typedef struct {
    /**
     * @brief 消息标识符, uint64_t类型的整数, 与属性上报或事件上报的消息标示符一致
     */
    uint32_t msg_id;
    /**
     * @brief 设备端错误码, 200-请求成功, 更多错误码码查看<a href="https://help.aliyun.com/document_detail/120329.html">设备端错误码</a>
     */
    uint32_t code;
    /**
     * @brief 指向云端应答数据的指针
     */
    char *data;
    /**
     * @brief 云端应答数据的长度
     */
    uint32_t data_len;
    /**
     * @brief 指向状态消息字符串的指针, 当设备端上报请求成功时对应的应答消息为"success", 若请求失败则应答消息中包含错误信息
     */
    char *message;
    /**
     * @brief 消息字符串的长度
     */
    uint32_t message_len;
} aiot_devinfo_recv_generic_reply_t;

/**
 * @brief devinfo模块收到从网络上来的报文时, 通知用户的报文内容
 */
typedef struct {
    char *product_key;
    char *device_name;
    /**
     * @brief 报文内容所对应的报文类型, 更多信息请参考@ref aiot_devinfo_recv_type_t
     */
    aiot_devinfo_recv_type_t  type;
    union {
        /**
         * @brief 从云端收到的更新或删除设备标签的应答
         */
        aiot_devinfo_recv_generic_reply_t generic_reply;
    } data;
} aiot_devinfo_recv_t;

/**
 * @brief devinfo模块收到从网络上来的报文时, 通知用户所调用的数据回调函数
 *
 * @param[in] handle devinfo会话句柄
 * @param[in] packet devinfo消息结构体, 存放收到的devinfo报文内容
 * @param[in] userdata 用户上下文
 *
 * @return void
 */
typedef void (* aiot_devinfo_recv_handler_t)(void *handle, const aiot_devinfo_recv_t *packet, void *userdata);

/**
 * @brief devinfo模块内部发生值得用户关注的状态变化时, 通知用户的事件类型
 */
typedef enum {
    /**
     * @brief 收到的应答中设备信息不合法, 无法获取product key和device name
     */
    AIOT_DEVINFOEVT_INVALID_DEVINFO,
    /**
     * @brief 收到的应答中字段不合法
     */
    AIOT_DEVINFOEVT_INVALID_RESPONSE,
    /**
     * @brief 收到的应答中字段格式错误
     */
    AIOT_DEVINFOEVT_INVALID_RESPONSE_FORMAT,
} aiot_devinfo_event_type_t;

/**
 * @brief devinfo模块内部发生值得用户关注的状态变化时, 通知用户的事件内容
 */
typedef struct {
    /**
     * @brief 事件内容所对应的事件类型, 更多信息请参考@ref aiot_devinfo_event_type_t
     */
    aiot_devinfo_event_type_t  type;
} aiot_devinfo_event_t;

/**
 * @brief devinfo模块内部发生值得用户关注的状态变化时, 通知用户所调用的事件回调函数
 *
 * @param[in] handle, devinfo会话句柄
 * @param[in] event, devinfo模块中发生的事件的内容
 * @param[in] userdata, 用户上下文
 *
 * @return void
 */
typedef void (*aiot_devinfo_event_handler_t)(void *handle, const aiot_devinfo_event_t *event, void *userdata);

/**
 * @brief @ref aiot_devinfo_msg_t 中的发送消息类型
 *
 * @details
 *
 * 消息类型有两个, 分别是更新设备标签和删除设备标签
 */
typedef enum {
    /**
     * @brief 更新设备标签
     */
    AIOT_DEVINFO_MSG_UPDATE,
    /**
     * @brief 删除设备标签
     */
    AIOT_DEVINFO_MSG_DELETE
} aiot_devinfo_msg_type_t;

/**
 * @brief 更新或删除设备标签的params内容
 */
typedef struct {
    char *params;
} aiot_devinfo_msg_data_t;

typedef struct {
    /**
     * @brief 设备的product key
     */
    char *product_key;
    /**
     * @brief 设备的device name
     */
    char *device_name;
    /**
     * @brief 消息类型, 更多信息请参考@ref aiot_devinfo_msg_type_t
     */
    aiot_devinfo_msg_type_t type;
    union {
        /**
         * @brief 更新设备标签, 格式："[{\"attrKey\":\"xxx\",\"attrValue\":\"yyy\"}]"
         *
         * @details
         *
         * 从上述格式可以看出，更新设备标签的格式是一个JSON数组，一次可按attrKey和attrValue上报多组设备标签
         */
        aiot_devinfo_msg_data_t update;
        /**
         * @brief 删除设备标签, 格式："[{\"attrKey\":\"xxx\"}]"
         *
         * @details
         *
         * 从上述格式可以看出，删除设备标签的格式是一个JSON数组，一次可按attrKey删除多组设备标签
         */
        aiot_devinfo_msg_data_t delete;
    } data;
} aiot_devinfo_msg_t;

/**
 * @brief @ref aiot_devinfo_setopt 接口的option参数可选值.
 *
 * @details 下文每个选项中的数据类型, 指的是@ref aiot_devinfo_setopt 中, data参数的数据类型
 *
 * 1. data的数据类型是void *时, 以配置@ref AIOT_DEVINFOOPT_MQTT_HANDLE 为例:
 *
 *    void *mqtt_handle = aiot_mqtt_init();
 *    aiot_devinfo_setopt(devinfo_handle, AIOT_DEVINFOOPT_MQTT_HANDLE, mqtt_handle);
 *
 * 2. data的数据类型是其他数据类型时, 以配置@ref AIOT_DEVINFOOPT_DEINIT_TIMEOUT_MS 为例:
 *
 *    uint32_t deinit_timeout_ms = 443;
 *    aiot_devinfo_setopt(devinfo_handle, AIOT_DEVINFOOPT_DEINIT_TIMEOUT_MS, (void *)&deinit_timeout_ms);
 */
typedef enum {
    /**
     * @brief devinfo会话 需要的MQTT句柄, 需要先建立MQTT连接, 再设置MQTT句柄
     */
    AIOT_DEVINFOOPT_MQTT_HANDLE,

    /**
     * @brief 设置回调, 它在SDK收到网络报文的时候被调用, 告知用户
     *
     * @details
     *
     * 数据类型: ( @ref aiot_devinfo_recv_handler_t)
     */
    AIOT_DEVINFOOPT_RECV_HANDLER,

    /**
     * @brief 设置回调, 它在SDK发生内部状态变更时被调用, 告知用户
     *
     * @details
     *
     * 数据类型: ( @ref aiot_devinfo_event_handler_t)
     */
    AIOT_DEVINFOOPT_EVENT_HANDLER,

    /**
     * @brief 用户需要SDK暂存的上下文, 数据类型为(void *)
     *
     * @details 这个上下文指针会在 AIOT_DEVINFOOPT_RECV_HANDLER 和 AIOT_DEVINFOOPT_EVENT_HANDLER 设置的回调被调用时, 由SDK传给用户
     */
    AIOT_DEVINFOOPT_USERDATA,

    /**
     * @brief 销毁devinfo实例时, 等待其他api执行完毕的时间
     *
     * @details
     *
     * 当调用@ref aiot_devinfo_deinit 销毁devinfo实例时, 若继续调用其他aiot_devinfo_xxx API, API会返回@ref STATE_USER_INPUT_EXEC_DISABLED 错误
     *
     * 此时, 用户应该停止调用其他aiot_devinfo_xxx API
     *
     * 数据类型: (uint32_t *) 默认值: (2 * 1000) ms
     */
    AIOT_DEVINFOOPT_DEINIT_TIMEOUT_MS,
    AIOT_DEVINFOOPT_MAX
} aiot_devinfo_option_t;

/**
 * @brief 创建devinfo会话实例, 并以默认值配置会话参数
 *
 * @return void *
 * @retval 非NULL devinfo实例的句柄
 * @retval NULL   初始化失败, 一般是内存分配失败导致
 *
 */
void *aiot_devinfo_init(void);

/**
 * @brief 配置devinfo会话
 *
 * @param[in] handle devinfo会话句柄
 * @param[in] option 配置选项, 更多信息请参考@ref aiot_devinfo_option_t
 * @param[in] data   配置选项数据, 更多信息请参考@ref aiot_devinfo_option_t
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  参数配置失败
 * @retval >=STATE_SUCCESS 参数配置成功
 *
 */
int32_t aiot_devinfo_setopt(void *handle, aiot_devinfo_option_t option, void *data);

/**
 * @brief 结束devinfo会话, 销毁实例并回收资源
 *
 * @param[in] handle 指向devinfo会话句柄的指针
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  执行失败
 * @retval >=STATE_SUCCESS 执行成功
 *
 */
int32_t aiot_devinfo_deinit(void **handle);

/**
 * @brief 向devinfo服务器发送devinfo消息请求
 *
 * @param handle devinfo会话句柄
 * @param msg devinfo发送给云端的删除/更新设备标签信息的报文
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  请求发送失败
 * @retval >=STATE_SUCCESS 请求发送成功
 */
int32_t aiot_devinfo_send(void *handle, aiot_devinfo_msg_t *msg);

#if defined(__cplusplus)
}
#endif

#endif  /* __AIOT_DEVINFO_API_H__ */


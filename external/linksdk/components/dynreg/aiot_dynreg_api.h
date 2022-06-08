/**
 * @file aiot_dynreg_api.h
 * @brief dynreg模块头文件, 提供获取设备信息的能力
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
#ifndef __AIOT_DYNREG_API_H__
#define __AIOT_DYNREG_API_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief -0x0600~-0x06FF表达SDK在dynreg模块内的状态码
 */
#define STATE_DYNREG_BASE                                             (-0x0600)

/**
 * @brief 执行@ref aiot_dynreg_deinit 时, 等待其他API执行结束的超过设定的超时时间, MQTT实例销毁失败
 */
#define STATE_DYNREG_DEINIT_TIMEOUT                                   (-0x0601)

/**
 * @brief 需要首先执行@ref aiot_dynreg_send_request 发送dynreg请求
 */
#define STATE_DYNREG_NEED_SEND_REQUEST                                (-0x0602)

/**
 * @brief dynreg模块返回了错误的http status code
 */
#define STATE_DYNREG_INVALID_STATUS_CODE                              (-0x0603)

/**
 * @brief 收到非法的device secret
 */
#define STATE_DYNREG_INVALID_DEVICE_SECRET                            (-0x0604)

/**
 * @brief dynreg模块收到从网络上来的报文时, 通知用户的报文类型
 */
typedef enum {
    /**
     * @brief dynreg模块返回的http status code
     */
    AIOT_DYNREGRECV_STATUS_CODE,
    /**
     * @brief dynreg模块返回的设备信息
     */
    AIOT_DYNREGRECV_DEVICE_INFO,
} aiot_dynreg_recv_type_t;

/**
 * @brief dynreg模块收到从网络上来的报文时, 通知用户的报文内容
 */
typedef struct {
    /**
     * @brief 报文内容所对应的报文类型, 更多信息请参考@ref aiot_dynreg_recv_type_t
     */
    aiot_dynreg_recv_type_t  type;
    union {
        /**
         * @brief dynreg模块返回的http status code
         */
        struct {
            uint32_t code;
        } status_code;
        /**
         * @brief dynreg模块返回的设备信息
         */
        struct {
            char *device_secret;
        } device_info;
    } data;
} aiot_dynreg_recv_t;

/**
 * @brief dynreg模块收到从网络上来的报文时, 通知用户所调用的数据回调函数
 *
 * @param[in] handle dynreg会话句柄
 * @param[in] packet dynreg消息结构体, 存放收到的dynreg报文内容
 * @param[in] userdata 用户上下文
 *
 * @return void
 */
typedef void (* aiot_dynreg_recv_handler_t)(void *handle,
        const aiot_dynreg_recv_t *packet, void *userdata);

/**
 * @brief @ref aiot_dynreg_setopt 接口的option参数可选值.
 *
 * @details 下文每个选项中的数据类型, 指的是@ref aiot_dynreg_setopt 中, data参数的数据类型
 *
 * 1. data的数据类型是char *时, 以配置@ref AIOT_DYNREGOPT_HOST 为例:
 *
 *    char *host = "xxx";
 *    aiot_dynreg_setopt(dynreg_handle, AIOT_DYNREGOPT_HOST, host);
 *
 * 2. data的数据类型是其他数据类型时, 以配置@ref AIOT_DYNREGOPT_PORT 为例:
 *
 *    uint16_t port = 443;
 *    aiot_mqtt_setopt(dynreg_handle, AIOT_DYNREGOPT_PORT, (void *)&port);
 */
typedef enum {
    /**
     * @brief http动态注册 服务器建联时, 网络使用的安全凭据
     *
     * @details
     *
     * 该配置项用于为底层网络配置@ref aiot_sysdep_network_cred_t 安全凭据数据
     *
     * 应当把 @ref aiot_sysdep_network_cred_t 中option配置为@ref AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA , 以tls方式建联
     *
     * 数据类型: (aiot_sysdep_network_cred_t *)
     */
    AIOT_DYNREGOPT_NETWORK_CRED,

    /**
     * @brief http动态注册 服务器的域名地址或者ip地址
     *
     * @details
     *
     * 阿里云物联网平台 http动态注册 服务器域名地址列表:
     *
     * | 域名地址                                        | 区域    | 端口号
     * |-------------------------------------------------|---------|---------
     * | iot-auth.cn-shanghai.aliyuncs.com               | 国内    | 443
     * | iot-auth.ap-southeast-1.aliyuncs.com            | 海外    | 443
     *
     * 数据类型: (char *)
     */
    AIOT_DYNREGOPT_HOST,

    /**
     * @brief http动态注册 服务器的端口号
     *
     * @details
     *
     * 连接阿里云物联网平台 http动态注册 服务器时:
     *
     * 必须使用tls方式建联, 端口号设置为443
     *
     * 数据类型: (uint16_t *)
     */
    AIOT_DYNREGOPT_PORT,

    /**
     * @brief 设备的productKey, 可从<a href="http://iot.console.aliyun.com/">阿里云物联网平台控制台</a>获取
     *
     * @details
     *
     * 数据类型: (char *)
     */
    AIOT_DYNREGOPT_PRODUCT_KEY,

    /**
     * @brief 设备的productSecret, 可从<a href="http://iot.console.aliyun.com/">阿里云物联网平台控制台</a>获取
     *
     * @details
     *
     * 数据类型: (char *)
     */
    AIOT_DYNREGOPT_PRODUCT_SECRET,

    /**
     * @brief 设备的deviceName, 可从<a href="http://iot.console.aliyun.com/">阿里云物联网平台控制台</a>获取
     *
     * @details
     *
     * 数据类型: (char *)
     */
    AIOT_DYNREGOPT_DEVICE_NAME,

    /**
     * @brief dynreg会话发送消息时可消费的最长时间间隔
     *
     * @details
     *
     * 数据类型: (uint32_t) 默认值: (5 * 1000) ms
     */
    AIOT_DYNREGOPT_SEND_TIMEOUT_MS,

    /**
     * @brief dynreg会话接收消息时可消费的最长时间间隔
     *
     * @details
     *
     * 数据类型: (uint32_t) 默认值: (5 * 1000) ms
     */
    AIOT_DYNREGOPT_RECV_TIMEOUT_MS,

    /**
     * @brief 设置回调, 它在SDK收到网络报文的时候被调用, 告知用户
     *
     * @details
     *
     * 数据类型: (aiot_dynreg_http_recv_handler_t)
     */
    AIOT_DYNREGOPT_RECV_HANDLER,

    /**
     * @brief 用户需要SDK暂存的上下文
     *
     * @details
     *
     * 这个上下文指针会在 AIOT_DYNREGOPT_RECV_HANDLER 和 AIOT_DYNREGOPT_EVENT_HANDLER 设置的回调被调用时, 由SDK传给用户
     *
     * 数据类型: (void *)
     */
    AIOT_DYNREGOPT_USERDATA,

    /**
     * @brief dynreg模块接收消息的超时时间
     *
     * @details
     *
     * 数据类型: (uint32_t) 默认值: (5 * 1000) ms
     */
    AIOT_DYNREGOPT_TIMEOUT_MS,

    /**
     * @brief 销毁dynreg实例时, 等待其他api执行完毕的时间
     *
     * @details
     *
     * 当调用@ref aiot_dynreg_deinit 销毁MQTT实例时, 若继续调用其他aiot_dynreg_xxx API, API会返回@ref STATE_USER_INPUT_EXEC_DISABLED 错误
     *
     * 此时, 用户应该停止调用其他aiot_dynreg_xxx API
     *
     * 数据类型: (uint32_t *) 默认值: (2 * 1000) ms
     */
    AIOT_DYNREGOPT_DEINIT_TIMEOUT_MS,
    AIOT_DYNREGOPT_MAX
} aiot_dynreg_option_t;

/**
 * @brief 创建dynreg会话实例, 并以默认值配置会话参数
 *
 * @return void *
 * @retval 非NULL dynreg实例的句柄
 * @retval NULL   初始化失败, 一般是内存分配失败导致
 *
 */
void *aiot_dynreg_init(void);

/**
 * @brief 配置dynreg会话
 *
 * @param[in] handle dynreg会话句柄
 * @param[in] option 配置选项, 更多信息请参考@ref aiot_dynreg_option_t
 * @param[in] data   配置选项数据, 更多信息请参考@ref aiot_dynreg_option_t
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  参数配置失败
 * @retval >=STATE_SUCCESS 参数配置成功
 *
 */
int32_t aiot_dynreg_setopt(void *handle, aiot_dynreg_option_t option, void *data);

/**
 * @brief 结束dynreg会话, 销毁实例并回收资源
 *
 * @param[in] handle 指向dynreg会话句柄的指针
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  执行失败
 * @retval >=STATE_SUCCESS 执行成功
 *
 */
int32_t aiot_dynreg_deinit(void **handle);

/**
 * @brief 向dynreg服务器发送dynreg消息请求
 *
 * @param handle dynreg会话句柄
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  请求发送失败
 * @retval >=STATE_SUCCESS 请求发送成功
 */
int32_t aiot_dynreg_send_request(void *handle);

/**
 * @brief 从网络上收取dynreg消息
 *
 * @param handle dynreg会话句柄
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  数据接收失败
 * @retval >=STATE_SUCCESS 数据接收成功
 */
int32_t aiot_dynreg_recv(void *handle);

#if defined(__cplusplus)
}
#endif

#endif  /* __AIOT_DYNREG_API_H__ */


/**
 * @file aiot_dynregmq_api.h
 * @brief dynregmq模块头文件, 提供了基于MQTT的设备信息动态注册能力
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
#ifndef __AIOT_DYNREGMQ_API_H__
#define __AIOT_DYNREGMQ_API_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief -0x0500~-0x05FF表达SDK在dynregmq模块内的状态码
 */
#define STATE_DYNREGMQ_BASE                                             (-0x0500)

/**
 * @brief 执行@ref aiot_dynregmq_deinit 时, 等待其他API执行结束的超过设定的超时时间, DYNREGMQ实例销毁失败
 */
#define STATE_DYNREGMQ_DEINIT_TIMEOUT                                   (-0x0501)

/**
 * @brief 需要首先执行@ref aiot_dynregmq_send_request 发送动态注册请求
 */
#define STATE_DYNREGMQ_NEED_SEND_REQUEST                                (-0x0502)

/**
 * @brief 接收服务器应答超时
 */
#define STATE_DYNREGMQ_AUTH_TIMEOUT                                     (-0x0503)

/**
 * @brief @ref aiot_dynregmq_setopt 接口的option参数可选值.
 *
 * @details 下文每个选项中的数据类型, 指的是@ref aiot_dynregmq_setopt 中, data参数的数据类型
 *
 * 1. data的数据类型是char *时, 以配置@ref AIOT_DYNREGMQOPT_HOST 为例:
 *
 *    char *host = "xxx";
 *    aiot_dynregmq_setopt(dynregmq_handle, AIOT_DYNREGMQOPT_HOST, host);
 *
 * 2. data的数据类型是其他数据类型时, 以配置@ref AIOT_DYNREGMQOPT_PORT 为例:
 *
 *    uint16_t port = 443;
 *    aiot_mqtt_setopt(dynregmq_handle, AIOT_DYNREGMQOPT_PORT, (void *)&port);
 */
typedef enum {
    /**
     * @brief mqtt动态注册 服务器建联时, 网络使用的安全凭据, 动态注册必需使用TLS方式建连
     *
     * @details
     *
     * 该配置项用于为底层网络配置@ref aiot_sysdep_network_cred_t 安全凭据数据
     *
     * 应当把 @ref aiot_sysdep_network_cred_t 中option配置为@ref AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA , 以tls方式建联
     *
     * 数据类型: (aiot_sysdep_network_cred_t *)
     */
    AIOT_DYNREGMQOPT_NETWORK_CRED,

    /**
     * @brief mqtt动态注册 服务器的域名地址或者ip地址
     *
     * @details
     *
     * 阿里云物联网平台域名地址列表(必须使用自己的product key替换${pk}):
     *
     * 使用tls证书方式建联:
     *
     * | 域名地址                                        | 区域    | 端口号
     * |-------------------------------------------------|---------|---------
     * | ${pk}.iot-as-mqtt.cn-shanghai.aliyuncs.com      | 上海    | 443
     * | ${pk}.iot-as-mqtt.ap-southeast-1.aliyuncs.com   | 新加坡  | 443
     * | ${pk}.iot-as-mqtt.ap-northeast-1.aliyuncs.com   | 日本    | 443
     * | ${pk}.iot-as-mqtt.us-west-1.aliyuncs.com        | 美西    | 443
     * | ${pk}.iot-as-mqtt.eu-central-1.aliyuncs.com     | 德国    | 443
     *
     * 数据类型: (char *)
     */
    AIOT_DYNREGMQOPT_HOST,

    /**
     * @brief mqtt动态注册 服务器的端口号
     *
     * @details
     *
     * 连接阿里云物联网平台 mqtt动态注册 服务器时:
     *
     * 必须使用tls方式建联, 端口号设置为443
     *
     * 数据类型: (uint16_t *)
     */
    AIOT_DYNREGMQOPT_PORT,

    /**
     * @brief 设备的productKey, 可从<a href="http://iot.console.aliyun.com/">阿里云物联网平台控制台</a>获取
     *
     * @details
     *
     * 数据类型: (char *)
     */
    AIOT_DYNREGMQOPT_PRODUCT_KEY,

    /**
     * @brief 设备的productSecret, 可从<a href="http://iot.console.aliyun.com/">阿里云物联网平台控制台</a>获取
     *
     * @details
     *
     * 数据类型: (char *)
     */
    AIOT_DYNREGMQOPT_PRODUCT_SECRET,

    /**
     * @brief 设备的deviceName, 可从<a href="http://iot.console.aliyun.com/">阿里云物联网平台控制台</a>获取
     *
     * @details
     *
     * 数据类型: (char *)
     */
    AIOT_DYNREGMQOPT_DEVICE_NAME,

    /**
     * @brief dynregmq会话发送消息时可消费的最长时间间隔
     *
     * @details
     *
     * 数据类型: (uint32_t) 默认值: (5 * 1000) ms
     */
    AIOT_DYNREGMQOPT_SEND_TIMEOUT_MS,

    /**
     * @brief dynregmq会话接收消息时可消费的最长时间间隔
     *
     * @details
     *
     * 数据类型: (uint32_t) 默认值: (5 * 1000) ms
     */
    AIOT_DYNREGMQOPT_RECV_TIMEOUT_MS,

    /**
     * @brief 设置回调, 它在SDK收到网络报文的时候被调用, 告知用户
     *
     * @details
     *
     * 数据类型: (aiot_dynregmq_http_recv_handler_t)
     */
    AIOT_DYNREGMQOPT_RECV_HANDLER,

    /**
     * @brief 用户需要SDK暂存的上下文
     *
     * @details
     *
     * 这个上下文指针会在 AIOT_DYNREGMQOPT_RECV_HANDLER 和 AIOT_DYNREGMQOPT_EVENT_HANDLER 设置的回调被调用时, 由SDK传给用户
     *
     * 数据类型: (void *)
     */
    AIOT_DYNREGMQOPT_USERDATA,

    /**
     * @brief dynregmq模块接收消息的超时时间
     *
     * @details
     *
     * 数据类型: (uint32_t) 默认值: (5 * 1000) ms
     */
    AIOT_DYNREGMQOPT_TIMEOUT_MS,

    /**
     * @brief 销毁dynregmq实例时, 等待其他api执行完毕的时间
     *
     * @details
     *
     * 当调用@ref aiot_dynregmq_deinit 销毁MQTT实例时, 若继续调用其他aiot_dynregmq_xxx API, API会返回@ref STATE_USER_INPUT_EXEC_DISABLED 错误
     *
     * 此时, 用户应该停止调用其他aiot_dynregmq_xxx API
     *
     * 数据类型: (uint32_t *) 默认值: (2 * 1000) ms
     */
    AIOT_DYNREGMQOPT_DEINIT_TIMEOUT_MS,

    /**
     * @brief 是否使用免白名单功能
     *
     * @details
     *
     *  1. 配置为 0 则为白名单模式, 使用这种模式用户必须提前在控制台录入deviceName, 动态注册完成后服务会返回deviceSecret, 用户可通过
     *  AIOT_DYNREGMQRECV_DEVICEINFO_WL类型数据回调获取到deviceSecret.
     *  2. 配置为 1 则为免白名单模式, 使用这种模式用户无需提前在控制台录入deviceName, 动态注册完成后服务会返回MQTT建连信息, 用户可通过
     *  AIOT_DYNREGMQRECV_DEVICEINFO_NWL类型数据回调获取到clientid, username, password. 用户需要将这三个参数通过
     *  aiot_mqtt_setopt接口以AIOT_MQTTOPT_CLIENTID, AIOT_MQTTOPT_USERNAME, AIOT_MQTTOPT_PASSWORD配置选项
     *  配置到MQTT句柄中。
     *
     * 数据类型: (uint8_t *) 默认值: (0)
     *
     */
    AIOT_DYNREGMQOPT_NO_WHITELIST,

    /**
     * @brief 用户购买的物联网平台实例ID. 当用户使用自购实例, 且使用免白名单方式时, 必须设置实例ID
     *
     * @details
     *
     * 数据类型: (char *)
     *
     */
    AIOT_DYNREGMQOPT_INSTANCE_ID,

    AIOT_DYNREGMQOPT_MAX
} aiot_dynregmq_option_t;


/**
 * @brief dynregmq模块收到从网络上来的报文时, 通知用户的报文类型
 */
typedef enum {
    /**
     * @brief 白名单模式下服务器返回的设备信息
     */
    AIOT_DYNREGMQRECV_DEVICEINFO_WL,

    /**
     * @brief 免白名单模式下服务器返回的设备信息
     */
    AIOT_DYNREGMQRECV_DEVICEINFO_NWL,
} aiot_dynregmq_recv_type_t;

/**
 * @brief dynregmq模块收到从网络上来的报文时, 通知用户的报文内容
 */
typedef struct {
    /**
     * @brief 报文内容所对应的报文类型, 更多信息请参考@ref aiot_dynregmq_recv_type_t
     */
    aiot_dynregmq_recv_type_t  type;
    union {
        /**
         * @brief 白名单模式下服务器返回的设备信息
         */
        struct {
            char *device_secret;
        } deviceinfo_wl;

        /**
         * @brief 免白名单模式下服务器返回的设备信息
         */
        struct {
            char *clientid;
            char *username;
            char *password;
        } deviceinfo_nwl;
    } data;
} aiot_dynregmq_recv_t;

/**
 * @brief dynregmq模块收到从网络上来的报文时, 通知用户所调用的数据回调函数
 *
 * @param[in] handle dynregmq会话句柄
 * @param[in] packet dynregmq消息结构体, 存放收到的dynregmq报文内容
 * @param[in] userdata 用户上下文
 *
 * @return void
 */
typedef void (* aiot_dynregmq_recv_handler_t)(void *handle,
        const aiot_dynregmq_recv_t *packet, void *userdata);


/**
 * @brief 创建dynregmq会话实例, 并以默认值配置会话参数
 *
 * @return void *
 * @retval 非NULL dynregmq实例的句柄
 * @retval NULL   初始化失败, 一般是内存分配失败导致
 *
 */
void *aiot_dynregmq_init(void);

/**
 * @brief 配置dynregmq会话
 *
 * @param[in] handle dynregmq会话句柄
 * @param[in] option 配置选项, 更多信息请参考@ref aiot_dynregmq_option_t
 * @param[in] data   配置选项数据, 更多信息请参考@ref aiot_dynregmq_option_t
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  参数配置失败
 * @retval >=STATE_SUCCESS 参数配置成功
 *
 */
int32_t aiot_dynregmq_setopt(void *handle, aiot_dynregmq_option_t option, void *data);

/**
 * @brief 结束dynregmq会话, 销毁实例并回收资源
 *
 * @param[in] handle 指向dynregmq会话句柄的指针
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  执行失败
 * @retval >=STATE_SUCCESS 执行成功
 *
 */
int32_t aiot_dynregmq_deinit(void **handle);

/**
 * @brief 向dynregmq服务器发送dynregmq消息请求
 *
 * @param handle dynregmq会话句柄
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  请求发送失败
 * @retval >=STATE_SUCCESS 请求发送成功
 */
int32_t aiot_dynregmq_send_request(void *handle);

/**
 * @brief 从网络上收取dynregmq消息
 *
 * @param handle dynregmq会话句柄
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  数据接收失败
 * @retval >=STATE_SUCCESS 数据接收成功
 */
int32_t aiot_dynregmq_recv(void *handle);

#if defined(__cplusplus)
}
#endif

#endif  /* __AIOT_DYNREGMQMQ_API_H__ */


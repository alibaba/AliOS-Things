/**
 * @file aiot_bootstrap_api.h
 * @brief Bootstrap模块头文件, 可用于获取阿里云物联网平台的连接信息
 *
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 *
 */

#ifndef _AIOT_BOOTSTRAP_API_H_
#define _AIOT_BOOTSTRAP_API_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief -0x0700~-0x07FF表达SDK在 bootstrap 模块内的状态码
 */
#define STATE_BOOTSTRAP_BASE                                        (-0x0700)

/**
 * @brief 执行@ref aiot_bootstrap_deinit 时, 等待其他API执行结束的超过设定的超时时间, MQTT实例销毁失败
 */
#define STATE_BOOTSTRAP_DEINIT_TIMEOUT                              (-0x0701)

/**
 * @brief 需要首先执行@ref aiot_bootstrap_send_request 发送bootstrap请求
 */
#define STATE_BOOTSTRAP_NEED_SEND_REQUEST                           (-0x0702)

/**
 * @brief bootstrap 服务端返回了错误的http status code
 */
#define STATE_BOOTSTRAP_INVALID_STATUS_CODE                         (-0x0703)

/**
 * @brief 收到非法的bootstrap连接信息
 */
#define STATE_BOOTSTRAP_INVALID_CONNECTION_INFO                     (-0x0704)

/**
 * @brief bootstrap报文类型
 *
 * @details
 *
 * 传入 @ref aiot_bootstrap_recv_handler_t 的MQTT报文类型
 */
typedef enum {
    /**
     * @brief bootstrap 服务器返回的http status code
     */
    AIOT_BOOTSTRAPRECV_STATUS_CODE,
    /**
     * @brief 物联网平台连接信息, 包含域名和端口号
     */
    AIOT_BOOTSTRAPRECV_CONNECTION_INFO,
    /**
     * @brief bootstrap通知信息
     */
    AIOT_BOOTSTRAPRECV_NOTIFY
} aiot_bootstrap_recv_type_t;

/**
 * @brief bootstrap接收到的数据
 */
typedef struct {
    /**
     * @brief bootstrap报文类型, 更多信息请参考@ref aiot_bootstrap_recv_type_t
     */
    aiot_bootstrap_recv_type_t type;
    /**
     * @brief bootstrap报文联合体, 内容根据type进行选择
     */
    union {
        /**
         * @brief bootstrap 服务器返回的http status code
         */
        struct {
            uint32_t code;
        } status_code;
        /**
         * @brief 物联网平台连接信息
         */
        struct {
            char *host;
            uint16_t port;
        } connection_info;
        struct {
            uint32_t cmd;
        } notify;
    } data;
} aiot_bootstrap_recv_t;

/**
 * @brief bootstrap数据接收回调函数原型
 *
 * @param[in] handle bootstrap实例句柄
 * @param[in] packet bootstrap数据结构体, 存放收到的bootstrap信息
 * @param[in] userdata 用户上下文
 *
 * @return void
 */
typedef void (*aiot_bootstrap_recv_handler_t)(void *handle, const aiot_bootstrap_recv_t *packet, void *userdata);

/**
 * @brief bootstrap内部事件类型
 */
typedef enum {
    /**
     * @brief 收到的bootstrap应答中字段不合法
     */
    AIOT_BOOTSTRAPEVT_INVALID_RESPONSE,
    /**
     * @brief 收到的bootstrap应答中cmd字段格式错误
     */
    AIOT_BOOTSTRAPEVT_INVALID_CMD,
} aiot_bootstrap_event_type_t;

/**
 * @brief bootstrap内部事件
 */
typedef struct {
    /**
     * @brief bootstrap内部事件类型. 更多信息请参考@ref aiot_bootstrap_event_type_t
     *
     */
    aiot_bootstrap_event_type_t type;
} aiot_bootstrap_event_t;

/**
 * @brief bootstrap事件回调函数
 *
 * @details
 *
 * 当NTP内部事件被触发时, 调用此函数
 *
 */
typedef void (*aiot_bootstrap_event_handler_t)(void *handle, const aiot_bootstrap_event_t *event, void *userdata);

typedef enum {
    /**
     * @brief bootstrap会话需要的MQTT句柄, 需要先建立MQTT连接, 再设置MQTT句柄
     *
     * @details
     *
     * 数据类型: (void *)
     */
    AIOT_BOOTSTRAPOPT_MQTT_HANDLE,

    /**
     * @brief bootstrap 服务器建联时, 网络使用的安全凭据
     *
     * @details
     *
     * 该配置项用于为底层网络配置@ref aiot_sysdep_network_cred_t 安全凭据数据
     *
     * 应当把 @ref aiot_sysdep_network_cred_t 中option配置为@ref AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA , 以tls方式建联
     *
     * 数据类型: (aiot_sysdep_network_cred_t *)
     */
    AIOT_BOOTSTRAPOPT_NETWORK_CRED,

    /**
     * @brief bootstrap 服务器的域名地址或者ip地址
     *
     * @details
     *
     * 阿里云物联网平台 bootstrap 服务器域名地址列表:
     *
     * | 域名地址                                        | 区域    | 端口号
     * |-------------------------------------------------|---------|---------
     * | iot-auth-global.aliyuncs.com                    | 全球    | 443
     *
     * 数据类型: (char *) 默认值: iot-auth-global.aliyuncs.com
     */
    AIOT_BOOTSTRAPOPT_HOST,

    /**
     * @brief bootstrap 服务器的端口号
     *
     * @details
     *
     * 连接阿里云物联网平台 bootstrap 服务器时:
     *
     * 必须使用tls方式建联, 端口号设置为443
     *
     * 数据类型: (uint16_t *) 默认值: 443
     */
    AIOT_BOOTSTRAPOPT_PORT,

    /**
     * @brief 设备的product key, 可从<a href="http://iot.console.aliyun.com/">阿里云物联网平台控制台</a>获取
     *
     * @details
     *
     * 数据类型: (char *)
     */
    AIOT_BOOTSTRAPOPT_PRODUCT_KEY,

    /**
     * @brief 设备的device name, 可从<a href="http://iot.console.aliyun.com/">阿里云物联网平台控制台</a>获取
     *
     * @details
     *
     * 数据类型: (char *)
     */
    AIOT_BOOTSTRAPOPT_DEVICE_NAME,

    /**
     * @brief 配置设备所在的regionId
     * 
     * @details
     * 
     * |          地域名称         | RegionId |        地域名称        | RegionId |
     * |-------------------------|-----------|----------------------|----------|
     * |         华东2-上海        |    0     |    亚太东南 1-新加坡    |    1     |
     * |     亚太东北 1-日本东京    |    2     |   美国东部 1-弗吉尼亚    |    3    |
     * |   欧洲中部 1-德国法兰克福   |    4     |     美国西部 1-硅谷     |    5    |
     * |        华东 1-杭州        |    6     |       华南 1-深圳      |    7    |
     * |        华北 2-北京        |    8     |       华北 1-青岛      |    9    |
     * |       华北 3-张家口       |    10     |      华北 5-呼和浩特    |   11    |
     * |        华南 2-河源        |    12     |       西南 1-成都      |   13    |
     * |           香港           |    14     |  亚太东南 2-澳大利亚悉尼 |   15    |
     * |   亚太东南 3-马来西亚吉隆坡 |    16     |   亚太东南 5-印尼雅加达  |   17    |
     * |     亚太南部 1-印度孟买    |    18     |        英国-伦敦       |   19    |
     * |    中东东部 1-阿联酋迪拜   |    20     |                       |         |
     * 
     * 数据类型: (char *)
     */
    AIOT_BOOTSTRAPOPT_REGIONID,

    /**
     * @brief bootstrap会话发送消息时可消费的最长时间间隔
     *
     * @details
     *
     * 数据类型: (uint32_t *) 默认值: (5 * 1000) ms
     */
    AIOT_BOOTSTRAPOPT_SEND_TIMEOUT_MS,

    /**
     * @brief bootstrap会话接收消息时可消费的最长时间间隔
     *
     * @details
     *
     * 数据类型: (uint32_t *) 默认值: (5 * 1000) ms
     */
    AIOT_BOOTSTRAPOPT_RECV_TIMEOUT_MS,

    /**
     * @brief 从 bootstrap 服务器收取的数据从此回调函数进行通知
     *
     * @details
     *
     *
     * 数据类型: (aiot_bootstrap_recv_handler_t)
     */
    AIOT_BOOTSTRAPOPT_RECV_HANDLER,

    /**
     * @brief bootstrap内部发生的事件会从此回调函数进行通知
     *
     * @details
     *
     * 数据类型: ( @ref aiot_bootstrap_event_handler_t )
     */
    AIOT_BOOTSTRAPOPT_EVENT_HANDLER,

    /**
     * @brief 用户需要SDK暂存的上下文
     *
     * @details
     *
     * 该上下文会在 @ref AIOT_BOOTSTRAPOPT_RECV_HANDLER 注册的回调函数中传回给用户
     *
     * 数据类型: (void *)
     */
    AIOT_BOOTSTRAPOPT_USERDATA,

    /**
     * @brief 等待bootstrap应答的超时时间
     *
     * @details
     *
     * 数据类型: (uint32_t *) 默认值: (5 * 1000) ms
     */
    AIOT_BOOTSTRAPOPT_TIMEOUT_MS,

    /**
     * @brief 销毁Bootstrap实例时, 等待其他api执行完毕的时间
     *
     * @details
     *
     * 当调用@ref aiot_bootstrap_deinit 销毁MQTT实例时, 若继续调用其他aiot_bootstrap_xxx API, API会返回@ref STATE_USER_INPUT_EXEC_DISABLED 错误
     *
     * 此时, 用户应该停止调用其他aiot_bootstrap_xxx API
     *
     * 数据类型: (uint32_t *) 默认值: (2 * 1000) ms
     */
    AIOT_BOOTSTRAPOPT_DEINIT_TIMEOUT_MS,
    AIOT_BOOTSTRAPOPT_MAX
} aiot_bootstrap_option_t;

/**
 * @brief 初始化bootstrap实例并设置默认参数
 *
 * @return void*
 * @retval 非NULL bootstrap实例句柄
 * @retval NULL 初始化失败, 一般是内存分配失败导致
 *
 */
void *aiot_bootstrap_init(void);

/**
 * @brief 设置bootstrap参数
 *
 * @param[in] handle bootstrap句柄
 * @param[in] option 配置选项, 更多信息请参考@ref aiot_bootstrap_option_t
 * @param[in] data   配置选项数据, 更多信息请参考@ref aiot_bootstrap_option_t
 *
 * @return int32_t
 * @retval <STATE_SUCCESS 参数设置失败
 * @retval >=STATE_SUCCESS 参数设置成功
 *
 */
int32_t aiot_bootstrap_setopt(void *handle, aiot_bootstrap_option_t option, void *data);

/**
 * @brief 释放bootstrap实例句柄的资源
 *
 * @param[in] handle 指向bootstrap实例句柄的指针
 *
 * @return int32_t
 * @retval <STATE_SUCCESS 执行失败
 * @retval >=STATE_SUCCESS 执行成功
 *
 */
int32_t aiot_bootstrap_deinit(void **handle);

/**
 * @brief 向bootstrap服务器发送请求
 *
 * @param handle bootstrap句柄
 *
 * @return int32_t
 * @retval <STATE_SUCCESS 请求发送失败
 * @retval >=STATE_SUCCESS 请求发送成功
 */
int32_t aiot_bootstrap_send_request(void *handle);

/**
 * @brief 从bootstrap服务器接收应答
 *
 * @param handle bootstrap句柄
 *
 * @return int32_t
 * @retval <STATE_SUCCESS 数据接收失败
 * @retval >=STATE_SUCCESS 数据接收成功
 */
int32_t aiot_bootstrap_recv(void *handle);

#if defined(__cplusplus)
}
#endif

#endif


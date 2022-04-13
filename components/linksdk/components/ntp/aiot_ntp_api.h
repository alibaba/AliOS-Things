/**
 * @file aiot_ntp_api.h
 * @brief ntp模块头文件, 提供获取utc时间的能力
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 * @details
 *
 * NTP模块用于从阿里云物联网平台上获取UTC时间, API的使用流程如下:
 *
 * 1. 首先参考 @ref aiot_mqtt_api.h 的说明, 保证成功建立与物联网平台的`MQTT`连接
 *
 * 2. 调用 @ref aiot_ntp_init 初始化ntp会话, 获取会话句柄
 *
 * 3. 调用 @ref aiot_ntp_setopt 配置NTP会话的参数, 常用配置项见 @ref aiot_ntp_setopt 的说明
 *
 * 4. 调用 @ref aiot_ntp_send_request 发送NTP请求
 *
 * 5. 收到的UTC时间经SDK处理后会调用由 @ref aiot_ntp_setopt 配置的 @ref AIOT_NTPOPT_RECV_HANDLER 回调函数, 通知用户当前的时间
 *
 */
#ifndef __AIOT_NTP_API_H__
#define __AIOT_NTP_API_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief -0x1100~-0x11FF表达SDK在ntp模块内的状态码
 */
#define STATE_NTP_BASE                                             (-0x1100)

/**
 * @brief MQTT会话句柄未设置, 请通过 @ref aiot_ntp_setopt 设置MQTT会话句柄
 */
#define STATE_NTP_MISSING_MQTT_HANDLE                              (-0x1101)

/**
 * @brief ntp模块收到从网络上来的报文时, 通知用户的报文类型
 */
typedef enum {
    AIOT_NTPRECV_LOCAL_TIME
} aiot_ntp_recv_type_t;

/**
 * @brief ntp模块收到从网络上来的报文时, 通知用户的报文内容
 */
typedef struct {
    /**
     * @brief 报文内容所对应的报文类型, 更多信息请参考@ref aiot_ntp_recv_type_t
     */
    aiot_ntp_recv_type_t  type;
    union {
        /**
         * @brief utc事件戳以及时区换算后的日期, 以 @ref AIOT_NTPOPT_TIME_ZONE 设置的时区为准
         */
        struct {
            uint64_t timestamp;
            uint32_t year;
            uint32_t mon;
            uint32_t day;
            uint32_t hour;
            uint32_t min;
            uint32_t sec;
            uint32_t msec;
        } local_time;
    } data;
} aiot_ntp_recv_t;

/**
 * @brief ntp模块收到从网络上来的报文时, 通知用户所调用的数据回调函数
 *
 * @param[in] handle ntp会话句柄
 * @param[in] packet ntp消息结构体, 存放收到的ntp报文内容
 * @param[in] userdata 用户上下文
 *
 * @return void
 */
typedef void (* aiot_ntp_recv_handler_t)(void *handle,
        const aiot_ntp_recv_t *packet, void *userdata);

/**
 * @brief ntp内部事件类型
 */
typedef enum {
    /**
     * @brief 收到的ntp应答中字段不合法
     */
    AIOT_NTPEVT_INVALID_RESPONSE,
    /**
     * @brief 收到的ntp应答中时间字段格式错误
     */
    AIOT_NTPEVT_INVALID_TIME_FORMAT,
} aiot_ntp_event_type_t;

/**
 * @brief NTP内部事件
 */
typedef struct {
    /**
     * @brief NTP内部事件类型. 更多信息请参考@ref aiot_ntp_event_type_t
     *
     */
    aiot_ntp_event_type_t type;
} aiot_ntp_event_t;

/**
 * @brief ntp事件回调函数
 *
 * @details
 *
 * 当NTP内部事件被触发时, 调用此函数
 *
 */
typedef void (*aiot_ntp_event_handler_t)(void *handle, const aiot_ntp_event_t *event, void *userdata);

/**
 * @brief @ref aiot_ntp_setopt 接口的option参数可选值.
 *
 * @details 下文每个选项中的数据类型, 指的是@ref aiot_ntp_setopt 中, data参数的数据类型
 *
 * 1. data的数据类型是char *时, 以配置@ref AIOT_NTPOPT_MQTT_HANDLE 为例:
 *
 *    void *mqtt_handle = aiot_mqtt_init();
 *    aiot_ntp_setopt(ntp_handle, AIOT_NTPOPT_MQTT_HANDLE, mqtt_handle);
 *
 * 2. data的数据类型是其他数据类型时, 以配置@ref AIOT_NTPOPT_TIME_ZONE 为例:
 *
 *    int8_t time_zone = 8;
 *    aiot_mqtt_setopt(ntp_handle, AIOT_NTPOPT_TIME_ZONE, (void *)&time_zone);
 */
typedef enum {
    /**
     * @brief ntp会话 需要的MQTT句柄, 需要先建立MQTT连接, 再设置MQTT句柄
     *
     * @details
     *
     * 数据类型: (void *)
     */
    AIOT_NTPOPT_MQTT_HANDLE,

    /**
     * @brief ntp会话 获取到utc时间后会根据此时区值转换成本地时间, 再通过 @ref aiot_ntp_recv_handler_t 通知
     *
     * @details
     *
     * 取值示例: 东8区, 取值为8; 西3区, 取值为-3
     *
     * 数据类型: (int8_t *)
     */
    AIOT_NTPOPT_TIME_ZONE,

    /**
     * @brief 设置回调, 它在SDK收到网络报文的时候被调用, 告知用户
     * 
     * @details
     * 
     * 数据类型: ( @ref aiot_ntp_recv_handler_t )
     */
    AIOT_NTPOPT_RECV_HANDLER,

    /**
     * @brief ntp内部发生的事件会从此回调函数进行通知
     *
     * @details
     *
     * 数据类型: ( @ref aiot_ntp_event_handler_t )
     */
    AIOT_NTPOPT_EVENT_HANDLER,

    /**
     * @brief 用户需要SDK暂存的上下文
     *
     * @details 这个上下文指针会在 AIOT_NTPOPT_RECV_HANDLER 和 AIOT_NTPOPT_EVENT_HANDLER 设置的回调被调用时, 由SDK传给用户
     * 
     * 数据类型: (void *)
     */
    AIOT_NTPOPT_USERDATA,

    /**
     * @brief 销毁ntp实例时, 等待其他api执行完毕的时间
     *
     * @details
     *
     * 当调用@ref aiot_ntp_deinit 销毁NTP实例时, 若继续调用其他aiot_ntp_xxx API, API会返回@ref STATE_USER_INPUT_EXEC_DISABLED 错误
     *
     * 此时, 用户应该停止调用其他aiot_ntp_xxx API
     *
     * 数据类型: (uint32_t *) 默认值: (2 * 1000) ms
     */
    AIOT_NTPOPT_DEINIT_TIMEOUT_MS,
    AIOT_NTPOPT_MAX
} aiot_ntp_option_t;

/**
 * @brief 创建ntp会话实例, 并以默认值配置会话参数
 *
 * @return void *
 * @retval 非NULL ntp实例的句柄
 * @retval NULL   初始化失败, 一般是内存分配失败导致
 *
 */
void *aiot_ntp_init(void);

/**
 * @brief 配置ntp会话
 *
 * @details
 *
 * 常见的配置项如下
 *
 * + `AIOT_NTPOPT_MQTT_HANDLE`: 已建立连接的MQTT会话句柄
 *
 * + `AIOT_NTPOPT_TIME_ZONE`: 时区设置, SDK会将收到的UTC时间按配置的时区进行转换
 *
 * + `AIOT_NTPOPT_RECV_HANDLER`: 时间数据接收回调函数, SDK将UTC时间转换完成后, 通过此回调函数输出
 *
 * @param[in] handle ntp会话句柄
 * @param[in] option 配置选项, 更多信息请参考@ref aiot_ntp_option_t
 * @param[in] data   配置选项数据, 更多信息请参考@ref aiot_ntp_option_t
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  参数配置失败
 * @retval >=STATE_SUCCESS 参数配置成功
 *
 */
int32_t aiot_ntp_setopt(void *handle, aiot_ntp_option_t option, void *data);

/**
 * @brief 结束ntp会话, 销毁实例并回收资源
 *
 * @param[in] handle 指向ntp会话句柄的指针
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  执行失败
 * @retval >=STATE_SUCCESS 执行成功
 *
 */
int32_t aiot_ntp_deinit(void **handle);

/**
 * @brief 向ntp服务器发送ntp消息请求
 *
 * @details
 *
 * 发送NTP请求, 然后SDK会调用通过 @ref aiot_ntp_setopt 配置的 @ref AIOT_NTPOPT_RECV_HANDLER 回调函数, 通知用户当前的时间
 *
 * @param handle ntp会话句柄
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  请求发送失败
 * @retval >=STATE_SUCCESS 请求发送成功
 */
int32_t aiot_ntp_send_request(void *handle);

#if defined(__cplusplus)
}
#endif

#endif  /* __AIOT_NTP_API_H__ */


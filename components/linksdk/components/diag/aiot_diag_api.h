/**
 * @file aiot_diag_api.h
 * @brief diag模块头文件, 提供诊断SDK的能力
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
#ifndef __AIOT_DIAG_API_H__
#define __AIOT_DIAG_API_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief -0x0.00~-0x0.FF表达SDK在diag模块内的状态码
 */
#define STATE_DIAG_BASE                                             (-0x1400)

/**
 * @brief STATE_DIAG_CODE1的说明
 */
#define STATE_DIAG_LOG_UNKNOWN_STATE_CODE_BASE                      (-0x1401)

/**
 * @brief STATE_DIAG_CODE2的说明
 */
#define STATE_DIAG_CODE2                                            (-0x1402)

/**
 * @brief diag模块收到从网络上来的报文时, 通知用户的报文类型
 */
typedef enum {
    AIOT_DIAGRECV_DIAG_CONTROL
} aiot_diag_recv_type_t;

/**
 * @brief diag模块收到从网络上来的报文时, 通知用户的报文内容
 */
typedef struct {
    /**
     * @brief 报文内容所对应的报文类型, 更多信息请参考@ref aiot_diag_recv_type_t
     */
    aiot_diag_recv_type_t  type;
    union {
        /**
         * @brief 收到的云端控制指令
         */
        struct {
            /**
             * @brief 0: 关闭诊断功能，1: 打开诊断功能
             */
            uint32_t data;
        } diag_control;
    } data;
} aiot_diag_recv_t;

/**
 * @brief diag模块收到从网络上来的报文时, 通知用户所调用的数据回调函数
 *
 * @param[in] handle diag会话句柄
 * @param[in] packet diag消息结构体, 存放收到的diag报文内容
 * @param[in] userdata 用户上下文
 *
 * @return void
 */
typedef void (* aiot_diag_recv_handler_t)(void *handle, const aiot_diag_recv_t *packet, void *userdata);

/**
 * @brief diag模块内部发生值得用户关注的状态变化时, 通知用户的事件类型
 */
typedef enum {
    /**
     * @brief 诊断模块产生的告警信息
     */
    AIOT_DIAGEVT_ALERT
} aiot_diag_event_type_t;

/**
 * @brief diag模块内部发生值得用户关注的状态变化时, 通知用户的事件内容
 */
typedef struct {
    /**
     * @brief 事件内容所对应的事件类型, 更多信息请参考@ref aiot_diag_event_type_t
     */
    aiot_diag_event_type_t  type;
    union {
        struct {
            /**
             * @brief 告警模块名
             */
            char *module_name;
            /** 
             * @brief 告警级别
             */
            char *level;
            /**
             * @brief 告警信息描述字符串
             */
            char *desc;
        } alert;
    } data;
} aiot_diag_event_t;

/**
 * @brief diag模块内部发生值得用户关注的状态变化时, 通知用户所调用的事件回调函数
 *
 * @param[in] handle, diag会话句柄
 * @param[in] event, diag模块中发生的事件的内容
 * @param[in] userdata, 用户上下文
 *
 * @return void
 */
typedef void (*aiot_diag_event_handler_t)(void *handle, const aiot_diag_event_t *event, void *userdata);

/**
 * @brief 诊断项的配置参数
 */
typedef struct {
    /**
     * @brief 对当前诊断项是否进行诊断的开关
     * 
     * @details
     * 
     * 0: 关闭对当前诊断项的诊断, 1: 打开对当前诊断项的诊断
     */
    uint8_t enabled;
    /**
     * @brief 对当前诊断项连续两次告警的最小时间间隔
     */
    uint32_t interval_ms;
    /**
     * @brief warning级别告警的阈值
     */
    int64_t warning_threashold;
    /**
     * @brief fatal级别告警的阈值
     */
    int64_t fatal_threshold;
} aiot_diag_config_t;

/**
 * @brief @ref aiot_diag_setopt 接口的option参数可选值.
 *
 * @details 下文每个选项中的数据类型, 指的是@ref aiot_diag_setopt 中, data参数的数据类型
 *
 * 1. data的数据类型是void *时, 以配置@ref AIOT_DIAGOPT_MQTT_HANDLE 为例:
 *
 *    void *mqtt_handle = aiot_mqtt_init();
 *    aiot_diag_setopt(diag_handle, AIOT_DIAGOPT_MQTT_HANDLE, mqtt_handle);
 *
 * 2. data的数据类型是其他数据类型时, 以配置@ref AIOT_DIAGOPT_LOCAL_REPORT_ENABLED 为例:
 *
 *    uint8_t local_report_enabled = 1;
 *    aiot_mqtt_setopt(diag_handle, AIOT_DIAGOPT_LOCAL_REPORT_ENABLED, (void *)&local_report_enabled);
 */
typedef enum {
    /**
     * @brief diag会话 需要的MQTT句柄, 需要先建立MQTT连接, 再设置MQTT句柄
     *
     * @details
     *
     * 数据类型: (void *)
     */
    AIOT_DIAGOPT_MQTT_HANDLE,

    /**
     * @brief 是否需要从事件回调函数中输出告警信息
     * 
     * @details
     * 
     * 0: 不从事件回调函数中输出告警信息, 1: 从事件回调函数中输出告警信息
     * 
     * 数据类型: (uint8_t *)
     */
    AIOT_DIAGOPT_LOCAL_REPORT_ENABLED,

    /**
     * @brief 是否需要上报告警信息至云端
     * 
     * @details
     * 
     * 0: 不上报告警信息至云端, 1: 上报告警信息至云端
     * 
     * 数据类型: (uint8_t *)
     */
    AIOT_DIAGOPT_CLOUD_REPORT_ENABLED,

    /**
     * @brief MQTT建联时长告警配置
     * 
     * @details
     * 
     * 数据类型: ( @ref aiot_diag_config_t )
     */
    AIOT_DIAGOPT_MQTT_CONNECTION,

    /**
     * @brief MQTT心跳丢失告警配置
     * 
     * @details
     * 
     * 数据类型: ( @ref aiot_diag_config_t )
     */
    AIOT_DIAGOPT_MQTT_HEARTBEAT,

    /**
     * @brief Alink协议上行报文的回复速度告警配置
     * 
     * @details
     * 
     * 数据类型: ( @ref aiot_diag_config_t )
     */
    AIOT_DIAGOPT_ALINK_UPLINK,

    /**
     * @brief 设置回调, 它在SDK收到网络报文的时候被调用, 告知用户
     * 
     * @details
     * 
     * 数据类型: ( @ref aiot_diag_recv_handler_t )
     */
    AIOT_DIAGOPT_RECV_HANDLER,

    /**
     * @brief diag内部发生的事件会从此回调函数进行通知
     *
     * @details
     *
     * 数据类型: ( @ref aiot_diag_event_handler_t )
     */
    AIOT_DIAGOPT_EVENT_HANDLER,

    /**
     * @brief 用户需要SDK暂存的上下文
     *
     * @details 这个上下文指针会在 AIOT_DIAGOPT_RECV_HANDLER 和 AIOT_DIAGOPT_EVENT_HANDLER 设置的回调被调用时, 由SDK传给用户
     * 
     * 数据类型: (void *)
     */
    AIOT_DIAGOPT_USERDATA,
    AIOT_DIAGOPT_MAX
} aiot_diag_option_t;

/**
 * @brief 创建diag会话实例, 并以默认值配置会话参数
 *
 * @return void *
 * @retval 非NULL diag实例的句柄
 * @retval NULL   初始化失败, 一般是内存分配失败导致
 *
 */
void *aiot_diag_init(void);

/**
 * @brief 配置diag会话
 *
 * @param[in] handle diag会话句柄
 * @param[in] option 配置选项, 更多信息请参考@ref aiot_diag_option_t
 * @param[in] data   配置选项数据, 更多信息请参考@ref aiot_diag_option_t
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  参数配置失败
 * @retval >=STATE_SUCCESS 参数配置成功
 *
 */
int32_t aiot_diag_setopt(void *handle, aiot_diag_option_t option, void *data);

/**
 * @brief 结束diag会话, 销毁实例并回收资源
 *
 * @param[in] handle 指向diag会话句柄的指针
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  执行失败
 * @retval >=STATE_SUCCESS 执行成功
 *
 */
int32_t aiot_diag_deinit(void **handle);

/**
 * @brief 开始诊断SDK内部信息
 *
 * @param handle diag会话句柄
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  请求发送失败
 * @retval >=STATE_SUCCESS 请求发送成功
 */
int32_t aiot_diag_start(void *handle);

/**
 * @brief 停止诊断SDK内部信息
 *
 * @param handle diag会话句柄
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  数据接收失败
 * @retval >=STATE_SUCCESS 数据接收成功
 */
int32_t aiot_diag_stop(void *handle);

#if defined(__cplusplus)
}
#endif

#endif  /* __AIOT_DIAG_API_H__ */


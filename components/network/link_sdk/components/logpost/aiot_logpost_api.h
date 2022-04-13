/**
 * @file aiot_logpost_api.h
 * @brief logpost模块头文件, 提供设备端日志上云的能力
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 * @details
 *
 * 1. 在使用日志上云模块前, 用户应首先创建好一个MQTT实例
 *
 * 2. 调用`aiot_logpost_init`创建一个日志上云实例, 保存实例句柄
 *
 * 3. 调用`aiot_logpost_setopt`配置`AIOT_LOGPOSTOPT_MQTT_HANDLE`选项以设置MQTT句柄, 此选项为强制配置选项
 *
 * 4. 调用`aiot_logpost_setopt`配置`AIOT_LOGPOSTOPT_EVENT_HANDLER`和`AIOT_LOGPOSTOPT_USER_DATA`选项以注册事件接收回调函数和用户上下文数据指针
 *
 * 5. 在使用`aiot_logpost_send`发送日志消息前, 应先完成MQTT实例的建连
 *
 */
#ifndef __AIOT_LOGPOST_API_H__
#define __AIOT_LOGPOST_API_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief -0x1500~-0x15FF表达SDK在logpost模块内的状态码
 */
#define STATE_LOGPOST_BASE                                              (-0x1500)

/**
 * @brief 用户未调用@ref aiot_logpost_setopt 配置MQTT句柄
 */
#define STATE_LOGPOST_MQTT_HANDLE_IS_NULL                               (-0x1501)

/**
 * @brief 日志上报被云端配置为关闭状态
 */
#define STATE_LOGPOST_POST_TURN_OFF                                     (-0x1502)

/**
 * @brief 日志消息的日志级别有误
 */
#define STATE_LOGPOST_LOGLEVEL_ERROR                                    (-0x1503)

/**
 * @brief 日志消息的模块名称字段为NULL
 */
#define STATE_LOGPOST_LOG_MODULE_NAME_IS_NULL                           (-0x1504)

/**
 * @brief 日志消息的日志内容字段为NULL
 */
#define STATE_LOGPOST_LOG_CONTENT_IS_NULL                               (-0x1505)

/**
 * @brief 日志消息的日志内容字段字符串长度大于4096个字节
 */
#define STATE_LOGPOST_LOG_CONTENT_TOO_LONG                              (-0x1506)

/**
 * @brief 接收到服务器下行消息时的内部日志状态码
 */
#define STATE_LOGPOST_LOG_RECV                                          (-0x1507)

/**
 * @brief 解析服务器下行消息失败时的内部日志状态码
 */
#define SATAE_LOGPOST_LOG_PARSE_MSG_FAILED                              (-0x1508)


/**
 * @brief @ref aiot_logpost_setopt 接口的option参数可选值.
 *
 * @details 下文每个选项中的数据类型, 指的是@ref aiot_logpost_setopt 中, data参数的数据类型
 */
typedef enum {
    /**
     * @brief 模块依赖的MQTT句柄
     *
     * @details
     *
     * LOGPOST模块依赖底层的MQTT模块, 用户必需配置正确的MQTT句柄, 否则无法正常工作, 数据类型为(void *)
     */
    AIOT_LOGPOSTOPT_MQTT_HANDLE,

    /**
     * @brief 设置回调, 它在SDK收到网络报文的时候被调用, 告知用户, 数据类型为(aiot_logpost_recv_handler_t)
     */
    AIOT_LOGPOSTOPT_EVENT_HANDLER,

    /**
     * @brief 用户需要SDK暂存的上下文, 数据类型为(void *)
     *
     * @details 这个上下文指针会在 AIOT_LOGPOSTOPT_RECV_HANDLER 设置的回调被调用时, 由SDK传给用户
     */
    AIOT_LOGPOSTOPT_USERDATA,

    /**
     * @brief 配置选项数量最大值, 不可用作配置参数
     */
    AIOT_LOGPOSTOPT_MAX,
} aiot_logpost_option_t;


/**
 * @brief 日志级别枚举类型定义
 */
typedef enum {
    AIOT_LOGPOST_LEVEL_FATAL,
    AIOT_LOGPOST_LEVEL_ERR,
    AIOT_LOGPOST_LEVEL_WARN,
    AIOT_LOGPOST_LEVEL_INFO,
    AIOT_LOGPOST_LEVEL_DEBUG,
} aiot_logpost_level_t;

/**
 * @brief 日志数据结构体定义
 *
 */
typedef struct {
    /**
     * @brief utc时间戳, 单位为ms, 此数值会直接展示在云端控制台设备日志页面
     */
    uint64_t timestamp;

    /**
     * @brief 日志级别, 请查看@ref aiot_logpost_level_t 定义
     */
    aiot_logpost_level_t loglevel;

    /**
     * @brief 模块名称, <b>必须为以结束符'\0'结尾的字符串</b>
     */
    char *module_name;

    /**
     * @brief 状态码, 可用于标识日志对应的状态
     */
    int32_t code;

    /**
     * @brief 消息标示符, 用于标识云端下行消息, 可从data-module模块的消息接收回调函数中获得对应的标识符, 如果用户设置为0, 此字段将不上传。
     */
    uint64_t msg_id;

    /**
     * @brief 日志内容, <b>必须为以结束符'\0'结尾的字符串</b>
     */
    char *content;
} aiot_logpost_msg_t;


/**
 * @brief logpost模块内部发生值得用户关注的状态变化时, 通知用户的事件类型
 */
typedef enum {
    /**
     * @brief 接受到云端下发的日志配置数据
     */
    AIOT_LOGPOSTEVT_CONFIG_DATA,
} aiot_logpost_event_type_t;

/**
 * @brief logpost模块内部发生值得用户关注的状态变化时, 通知用户的事件内容
 */
typedef struct {
    /**
     * @brief 事件内容所对应的事件类型, 更多信息请参考@ref aiot_logpost_event_type_t
     */
    aiot_logpost_event_type_t  type;

    union {
        /**
         * @brief 日志配置数据结构体
         */
        struct {
            /**
             * @brief 日志开关状态, 0: 关闭日志上传; 1: 打开日志上传
             */
            uint8_t on_off;
        } config_data;
    } data;
} aiot_logpost_event_t;


/**
 * @brief logpost模块内部发生值得用户关注的状态变化时, 通知用户所调用的事件回调函数
 *
 * @param[in] handle logpost会话句柄
 * @param[in] event logpost模块中发生的事件的内容
 * @param[in] userdata 指向用户上下文数据的指针, 这个指针由用户通过调用@ref aiot_logpost_setopt 配置@ref AIOT_LOGPOSTOPT_USERDATA 选项设置
 *
 * @return void
 */
typedef void (*aiot_logpost_event_handler_t)(void *handle,
        const aiot_logpost_event_t *event, void *userdata);

/**
 * @brief 创建logpost会话实例, 并以默认值配置会话参数
 *
 * @return void *
 * @retval 非NULL logpost实例的句柄
 * @retval NULL   初始化失败, 一般是内存分配失败导致
 */
void *aiot_logpost_init(void);

/**
 * @brief 配置logpost会话
 *
 * @param[in] handle logpost会话句柄
 * @param[in] option 配置选项, 更多信息请参考@ref aiot_logpost_option_t
 * @param[in] data   配置选项数据, 更多信息请参考@ref aiot_logpost_option_t
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  参数配置失败
 * @retval STATE_SUCCESS 参数配置成功
 * @retval STATE_USER_INPUT_NULL_POINTER 入参handle或data为NULL
 * @retval STATE_USER_INPUT_OUT_RANGE 入参optioin的枚举值>=AIOT_LOGPOSTOPT_MAX
 * @retval others 参考@ref aiot_state_api.h
 */
int32_t aiot_logpost_setopt(void *handle, aiot_logpost_option_t option, void *data);

/**
 * @brief 向服务器发送日志消息
 *
 * @param[in] handle logpost会话句柄
 * @param[in] msg    消息结构体, 可指定日志对应模块, 日志级别等, 更多信息请参考@ref aiot_logpost_msg_t
 *
 * @return int32_t
 * @retval STATE_SUCCESS 请求发送成功
 * @retval STATE_USER_INPUT_NULL_POINTER 入参<i>handle</i>或<i>msg</i>为NULL
 * @retval STATE_SYS_DEPEND_MALLOC_FAILED 内存分配失败
 * @retval STATE_LOGPOST_MQTT_HANDLE_IS_NULL 用户未调用@ref aiot_logpost_setopt 配置MQTT句柄
 * @retval others 参考@ref aiot_state_api.h 或@ref STATE_SHADOW_BASE 中对应的错误码说明
 *
 */
int32_t aiot_logpost_send(void *handle, aiot_logpost_msg_t *msg);

/**
 * @brief 结束logpost会话, 销毁实例并回收资源
 *
 * @param[in] handle 指向logpost会话句柄的指针
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  执行失败
 * @retval >=STATE_SUCCESS 执行成功
 */
int32_t aiot_logpost_deinit(void **handle);


#if defined(__cplusplus)
}
#endif

#endif  /* __AIOT_LOGPOST_API_H__ */


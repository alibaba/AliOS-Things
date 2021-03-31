/**
 * @file aiot_dm_api.h
 * @brief 数据模型模块头文件, 提供了物模型数据格式的上云能力, 包括属性, 事件, 服务和物模型二进制格式的数据上下行能力
 * @date 2020-01-20
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 * @details
 *
 * 请按照以下流程使用API
 *
 * 1. 在使用物模型模块前, 用户应首先创建好一个MQTT实例
 *
 * 2. 调用`aiot_dm_init`创建一个物模型实例, 保存实例句柄
 *
 * 3. 调用`aiot_dm_setopt`配置`AIOT_DMOPT_MQTT_HANDLE`选项以设置MQTT句柄, 此选项为强制配置选项
 *
 * 4. 调用`aiot_dm_setopt`配置`AIOT_DMOPT_RECV_HANDLER`和`AIOT_DMOPT_USERDATA`选项以注册数据接受回调函数和用户上下文数据指针
 *
 * 5. 在使用`aiot_dm_send`发送消息前, 应先完成MQTT实例的建连
 *
 * 6. 调动`aiot_dm_send`发送不同类型的消息到云平台, 在注册的回调函数中处理各种类型的云平台下行消息
 *
 */

#ifndef __AIOT_DM_API_H__
#define __AIOT_DM_API_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief -0x0A00~-0x0AFF表达SDK在data-model模块内的状态码
 */
#define STATE_DM_BASE                                           (-0x0A00)

/**
 * @brief 用户发送@ref AIOT_DMMSG_EVENT_POST 消息时, 消息数据中的event_id为NULL
 */
#define STATE_DM_EVENT_ID_IS_NULL                               (-0x0A01)

/**
 * @brief 用户发送@ref AIOT_DMMSG_ASYNC_SERVICE_REPLY 或@ref AIOT_DMMSG_SYNC_SERVICE_REPLY 消息时, 消息数据中的event_id为NULL
 */
#define STATE_DM_SERVICE_ID_IS_NULL                             (-0x0A02)

/**
 * @brief 用户发送@ref AIOT_DMMSG_SYNC_SERVICE_REPLY 消息时, 消息数据中的rrpc_id为NULL
 */
#define STATE_DM_RRPC_ID_IS_NULL                                (-0x0A03)

/**
 * @brief 用户发送请求类消息时, 消息数据中的param为NULL
 */
#define STATE_DM_MSG_PARAMS_IS_NULL                             (-0X0A04)

/**
 * @brief 用户发送应答类消息时, 消息数据中的data为NULL
 */
#define STATE_DM_MSG_DATA_IS_NULL                               (-0X0A05)

/**
 * @brief 解析下行数据对应的topic时发生错误
 */
#define STATE_DM_INTERNAL_TOPIC_ERROR                           (-0x0A06)

/**
 * @brief 用户未调用@ref aiot_dm_setopt 配置MQTT句柄
 */
#define STATE_DM_MQTT_HANDLE_IS_NULL                            (-0x0A07)

/**
 * @brief 接收到服务器下行消息时的日志状态码
 */
#define STATE_DM_LOG_RECV                                       (-0x0A08)

/**
 * @brief 解析服务器下行消息失败时的日志状态码
 */
#define SATAE_DM_LOG_PARSE_RECV_MSG_FAILED                      (-0x0A09)


/**
 * @brief data-model模块的配置选项枚举类型定义. @ref aiot_dm_setopt 函数入数data的数据类型根据不同的选项而不同
 *
 */
typedef enum {
    /**
     * @brief 模块依赖的MQTT句柄
     *
     * @details
     *
     * data-model模块依赖底层的MQTT模块, 用户必需配置正确的MQTT句柄, 否则无法正常工作
     *
     * 数据类型: (void *)
     */
    AIOT_DMOPT_MQTT_HANDLE,

    /**
     * @brief 数据接收回调函数, data-model接收物联网平台的下行消息后调用此回调函数
     *
     * @details
     *
     * 数据类型: (aiot_dm_recv_handler_t), 详细查看@ref aiot_dm_recv_handler_t 回调函数原型
     */
    AIOT_DMOPT_RECV_HANDLER,

    /**
     * @brief 指向用户上下文数据的指针
     *
     * @details
     *
     * 在用户注册的@ref aiot_dm_recv_handler_t 数据接收回调函数中会同过userdata参数将此指针返回给用户
     *
     * 数据类型: (void *)
     */
    AIOT_DMOPT_USERDATA,

    /**
     * @brief 用户是否希望接收post消息后的reply
     *
     * @details
     *
     * 是否要接收云端的reply消息. 1表示要接收, 0表示不要.
     *
     * 数据类型: (uint8_t) *)
     */
    AIOT_DMOPT_POST_REPLY,

    /**
     * @brief 配置选项数量最大值, 不可用作配置参数
     */
    AIOT_DMOPT_MAX,
} aiot_dm_option_t;

/**
 * @brief data-model模块发送消息类型
 *
 * @details
 *
 * 这个枚举类型包括了dm模块支持发送的所有数据类型, 不同的消息类型将对于不同的消息结构体
 * 用户可查看网页文档<a href="https://help.aliyun.com/document_detail/89301.html">设备属性/事件/服务</a>进一步了解各种数据类型
 *
 */
typedef enum {
    /**
     * @brief 属性上报, 消息结构体参考@ref aiot_dm_msg_property_post_t \n
     * 成功发送此消息后, 将会收到@ref AIOT_DMRECV_GENERIC_REPLY 类型的应答消息
     */
    AIOT_DMMSG_PROPERTY_POST,

    /**
     * @brief 事件上报, 消息结构体参考@ref aiot_dm_msg_event_post_t \n
     * 成功发送此消息后, 将会收到@ref AIOT_DMRECV_GENERIC_REPLY 类型的应答消息
     */
    AIOT_DMMSG_EVENT_POST,

    /**
     * @brief 属性设置应答, 消息结构体参考@ref aiot_dm_msg_property_set_reply_t
     */
    AIOT_DMMSG_PROPERTY_SET_REPLY,

    /**
     * @brief 异步服务应答, 消息结构体参考@ref aiot_dm_msg_async_service_reply_t
     */
    AIOT_DMMSG_ASYNC_SERVICE_REPLY,

    /**
     * @brief 同步服务应答, 消息结构体参考@ref aiot_dm_msg_sync_service_reply_t
     */
    AIOT_DMMSG_SYNC_SERVICE_REPLY,

    /**
     * @brief 二进制格式的物模型上行数据, 消息结构体参考@ref aiot_dm_msg_raw_data_t
     */
    AIOT_DMMSG_RAW_DATA,

    /**
     * @brief 二进制格式的同步服务应答, 消息结构体参考@ref aiot_dm_msg_raw_service_reply_t
     */
    AIOT_DMMSG_RAW_SERVICE_REPLY,

    /**
     * @brief 获取期望属性值, 消息结构体请参考@ref aiot_dm_msg_get_desired_t, \n
     * 成功发送此消息后, 将会收到@ref AIOT_DMRECV_GENERIC_REPLY 类型的应答消息
     */
    AIOT_DMMSG_GET_DESIRED,

    /**
     * @brief 清除指定的期望值, 消息结构体请参考@ref aiot_dm_msg_delete_desired_t \n
     * 成功发送此消息后, 将会收到@ref AIOT_DMRECV_GENERIC_REPLY 类型的应答消息
     */
    AIOT_DMMSG_DELETE_DESIRED,

    /**
     * @brief 清除指定的期望值, 消息结构体请参考@ref aiot_dm_msg_delete_desired_t \n
     * 成功发送此消息后, 将会收到@ref AIOT_DMRECV_GENERIC_REPLY 类型的应答消息
     */
    AIOT_DMMSG_PROPERTY_BATCH_POST,

    /**
     * @brief 消息数量最大值, 不可用作消息类型
     */
    AIOT_DMMSG_MAX,
} aiot_dm_msg_type_t;

/**
 * @brief <b>物模型属性上报</b>消息结构体
 */
typedef struct {
    /**
     * @brief 字符串形式的JSON结构体, <b>必须以结束符'\0'结尾</b>. 包含用户要上报的属性数据, 如<i>"{\"LightSwitch\":0}"</i>
     */
    char *params;
} aiot_dm_msg_property_post_t;

/**
 * @brief <b>物模型事件上报</b>消息结构体
 */
typedef struct {
    /**
     * @brief 事件标示符, <b>必须为以结束符'\0'结尾的字符串</b>
     */
    char *event_id;
    /**
     * @brief 字符串形式的JSON结构体, <b>必须以结束符'\0'结尾</b>. 包含用户要上报的事件数据, 如<i>"{\"ErrorNum\":0}"</i>
     */
    char *params;
} aiot_dm_msg_event_post_t;

/**
 * @brief <b>属性设置应答</b>消息结构体, 用户在收到@ref AIOT_DMRECV_PROPERTY_SET 类型的属性设置后, 可发送此消息进行回复
 */
typedef struct {
    /**
     * @brief 消息标识符, uint64_t类型的整数, <b>必须与属性设置的消息标示符一致</b>
     */
    uint64_t msg_id;
    /**
     * @brief 设备端状态码, 200-请求成功, 更多状态码查看<a href="https://help.aliyun.com/document_detail/89309.html">设备端通用code</a>
     */
    uint32_t code;
    /**
     * @brief 设备端应答数据, 字符串形式的JSON结构体, <b>必须以结束符'\0'结尾</b>, 如<i>"{}"</i>表示应答数据为空
     */
    char    *data;
} aiot_dm_msg_property_set_reply_t;

/**
 * @brief <b>异步服务应答</b>消息结构体, 用户在收到@ref AIOT_DMRECV_ASYNC_SERVICE_INVOKE 类型的异步服务调用消息后, 应发送此消息进行应答
 */
typedef struct {
    /**
     * @brief 消息标识符, uint64_t类型的整数, <b>必须与异步服务调用的消息标示符一致</b>
     */
    uint64_t msg_id;
    /**
     * @brief 服务标示符, 标识了要响应服务
     */
    char    *service_id;
    /**
     * @brief 设备端状态码, 200-请求成功, 更多状态码查看<a href="https://help.aliyun.com/document_detail/89309.html">设备端通用code</a>
     */
    uint32_t code;
    /**
     * @brief 设备端应答数据, 字符串形式的JSON结构体, <b>必须以结束符'\0'结尾</b>, 如<i>"{}"</i>表示应答数据为空
     */
    char    *data;
} aiot_dm_msg_async_service_reply_t;

/**
 * @brief <b>同步服务应答</b>消息结构体, 用户在收到@ref AIOT_DMRECV_SYNC_SERVICE_INVOKE 类型的同步服务调用消息后, 应在超时时间(默认7s)内进行应答
 */
typedef struct {
    /**
     * @brief 消息标识符, uint64_t类型的整数, <b>必须与同步服务调用的消息标示符一致</b>
     */
    uint64_t msg_id;
    /**
     * @brief RRPC标示符, 用于唯一标识每一个同步服务的字符串, <b>必须与同步服务调用消息的RRPC标示符一致</b>
     */
    char    *rrpc_id;
    /**
     * @brief 服务标示符, 标识了要响应服务
     */
    char    *service_id;
    /**
     * @brief 设备端状态码, 200-请求成功, 更多状态码查看<a href="https://help.aliyun.com/document_detail/89309.html">设备端通用code</a>
     */
    uint32_t code;
    /**
     * @brief 设备端应答数据, 字符串形式的JSON结构体, <b>必须以结束符'\0'结尾</b>, 如<i>"{}"</i>表示应答数据为空
     */
    char    *data;
} aiot_dm_msg_sync_service_reply_t;

/**
 * @brief <b>物模型二进制数据</b>消息结构体, 发送的二进制数据将通过物联网平台的JavaScript脚本转化为JSON格式数据, 用户发送此消息前应确保已正确启用云端解析脚本
 */
typedef struct {
    /**
     * @brief 指向待发送二进制数据的指针
     */
    uint8_t *data;
    /**
     * @brief 待发送数据的长度
     */
    uint32_t data_len;
} aiot_dm_msg_raw_data_t;

/**
 * @brief <b>二进制格式的同步服务应答</b>消息结构体, 用户在收到@ref AIOT_DMRECV_RAW_SYNC_SERVICE_INVOKE 类型消息后, 应在超时时间(默认7s)内进行应答\n
 * 用户在使用此消息前应确保已启用云端解析脚本, 并且脚本工作正常
 */
typedef struct {
    /**
     * @brief RRPC标示符, 特殊字符串, <b>必须与同步服务调用消息的RRPC标示符一致</b>
     */
    char    *rrpc_id;
    /**
     * @brief 指向待发送二进制数据的指针
     */
    uint8_t *data;
    /**
     * @brief 待发送数据的长度
     */
    uint32_t data_len;
} aiot_dm_msg_raw_service_reply_t;

/**
 * @brief <b>获取期望属性值</b>消息结构体, 发送
 */
typedef struct {
    /**
     * @brief 字符串形式的JSON<b>数组</b>, <b>必须以结束符'\0'结尾</b>. 应包含用户要获取的期望属性的ID, 如<i>"[\"LightSwitch\"]"</i>
     */
    char *params;
} aiot_dm_msg_get_desired_t;

/**
 * @brief <b>删除指定期望值</b>消息结构体
 */
typedef struct {
    /**
     * @brief 字符串形式的JSON结构体, <b>必须以结束符'\0'结尾</b>. 应包含用户要删除的期望属性的ID和期望值版本号, 如<i>"{\"LightSwitch\":{\"version\":1},\"Color\":{}}"</i>
     */
    char *params;
} aiot_dm_msg_delete_desired_t;


/**
 * @brief <b>物模型属性上报</b>消息结构体
 */
typedef struct {
    /**
     * @brief 字符串形式的JSON结构体, <b>必须以结束符'\0'结尾</b>. 包含用户要批量上报的属性和事件数据, 如 {"properties":{"Power": [ { "value": "on", "time": 1524448722000 },
     *  { "value": "off", "time": 1524448722001 } ], "WF": [ { "value": 3, "time": 1524448722000 }]}, "events": {"alarmEvent": [{ "value": { "Power": "on", "WF": "2"},
     *  "time": 1524448722000}]}}
     */
    char *params;
} aiot_dm_msg_property_batch_post_t;

/**
 * @brief data-model模块发送消息的消息结构体
 */
typedef struct {
    /**
     * @brief 消息所属设备的product_key, 若为NULL则使用通过aiot_dm_setopt配置的product_key\n
     * 在网关子设备场景下, 可通过指定为子设备的product_key来发送子设备的消息到云端
     */
    char *product_key;
    /**
     * @brief 消息所属设备的device_name, 若为NULL则使用通过aiot_dm_setopt配置的device_name\n
     * 在网关子设备场景下, 可通过指定为子设备的product_key来发送子设备的消息到云端
     */
    char *device_name;
    /**
     * @brief 消息类型, 可参考@ref aiot_dm_msg_type_t
     */
    aiot_dm_msg_type_t type;
    /**
     * @brief 消息数据联合体, 不同的消息类型将使用不同的消息结构体
     */
    union {
        aiot_dm_msg_property_post_t         property_post;
        aiot_dm_msg_event_post_t            event_post;
        aiot_dm_msg_property_set_reply_t    property_set_reply;
        aiot_dm_msg_sync_service_reply_t    sync_service_reply;
        aiot_dm_msg_async_service_reply_t   async_service_reply;
        aiot_dm_msg_raw_data_t              raw_data;
        aiot_dm_msg_raw_service_reply_t     raw_service_reply;
        aiot_dm_msg_get_desired_t           get_desired;
        aiot_dm_msg_delete_desired_t        delete_desired;
    } data;
} aiot_dm_msg_t;


/**
 * @brief data-model模块接受消息类型枚举
 *
 * @details
 *
 * 这个枚举类型包括了dm模块支持接收的所有数据类型, 不同的消息类型将对于不同的消息结构体
 * 用户可查看网页文档<a href="https://help.aliyun.com/document_detail/89301.html">设备属性/事件/服务</a>进一步了解各种数据类型
 *
 */
typedef enum {
    /**
     * @brief 上报属性/实践后服务器返回的应答消息, 消息数据结构体参考@ref aiot_dm_recv_generic_reply_t
     */
    AIOT_DMRECV_GENERIC_REPLY,

    /**
     * @brief 服务器下发的属性设置消息, 消息数据结构体参考@ref aiot_dm_recv_property_set_t
     */
    AIOT_DMRECV_PROPERTY_SET,

    /**
     * @brief 服务器下发的异步服务调用消息, 消息数据结构体参考@ref aiot_dm_recv_async_service_invoke_t
     */
    AIOT_DMRECV_ASYNC_SERVICE_INVOKE,

    /**
     * @brief 服务器下发的同步服务调用消息, 消息数据结构体参考@ref aiot_dm_recv_sync_service_invoke_t
     */
    AIOT_DMRECV_SYNC_SERVICE_INVOKE,

    /**
     * @brief 服务器对设备上报的二进制数据应答, 消息数据结构体参考@ref aiot_dm_recv_raw_data_t
     */
    AIOT_DMRECV_RAW_DATA_REPLY,

    /**
     * @brief 服务器下发的物模型二进制数据, 消息数据结构体参考@ref aiot_dm_recv_raw_data_t
     */
    AIOT_DMRECV_RAW_DATA,

    /**
     * @brief 服务器下发的二进制格式的同步服务调用消息, 消息数据结构体参考@ref aiot_dm_recv_raw_service_invoke_t
     */
    AIOT_DMRECV_RAW_SYNC_SERVICE_INVOKE,

    /**
     * @brief 消息数量最大值, 不可用作消息类型
     */
    AIOT_DMRECV_MAX,
} aiot_dm_recv_type_t;

/**
 * @brief <b>云端通用应答</b>消息结构体, 设备端上报@ref AIOT_DMMSG_PROPERTY_POST, @ref AIOT_DMMSG_EVENT_POST 或者@ref AIOT_DMMSG_GET_DESIRED 等消息后, 服务器会应答此消息
 */
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
} aiot_dm_recv_generic_reply_t;

/**
 * @brief <b>属性设置</b>消息结构体
 */
typedef struct {
    /**
     * @brief 消息标识符, uint64_t类型的整数
     */
    uint64_t    msg_id;
    /**
     * @brief 服务器下发的属性数据, 为字符串形式的JSON结构体, 此字符串<b>不</b>以结束符'\0'结尾, 如<i>"{\"LightSwitch\":0}"</i>
     */
    char       *params;
    /**
     * @brief 属性数据的字符串长度
     */
    uint32_t    params_len;
} aiot_dm_recv_property_set_t;

/**
 * @brief <b>同步服务调用</b>消息结构体, 用户收到同步服务后, 必须在超时时间(默认7s)内进行应答
 */
typedef struct {
    /**
     * @brief 消息标识符, uint64_t类型的整数
     */
    uint64_t    msg_id;
    /**
     * @brief RRPC标识符, 用于唯一标识每一个同步服务的特殊字符串
     */
    char       *rrpc_id;
    /**
     * @brief 服务标示符, 字符串内容由用户定义的物模型决定
     */
    char       *service_id;
    /**
     * @brief 服务调用的输入参数数据, 为字符串形式的JSON结构体, 此字符串<b>不</b>以结束符'\0'结尾, 如<i>"{\"LightSwitch\":0}"</i>
     */
    char       *params;
    /**
     * @brief 输入参数的字符串长度
     */
    uint32_t    params_len;
} aiot_dm_recv_sync_service_invoke_t;

/**
 * @brief <b>同步服务调用</b>消息结构体
 */
typedef struct {
    /**
     * @brief 消息标识符, uint64_t类型的整数
     */
    uint64_t    msg_id;
    /**
     * @brief 服务标示符, 字符串内容由用户定义的物模型决定
     */
    char       *service_id;
    /**
     * @brief 服务调用的输入参数数据, 为字符串形式的JSON结构体, 此字符串<b>不</b>以结束符'\0'结尾, 如<i>"{\"LightSwitch\":0}"</i>
     */
    char       *params;
    /**
     * @brief 输入参数的字符串长度
     */
    uint32_t    params_len;
} aiot_dm_recv_async_service_invoke_t;

/**
 * @brief <b>物模型二进制数据</b>消息结构体, 服务器的JSON格式物模型数据将通过物联网平台的JavaScript脚本转化为二进制数据, 用户在接收此消息前应确保已正确启用云端解析脚本
 */
typedef struct {
    /**
     * @brief 指向接受数据缓冲区的指针
     */
    uint8_t    *data;
    /**
     * @brief 二进制数据的长度
     */
    uint32_t    data_len;
} aiot_dm_recv_raw_data_t;

/**
 * @brief <b>二进制数据的同步服务调用</b>消息结构体, 服务器的JSON格式物模型数据将通过物联网平台的JavaScript脚本转化为二进制数据, 用户在接收此消息前应确保已正确启用云端解析脚本
 */
typedef struct {
    /**
     * @brief RRPC标识符, 用于唯一标识每一个同步服务的特殊字符串
     */
    char       *rrpc_id;
    /**
     * @brief 指向接受数据缓冲区的指针
     */
    uint8_t    *data;
    /**
     * @brief 二进制数据的长度
     */
    uint32_t    data_len;
} aiot_dm_recv_raw_service_invoke_t;

/**
 * @brief data-model模块接收消息的结构体
 */
typedef struct {
    /**
     * @brief 消息所属设备的product_key, 不配置则默认使用MQTT模块配置的product_key
     */
    char *product_key;
    /**
     * @brief 消息所属设备的device_name, 不配置则默认使用MQTT模块配置的device_name
     */
    char *device_name;
    /**
     * @brief 接收消息的类型, 可参考@ref aiot_dm_recv_type_t
     */
    aiot_dm_recv_type_t type;
    /**
     * @brief 消息数据联合体, 不同的消息类型将使用不同的消息结构体
     */
    union {
        aiot_dm_recv_generic_reply_t        generic_reply;
        aiot_dm_recv_property_set_t         property_set;
        aiot_dm_recv_async_service_invoke_t async_service_invoke;
        aiot_dm_recv_sync_service_invoke_t  sync_service_invoke;
        aiot_dm_recv_raw_data_t             raw_data;
        aiot_dm_recv_raw_service_invoke_t   raw_service_invoke;
    } data;
} aiot_dm_recv_t;

/**
 * @brief data-model模块消息接收回调函数的函数原型定义, 当模块接收到服务器下行数据后将调用此回调函数, 并将消息数据通过<i>recv</i>参数输入给用户, \n
 * 同时将用户上下文数据指针通过<i>userdata</i>参数返回给用户
 *
 * @param[in] handle data-model实例句柄
 * @param[in] recv   服务下发的消息数据, <b>消息结构体中的所有数据指针在离开回调函数后将失效, 保存消息数据必须使用内存复制的方式</b>
 * @param[in] userdata 指向用户上下文数据的指针, 这个指针由用户通过调用@ref aiot_dm_setopt 配置@ref AIOT_DMOPT_USERDATA 选项设置
 *
 * @return void
 */
typedef void (*aiot_dm_recv_handler_t)(void *handle, const aiot_dm_recv_t *recv, void *userdata);

/**
 * @brief 初始化data-model实例
 *
 * @return void*
 * @retval 非NULL data-model实例句柄
 * @retval NULL 初始化失败, 一般是内存分配失败导致
 */
void *aiot_dm_init(void);

/**
 * @brief 设置data-model参数
 *
 * @param[in] handle data-model实例句柄
 * @param[in] option 配置选项, 更多信息请查看@ref aiot_dm_option_t
 * @param[in] data   配置数据, 更多信息请查看@ref aiot_dm_option_t
 *
 * @return int32_t
 * @retval STATE_SUCCESS 参数配置成功
 * @retval STATE_USER_INPUT_NULL_POINTER 入参handle或data为NULL
 * @retval STATE_USER_INPUT_OUT_RANGE 入参optioin的枚举值>=AIOT_DMOPT_MAX
 * @retval others 参考@ref aiot_state_api.h
 *
 */
int32_t aiot_dm_setopt(void *handle, aiot_dm_option_t option, void *data);

/**
 * @brief 发送一条data-model消息到物联网平台, 消息类型和消息数据由msg入参决定
 *
 * @param[in] handle data-model实例句柄
 * @param[in] msg    消息结构体, 可指定发送消息的设备<i>productKey</i>, <i>deviceName</i>; 消息类型, 消息数据等, 更多信息请参考@ref aiot_dm_msg_t
 *
 * @return int32_t
 * @retval >=STATE_SUCCESS 消息发送成功, 对于@ref AIOT_DMMSG_PROPERTY_POST, @ref AIOT_DMMSG_EVENT_POST, @ref AIOT_DMMSG_GET_DESIRED 和@ref AIOT_DMMSG_DELETE_DESIRED 消息, \n
 * 发送成功返回值为>STATE_SUCCESS的消息标示符<i>msg_id</i>值
 * @retval STATE_USER_INPUT_NULL_POINTER 入参<i>handle</i>或<i>msg</i>为NULL
 * @retval STATE_USER_INPUT_OUT_RANGE 入参<i>msg</i>的结构体成员<i>type</i> >= AIOT_DMMSG_MAX
 * @retval STATE_SYS_DEPEND_MALLOC_FAILED 内存分配失败
 * @retval STATE_DM_MQTT_HANDLE_IS_NULL 用户未调用@ref aiot_dm_setopt 配置MQTT句柄
 * @retval others 参考@ref aiot_state_api.h 或@ref STATE_DM_BASE 中对应的错误码说明
 *
 */
int32_t aiot_dm_send(void *handle, const aiot_dm_msg_t *msg);

/**
 * @brief 销毁data-model实例, 释放资源
 *
 * @param[in] p_handle 指向data-model实例句柄的指针
 * @return int32_t
 * @retval STATE_SUCCESS 执行成功
 * @retval <STATE_SUCCESS 执行失败
 *
 */
int32_t aiot_dm_deinit(void **p_handle);


#if defined(__cplusplus)
}
#endif

#endif /* #ifndef __AIOT_DM_API_H__ */


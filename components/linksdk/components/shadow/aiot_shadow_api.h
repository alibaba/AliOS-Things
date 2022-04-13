/**
 * @file aiot_shadow_api.h
 * @brief shadow模块头文件, 提供更新, 删除, 获取设备影子的能力
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 * @details
 *
 * 请按照以下流程使用API
 *
 * 1. 在使用设备影子模块前, 用户应首先创建好一个MQTT实例
 *
 * 2. 调用`aiot_shadow_init`创建一个设备影子实例, 保存实例句柄
 *
 * 3. 调用`aiot_shadow_setopt`配置`AIOT_SHADOWOPT_MQTT_HANDLE`选项以设置MQTT句柄, 此选项为强制配置选项
 *
 * 4. 调用`aiot_shadow_setopt`配置`AIOT_SHADOWOPT_RECV_HANDLER`和`AIOT_SHADOWOPT_USERDATA`选项以注册数据接受回调函数和用户上下文数据指针
 *
 * 5. 在使用`aiot_shadow_send`发送消息前, 应先完成MQTT实例的建连
 *
 * 6. 调动`aiot_shadow_send`发送更新设备影子, 获取设备影子或删除设备等消息到云平台, 在注册的回调函数中处理各种类型的云端应答消息或主动下推消息
 *
 */
#ifndef __AIOT_SHADOW_API_H__
#define __AIOT_SHADOW_API_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief -0x1300~-0x13FF表达SDK在data-model模块内的状态码
 */
#define STATE_SHADOW_BASE                                           (-0x1300)

/**
 * @brief 用户未调用@ref aiot_shadow_setopt 配置MQTT句柄
 */
#define STATE_SHADOW_MQTT_HANDLE_IS_NULL                            (-0x1301)

/**
 * @brief 用户上报@ref AIOT_SHADOWMSG_UPDATE 或 @ref AIOT_SHADOWMSG_DELETE_REPORTED 消息时, 消息结构体中的reported为NULL
 *
 */
#define STATE_SHADOW_REPORTED_DATA_IS_NULL                          (-0x1302)

/**
 * @brief 解析下行数据对应的topic时发生错误
 */
#define STATE_SHADOW_INTERNAL_TOPIC_ERROR                           (-0x1303)

/**
 * @brief 接收到服务器下行消息时的日志状态码
 */
#define STATE_SHADOW_LOG_RECV                                       (-0x1304)

/**
 * @brief 解析服务器下行消息失败时的日志状态码
 */
#define SATAE_SHADOW_LOG_PARSE_RECV_MSG_FAILED                      (-0x1305)


/**
 * @brief @ref aiot_shadow_setopt 接口的option参数可选值.
 *
 * @details 下文每个选项中的数据类型, 指的是@ref aiot_shadow_setopt 中, data参数的数据类型
 *
 * 当data的数据类型是char *时, 以配置@ref AIOT_SHADOWOPT_PRODUCT_KEY 为例:
 *
 *    char *product_key = "xxx";
 *    aiot_shadow_setopt(shadow_handle, AIOT_SHADOWOPT_PRODUCT_KEY, product_key);
 *
 */
typedef enum {
    /**
     * @brief 模块依赖的MQTT句柄
     *
     * @details
     *
     * shadow模块依赖底层的MQTT模块, 用户必需配置正确的MQTT句柄, 否则无法正常工作, 数据类型为(void *)
     */
    AIOT_SHADOWOPT_MQTT_HANDLE,

    /**
     * @brief 设置回调, 它在SDK收到网络报文的时候被调用, 告知用户, 数据类型为(aiot_shadow_recv_handler_t)
     */
    AIOT_SHADOWOPT_RECV_HANDLER,

    /**
     * @brief 用户需要SDK暂存的上下文, 数据类型为(void *)
     *
     * @details 这个上下文指针会在 AIOT_SHADOWOPT_RECV_HANDLER 设置的回调被调用时, 由SDK传给用户
     */
    AIOT_SHADOWOPT_USERDATA,

    /**
     * @brief 配置选项数量最大值, 不可用作配置参数
     */
    AIOT_SHADOWOPT_MAX,
} aiot_shadow_option_t;

/**
 * @brief shadow模块发送消息类型
 *
 * @details
 *
 * 这个枚举类型包括了shadow模块支持发送的所有数据类型, 不同的消息类型将对于不同的消息结构体
 *
 */
typedef enum {
    /**
     * @brief 更新设备影子中的reported值, 消息结构体参考@ref aiot_shadow_msg_update_t
     *
     */
    AIOT_SHADOWMSG_UPDATE,

    /**
     * @brief 清空设备影子的desired值, 消息结构体参考@ref aiot_shadow_msg_clean_desired_t
     *
     */
    AIOT_SHADOWMSG_CLEAN_DESIRED,

    /**
     * @brief 获取设备影子, 消息结构体参考@ref aiot_shadow_msg_get_t
     *
     */
    AIOT_SHADOWMSG_GET,

    /**
     * @brief 删除设备影子的部分或全部reported值, 消息结构体参考@ref aiot_shadow_msg_delete_reported_t
     *
     */
    AIOT_SHADOWMSG_DELETE_REPORTED,

    /**
     * @brief 消息数量最大值, 不可用作消息类型
     */
    AIOT_SHADOWMSG_MAX,
} aiot_shadow_msg_type_t;

/**
 * @brief 用于<b>更新设备影子中的reported数据</b>的消息结构体
 */
typedef struct {
    /**
     * @brief 设备影子reported object字符串, <b>必须为以结束符'\0'结尾的字符串</b>, 如"{\"LightSwitch\": 1}"
     */
    char *reported;

    /**
     * @brief 设备影子的目标版本, <b>必须大于设备影子的当前版本</b>, 若设置为-1将清空设备影子数据, 并将设备影子版本更新为0
     */
    int64_t version;
} aiot_shadow_msg_update_t;

/**
 * @brief 用于<b>清除设备影子中的desired数据</b>的消息结构体
 */
typedef struct {
    /**
     * @brief 设备影子的目标版本, <b>必须大于设备影子的当前版本</b>
     */
    int64_t version;
} aiot_shadow_msg_clean_desired_t;

/**
 * @brief 用于<b>获取设备影子</b>的消息结构体,
 */
typedef struct {
    /**
     * @brief 保留字段
     */
    uint32_t resevered;
} aiot_shadow_msg_get_t;

/**
 * @brief 用于<b>删除设备影子中的reported数据</b>的消息结构体
 */
typedef struct {
    /**
     * @brief 用户将要删除的reported数据, <b>必须为以结束符'\0'结尾的字符串</b>. \n
     * 若要删除全部reported数据, 则应填写"\"null\""字符串 \n
     * 若要删除部分reported数据, 则将对应的值定义为null, 如只清除LightSwitch的值应填写"{\"LightSwitch\":\"null\"}"
     */
    char *reported;

    /**
     * @brief 设备影子的目标版本, <b>必须大于设备影子的当前版本</b>
     */
    int64_t version;
} aiot_shadow_msg_delete_reported_t;

/**
 * @brief data-model模块发送消息的消息结构体
 */
typedef struct {
    /**
     * @brief 消息所属设备的product_key, 若为NULL则使用通过aiot_shadow_setopt配置的product_key \n
     * 在网关子设备场景下, 可通过指定为子设备的product_key来发送子设备的消息到云端
     */
    char *product_key;
    /**
     * @brief 消息所属设备的device_name, 若为NULL则使用通过aiot_shadow_setopt配置的device_name \n
     * 在网关子设备场景下, 可通过指定为子设备的product_key来发送子设备的消息到云端
     */
    char *device_name;
    /**
     * @brief 消息类型, 可参考@ref aiot_shadow_msg_type_t
     */
    aiot_shadow_msg_type_t type;
    /**
     * @brief 消息数据联合体, 不同的消息类型将使用不同的消息结构体
     */
    union {
        aiot_shadow_msg_update_t            update;
        aiot_shadow_msg_clean_desired_t     clean_desired;
        aiot_shadow_msg_get_t               get;
        aiot_shadow_msg_delete_reported_t   delete_reporte;
    } data;
} aiot_shadow_msg_t;


/**
 * @brief shadow模块收到从网络上来的报文时, 通知用户的报文类型
 */
typedef enum {
    /**
     * @brief 设备发送 @ref AIOT_SHADOWMSG_UPDATE, @ref AIOT_SHADOWMSG_CLEAN_DESIRED 或 @ref AIOT_SHADOWMSG_DELETE_REPORTED 这3这类型消息后, 云端返回的应答消息, \n
     * 消息数据结构体参考 @ref aiot_shadow_recv_generic_reply_t
     */
    AIOT_SHADOWRECV_GENERIC_REPLY,

    /**
     * @brief 设备在线时, 云端自动下发的影子内容, 消息数据结构体参考 @ref aiot_shadow_recv_control_t
     */
    AIOT_SHADOWRECV_CONTROL,

    /**
     * @brief 主动获取设备影子内容云端返回的影子内容, 消息数据结构体参考 @ref aiot_shadow_recv_get_reply_t
     */
    AIOT_SHADOWRECV_GET_REPLY,
} aiot_shadow_recv_type_t;

/**
 * @brief 设备发送 @ref AIOT_SHADOWMSG_UPDATE, @ref AIOT_SHADOWMSG_CLEAN_DESIRED 或 @ref AIOT_SHADOWMSG_DELETE_REPORTED 类型消息后, 云端返回的应答消息
 */
typedef struct {
    /**
     * @brief 指向应答数据的指针
     */
    char *payload;

    /**
     * @brief 应答数据长度
     */
    uint32_t payload_len;

    /**
     * @brief 应答状态字符串, 云端处理成功则为<b>success</b>, 发送消息错误则为<b>error</b>, 错误信息和错误码放在在payload中
     */
    char *status;

    /**
     * @brief 应答报文对应的时间戳
     */
    uint64_t timestamp;
} aiot_shadow_recv_generic_reply_t;

/**
 * @brief 如果设备在线, 用户应用调用云端API<a href="https://help.aliyun.com/document_detail/69954.html#doc-api-Iot-UpdateDeviceShadow">UpdateDeviceShadow</a>后云端下推的消息
 */
typedef struct {
    /**
     * @brief 指向设备影子数据的指针
     */
    char *payload;

    /**
     * @brief 设备影子数据长度
     */
    uint32_t payload_len;

    /**
     * @brief 设备影子版本
     */
    uint64_t version;
} aiot_shadow_recv_control_t;

/**
 * @brief 设备发送 @ref AIOT_SHADOWMSG_GET 类型消息后, 云端返回的设备影子数据
 */
typedef struct {
    /**
     * @brief 指向设备影子数据的指针
     */
    char *payload;

    /**
     * @brief 设备影子数据长度
     */
    uint32_t payload_len;

    /**
     * @brief 设备影子版本号
     */
    uint64_t version;
} aiot_shadow_recv_get_reply_t;

/**
 * @brief shadow模块收到从网络上来的报文时, 通知用户的报文内容
 */
typedef struct {
    /**
     * @brief 消息所属设备的product_key
     */
    char *product_key;
    /**
     * @brief 消息所属设备的device_name
     */
    char *device_name;
    /**
     * @brief 报文内容所对应的报文类型, 更多信息请参考@ref aiot_shadow_recv_type_t
     */
    aiot_shadow_recv_type_t  type;
    /**
     * @brief 消息数据联合体, 不同的消息类型将使用不同的消息结构体
     */
    union {
        aiot_shadow_recv_generic_reply_t generic_reply;
        aiot_shadow_recv_control_t control;
        aiot_shadow_recv_get_reply_t get_reply;
    } data;
} aiot_shadow_recv_t;


/**
 * @brief shadow模块收到从网络上来的报文时, 通知用户所调用的数据回调函数
 *
 * @param[in] handle shadow会话句柄
 * @param[in] recv shadow接受消息结构体, 存放收到的shadow报文内容
 * @param[in] userdata 指向用户上下文数据的指针, 这个指针由用户通过调用@ref aiot_shadow_setopt 配置@ref AIOT_SHADOWOPT_USERDATA 选项设置
 *
 * @return void
 */
typedef void (* aiot_shadow_recv_handler_t)(void *handle,
        const aiot_shadow_recv_t *recv, void *userdata);

/**
 * @brief 创建shadow会话实例, 并以默认值配置会话参数
 *
 * @return void *
 * @retval 非NULL shadow实例的句柄
 * @retval NULL   初始化失败, 一般是内存分配失败导致
 *
 */
void *aiot_shadow_init(void);

/**
 * @brief 配置shadow会话
 *
 * @param[in] handle shadow会话句柄
 * @param[in] option 配置选项, 更多信息请参考@ref aiot_shadow_option_t
 * @param[in] data   配置选项数据, 更多信息请参考@ref aiot_shadow_option_t
 *
 * @return int32_t
 * @retval STATE_SUCCESS 参数配置成功
 * @retval STATE_USER_INPUT_NULL_POINTER 入参handle或data为NULL
 * @retval STATE_USER_INPUT_OUT_RANGE 入参optioin的枚举值>=AIOT_SHADOWOPT_MAX
 * @retval others 参考@ref aiot_state_api.h
 *
 */
int32_t aiot_shadow_setopt(void *handle, aiot_shadow_option_t option, void *data);

/**
 * @brief 向服务器发送shadow消息请求
 *
 * @param[in] handle shadow会话句柄
 * @param[in] msg    消息结构体, 可指定发送消息的设备<i>productKey</i>, <i>deviceName</i>; 消息类型, 消息数据等, 更多信息请参考@ref aiot_shadow_msg_t
 *
 * @return int32_t
 * @retval STATE_SUCCESS 请求发送成功
 * @retval STATE_USER_INPUT_NULL_POINTER 入参<i>handle</i>或<i>msg</i>为NULL
 * @retval STATE_USER_INPUT_OUT_RANGE 入参<i>msg</i>的结构体成员<i>type</i> >= AIOT_SHADOWMSG_MAX
 * @retval STATE_SYS_DEPEND_MALLOC_FAILED 内存分配失败
 * @retval STATE_SHADOW_MQTT_HANDLE_IS_NULL 用户未调用@ref aiot_shadow_setopt 配置MQTT句柄
 * @retval others 参考@ref aiot_state_api.h 或@ref STATE_SHADOW_BASE 中对应的错误码说明
 *
 */
int32_t aiot_shadow_send(void *handle, aiot_shadow_msg_t *msg);

/**
 * @brief 结束shadow会话, 销毁实例并回收资源
 *
 * @param[in] handle 指向shadow会话句柄的指针
 *
 * @return int32_t
 * @retval STATE_SUCCESS 执行成功
 * @retval <STATE_SUCCESS 执行失败
 *
 */
int32_t aiot_shadow_deinit(void **handle);


#if defined(__cplusplus)
}
#endif

#endif  /* __AIOT_SHADOW_API_H__ */


/**
 * @file aiot_subdev_api.h
 * @brief subdev模块头文件, 提供子设备管理的能力
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
#ifndef __AIOT_SUBDEV_API_H__
#define __AIOT_SUBDEV_API_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief -0x0C00~-0x0CFF表达SDK在subdev模块内的状态码
 */
#define STATE_SUBDEV_BASE                                             (-0x0C00)

/**
 * @brief subdev模块收到从网络上来的报文时, 通知用户的报文类型
 */
typedef enum {
    AIOT_SUBDEVRECV_TOPO_ADD_REPLY,
    AIOT_SUBDEVRECV_TOPO_DELETE_REPLY,
    AIOT_SUBDEVRECV_TOPO_GET_REPLY,
    AIOT_SUBDEVRECV_BATCH_LOGIN_REPLY,
    AIOT_SUBDEVRECV_BATCH_LOGOUT_REPLY,
    AIOT_SUBDEVRECV_SUB_REGISTER_REPLY,
    AIOT_SUBDEVRECV_PRODUCT_REGISTER_REPLY,
    AIOT_SUBDEVRECV_TOPO_CHANGE_NOTIFY
} aiot_subdev_recv_type_t;

typedef struct {
    uint32_t     msg_id;
    uint32_t     code;
    char*        data;
    char*        message;
    char*        product_key;
    char*        device_name;
} aiot_subdev_generic_reply_t;

typedef struct {
    uint32_t     msg_id;
    char*        params;
    char*        product_key;
    char*        device_name;
} aiot_subdev_generic_notify_t;

/**
 * @brief subdev模块收到从网络上来的报文时, 通知用户的报文内容
 */
typedef struct {
    /**
     * @brief 报文内容所对应的报文类型, 更多信息请参考@ref aiot_subdev_recv_type_t
     */
    aiot_subdev_recv_type_t  type;
    union {
        /**
         * @brief 从设备发起请求消息后，收到的云端的应答
         */
        aiot_subdev_generic_reply_t  generic_reply;

        /**
         * @brief 收到的云端的通知
         */
        aiot_subdev_generic_notify_t generic_notify;
    } data;
} aiot_subdev_recv_t;

/**
 * @brief subdev模块收到从网络上来的报文时, 通知用户所调用的数据回调函数
 *
 * @param[in] handle subdev会话句柄
 * @param[in] packet subdev消息结构体, 存放收到的subdev报文内容
 * @param[in] userdata 用户上下文
 *
 * @return void
 */
typedef void (* aiot_subdev_recv_handler_t)(void *handle,
        const aiot_subdev_recv_t *packet, void *user_data);


/* TODO: 列举 SUBDEV 可能发生的所有内部状态变更(可选), 如MQTT重连, OTA收到通知, CoAP的token过期, 与上一段之间有2个空行 */
/* TODO: 填上 doxygen 的注释说明 */
/* TODO: 由于面向用户, 模块名字符串长度 > 6个字符的时候, 改成缩写, 例如: DYNREG_HTTP_EVT -> DRHTTPEVT */

/**
 * @brief subdev模块内部发生值得用户关注的状态变化时, 通知用户的事件类型
 */
typedef enum {
    /**
     * @brief 非法的应答报文
     */
    AIOT_SUBDEVEVT_INVALID_RESPONSE,
    /**
     * @brief 应答报文的id字段非法
     */
    AIOT_SUBDEVEVT_INVALID_ID,
    /**
     * @brief 应答报文的code字段非法
     */
    AIOT_SUBDEVEVT_INVALID_CODE
} aiot_subdev_event_type_t;

/**
 * @brief subdev模块内部发生值得用户关注的状态变化时, 通知用户的事件内容
 */
typedef struct {
    /**
     * @brief 事件内容所对应的事件类型, 更多信息请参考@ref aiot_subdev_event_type_t
     */
    aiot_subdev_event_type_t  type;
} aiot_subdev_event_t;

/**
 * @brief subdev模块内部发生值得用户关注的状态变化时, 通知用户所调用的事件回调函数
 *
 * @param[in] handle, subdev会话句柄
 * @param[in] event, subdev模块中发生的事件的内容
 * @param[in] userdata, 用户上下文
 *
 * @return void
 */
typedef void (*aiot_subdev_event_handler_t)(void *handle,
        const aiot_subdev_event_t *event, void *userdata);

/**
 * @brief 
 * 
 */
typedef struct {
    char *product_key;
    char *device_name;
    char *device_secret;
    char *product_secret;
} aiot_subdev_dev_t;

/**
 * @brief @ref aiot_subdev_setopt 接口的option参数可选值.
 *
 * @details 下文每个选项中的数据类型, 指的是@ref aiot_subdev_setopt 中, data参数的数据类型
 *
 * 1. data的数据类型是char *时, 以配置@ref AIOT_SUBDEVOPT_MQTT_HANDLE 为例:
 *
 *    void *mqtt_handle = aiot_mqtt_init();
 *    aiot_subdev_setopt(subdev_handle, AIOT_SUBDEVOPT_MQTT_HANDLE, mqtt_handle);
 */
typedef enum {
    /**
     * @brief subdev会话 需要的MQTT句柄, 需要先建立MQTT连接，再设置MQTT句柄
     * 
     * @details
     * 
     * 数据类型: (void *)
     */
    AIOT_SUBDEVOPT_MQTT_HANDLE,

    /**
     * @brief 设置回调, 它在SDK收到网络报文的时候被调用, 告知用户, 数据类型为(aiot_subdev_recv_handler_t)
     * 
     * @details
     * 
     * 数据类型: (aiot_subdev_recv_handler_t)
     */
    AIOT_SUBDEVOPT_RECV_HANDLER,

    /**
     * @brief 设置回调, 它在SDK发生内部状态变更时被调用, 告知用户, 数据类型为(aiot_subdev_event_handler_t)
     * 
     * @details
     * 
     * 数据类型: (aiot_subdev_event_handler_t)
     */
    AIOT_SUBDEVOPT_EVENT_HANDLER,

    /**
     * @brief 用户需要SDK暂存的上下文, 数据类型为(void *)
     *
     * @details 这个上下文指针会在 AIOT_SUBDEVOPT_RECV_HANDLER 和 AIOT_SUBDEVOPT_EVENT_HANDLER 设置的回调被调用时, 由SDK传给用户
     * 
     * 数据类型: (void *)
     */
    AIOT_SUBDEVOPT_USERDATA,
    AIOT_SUBDEVOPT_MAX
} aiot_subdev_option_t;

/**
 * @brief 创建subdev会话实例, 并以默认值配置会话参数
 *
 * @return void *
 * @retval 非NULL subdev实例的句柄
 * @retval NULL   初始化失败, 一般是内存分配失败导致
 *
 */
void *aiot_subdev_init(void);

/**
 * @brief 配置subdev会话
 *
 * @param[in] handle subdev会话句柄
 * @param[in] option 配置选项, 更多信息请参考@ref aiot_subdev_option_t
 * @param[in] data   配置选项数据, 更多信息请参考@ref aiot_subdev_option_t
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  参数配置失败
 * @retval >=STATE_SUCCESS 参数配置成功
 *
 */
int32_t aiot_subdev_setopt(void *handle, aiot_subdev_option_t option, void *data);

/**
 * @brief 结束subdev会话, 销毁实例并回收资源
 *
 * @param[in] handle 指向subdev会话句柄的指针
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  执行失败
 * @retval >=STATE_SUCCESS 执行成功
 *
 */
int32_t aiot_subdev_deinit(void **handle);

/**
 * @brief 向物联网平台发送添加子设备与网关topo关系的请求
 *
 * @param handle subdev会话句柄
 * @param dev 需要添加topo关系的子设备数组
 * @param dev_num 子设备数组中的子设备数量
 * 
 * @return int32_t
 * @retval <STATE_SUCCESS  请求发送失败
 * @retval >=STATE_SUCCESS 请求发送成功
 * 
 * @note
 * 
 * @ref aiot_subdev_dev_t 中需要填写product key, device name 和 device secret
 */
int32_t aiot_subdev_send_topo_add(void *handle, aiot_subdev_dev_t dev[], uint32_t dev_num);

/**
 * @brief 向物联网平台发送删除子设备与网关topo关系的请求
 *
 * @param handle subdev会话句柄
 * @param dev 需要删除topo关系的子设备数组
 * @param dev_num 子设备数组中的子设备数量
 * 
 * @return int32_t
 * @retval <STATE_SUCCESS  请求发送失败
 * @retval >=STATE_SUCCESS 请求发送成功
 * 
 * @note
 * 
 * @ref aiot_subdev_dev_t 中需要填写product key, device name
 */
int32_t aiot_subdev_send_topo_delete(void *handle, aiot_subdev_dev_t dev[], uint32_t dev_num);

/**
 * @brief 向物联网平台发送查询子设备与网关topo关系的请求
 *
 * @param handle subdev会话句柄
 * 
 * @return int32_t
 * @retval <STATE_SUCCESS  请求发送失败
 * @retval >=STATE_SUCCESS 请求发送成功
 * 
 * @note
 * 
 * @ref aiot_subdev_dev_t 中需要填写product key, device name
 */
int32_t aiot_subdev_send_topo_get(void *handle);

/**
 * @brief 向物联网平台发送子设备上线请求
 *
 * @param handle subdev会话句柄
 * @param dev 需要上线的子设备数组
 * @param dev_num 子设备数组中的子设备数量
 * 
 * @return int32_t
 * @retval <STATE_SUCCESS  请求发送失败
 * @retval >=STATE_SUCCESS 请求发送成功
 * 
 * @note
 * 
 * @ref aiot_subdev_dev_t 中需要填写product key, device name, device secret
 */
int32_t aiot_subdev_send_batch_login(void *handle, aiot_subdev_dev_t dev[], uint32_t dev_num);

/**
 * @brief 向物联网平台发送子设备下线请求
 *
 * @param handle subdev会话句柄
 * @param dev 需要下线的子设备数组
 * @param dev_num 子设备数组中的子设备数量
 * 
 * @return int32_t
 * @retval <STATE_SUCCESS  请求发送失败
 * @retval >=STATE_SUCCESS 请求发送成功
 * 
 * @note
 * 
 * @ref aiot_subdev_dev_t 中需要填写product key, device name
 */
int32_t aiot_subdev_send_batch_logout(void *handle, aiot_subdev_dev_t dev[], uint32_t dev_num);

/**
 * @brief 向物联网平台发送子设备动态注册请求
 *
 * @param handle subdev会话句柄
 * @param dev 需要动态注册的子设备数组
 * @param dev_num 子设备数组中的子设备数量
 * 
 * @return int32_t
 * @retval <STATE_SUCCESS  请求发送失败
 * @retval >=STATE_SUCCESS 请求发送成功
 * 
 * @note
 * 
 * @ref aiot_subdev_dev_t 中需要填写product key, device name
 */
int32_t aiot_subdev_send_sub_register(void *handle, aiot_subdev_dev_t dev[], uint32_t dev_num);

/**
 * @brief 向物联网平台发送子设备动态注册请求(可从其他网关抢绑子设备)
 *
 * @param handle subdev会话句柄
 * @param dev 需要动态注册的子设备数组
 * @param dev_num 子设备数组中的子设备数量
 * 
 * @return int32_t
 * @retval <STATE_SUCCESS  请求发送失败
 * @retval >=STATE_SUCCESS 请求发送成功
 * 
 * @note
 * 
 * @ref aiot_subdev_dev_t 中需要填写product key, device name, product secret
 */
int32_t aiot_subdev_send_product_register(void *handle, aiot_subdev_dev_t dev[], uint32_t dev_num);

#if defined(__cplusplus)
}
#endif

#endif  /* __AIOT_SUBDEV_API_H__ */


/**
 * @file aiot_mqtt_api.h
 * @brief MQTT模块头文件, 提供用MQTT协议连接阿里云物联网平台的能力
 * @date 2019-12-27
 *
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 *
 * @details
 *
 * MQTT模块用于建立与阿里云物联网平台的连接, API使用流程如下:
 *
 * 1. 调用 @ref aiot_mqtt_init 初始化MQTT会话, 获取会话句柄
 *
 * 2. 调用 @ref aiot_mqtt_setopt 配置MQTT会话的参数, 常用配置项见 @ref aiot_mqtt_setopt 的说明
 *
 * 3. 调用 @ref aiot_mqtt_connect 建立与阿里云物联网平台的连接
 *
 * 4. 启动一个线程, 线程中间歇性调用 @ref aiot_mqtt_process 处理心跳和QoS1的消息
 *
 * 5. 启动一个线程, 线程中持续调用 @ref aiot_mqtt_recv 接收网络上的MQTT报文
 *
 *    + 当接收到一条报文时, 按以下顺序检查当前MQTT会话的参数, 当满足某条的描述时, 会通过对应的回调函数进行通知, 并停止检查
 *
 *      + 检查收到的报文topic是否已经通过 @ref aiot_mqtt_setopt 的 @ref AIOT_MQTTOPT_APPEND_TOPIC_MAP 参数配置回调函数
 *
 *      + 检查收到的报文topic是否已经通过 @ref aiot_mqtt_sub API配置回调函数
 *
 *      + 检查是否通过 @ref aiot_mqtt_setopt 的 @ref AIOT_MQTTOPT_RECV_HANDLER 参数配置默认回调函数
 *
 * 6. 经过以上步骤后, MQTT连接已建立并能保持与物联网平台的连接, 接下来按自己的场景用 @ref aiot_mqtt_sub 和 @ref aiot_mqtt_pub 等API实现业务逻辑即可
 *
 */

#ifndef _AIOT_MQTT_API_H_
#define _AIOT_MQTT_API_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief MQTT报文类型
 *
 * @details
 *
 * 传入@ref aiot_mqtt_recv_handler_t 的MQTT报文类型
 */
typedef enum {
    /**
     * @brief MQTT PUBLISH报文
     */
    AIOT_MQTTRECV_PUB,

    /**
     * @brief MQTT PINGRESP报文
     */
    AIOT_MQTTRECV_HEARTBEAT_RESPONSE,

    /**
     * @brief MQTT SUBACK报文
     */
    AIOT_MQTTRECV_SUB_ACK,

    /**
     * @brief MQTT UNSUB报文
     */
    AIOT_MQTTRECV_UNSUB_ACK,

    /**
     * @brief MQTT PUBACK报文
     */
    AIOT_MQTTRECV_PUB_ACK,
} aiot_mqtt_recv_type_t;


typedef struct {
    /**
     * @brief MQTT报文类型, 更多信息请参考@ref aiot_mqtt_recv_type_t
     */
    aiot_mqtt_recv_type_t type;
    /**
     * @brief MQTT报文联合体, 内容根据type进行选择
     */
    union {
        /**
         * @brief MQTT PUBLISH报文
         */
        struct {
            uint8_t qos;
            char *topic;
            uint16_t topic_len;
            uint8_t *payload;
            uint32_t payload_len;
        } pub;
        /**
         * @brief AIOT_MQTTRECV_SUB_ACK
         */
        struct {
            int32_t res;
            uint8_t max_qos;
            uint16_t packet_id;
        } sub_ack;
        /**
         * @brief AIOT_MQTTRECV_UNSUB_ACK
         */
        struct {
            uint16_t packet_id;
        } unsub_ack;
        /**
         * @brief AIOT_MQTTRECV_PUB_ACK
         */
        struct {
            uint16_t packet_id;
        } pub_ack;
    } data;
} aiot_mqtt_recv_t;

/**
 * @brief MQTT报文接收回调函数原型
 *
 * @param[in] handle MQTT实例句柄
 * @param[in] packet MQTT报文结构体, 存放收到的MQTT报文
 * @param[in] userdata 用户上下文
 *
 * @return void
 */
typedef void (*aiot_mqtt_recv_handler_t)(void *handle, const aiot_mqtt_recv_t *packet, void *userdata);

/**
 * @brief MQTT内部事件类型
 */
typedef enum {
    /**
     * @brief 当MQTT实例第一次连接网络成功时, 触发此事件
     */
    AIOT_MQTTEVT_CONNECT,
    /**
     * @brief 当MQTT实例断开网络连接后重连成功时, 触发此事件
     */
    AIOT_MQTTEVT_RECONNECT,
    /**
     * @brief 当MQTT实例断开网络连接时, 触发此事件
     */
    AIOT_MQTTEVT_DISCONNECT
} aiot_mqtt_event_type_t;

typedef enum {
    /**
     * @brief MQTT实例网络连接由于网络故障而断开
     */
    AIOT_MQTTDISCONNEVT_NETWORK_DISCONNECT,
    /**
     * @brief MQTT实例网络连接由于心跳丢失超过指定次数(@ref AIOT_MQTTOPT_HEARTBEAT_MAX_LOST )而断开
     */
    AIOT_MQTTDISCONNEVT_HEARTBEAT_DISCONNECT
} aiot_mqtt_disconnect_event_type_t;

/**
 * @brief MQTT内部事件
 */
typedef struct {
    /**
     * @brief MQTT内部事件类型. 更多信息请参考@ref aiot_mqtt_event_type_t
     *
     */
    aiot_mqtt_event_type_t type;
    /**
     * @brief MQTT事件数据联合体
     */
    union {
        /**
         * @brief MQTT连接断开时, 具体的断开原因
         */
        aiot_mqtt_disconnect_event_type_t disconnect;
    } data;
} aiot_mqtt_event_t;

/**
 * @brief MQTT事件回调函数
 *
 * @details
 *
 * 当MQTT内部事件被触发时, 调用此函数. 如连接成功/断开连接/重连成功
 *
 */
typedef void (*aiot_mqtt_event_handler_t)(void *handle, const aiot_mqtt_event_t *event, void *userdata);

/**
 * @brief 使用 @ref aiot_mqtt_setopt 配置 @ref AIOT_MQTTOPT_APPEND_TOPIC_MAP 时的数据
 *
 * @details
 *
 * 用于在建立MQTT连接前配置topic与相应的回调函数
 *
 */
typedef struct {
    char *topic;
    aiot_mqtt_recv_handler_t handler;
    void *userdata;
} aiot_mqtt_topic_map_t;

/**
 * @brief @ref aiot_mqtt_setopt 函数的option参数. 对于下文每一个选项中的数据类型, 指的是@ref aiot_mqtt_setopt 中的data参数的数据类型
 *
 * @details
 *
 * 1. data的数据类型是char *时, 以配置@ref AIOT_MQTTOPT_HOST 为例:
 *
 *    char *host = "xxx";
 *
 *    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_HOST, host);
 *
 * 2. data的数据类型是其他数据类型时, 以配置@ref AIOT_MQTTOPT_PORT 为例:
 *
 *    uint16_t port = 443;
 *
 *    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_PORT, (void *)&port);
 */
typedef enum {
    /**
     * @brief MQTT 服务器的域名地址或者ip地址
     *
     * @details
     *
     * 阿里云物联网平台域名地址列表(必须使用自己的product key替换${pk}):
     *
     * 使用tcp或tls证书方式建联:
     *
     * | 域名地址                                        | 区域    | 端口号
     * |-------------------------------------------------|---------|---------
     * | ${pk}.iot-as-mqtt.cn-shanghai.aliyuncs.com      | 上海    | 443
     * | ${pk}.iot-as-mqtt.ap-southeast-1.aliyuncs.com   | 新加坡  | 443
     * | ${pk}.iot-as-mqtt.ap-northeast-1.aliyuncs.com   | 日本    | 443
     * | ${pk}.iot-as-mqtt.us-west-1.aliyuncs.com        | 美西    | 443
     * | ${pk}.iot-as-mqtt.eu-central-1.aliyuncs.com     | 德国    | 443
     *
     * 使用tls psk方式建联:
     *
     * | 域名地址                                | 区域    | 端口号
     * |-----------------------------------------|---------|---------
     * | ${pk}.itls.cn-shanghai.aliyuncs.com     | 上海    | 1883
     *
     * 使用tls x509客户端证书方式建联:
     *
     * | 域名地址                            | 区域    | 端口号
     * |-------------------------------------|---------|---------
     * | x509.itls.cn-shanghai.aliyuncs.com  | 上海    | 1883
     *
     * 数据类型: (char *)
     */
    AIOT_MQTTOPT_HOST,

    /**
     * @brief MQTT 服务器的端口号
     *
     * @details
     *
     * 连接阿里云物联网平台时:
     *
     * 1. 如果使用的是tcp或者tls证书方式, 端口号设置为443
     *
     * 2. 如果使用的是tls psk和tls x509客户端证书方式, 端口号设置为1883
     *
     * 数据类型: (uint16_t *)
     */
    AIOT_MQTTOPT_PORT,

    /**
     * @brief 设备的product key, 可从<a href="http://iot.console.aliyun.com/">阿里云物联网平台控制台</a>获取
     *
     * @details
     *
     * 数据类型: (char *)
     */
    AIOT_MQTTOPT_PRODUCT_KEY,

    /**
     * @brief 设备的device name, 可从<a href="http://iot.console.aliyun.com/">阿里云物联网平台控制台</a>获取
     *
     * @details
     *
     * 数据类型: (char *)
     */
    AIOT_MQTTOPT_DEVICE_NAME,

    /**
     * @brief 设备的device secret, 可从<a href="http://iot.console.aliyun.com/">阿里云物联网平台控制台</a>获取
     *
     * @details
     *
     * 数据类型: (char *)
     */
    AIOT_MQTTOPT_DEVICE_SECRET,

    /**
     * @brief 设备连接阿里云物联网平台时的扩展clientid
     *
     * @details
     *
     * 若需要上报模组商id和模组id以及os信息, 按以下格式填写:
     *
     * "mid=<模组ID>,pid=<模组商ID>,os=<操作系统>"
     *
     * 数据类型: (char *)
     */
    AIOT_MQTTOPT_EXTEND_CLIENTID,

    /**
     * @brief 设备连接阿里云物联网平台时的安全模式, 使用标准的tcp或tls时无需配置
     *
     * @details
     *
     * 数据类型: (char *)
     */
    AIOT_MQTTOPT_SECURITY_MODE,

    /**
     * @brief 使用自定义连接凭据连接mqtt服务器时, 凭据的username
     *
     * @details
     *
     * 数据类型: (char *)
     */
    AIOT_MQTTOPT_USERNAME,

    /**
     * @brief 使用自定义连接凭据连接mqtt服务器时, 凭据的password
     *
     * @brief
     *
     * 数据类型: (char *)
     */
    AIOT_MQTTOPT_PASSWORD,

    /**
     * @brief 使用自定义连接凭据连接mqtt服务器时, 凭据的clientid
     *
     * @details
     *
     * 数据类型: (char *)
     */
    AIOT_MQTTOPT_CLIENTID,

    /**
     * @brief MQTT建联时, CONNECT报文中的心跳间隔参数
     *
     * @details
     *
     * 受阿里云物联网平台限制, 取值范围为30 ~ 1200s
     *
     * 1. 如果设置的值小于30, mqtt建联会被云端拒绝,  @ref aiot_mqtt_connect 函数会返回@ref STATE_MQTT_CONNACK_RCODE_SERVER_UNAVAILABLE 错误
     *
     * 2. 如果设置的值大于1200, mqtt连接仍然可以建立, 但此参数会被服务器覆盖为1200
     *
     * 数据类型: (uint16_t *) 取值范围: 30 ~ 1200s 默认值: 1200s
     */
    AIOT_MQTTOPT_KEEPALIVE_SEC,

    /**
     * @brief MQTT建联时, CONNECT报文中的clean session参数
     *
     * @details
     *
     * 1. 设备上线时如果clean session为0, 那么上线前服务器推送QoS1的消息会在此时推送给设备
     *
     * 2. 设备上线时如果clean session为1, 那么上线前服务器推送的QoS1的消息会被丢弃
     *
     * 数据类型: (uint8_t *) 取值范围: 0, 1 默认值: 1
     */
    AIOT_MQTTOPT_CLEAN_SESSION,

    /**
     * @brief MQTT建联时, 网络使用的安全凭据
     *
     * @details
     *
     * 该配置项用于为底层网络配置@ref aiot_sysdep_network_cred_t 安全凭据数据
     *
     * 1. 若该选项不配置, 那么MQTT将以tcp方式直接建联
     *
     * 2. 若@ref aiot_sysdep_network_cred_t 中option配置为@ref AIOT_SYSDEP_NETWORK_CRED_NONE , MQTT将以tcp方式直接建联
     *
     * 3. 若@ref aiot_sysdep_network_cred_t 中option配置为@ref AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA , MQTT将以tls方式建联
     *
     * 4. 若@ref aiot_sysdep_network_cred_t 中option配置为@ref AIOT_SYSDEP_NETWORK_CRED_SVRCERT_PSK , MQTT将以tls psk方式建联
     *
     * 数据类型: (aiot_sysdep_network_cred_t *)
     */
    AIOT_MQTTOPT_NETWORK_CRED,

    /**
     * @brief MQTT建联时, 建立网络连接的超时时间
     *
     * @details
     *
     * 指建立socket连接的超时时间
     *
     * 数据类型: (uint32_t *) 默认值: (5 *1000) ms
     *
     */
    AIOT_MQTTOPT_CONNECT_TIMEOUT_MS,

    /**
     * @brief 配置MQTT PINGREQ报文发送时间间隔. (心跳发送间隔)
     *
     * @details
     *
     * 数据类型: (uint32_t *) 默认值: (25 * 1000) ms
     */
    AIOT_MQTTOPT_HEARTBEAT_INTERVAL_MS,

    /**
     * @brief 配置MQTT PINGRESP报文允许连续丢失的最大次数, 当超过这个次数时, 触发重连机制
     *
     * @details
     *
     * 数据类型: (uint8_t *) 默认值: (2)
     */
    AIOT_MQTTOPT_HEARTBEAT_MAX_LOST,

    /**
     * @brief 打开/关闭MQTT重连机制
     *
     * @details
     *
     * 数据类型: (uint8_t *) 取值范围: 0, 1 默认值: 1
     */
    AIOT_MQTTOPT_RECONN_ENABLED,

    /**
     * @brief 当由于心跳丢失或者网络断开触发重连机制时, 尝试重连的时间间隔
     *
     * @details
     *
     * 数据类型: (uint32_t *) 默认值: (2 * 1000) ms
     */
    AIOT_MQTTOPT_RECONN_INTERVAL_MS,

    /**
     * @brief MQTT发送数据时, 在协议栈花费的最长时间
     *
     * @details
     *
     * 数据类型: (uint32_t *) 默认值: (5 * 1000) ms
     */
    AIOT_MQTTOPT_SEND_TIMEOUT_MS,

    /**
     * @brief MQTT接收数据时, 在协议栈花费的最长时间
     *
     * @details
     *
     * 数据类型: (uint32_t *) 默认值: (5 * 1000) ms
     */
    AIOT_MQTTOPT_RECV_TIMEOUT_MS,

    /**
     * @brief QoS1消息重发间隔
     *
     * @details
     *
     * 当发送qos1 MQTT PUBLISH报文后, 如果在@ref AIOT_MQTTOPT_REPUB_TIMEOUT_MS 时间内未收到mqtt PUBACK报文,
     * @ref aiot_mqtt_process 会重新发送此qo1 MQTT PUBLISH报文, 直到收到PUBACK报文为止
     *
     * 数据类型: (uint32_t *) 默认值: (3 * 1000) ms
     */
    AIOT_MQTTOPT_REPUB_TIMEOUT_MS,

    /**
     * @brief 销毁MQTT实例时, 等待其他api执行完毕的时间
     *
     * @details
     *
     * 当调用@ref aiot_mqtt_deinit 销毁MQTT实例时, 若继续调用其他aiot_mqtt_xxx API, API会返回@ref STATE_USER_INPUT_EXEC_DISABLED 错误
     *
     * 此时, 用户应该停止调用其他aiot_mqtt_xxx API
     *
     * 数据类型: (uint32_t *) 默认值: (2 * 1000) ms
     */
    AIOT_MQTTOPT_DEINIT_TIMEOUT_MS,

    /**
     * @brief 从MQTT服务器收取的数据从此默认回调函数进行通知
     *
     * @details
     *
     * 1. 若没有配置该回调函数, 当有消息到达但找不到对应的已注册topic时, 消息会被丢弃
     *
     * 2. 若已配置该回调函数, 当有消息到达但找不到对应的已注册topic时, 消息从此默认回调函数进行通知
     *
     * 数据类型: ( @ref aiot_mqtt_recv_handler_t )
     */
    AIOT_MQTTOPT_RECV_HANDLER,

    /**
     * @brief MQTT客户端内部发生的事件会从此回调函数进行通知, 如上线/断线/重新上线
     *
     * @details
     *
     * 数据类型: ( @ref aiot_mqtt_event_handler_t )
     */
    AIOT_MQTTOPT_EVENT_HANDLER,

    /**
     * @brief 可在MQTT建立连接之前配置MQTT topic与其对应的回调函数
     *
     * @details
     *
     * 数据类型: ( @ref aiot_mqtt_topic_map_t )
     */
    AIOT_MQTTOPT_APPEND_TOPIC_MAP,

    /**
     * @brief 取消之前建立的MQTT topic与其回调函数的对应关系
     *
     * @details
     *
     * 数据类型: ( @ref aiot_mqtt_topic_map_t )
     */
    AIOT_MQTTOPT_REMOVE_TOPIC_MAP,

    /**
     * @brief 在publish消息的topic上附加请求ID字符串, 用于全链路日志追踪
     *
     * @details
     *
     * 数据类型: (uint8_t *) 默认值: 0
     *
     * 配置为0则不附加请求ID字符串, 配置为1将附加请求ID字符串
     */
    AIOT_MQTTOPT_APPEND_REQUESTID,

    /**
     * @brief 用户需要SDK暂存的上下文
     *
     * @details
     *
     * 1. 该上下文会在@ref AIOT_MQTTOPT_RECV_HANDLER 和@ref AIOT_MQTTOPT_EVENT_HANDLER 中传回给用户
     *
     * 2. 当使用@ref AIOT_MQTTOPT_APPEND_TOPIC_MAP 或者@ref aiot_mqtt_sub 时未指定userdata, 该上下文也会传给这些回调函数
     *
     * 数据类型: (void *)
     */
    AIOT_MQTTOPT_USERDATA,

    AIOT_MQTTOPT_MAX
} aiot_mqtt_option_t;

/**
 * @brief 初始化mqtt实例并设置默认参数
 *
 * @return void*
 * @retval 非NULL MQTT实例句柄
 * @retval NULL 初始化失败, 一般是内存分配失败导致
 *
 */
void   *aiot_mqtt_init(void);

/**
 * @brief 设置mqtt参数
 *
 * @details
 *
 * 下面列出常用的配置选项, 至少需要配置以下选项才可使用MQTT的基本功能
 *
 * 其余配置选项均设有默认值, 可按业务需要进行调整
 *
 * + `AIOT_MQTTOPT_HOST`: 配置连接的阿里云MQTT站点地址
 *
 * + `AIOT_MQTTOPT_PORT`: 配置连接的阿里云MQTT站点端口号
 *
 * + `AIOT_MQTTOPT_PRODUCT_KEY`: 配置设备的 productKey
 *
 * + `AIOT_MQTTOPT_DEVICE_NAME`: 配置设备的 deviceName
 *
 * + `AIOT_MQTTOPT_DEVICE_SECRET`: 配置设备的 deviceSecret
 *
 * + `AIOT_MQTTOPT_NETWORK_CRED`: 配置建立MQTT连接时的安全凭据
 *
 * + `AIOT_MQTTOPT_RECV_HANDLER`: 配置默认的数据接收回调函数
 *
 * + `AIOT_MQTTOPT_EVENT_HANDLER`: 配置MQTT事件通知回调函数
 *
 * @param[in] handle mqtt句柄
 * @param[in] option 配置选项, 更多信息请参考@ref aiot_mqtt_option_t
 * @param[in] data   配置选项数据, 更多信息请参考@ref aiot_mqtt_option_t
 *
 * @return int32_t
 * @retval <STATE_SUCCESS 参数设置失败, 更多信息请参考@ref aiot_state_api.h
 * @retval >=STATE_SUCCESS 参数设置成功
 *
 */
int32_t aiot_mqtt_setopt(void *handle, aiot_mqtt_option_t option, void *data);

/**
 * @brief 释放mqtt实例句柄的资源
 *
 * @param[in] handle 指向mqtt实例句柄的指针
 *
 * @return int32_t
 * @retval <STATE_SUCCESS 执行失败, 更多信息请参考@ref aiot_state_api.h
 * @retval >=STATE_SUCCESS 执行成功
 *
 */
int32_t aiot_mqtt_deinit(void **handle);

/**
 * @brief 与MQTT服务器建立连接
 *
 * @details
 *
 * 使用@ref aiot_mqtt_setopt 配置的mqtt连接参数连接mqtt服务器, 使用的建联参数按如下顺序选择
 *
 * 1. 若配置了以下选项, 直接用配置的连接参数连接 @ref AIOT_MQTTOPT_HOST 选项指定的任意MQTT服务器
 *
 *    + @ref AIOT_MQTTOPT_USERNAME
 *    + @ref AIOT_MQTTOPT_PASSWORD
 *    + @ref AIOT_MQTTOPT_CLIENTID
 *
 * 2. 若配置了以下选项, 则强制以阿里云平台的签名算法计算连接参数作为MQTT的用户名/密码, 连接阿里云平台
 *
 *    + @ref AIOT_MQTTOPT_PRODUCT_KEY
 *    + @ref AIOT_MQTTOPT_DEVICE_NAME
 *    + @ref AIOT_MQTTOPT_DEVICE_SECRET
 *
 * @param[in] handle MQTT实例句柄
 *
 * @return int32_t
 * @retval <STATE_SUCCESS 执行失败, 更多信息请参考@ref aiot_state_api.h
 * @retval >=STATE_SUCCESS 执行成功
 *
 * @note
 *
 * 当配置@ref AIOT_MQTTOPT_USERNAME , @ref AIOT_MQTTOPT_PASSWORD 和@ref AIOT_MQTTOPT_CLIENTID 配置自定义连接凭据时,
 *
 * 此函数会忽略@ref AIOT_MQTTOPT_PRODUCT_KEY , @ref AIOT_MQTTOPT_DEVICE_NAME 和@ref AIOT_MQTTOPT_DEVICE_SECRET,
 *
 * 直接使用自定义凭据连接指定的MQTT服务器
 */
int32_t aiot_mqtt_connect(void *handle);

/**
 * @brief 与MQTT服务器断开连接
 *
 * @details
 *
 * 向MQTT服务器发送MQTT DISCONNECT报文, 然后断开网络连接
 *
 * 如果需要再次与MQTT服务器建立连接, 调用@ref aiot_mqtt_connect 即可
 *
 * @param[in] handle MQTT实例句柄
 *
 * @return int32_t
 * @retval <STATE_SUCCESS 执行失败, 更多信息请参考@ref aiot_state_api.h
 * @retval >=STATE_SUCCESS 执行成功
 */
int32_t aiot_mqtt_disconnect(void *handle);

/**
 * @brief 发送MQTT PINGREQ报文, 用于维持心跳
 *
 * @details
 *
 * @ref aiot_mqtt_process 包含了定时发送心跳的机制, 如果有特殊需要的话, 可以使用此函数直接发送心跳报文
 *
 * @param[in] handle MQTT实例句柄
 *
 * @return int32_t
 * @retval <STATE_SUCCESS 执行失败, 更多信息请参考@ref aiot_state_api.h
 * @retval >=STATE_SUCCESS 执行成功
 */
int32_t aiot_mqtt_heartbeat(void *handle);

/**
 * @brief 此函数用于处理定时心跳发送和qos1消息的重传逻辑
 *
 * @details
 *
 * 1. 发送心跳至mqtt broker以维护mqtt连接, 心跳发送间隔由@ref AIOT_MQTTOPT_HEARTBEAT_INTERVAL_MS 配置项控制
 *
 * 2. 如果一条qos1的mqtt PUBLISH报文在@ref AIOT_MQTTOPT_REPUB_TIMEOUT_MS 时间内没有收到mqtt PUBACK应答报文, 该函数会重发此消息, 直到成功为止
 *
 * @param[in] handle MQTT实例句柄
 *
 * @return int32_t
 * @retval <STATE_SUCCESS 执行失败, 更多信息请参考@ref aiot_state_api.h
 * @retval >=STATE_SUCCESS 执行成功
 *
 * @note
 *
 * 该函数为非阻塞, 需要间歇性被调用, 调用间隔应当小于@ref AIOT_MQTTOPT_HEARTBEAT_INTERVAL_MS 和@ref AIOT_MQTTOPT_REPUB_TIMEOUT_MS 时间内没有收到mqtt的最小值,
 *
 * 以确保心跳发送和QoS1消息的重传逻辑正常工作
 */
int32_t aiot_mqtt_process(void *handle);

/**
 * @brief 发送一条PUBLISH报文到MQTT服务器, QoS为0, 用于发布指定的消息
 *
 * @param[in] handle MQTT实例句柄
 * @param[in] topic 指定MQTT PUBLISH报文的topic
 * @param[in] payload 指定MQTT PUBLISH报文的payload
 * @param[in] payload_len 指定MQTT PUBLISH报文的payload_len
 * @param[in] qos 指定mqtt的qos值, 仅支持qos0和qos1
 *
 * @return int32_t
 * @retval <STATE_SUCCESS 执行失败, 更多信息请参考@ref aiot_state_api.h
 * @retval >=STATE_SUCCESS 执行成功
 */
int32_t aiot_mqtt_pub(void *handle, char *topic, uint8_t *payload, uint32_t payload_len, uint8_t qos);

/**
 * @brief 发送一条mqtt SUBSCRIBE报文到MQTT服务器, 用于订阅指定的topic
 *
 * @param[in] handle MQTT实例句柄
 * @param[in] topic 指定MQTT SUBSCRIBE报文的topic
 * @param[in] handler 与topic对应的MQTT PUBLISH报文回调函数, 当有消息发布到topic时, 该回调函数被调用
                    若handler为NULL传入, 则SDK调用@ref AIOT_MQTTOPT_RECV_HANDLER 配置的回调函数
                    若多次调用aiot_mqtt_sub()并对同一topic指定不同的handler, 有消息到达时不同handler都会被调用到
 * @param[in] qos 指定topic期望mqtt服务器支持的最大qos值, 仅支持qos0和qos1
 * @param[in] userdata 可让SDK代为保存的用户上下文, 当回调函数被调用时, 此上下文会通过handler传回给用户
 *                  若未指定该上下文, 那么通过@ref AIOT_MQTTOPT_USERDATA 配置的上下文会通过handler传回给用户
 *
 * @return int32_t
 * @retval <STATE_SUCCESS 执行失败, 更多信息请参考@ref aiot_state_api.h
 * @retval >=STATE_SUCCESS 执行成功
 */
int32_t aiot_mqtt_sub(void *handle, char *topic, aiot_mqtt_recv_handler_t handler, uint8_t qos, void *userdata);

/**
 * @brief 发送一条mqtt UNSUBSCRIBE报文到MQTT服务器, 用于取消订阅指定的topic
 *
 * @param[in] handle MQTT实例句柄
 * @param[in] topic 指定MQTT UNSUBSCRIBE报文的topic
 *
 * @return int32_t
 * @retval <STATE_SUCCESS 执行失败, 更多信息请参考@ref aiot_state_api.h
 * @retval >=STATE_SUCCESS 执行成功
 */
int32_t aiot_mqtt_unsub(void *handle, char *topic);

/**
 * @brief 尝试从网络上接收MQTT报文
 *
 * @details
 *
 * 除了从网络上接收MQTT报文之外, 本函数也包含了重连机制
 *
 * 1. 当MQTT心跳丢失超过@ref AIOT_MQTTOPT_HEARTBEAT_MAX_LOST 配置的次数时, 触发重连机制
 *
 *    + 重连间隔由 @ref AIOT_MQTTOPT_RECONN_INTERVAL_MS 指定
 *
 * 2. 当SDK检测到网络断开时, 触发重连机制
 *
 *    + 重连间隔由 @ref AIOT_MQTTOPT_RECONN_INTERVAL_MS 指定
 *
 * @param[in] handle
 *
 * @retval STATE_SYS_DEPEND_NWK_READ_LESSDATA 执行成功, 此时网络上暂无可以收取的MQTT报文
 * @retval >=STATE_SUCCESS 执行成功
 * @retval 其他返回值 执行失败, 更多信息请参考@ref aiot_state_api.h
 *
 * @note
 *
 * 当网络连接正常并且@ref aiot_mqtt_deinit 未被调用时, 该函数为阻塞, 需要持续被调用
 *
 * 1. 当网络连接断开时, 该函数会立即返回, 此时返回值为@ref STATE_SYS_DEPEND_NWK_CLOSED
 *
 * 2. 当@ref aiot_mqtt_deinit 被调用时, 该函数会立即返回, 此时返回值为@ref STATE_USER_INPUT_EXEC_DISABLED
 */
int32_t aiot_mqtt_recv(void *handle);

#if defined(__cplusplus)
}
#endif

#endif


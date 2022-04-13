/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef __INFRA_STATE_H__
#define __INFRA_STATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define STATE_BASE                                  (0x0000)

/* General: 0x0000 ~ 0x00FF */
/* API works as expected and returns sucess */
/* API 接口按预期完成工作, 并返回成功 */
#define STATE_SUCCESS                               (STATE_BASE - 0x0000)

/* General: 0x0000 ~ 0x00FF */

/* User Input: 0x0100 ~ 0x01FF */
#define STATE_USER_INPUT_BASE                       (-0x0100)

/* User input parameters contain unacceptable NULL pointer */
/* 用户传递给API的参数中含有不合理的空指针参数 */
#define STATE_USER_INPUT_NULL_POINTER               (STATE_USER_INPUT_BASE - 0x0001)
/* Some user input parameter(s) has value out of acceptable range */
/* 用户传递给API的参数中含有超出合理范围的取值 */
#define STATE_USER_INPUT_OUT_RANGE                  (STATE_USER_INPUT_BASE - 0x0002)
/* User input parameters contain unacceptable productKey */
/* 用户传递给API的参数中含有不合法的产品标识(productKey) */
#define STATE_USER_INPUT_PK                         (STATE_USER_INPUT_BASE - 0x0003)
/* User input parameters contain unacceptable productSecret */
/* 用户传递给API的参数中含有不合法的产品密钥(productSecret) */
#define STATE_USER_INPUT_PS                         (STATE_USER_INPUT_BASE - 0x0004)
/* User input parameters contain unacceptable deviceName */
/* 用户传递给API的参数中含有不合法的设备名称(deviceName) */
#define STATE_USER_INPUT_DN                         (STATE_USER_INPUT_BASE - 0x0005)
/* User input parameters contain unacceptable deviceSecret */
/* 用户传递给API的参数中含有不合法的设备密钥(deviceSecret) */
#define STATE_USER_INPUT_DS                         (STATE_USER_INPUT_BASE - 0x0006)
/* User input parameters contain unacceptable HTTP domain name */
/* 用户传递给API的参数中含有不合理的域名, SDK无法对其完成HTTP连接 */
#define STATE_USER_INPUT_HTTP_DOMAIN                (STATE_USER_INPUT_BASE - 0x0007)
/* User input parameters contain unacceptable MQTT domain name */
/* 用户传递给API的参数中含有不合理的域名, SDK无法对其完成MQTT连接 */
#define STATE_USER_INPUT_MQTT_DOMAIN                (STATE_USER_INPUT_BASE - 0x0008)
/* User input parameters contain unacceptable HTTP port */
/* 用户传递给API的参数中含有不合理的HTTP目的端口号 */
#define STATE_USER_INPUT_HTTP_PORT                  (STATE_USER_INPUT_BASE - 0x0009)
/* User input parameters contain unacceptable HTTP timeout */
/* 用户传递给API的参数中含有不合理的HTTP超时时间 */
#define STATE_USER_INPUT_HTTP_TIMEOUT               (STATE_USER_INPUT_BASE - 0x000A)
/* User input parameters contain unacceptable HTTP options */
/* 用户传递给API的参数中含有不合理的HTTP连接选项 */
#define STATE_USER_INPUT_HTTP_OPTION                (STATE_USER_INPUT_BASE - 0x000B)
/* User input parameters contain unacceptable post data */
/* 用户传递给API的参数中, 对HTTP POST含有不合理的报文描述, 如长度为负或空指针等 */
#define STATE_USER_INPUT_HTTP_POST_DATA             (STATE_USER_INPUT_BASE - 0x000C)
/* User input parameters contain unacceptable HTTP URL */
/* 用户传递给API的参数中含有不合理的HTTP目的URL */
#define STATE_USER_INPUT_HTTP_URL                   (STATE_USER_INPUT_BASE - 0x000D)
/* User input parameters contain unacceptable HTTP path */
/* 用户传递给API的参数中含有不合理的HTTP资源路径 */
#define STATE_USER_INPUT_HTTP_PATH                  (STATE_USER_INPUT_BASE - 0x000E)
/* User input parameters contain unacceptable PK/PS/DN/DS */
/* 用户传递给API的参数中含有不合理的产品标识/密钥或设备名称/密钥 */
#define STATE_USER_INPUT_META_INFO                  (STATE_USER_INPUT_BASE - 0x000F)
/* User input parameters contain unacceptable deviceID */
/* 用户传递给API的参数中含有不合理的 deviceID 设备标识符 */
#define STATE_USER_INPUT_DEVID                      (STATE_USER_INPUT_BASE - 0x0010)
/* User input parameters contain unacceptable device type */
/* 用户传递给API的参数中含有不合理的设备类型, 既不是master又不是slave */
#define STATE_USER_INPUT_DEVICE_TYPE                (STATE_USER_INPUT_BASE - 0x0011)
/* User input parameters contain unacceptable message type */
/* 用户传递给 IOT_Linkkit_Report() 等API的参数中含有不合理的消息类型 */
#define STATE_USER_INPUT_MSG_TYPE                   (STATE_USER_INPUT_BASE - 0x0012)
/* User input parameters contain unacceptable value */
/* 用户传递给API的参数中含有其它不合理的取值 */
#define STATE_USER_INPUT_INVALID                    (STATE_USER_INPUT_BASE - 0x0013)
/* User Input: 0x0100 ~ 0x01FF */

/* System: 0x0200 ~ 0x02FF */
#define STATE_SYS_DEPEND_BASE                       (-0x0200)

/* SDK run into exception when invoking HAL_Malloc() */
/* SDK调用的系统适配接口 HAL_Malloc() 返回异常, 未能成功分配内存 */
#define STATE_SYS_DEPEND_MALLOC                     (STATE_SYS_DEPEND_BASE - 0x0001)
/* SDK run into exception when invoking HAL_Kv_Get() */
/* SDK调用的系统适配接口 HAL_Kv_Get() 返回异常, 未能成功根据Key获取Value */
#define STATE_SYS_DEPEND_KV_GET                     (STATE_SYS_DEPEND_BASE - 0x0002)
/* SDK run into exception when invoking HAL_Kv_Set() */
/* SDK调用的系统适配接口 HAL_Kv_Set() 返回异常, 未能成功根据Key写入Value */
#define STATE_SYS_DEPEND_KV_SET                     (STATE_SYS_DEPEND_BASE - 0x0003)
/* SDK run into exception when invoking HAL_Kv_Del() */
/* SDK调用的系统适配接口 HAL_Kv_Del() 返回异常, 未能成功根据Key删除KV对 */
#define STATE_SYS_DEPEND_KV_DELETE                  (STATE_SYS_DEPEND_BASE - 0x0004)
/* SDK run into exception when invoking HAL_MutexCreate() */
/* SDK调用的系统适配接口 HAL_MutexCreate() 返回异常, 未能创建一个互斥锁 */
#define STATE_SYS_DEPEND_MUTEX_CREATE               (STATE_SYS_DEPEND_BASE - 0x0005)
/* SDK run into exception when invoking HAL_MutexLock() */
/* SDK调用的系统适配接口 HAL_MutexLock() 返回异常, 未能成功申请互斥锁 */
#define STATE_SYS_DEPEND_MUTEX_LOCK                 (STATE_SYS_DEPEND_BASE - 0x0006)
/* SDK run into exception when invoking HAL_MutexUnlock() */
/* SDK调用的系统适配接口 HAL_MutexUnlock() 返回异常, 未能成功释放互斥锁 */
#define STATE_SYS_DEPEND_MUTEX_UNLOCK               (STATE_SYS_DEPEND_BASE - 0x0007)
/* SDK run into exception when TX or RX through lower network layer */
/* SDK调用的系统适配接口 HAL_TCP_Read|Write() 或 HAL_SSL_Read|Write() 返回异常, 底层连接已关闭而未能成功进行网络收发 */
#define STATE_SYS_DEPEND_NWK_CLOSE                  (STATE_SYS_DEPEND_BASE - 0x0008)
/* SDK run into timeout when TX or RX through lower network layer */
/* SDK调用的系统适配接口 HAL_TCP_Read() 或 HAL_SSL_Read() 未能成功在预期的时间内得到网络回应 */
#define STATE_SYS_DEPEND_NWK_TIMEOUT                (STATE_SYS_DEPEND_BASE - 0x0009)
/* SDK run into invalid handler when lookup network lower layer connection */
/* SDK进行底层网络收发通信时, 看到系统对TCP连接或TLS连接的标识句柄是非法的 */
#define STATE_SYS_DEPEND_NWK_INVALID_HANDLE         (STATE_SYS_DEPEND_BASE - 0x000A)
/* SDK run into exception when RX through lower network layer */
/* SDK调用的系统适配接口 HAL_TCP_Read() 或 HAL_SSL_Read() 返回异常, 未能成功读取一段内容 */
#define STATE_SYS_DEPEND_NWK_READ_ERROR             (STATE_SYS_DEPEND_BASE - 0x000B)
/* SDK run into exception when invoking HAL_SemaphoreCreate() */
/* SDK调用的系统适配接口 HAL_SemaphoreCreate() 返回异常, 未能成功申请信号量 */
#define STATE_SYS_DEPEND_SEMAPHORE_CREATE           (STATE_SYS_DEPEND_BASE - 0x000C)
/* SDK run into exception when invoking HAL_SemaphoreWait() */
/* SDK调用的系统适配接口 HAL_SemaphoreWait() 返回异常, 未能成功在信号量上睡眠 */
#define STATE_SYS_DEPEND_SEMAPHORE_WAIT             (STATE_SYS_DEPEND_BASE - 0x000D)
/* SDK run into exception when invoking HAL_Snprintf() */
/* SDK调用的系统适配接口 HAL_Snprintf() 返回异常, 未能成功拼接格式化字符串 */
#define STATE_SYS_DEPEND_SNPRINTF                   (STATE_SYS_DEPEND_BASE - 0x000E)
/* SDK run into exception when invoking HAL_Firmware_Persistence_Write() */
/* SDK调用的系统适配接口 HAL_Firmware_Persistence_Write() 返回异常, 未能成功将固件写入ROM */
#define STATE_SYS_DEPEND_FIRMWAIRE_WIRTE            (STATE_SYS_DEPEND_BASE - 0x000F)

/* System: 0x0200 ~ 0x02FF */

/* MQTT: 0x0300 ~ 0x03FF */
#define STATE_MQTT_BASE                             (-0x0300)

/* Deserialized CONNACK from MQTT server says protocol version is unacceptable */
/* 设备发出MQTT建连请求后, 服务器应答的报文表示不能接受请求中的MQTT协议版本 */
#define STATE_MQTT_CONNACK_VERSION_UNACCEPT         (STATE_MQTT_BASE - 0x0001)
/* Deserialized CONNACK from MQTT server says identifier is rejected */
/* 设备发出MQTT建连请求后, 服务器应答的报文表示不能接受请求中的设备标识符 */
#define STATE_MQTT_CONNACK_IDENT_REJECT             (STATE_MQTT_BASE - 0x0002)
/* Deserialized CONNACK from MQTT server says service is not available */
/* 设备发出MQTT建连请求后, 服务器应答的报文表示当前MQTT服务不可用 */
#define STATE_MQTT_CONNACK_SERVICE_NA               (STATE_MQTT_BASE - 0x0003)
/* Deserialized CONNACK from MQTT server says it failed to authorize */
/* 设备发出MQTT建连请求后, 服务器应答的报文表示当前对请求设备未能完成鉴权 */
#define STATE_MQTT_CONNACK_NOT_AUTHORIZED           (STATE_MQTT_BASE - 0x0004)
/* Deserialized CONNACK from MQTT server says username/password is invalid */
/* 设备发出MQTT建连请求后, 服务器应答的报文表示对设备身份鉴权不通过 */
#define STATE_MQTT_CONNACK_BAD_USERDATA             (STATE_MQTT_BASE - 0x0005)
/* Skip current reconnect attemption until next timer expiration */
/* 跳过当前的重连动作, 因为上一次的重连过去的时间太短, 稍晚再进行连接 */
#define STATE_MQTT_WAIT_RECONN_TIMER                (STATE_MQTT_BASE - 0x0006)
/* Reserved buffer is too short when generate device signature for hostname */
/* 计算设备签名时, 为域名预留的字符串缓冲区太短, 不足容纳输出内容 */
#define STATE_MQTT_SIGN_HOSTNAME_BUF_SHORT          (STATE_MQTT_BASE - 0x0007)
/* Reserved buffer is too short when generate device signature for username */
/* 计算设备签名时, 为MQTT的用户名预留的字符串缓冲区太短, 不足容纳输出内容 */
#define STATE_MQTT_SIGN_USERNAME_BUF_SHORT          (STATE_MQTT_BASE - 0x0008)
/* Reserved buffer is too short when generate device signature for clientId */
/* 计算设备签名时, 为MQTT的clientId预留的字符串缓冲区太短, 不足容纳输出内容 */
#define STATE_MQTT_SIGN_CLIENTID_BUF_SHORT          (STATE_MQTT_BASE - 0x0009)
/* Reserved buffer is too short for signature generate source */
/* 计算设备签名时, 为拼接签名来源内容预留的字符串缓冲区太短, 不足容纳输出内容 */
#define STATE_MQTT_SIGN_SOURCE_BUF_SHORT            (STATE_MQTT_BASE - 0x000A)
/* SDK run into exception when invoking lower layer wrapper_mqtt_init() */
/* SDK调用的MQTT建连底层接口 wrapper_mqtt_init() 异常, 初始化失败 */
#define STATE_MQTT_WRAPPER_INIT_FAIL                (STATE_MQTT_BASE - 0x000B)
/* Failed to serialize connect request */
/* 序列化MQTT建连请求报文异常 */
#define STATE_MQTT_SERIALIZE_CONN_ERROR             (STATE_MQTT_BASE - 0x000C)
/* Failed to serialize acknowledge message of publish */
/* 序列化对服务器下推报文的应答消息异常 */
#define STATE_MQTT_SERIALIZE_PUBACK_ERROR           (STATE_MQTT_BASE - 0x000D)
/* Failed to serialize ping request */
/* 序列化MQTT心跳请求报文异常 */
#define STATE_MQTT_SERIALIZE_PINGREQ_ERROR          (STATE_MQTT_BASE - 0x000E)
/* Failed to serialize subscribe request */
/* 序列化MQTT订阅请求报文异常 */
#define STATE_MQTT_SERIALIZE_SUB_ERROR              (STATE_MQTT_BASE - 0x000F)
/* Failed to serialize unsubscribe request */
/* 序列化MQTT取消订阅请求报文异常 */
#define STATE_MQTT_SERIALIZE_UNSUB_ERROR            (STATE_MQTT_BASE - 0x0010)
/* Failed to serialize publish message */
/* 序列化MQTT上报消息报文异常 */
#define STATE_MQTT_SERIALIZE_PUB_ERROR              (STATE_MQTT_BASE - 0x0011)
/* Failed to deserialize connect response */
/* 反序列化MQTT服务器返回的建连应答报文异常 */
#define STATE_MQTT_DESERIALIZE_CONNACK_ERROR        (STATE_MQTT_BASE - 0x0012)
/* Failed to deserialize subscribe response */
/* 反序列化MQTT服务器返回的订阅应答报文异常 */
#define STATE_MQTT_DESERIALIZE_SUBACK_ERROR         (STATE_MQTT_BASE - 0x0013)
/* Failed to deserialize publish response */
/* 反序列化MQTT服务器下推的消息报文异常 */
#define STATE_MQTT_DESERIALIZE_PUB_ERROR            (STATE_MQTT_BASE - 0x0014)
/* Failed to deserialize unsubscribe response */
/* 反序列化MQTT服务器返回的取消订阅应答报文异常 */
#define STATE_MQTT_DESERIALIZE_UNSUBACK_ERROR       (STATE_MQTT_BASE - 0x0015)
/* Failed to read MQTT packet from network */
/* 未能成功从网络上读取服务器下发的MQTT报文内容 */
#define STATE_MQTT_PACKET_READ_ERROR                (STATE_MQTT_BASE - 0x0016)
/* Failed to interpret CONNACK from MQTT server */
/* 未能成功解析从MQTT服务器返回的建连请求应答 */
#define STATE_MQTT_CONNACK_UNKNOWN_ERROR            (STATE_MQTT_BASE - 0x0017)
/* Reserved buffer is too short when retrieve content from network */
/* 从网络协议栈读取MQTT下推报文时, 预留的缓冲区太短, 不足容纳待收取内容 */
#define STATE_MQTT_RX_BUFFER_TOO_SHORT              (STATE_MQTT_BASE - 0x0018)
/* Reserved buffer is too short when compose content going to network */
/* 组装MQTT上报消息准备发给网络协议栈时, 为报文内容预留的缓冲区太短, 不足容纳待发送内容 */
#define STATE_MQTT_TX_BUFFER_TOO_SHORT              (STATE_MQTT_BASE - 0x0019)
/* Reserved buffer is too short when compose topic for MQTT outgoing message */
/* 组装Topic准备进行MQTT发布或者订阅时, 为Topic字段预留的缓冲区太短, 不足容纳组装内容 */
#define STATE_MQTT_TOPIC_BUF_TOO_SHORT              (STATE_MQTT_BASE - 0x001A)
/* Retried time exceeds maximum when perform IOT_MQTT_Construct() */
/* 单独的一次 IOT_MQTT_Construct() 接口工作中, 已多次重试仍未连上服务端, 停止尝试返回 */
#define STATE_MQTT_CONN_RETRY_EXCEED_MAX            (STATE_MQTT_BASE - 0x001B)
/* Re-publish QoS1 message exceeds maximum */
/* 对QoS1类型的MQTT上报消息已多次重试, 仍未得到服务端收到的回应, 放弃重新发送 */
#define STATE_MQTT_QOS1_REPUB_EXCEED_MAX            (STATE_MQTT_BASE - 0x001C)
/* Invalid QoS input when publish MQTT message */
/* 对MQTT消息上报时, 遇到不合理的QoS参数取值 */
#define STATE_MQTT_PUB_QOS_INVALID                  (STATE_MQTT_BASE - 0x001D)
/* Skip current action because MQTT connection is break */
/* 因为当前MQTT连接已断开, 跳过当前可能的发送或接收动作 */
#define STATE_MQTT_IN_OFFLINE_STATUS                (STATE_MQTT_BASE - 0x001E)
/* Receive MQTT packet from network can not be interpret */
/* 从网络上收取到的MQTT报文, 无法解析其报文类型 */
#define STATE_MQTT_RECV_UNKNOWN_PACKET              (STATE_MQTT_BASE - 0x001F)
/* MQTT connection instance exceed allowed maximum */
/* 当前已创建的MQTT连接数量超出限制, 已无法再新建连接 */
#define STATE_MQTT_CLI_EXCEED_MAX                   (STATE_MQTT_BASE - 0x0020)
/* MQTT subscribe instance exceed allowed maximum */
/* 当前已订阅的MQTT主题数量超出限制, 已无法再新建订阅 */
#define STATE_MQTT_SUB_EXCEED_MAX                   (STATE_MQTT_BASE - 0x0021)
/* Invalid MQTT topic can not be interpret */
/* 遇到不合理的MQTT主题格式, 无法进行订阅或发布等动作 */
#define STATE_MQTT_UNEXP_TOPIC_FORMAT               (STATE_MQTT_BASE - 0x0022)
/* Wait too long time after syncronized subscribe request sent */
/* 进行同步的订阅请求时, 已超出限定时长而未得到服务端回应 */
#define STATE_MQTT_SYNC_SUB_TIMEOUT                 (STATE_MQTT_BASE - 0x0023)
/* Last connect request has not been responsed in async protocol stack */
/* 在异步协议栈上工作时, 上次的连接请求尚未得到服务端回应, 跳过当前连接请求 */
#define STATE_MQTT_ASYNC_STACK_CONN_IN_PROG         (STATE_MQTT_BASE - 0x0024)
/* Currrent SDK configuration does not support working with async protocol stack */
/* 当前SDK未被配置为支持在异步协议栈上工作 */
#define STATE_MQTT_ASYNC_STACK_NOT_SUPPORT          (STATE_MQTT_BASE - 0x0025)
/* Got unknown event when work with async protocol stack */
/* SDK在异步协议栈上工作时, 接收到来自协议栈的事件, 但不能理解 */
#define STATE_MQTT_ASYNC_STACK_UNKNOWN_EVENT        (STATE_MQTT_BASE - 0x0026)
/* Report connect relative parameters such as username and password */
/* MQTT发起连接的过程中, 上报连接相关参数如用户名和密码等 */
#define STATE_MQTT_CONN_USER_INFO                   (STATE_MQTT_BASE - 0x0027)
/* Report subscribe relative parameters such as topic string */
/* MQTT发起订阅的过程中, 上报订阅相关参数如Topic等 */
#define STATE_MQTT_SUB_INFO                         (STATE_MQTT_BASE - 0x0028)
/* Report publish relative parameters such as topic string */
/* MQTT发布上报的过程中, 上报发布相关参数如Topic等 */
#define STATE_MQTT_PUB_INFO                         (STATE_MQTT_BASE - 0x0029)

/* MQTT: 0x0300 ~ 0x03FF */

/* WiFi Provision: 0x0400 ~ 0x04FF */
#define STATE_WIFI_BASE                             (-0x0400)
/* WiFi provision got enabled since user press switch button */
/* WiFi配网进入使能状态 */
#define STATE_WIFI_ENABLE_AWSS                      (STATE_WIFI_BASE - 0x0001)
/* WiFi L2 frame is being processed */
/* WiFi配网正在处理1个2层报文 */
#define STATE_WIFI_PROCESS_FRAME                    (STATE_WIFI_BASE - 0x0002)
/* WIFi channel is being operated */
/* WiFi配网正在进行对信道进行切换/锁定等操作 */
#define STATE_WIFI_CHAN_SCAN                        (STATE_WIFI_BASE - 0x0003)
/* Successfully connect to specified SSID with decrypted password */
/* 已使用WiFi配网接收到的密码, 连接WiFi热点成功 */
#define STATE_WIFI_CONNECT_AP_SUCCESS               (STATE_WIFI_BASE - 0x0004)
/* Failed connecting specified SSID with decrypted password */
/* 已使用WiFi配网接收到的密码, 连接WiFi热点失败 */
#define STATE_WIFI_CONNECT_AP_FAILED                (STATE_WIFI_BASE - 0x0005)
/* Should not start new round WiFi provision since last round in progress */
/* 不应启动新的一轮WiFi配网, 因为上一轮仍未结束 */
#define STATE_WIFI_IN_PROGRESS                      (STATE_WIFI_BASE - 0x0006)
/* WiFi provision stopped on request */
/* 已按用户调用API的指令, 停止了WiFi配网进程 */
#define STATE_WIFI_FORCE_STOPPED                    (STATE_WIFI_BASE - 0x0007)
/* WiFi provision is releasing resources */
/* 正在释放WiFi配网所占用的内部资源 */
#define STATE_WIFI_DESTROY                          (STATE_WIFI_BASE - 0x0008)
/* WiFi provision successfully detect hint frame */
/* WiFi配网功能已成功监听到关键信息起始标志帧 */
#define STATE_WIFI_GOT_HINT_FRAME                   (STATE_WIFI_BASE - 0x0009)
/* WiFi provision interpret information encrypt type */
/* WiFi配网中发现关键信息的加密方式是一型一密或一机一密等 */
#define STATE_WIFI_ENCRYPT_TYPE                     (STATE_WIFI_BASE - 0x000A)
/* WiFi provision ran into mismatch CRC */
/* WiFi配网中发现报文的校验和不符合预期 */
#define STATE_WIFI_CRC_ERROR                        (STATE_WIFI_BASE - 0x000B)
/* WiFi provision failed to decrypt WiFi password */
/* WiFi配网中解密WiFi热点的密码失败 */
#define STATE_WIFI_PASSWD_DECODE_FAILED             (STATE_WIFI_BASE - 0x000C)
/* WiFi provision decrypt WiFi password successfully */
/* WiFi配网中解密WiFi热点的密码成功 */
#define STATE_WIFI_PASSWD_DECODE_SUCCESS            (STATE_WIFI_BASE - 0x000D)
/* WiFi configuration statistics report, including overall time, pack number, pack lens */
/* WiFi配网中的统计信息, 比如收到包的数量, 配网时间, 收包报文的总长度*/
#define STATE_WIFI_STATISTIC                        (STATE_WIFI_BASE - 0x000E)
/* Unexpected version info from cellphone when wifi-provision */
/* 手机端发来的配网协议的版本号不符合预期 */
#define STATE_WIFI_UNEXP_PROT_VERSION               (STATE_WIFI_BASE - 0x000F)
/* Debug info during multicast wifi-provision */
/* WiFi组播配网中的调试信息 */
#define STATE_WIFI_MCAST_DEBUG                      (STATE_WIFI_BASE - 0x0010)
/* Debug info during broadcast wifi-provision */
/* WiFi包长编码的一键配网中的调试信息 */
#define STATE_WIFI_BCAST_DEBUG                      (STATE_WIFI_BASE - 0x0011)
/* Debug info during zero-config wifi-provision for enrollee */
/* WiFi零配过程中的待配方调试信息 */
#define STATE_WIFI_ZCONFIG_ENROLLEE_DEBUG           (STATE_WIFI_BASE - 0x0012)
/* The params from registar in zeroconfig Wifi Configuration is invalid */
/* WiFi零配过程中的主配方发来的参数有误 */
#define STATE_WIFI_ZCONFIG_REGISTAR_PARAMS_ERROR    (STATE_WIFI_BASE - 0x0013)
/* Debug info during zero-config wifi-provision for registar */
/* WiFi零配过程中的主配方调试信息 */
#define STATE_WIFI_ZCONFIG_REGISTAR_DEBUG           (STATE_WIFI_BASE - 0x0014)
/* Debug info of p2p mode smartconfig */
/* WiFi p2p配网过程中的调试信息 */
#define STATE_WIFI_P2P_DEBUG                        (STATE_WIFI_BASE - 0x0015)
/* Debug info of dev-ap mode wifi-provision */
/* WiFi 设备热点配网过程中的调试信息 */
#define STATE_WIFI_DEV_AP_DEBUG                     (STATE_WIFI_BASE - 0x0016)
/* WiFi provision claiming other information */
/* WiFi配网中的其它过程信息 */
#define STATE_WIFI_OTHERS                           (STATE_WIFI_BASE - 0x0017)
/* MQTT init failed */
/* WiFi配网中 MQTT初始化失败, MQTT用于零配主配方 */
#define STATE_WIFI_MQTT_INIT_FAILED                 (STATE_WIFI_BASE - 0x0018)
/* CoAP init failed */
/* WiFi配网中 CoAP 初始化失败, CoAP用于所有配网模式 */
#define STATE_WIFI_COAP_INIT_FAILED                 (STATE_WIFI_BASE - 0x0019)
/* CoAP is not inited */
/* WiFi配网中 CoAP Server 未初始化 */
#define STATE_WIFI_COAP_UNINIT                      (STATE_WIFI_BASE - 0x001A)
/* Got an invalid coap resp msg */
/* WiFi配网中 收到CoAP非法应答消息 */
#define STATE_WIFI_COAP_RSP_INVALID                 (STATE_WIFI_BASE - 0x001B)
/* Build CoAP packet failed in wifi-provision */
/* WiFi配网中 CoAP 组包失败 */
#define STATE_WIFI_COAP_PKTBLD_FAILED               (STATE_WIFI_BASE - 0x001C)
/* Got invalid MQTT msg in wifi-provision */
/* WiFi配网中 收到非法 MQTT 消息 */
#define STATE_WIFI_INVALID_MQTT_EVENT               (STATE_WIFI_BASE - 0x001D)
/* Notify devinfo for phone AP */
/* 手机热点配网广播设备信息 */
#define STATE_WIFI_SENT_DEVINFO_NOTIFY              (STATE_WIFI_BASE - 0x001E)
/* Notify connectap event */
/* WiFi配网中广播通知连接ap事件 */
#define STATE_WIFI_SENT_CONNECTAP_NOTIFY            (STATE_WIFI_BASE - 0x001F)
/* Recieve connectap state query msg */
/* WiFi配网中收到查询连接ap状态的包 */
#define STATE_WIFI_GOT_CONNECTAP_QUERY              (STATE_WIFI_BASE - 0x0020)
/* Recieve devinfo query message */
/* WiFi配网中收到查询设备信息的包(手机热点) */
#define STATE_WIFI_GOT_DEVINFO_QUERY                (STATE_WIFI_BASE - 0x0021)
/* Resp Connect AP request */
/* WiFi配网中设备回复手机查询connectap事件 */
#define STATE_WIFI_SENT_CONNECTAP_RESP              (STATE_WIFI_BASE - 0x0022)
/* Resp devinfo request */
/* WiFi配网中设备回复手机查询设备信息事件 */
#define STATE_WIFI_SENT_DEVINFO_RESP                (STATE_WIFI_BASE - 0x0023)
/* WiFi Provision: 0x0400 ~ 0x04FF */


/* COAP: 0x0500 ~ 0x05FF */
#define STATE_COAP_BASE                             (-0x0500)

/* COAP: 0x0500 ~ 0x05FF */

/* HTTP: 0x0600 ~ 0x06FF */
#define STATE_HTTP_BASE                             (-0x0600)
/* Got HTTP response from server, says dynamic register request failed */
/* 使用动态注册/一型一密功能时, 从服务端返回的报文表示, 申请密钥失败 */
#define STATE_HTTP_DYNREG_FAIL_RESP                 (STATE_HTTP_BASE - 0x0001)
/* Got HTTP response from server, which contains invalid deviceSecret */
/* 使用动态注册/一型一密功能时, 从服务端返回的报文中, 解析得到的密钥不合理 */
#define STATE_HTTP_DYNREG_INVALID_DS                (STATE_HTTP_BASE - 0x0002)
/* Reserved buffer is too short when compose pre-auth HTTP request */
/* 组装预认证消息准备发给服务端时, 为HTTP上行报文预留的缓冲区太短, 不足容纳待发送内容 */
#define STATE_HTTP_PREAUTH_REQ_BUF_SHORT            (STATE_HTTP_BASE - 0x0003)
/* Reserved buffer is too short when retrieve pre-auth HTTP response */
/* 收取预认证请求的应答报文时, 为HTTP回应报文预留的缓冲区太短, 不足容纳待接收内容 */
#define STATE_HTTP_PREAUTH_INVALID_RESP             (STATE_HTTP_BASE - 0x0004)
/* Content length field does not exist in HTTP response from server */
/* 从HTTP服务器返回的报文中, 没有找到 Content-Length 字段 */
#define STATE_HTTP_RESP_MISSING_CONTENT_LEN         (STATE_HTTP_BASE - 0x0005)
/* Report preauth relative HTTP request parameters */
/* 进行HTTP预认证的过程中, 上报预认证请求的相关信息 */
#define STATE_HTTP_PREAUTH_REQ                      (STATE_HTTP_BASE - 0x0006)
/* Report preauth relative HTTP respond parameters */
/* 进行HTTP预认证的过程中, 上报预认证应答的相关信息 */
#define STATE_HTTP_PREAUTH_RSP                      (STATE_HTTP_BASE - 0x0007)

/* HTTP: 0x0600 ~ 0x06FF */

/* OTA: 0x0700 ~ 0x07FF */
#define STATE_OTA_BASE                              (-0x0700)

/* OTA: 0x0700 ~ 0x07FF */

/* Bind: 0x0800 ~ 0x08FF */
#define STATE_BIND_BASE                             (-0x0800)
/* Got external token configuration */
/* 绑定功能模块接收到来自外部的Token传入 */
#define STATE_BIND_SET_APP_TOKEN                    (STATE_BIND_BASE - 0x0001)
/* Failed to initialize CoAP server in LAN */
/* 绑定功能模块未能成功开启CoAP服务, 该服务用于局域网内的token传递 */
#define STATE_BIND_COAP_INIT_FAIL                   (STATE_BIND_BASE - 0x0002)
/* Sending bind token to cloud */
/* 正在将绑定token发送给服务端 */
#define STATE_BIND_REPORT_TOKEN                     (STATE_BIND_BASE - 0x0003)
/* Sent bind token to cloud gets success response */
/* 绑定token发送给服务端后, 已得到服务端成功接收的回应 */
#define STATE_BIND_REPORT_TOKEN_SUCCESS             (STATE_BIND_BASE - 0x0004)
/* Sending bind token in LAN */
/* 正在主动将绑定token在局域网内广播 */
#define STATE_BIND_NOTIFY_TOKEN_SENT                (STATE_BIND_BASE - 0x0005)
/* Got token query request from external devices in same LAN */
/* 正在接收到同一局域网内来自其它电子设备的token查询请求 */
#define STATE_BIND_RECV_TOKEN_QUERY                 (STATE_BIND_BASE - 0x0006)
/* Responding bind token in LAN */
/* 正在响应局域网内的token查询请求, 将token发送给其它电子设备 */
#define STATE_BIND_SENT_TOKEN_RESP                  (STATE_BIND_BASE - 0x0007)
/* Got invalid MQTT respond from server after msg report */
/* mqtt信息上报给服务端后, 接收到的回应报文不合法 */
#define STATE_BIND_MQTT_RSP_INVALID                 (STATE_BIND_BASE - 0x0008)
/* Got invalid MQTT msgid from server after token report */
/* 将绑定token上报给服务端后, 接收到的回应报文msgid不是最新的*/
#define STATE_BIND_MQTT_MSGID_INVALID               (STATE_BIND_BASE - 0x0009)
/* Got invalid CoAP respond from client after token notify */
/* 将绑定token上报给手机后, 接收到的回应报文不合法 */
#define STATE_BIND_COAP_RSP_INVALID                 (STATE_BIND_BASE - 0x000A)
/* Got invalid CoAP request from external devices in same LAN */
/* 接收到来自同一局域网内其它电子设备的token查询请求, 但请求报文不合法 */
#define STATE_BIND_COAP_REQ_INVALID                 (STATE_BIND_BASE - 0x000B)
/* Assemble external token failed */
/* app token 组装失败 */
#define STATE_BIND_ASSEMBLE_APP_TOKEN_FAILED        (STATE_BIND_BASE - 0x000C)
/* Register coap topic failed */
/* 注册coap topic失败 */
#define STATE_BIND_COAP_REGISTER_FAILED             (STATE_BIND_BASE - 0x000D)
/* Token is expired, need update */
/* token过期, 需要更新 */
#define STATE_BIND_TOKEN_EXPIRED                    (STATE_BIND_BASE - 0x000E)
/* Sent unbind msg to cloud gets success response */
/* 解绑信息发送给服务端后, 已得到服务端成功接收的回应 */
#define STATE_BIND_REPORT_RESET_SUCCESS             (STATE_BIND_BASE - 0x000F)
/* Got notify msg from cloud server */
/* 收到云端推送的事件, 如绑定与解绑 */
#define STATE_BIND_RECV_CLOUD_NOTIFY                (STATE_BIND_BASE - 0x0010)


/* Device Model: 0x0900 ~ 0x09FF */
#define STATE_DEV_MODEL_BASE                        (-0x0900)

/* Master device already open so skip current open action */
/* 主设备已经打开, 不必再次初始化 */
#define STATE_DEV_MODEL_MASTER_ALREADY_OPEN         (STATE_DEV_MODEL_BASE - 0x0001)
/* Master device already connect so skip current connect action */
/* 主设备已经连接服务端, 不必再次连接 */
#define STATE_DEV_MODEL_MASTER_ALREADY_CONNECT      (STATE_DEV_MODEL_BASE - 0x0002)
/* Master device not open yet so skip current action */
/* 当前动作需要主设备open成功, 但这尚未完成 */
#define STATE_DEV_MODEL_MASTER_NOT_OPEN_YET         (STATE_DEV_MODEL_BASE - 0x0003)
/* Master device not connect yet so skip current action */
/* 当前动作需要主设备连接服务端成功, 但这尚未完成 */
#define STATE_DEV_MODEL_MASTER_NOT_CONNECT_YET      (STATE_DEV_MODEL_BASE - 0x0004)
/* Requested device already created */
/* 当前请求创建的设备已经存在, 不必再次创建 */
#define STATE_DEV_MODEL_DEVICE_ALREADY_EXIST        (STATE_DEV_MODEL_BASE - 0x0005)
/* Requested device not found in list */
/* 当前请求对应的设备未找到, 无法对其操作 */
#define STATE_DEV_MODEL_DEVICE_NOT_FOUND            (STATE_DEV_MODEL_BASE - 0x0006)
/* Requested device can not be deleted */
/* 当前请求对应的设备不可删除, 无法对其进行删除操作 */
#define STATE_DEV_MODEL_SUBD_NOT_DELETEABLE         (STATE_DEV_MODEL_BASE - 0x0007)
/* Requested device not in login status so can not logout */
/* 当前请求对应的设备不在登入状态, 无法对其进行登出操作 */
#define STATE_DEV_MODEL_SUBD_NOT_LOGIN              (STATE_DEV_MODEL_BASE - 0x0008)
/* Internal error happens in device model function */
/* 物模型/子设备管理模块发生内部错误 */
#define STATE_DEV_MODEL_INTERNAL_ERROR              (STATE_DEV_MODEL_BASE - 0x0009)
/* Internal event about MQTT connect happens in device model function */
/* 物模型/子设备管理模块发现MQTT连接已经建立, 跳过再次建连动作 */
#define STATE_DEV_MODEL_INTERNAL_MQTT_DUP_INIT      (STATE_DEV_MODEL_BASE - 0x000A)
/* Internal error about MQTT unconnect happens in device model function */
/* 物模型/子设备管理模块发生MQTT连接未建立的内部错误 */
#define STATE_DEV_MODEL_INTERNAL_MQTT_NOT_INIT_YET  (STATE_DEV_MODEL_BASE - 0x000B)
/* Failed to open handler for cloud abstract layer */
/* 物模型模块中发生连接抽象层初始化失败 */
#define STATE_DEV_MODEL_CM_OPEN_FAILED              (STATE_DEV_MODEL_BASE - 0x000C)
/* Failed to find file descriptor in cloud abstract layer */
/* 物模型模块中未找到连接抽象层的fd */
#define STATE_DEV_MODEL_CM_FD_NOT_FOUND             (STATE_DEV_MODEL_BASE - 0x000D)
/* Internal error about file descriptor happens in device model function */
/* 物模型/子设备管理模块发生fd有关的内部错误 */
#define STATE_DEV_MODEL_CM_FD_ERROR                 (STATE_DEV_MODEL_BASE - 0x000E)
/* Failed to connect MQTT in device model function */
/* 物模型模块中发生MQTT连接建立失败 */
#define STATE_DEV_MODEL_MQTT_CONNECT_FAILED         (STATE_DEV_MODEL_BASE - 0x000F)
/* Got unexpected MQTT message from server in device model */
/* 物模型模块中接收到来自服务端的MQTT下推消息, 但报文内容不符合预期 */
#define STATE_DEV_MODEL_RECV_UNEXP_MQTT_PUB         (STATE_DEV_MODEL_BASE - 0x0010)
/* Got MQTT message from server but its JSON format is wrong */
/* 物模型模块中接收到来自服务端的MQTT下推消息, 但报文内容不是合理的JSON格式 */
#define STATE_DEV_MODEL_WRONG_JSON_FORMAT           (STATE_DEV_MODEL_BASE - 0x0011)
/* Failed to lookup value for specified key when parse JSON */
/* 物模型模块中从JSON报文解析KV对失败 */
#define STATE_DEV_MODEL_GET_JSON_ITEM_FAILED        (STATE_DEV_MODEL_BASE - 0x0012)
/* Service respond does not have correct request context in device model */
/* 物模型模块中发送服务回应报文给服务端时, 没有找到对应的服务端请求上下文 */
#define STATE_DEV_MODEL_SERVICE_CTX_NOT_EXIST       (STATE_DEV_MODEL_BASE - 0x0013)
/* OTA service is not enabled in device model */
/* 物模型模块中发现OTA功能未开启 */
#define STATE_DEV_MODEL_OTA_NOT_ENABLED             (STATE_DEV_MODEL_BASE - 0x0014)
/* OTA service is not initialized correctly in device model */
/* 物模型模块中发现OTA功能未能正确初始化 */
#define STATE_DEV_MODEL_OTA_NOT_INITED              (STATE_DEV_MODEL_BASE - 0x0015)
/* OTA service is initialized but failed in device model */
/* 物模型模块中发现对OTA功能初始化失败 */
#define STATE_DEV_MODEL_OTA_INIT_FAILED             (STATE_DEV_MODEL_BASE - 0x0016)
/* OTA for some sub-device is not finished yet so skip device switching */
/* 子设备OTA时, 由于当前仍有其它子设备OTA尚未完成, 而跳过当前操作 */
#define STATE_DEV_MODEL_OTA_STILL_IN_PROGRESS       (STATE_DEV_MODEL_BASE - 0x0017)
/* OTA firmware downloaded failed to pass integrity check */
/* 物模型模块中发现OTA下载的固件, 未能通过完整性校验 */
#define STATE_DEV_MODEL_OTA_IMAGE_CHECK_FAILED      (STATE_DEV_MODEL_BASE - 0x0018)
/* OTA type is neither cota or fota */
/* 物模型模块中发现OTA操作的类型既不是FOTA, 又不是COTA */
#define STATE_DEV_MODEL_OTA_TYPE_ERROR              (STATE_DEV_MODEL_BASE - 0x0019)
/* OTA fetching from cloud failed to get expected content */
/* 物模型模块中从云端获取下载内容未达预期 */
#define STATE_DEV_MODEL_OTA_FETCH_FAILED            (STATE_DEV_MODEL_BASE - 0x001A)
/* ALCS function failed to initialize itself */
/* 本地控制功能初始化失败 */
#define STATE_DEV_MODEL_ALCS_CONSTRUCT_FAILED       (STATE_DEV_MODEL_BASE - 0x001B)
/* Gateway/Sub-device management function is not configured on */
/* SDK当前未被配置为打开子设备管理/网关功能 */
#define STATE_DEV_MODEL_GATEWAY_NOT_ENABLED         (STATE_DEV_MODEL_BASE - 0x001C)
/* RRPC message contains too long identifier */
/* 物模型模块中发现RRPC请求的ID太长, 超过了可处理的最大长度 */
#define STATE_DEV_MODEL_RRPCID_TOO_LONG             (STATE_DEV_MODEL_BASE - 0x001D)
/* Configured rawdata+solo mode, will not proceed JSON messages */
/* 物模型模块已被配置为单品透传模式, 无法处理JSON报文 */
#define STATE_DEV_MODEL_IN_RAWDATA_SOLO             (STATE_DEV_MODEL_BASE - 0x001E)
/* Duplicated upstream device model messages */
/* 物模型模块中发现重复的上行报文请求 */
#define STATE_DEV_MODEL_DUP_UPSTREAM_MSG            (STATE_DEV_MODEL_BASE - 0x001F)
/* Corresponding upstream record not found for downstream messages */
/* 物模型模块处理下行报文时, 未找到与之对应的上行报文记录 */
#define STATE_DEV_MODEL_UPSTREAM_REC_NOT_FOUND      (STATE_DEV_MODEL_BASE - 0x0020)
/* Got negative respond from cloud in device model */
/* 物模型/子设备管理模块中得到来自云端对请求的拒绝报文 */
#define STATE_DEV_MODEL_REFUSED_BY_CLOUD            (STATE_DEV_MODEL_BASE - 0x0021)
/* Encount unexpected option when invoke dm_opt_get() */
/* 物模型模块中发现用于获取内部信息的option不合理 */
#define STATE_DEV_MODEL_INVALID_DM_OPTION           (STATE_DEV_MODEL_BASE - 0x0022)
/* Encount parsing failure when process URL in respond from cloud */
/* 物模型模块在处理云端回复报文时, 从中解析URL失败 */
#define STATE_DEV_MODEL_URL_SPLIT_FAILED            (STATE_DEV_MODEL_BASE - 0x0023)
/* Failed to parse Alink request or respond messages */
/* 解析Alink上行报文或下行报文失败 */
#define STATE_DEV_MODEL_ALINK_MSG_PARSE_FAILED      (STATE_DEV_MODEL_BASE - 0x0024)
/* Failed to upload device log to cloud */
/* 上传设备日志到云端日志接口失败 */
#define STATE_DEV_MODEL_LOG_REPORT_ERROR            (STATE_DEV_MODEL_BASE - 0x0025)
/* Bypass current topic subscription since auto subscribed */
/* 物模型模块被配置为免订阅模式, 跳过当前的系统Topic订阅 */
#define STATE_DEV_MODEL_IN_AUTOSUB_MODE             (STATE_DEV_MODEL_BASE - 0x0026)
/* Got message from cloud in device model */
/* 物模型/子设备管理模块中, 接收到来自云端的下推报文 */
#define STATE_DEV_MODEL_RX_CLOUD_MESSAGE            (STATE_DEV_MODEL_BASE - 0x0027)
/* Sending message to cloud in device model */
/* 物模型/子设备管理模块中, 正在发送去往云端的上行报文 */
#define STATE_DEV_MODEL_TX_CLOUD_MESSAGE            (STATE_DEV_MODEL_BASE - 0x0028)
/* Got message from local in device model */
/* 物模型/子设备管理模块中, 接收到来自本地控制设备的报文 */
#define STATE_DEV_MODLE_RX_LOCAL_MESSAGE            (STATE_DEV_MODEL_BASE - 0x0029)
/* Sending message to local in device model */
/* 物模型/子设备管理模块中, 正在发送去往本地控制设备的报文 */
#define STATE_DEV_MODLE_TX_LOCAL_MESSAGE            (STATE_DEV_MODEL_BASE - 0x002A)
/* Processing control messages in alcs protocol */
/* 物模型/子设备管理模块中, 正在处理本地控制协议中的控制类消息 */
#define STATE_DEV_MODLE_ALCS_CONTROL                (STATE_DEV_MODEL_BASE - 0x002B)
/* Processing message queue of device model and external modules */
/* 物模型模块中的消息队列正在被插入或删除元素 */
#define STATE_DEV_MODEL_MSGQ_OPERATION              (STATE_DEV_MODEL_BASE - 0x002C)
/* Message queue in device model is full */
/* 物模型模块中的消息队列已满 */
#define STATE_DEV_MODEL_MSGQ_FULL                   (STATE_DEV_MODEL_BASE - 0x002D)
/* Message queue in device model is empty */
/* 物模型模块中的消息队列已空 */
#define STATE_DEV_MODEL_MSGQ_EMPTY                  (STATE_DEV_MODEL_BASE - 0x002E)
/* Cache list in device model get inserted new element */
/* 物模型模块中的会话上下文链表正在被插入新元素 */
#define STATE_DEV_MODEL_CTX_LIST_INSERT             (STATE_DEV_MODEL_BASE - 0x002F)
/* Cache list in device model get removed element */
/* 物模型模块中的会话上下文链表正在被删除某元素 */
#define STATE_DEV_MODEL_CTX_LIST_REMOVE             (STATE_DEV_MODEL_BASE - 0x0030)
/* Element in cache list of device model removed since life-time over */
/* 物模型模块中的会话上下文元素因为闲置时间太长而被老化删除 */
#define STATE_DEV_MODEL_CTX_LIST_FADEOUT            (STATE_DEV_MODEL_BASE - 0x0031)
/* Cache list in device model get filled full */
/* 物模型模块中的会话上下文链表已满 */
#define STATE_DEV_MODEL_CTX_LIST_FULL               (STATE_DEV_MODEL_BASE - 0x0032)
/* Cache list in device model get consumed empty */
/* 物模型模块中的会话上下文链表已空 */
#define STATE_DEV_MODEL_CTX_LIST_EMPTY              (STATE_DEV_MODEL_BASE - 0x0033)
/* Log post to cloud get stopped */
/* 物模型模块中的日志上云已被停止 */
#define STATE_DEV_MODLE_LOG_REPORT_STOP             (STATE_DEV_MODEL_BASE - 0x0034)
/* Log post to cloud get switched */
/* 物模型模块中的日志上云已被调节 */
#define STATE_DEV_MODLE_LOG_REPORT_SWITCH           (STATE_DEV_MODEL_BASE - 0x0035)
/* Log post to cloud get transmitted */
/* 物模型模块中的日志上云消息已被发送 */
#define STATE_DEV_MODLE_LOG_REPORT_SEND             (STATE_DEV_MODEL_BASE - 0x0036)
/* Operating to list of sync request in device model */
/* 物模型模块中的同步请求队列正在被操作 */
#define STATE_DEV_MODEL_SYNC_REQ_LIST               (STATE_DEV_MODEL_BASE - 0x0037)
/* Event of alink protocol processing is happenning */
/* 物模型模块正在处理Alink协议相关的报文 */
#define STATE_DEV_MODEL_ALINK_PROT_EVENT            (STATE_DEV_MODEL_BASE - 0x0038)
/* Failed to transimit alcs packets into local network */
/* 物模型模块中, 在局域网内发送本地控制相关报文失败 */
#define STATE_DEV_MODEL_ALCS_SEND_FAILED            (STATE_DEV_MODEL_BASE - 0x0039)
/* Got invalid alink protocol mqtt packet */
/* 接收到非法的alink协议报文 */
#define STATE_DEV_MODEL_INVALID_ALINK_PAYLOAD       (STATE_DEV_MODEL_BASE - 0x003A)
/* Got invalid requested alink protocol mqtt topic */
/* 收到不符合当前场景的alink协议topic */
#define STATE_DEV_MODEL_INVALID_ALINK_TOPIC         (STATE_DEV_MODEL_BASE - 0x003B)
/* IOT_Linkkit_Yield already stopped */
/* IOT_Linkkit_Yield 已停止运行 */
#define STATE_DEV_MODEL_YIELD_STOPPED               (STATE_DEV_MODEL_BASE - 0x003C)
/* IOT_Linkkit_Yield still running */
/* IOT_Linkkit_Yield 仍在运行 */
#define STATE_DEV_MODEL_YIELD_RUNNING               (STATE_DEV_MODEL_BASE - 0x003D)

/* Device Model: 0x0900 ~ 0x09FF */

#ifdef __cplusplus
}
#endif
#endif  /* __INFRA_STATE_H__ */


/**
 * @file aiot_state_api.h
 * @brief SDK Core状态码头文件, 所有Core中的api返回值均在此列出
 * @date 2019-12-27
 *
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 *
 */

#ifndef _AIOT_STATE_API_H_
#define _AIOT_STATE_API_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief SDK的日志信息输出回调函数原型
 */
typedef int32_t (* aiot_state_logcb_t)(int32_t code, char *message);

/**
 * @brief 设置SDK的日志信息输出使用的回调函数
 *
 * @param handler 日志回调函数
 *
 * @return int32_t 保留
 */
int32_t aiot_state_set_logcb(aiot_state_logcb_t handler);

/**
 * @brief API执行成功
 *
 */
#define STATE_SUCCESS                                               (0x0000)

/**
 * @brief -0x0100~-0x01FF表达SDK检查用户输入参数时反馈的状态码
 *
 */
#define STATE_USER_INPUT_BASE                                       (-0x0100)

/**
 * @brief 用户输入参数中包含非法的空指针
 *
 */
#define STATE_USER_INPUT_NULL_POINTER                               (-0x0101)

/**
 * @brief 用户输入参数中包含越界的值
 *
 */
#define STATE_USER_INPUT_OUT_RANGE                                  (-0x0102)

/**
 * @brief 用户输入的配置项不能被SDK的模块所理解
 *
 */
#define STATE_USER_INPUT_UNKNOWN_OPTION                             (-0x0103)

/**
 * @brief 用户输入参数中缺少productKey
 *
 */
#define STATE_USER_INPUT_MISSING_PRODUCT_KEY                        (-0x0104)

/**
 * @brief 用户输入参数中缺少deviceName
 *
 */
#define STATE_USER_INPUT_MISSING_DEVICE_NAME                        (-0x0105)

/**
 * @brief 用户输入参数中缺少deviceSecret
 *
 */
#define STATE_USER_INPUT_MISSING_DEVICE_SECRET                      (-0x0106)

/**
 * @brief 用户输入参数中缺少productSecret
 *
 */
#define STATE_USER_INPUT_MISSING_PRODUCT_SECRET                     (-0x0107)

/**
 * @brief 用户输入参数中缺少域名地址或IP地址
 *
 */
#define STATE_USER_INPUT_MISSING_HOST                               (-0x0108)

/**
 * @brief 用户已调用销毁函数销毁了实例(如@ref aiot_mqtt_deinit), 其余对该实例操作的API不应该再被调用
 *
 */
#define STATE_USER_INPUT_EXEC_DISABLED                              (-0x0109)

/**
 * @brief 用户输入的JSON字符串解析失败
 *
 */
#define STATE_USER_INPUT_JSON_PARSE_FAILED                          (-0x010A)

/**
 * @brief -0x0200~-0x02FF表达SDK调用系统依赖函数时反馈的状态码
 *
 */
#define STATE_SYS_DEPEND_BASE                                       (-0x0200)

/**
 * @brief @ref aiot_sysdep_portfile_t::core_sysdep_malloc 申请内存失败
 *
 */
#define STATE_SYS_DEPEND_MALLOC_FAILED                              (-0x0201)

/**
 * @brief @ref aiot_sysdep_portfile_t::core_sysdep_network_setopt 的某个cred之外的入参非法
 *
 */
#define STATE_SYS_DEPEND_NWK_INVALID_OPTION                         (-0x0202)

/**
 * @brief @ref aiot_sysdep_portfile_t::core_sysdep_network_establish 建立网络失败
 *
 */
#define STATE_SYS_DEPEND_NWK_EST_FAILED                             (-0x0203)

/**
 * @brief SDK检测到网络已断开
 *
 */
#define STATE_SYS_DEPEND_NWK_CLOSED                                 (-0x0204)

/**
 * @brief SDK从网络上实际读取的数据比期望读取的少
 *
 */
#define STATE_SYS_DEPEND_NWK_READ_LESSDATA                          (-0x0205)

/**
 * @brief SDK向网络上写入的实际数据比期望写入的少
 *
 */
#define STATE_SYS_DEPEND_NWK_WRITE_LESSDATA                         (-0x0206)

/**
 * @brief @ref aiot_sysdep_portfile_t::core_sysdep_network_recv 超时返回
 *
 */
#define STATE_SYS_DEPEND_NWK_READ_OVERTIME                          (-0x0207)

/**
 * @brief @ref aiot_sysdep_portfile_t::core_sysdep_network_setopt 的cred入参非法
 *
 */
#define STATE_SYS_DEPEND_NWK_INVALID_CRED                           (-0x0208)

/**
 * @brief @ref aiot_sysdep_portfile_t::core_sysdep_network_send 发送数据遭遇异常
 *
 */
#define STATE_SYS_DEPEND_NWK_SEND_ERR                               (-0x0209)

/**
 * @brief @ref aiot_sysdep_portfile_t::core_sysdep_network_recv 接收数据遭遇异常
 *
 */
#define STATE_SYS_DEPEND_NWK_RECV_ERR                               (-0x020A)

/**
 * @brief -0x0300~-0x03FF表达SDK在MQTT模块内的状态码
 *
 */
#define STATE_MQTT_BASE                                             (-0x0300)

/**
 * @brief MQTT尝试建立连接时, 从服务端返回的CONNACK报文格式错误
 *
 */
#define STATE_MQTT_CONNACK_FMT_ERROR                                (-0x0301)

/**
 * @brief MQTT建连时, 服务端报错与当前客户端使用的MQTT协议版本不兼容
 *
 */
#define STATE_MQTT_CONNACK_RCODE_UNACCEPTABLE_PROTOCOL_VERSION      (-0x0302)

/**
 * @brief MQTT建连时, 服务端报错服务不可用, 可能是clientId取值不正确或心跳间隔不合理
 *
 */
#define STATE_MQTT_CONNACK_RCODE_SERVER_UNAVAILABLE                 (-0x0303)

/**
 * @brief MQTT建连时, 服务端返回用户名密码不合法
 *
 */
#define STATE_MQTT_CONNACK_RCODE_BAD_USERNAME_PASSWORD              (-0x0304)

/**
 * @brief MQTT建连时, 服务端返回认证失败, 用户名或者密码错误, 一般为三元组错误导致
 *
 */
#define STATE_MQTT_CONNACK_RCODE_NOT_AUTHORIZED                     (-0x0305)

/**
 * @brief MQTT建连时, 服务端返回未知的CONNACK报文
 *
 */
#define STATE_MQTT_CONNACK_RCODE_UNKNOWN                            (-0x0306)

/**
 * @brief MQTT缓存QoS1消息时, 检测到packetId发生卷绕
 *
 */
#define STATE_MQTT_PUBLIST_PACKET_ID_ROLL                           (-0x0307)

/**
 * @brief MQTT发布或订阅时, 检测到Topic格式不符合协议规范
 *
 */
#define STATE_MQTT_TOPIC_INVALID                                    (-0x0308)

/**
 * @brief MQTT发布或订阅或取消订阅时, SDK反馈Topic内容的状态码
 *
 */
#define STATE_MQTT_LOG_TOPIC                                        (-0x0309)

/**
 * @brief MQTT收发消息时, SDK反馈MQTT消息内容的状态码
 *
 */
#define STATE_MQTT_LOG_HEXDUMP                                      (-0x030A)

/**
 * @brief MQTT连接建立成功
 *
 */
#define STATE_MQTT_CONNECT_SUCCESS                                  (-0x030B)

/**
 * @brief SDK读取到的MQTT报文含有不符合协议规范的报文长度字段
 *
 */
#define STATE_MQTT_MALFORMED_REMAINING_LEN                          (-0x030C)

/**
 * @brief SDK读取到的MQTT报文不符合按协议规范所解析的字节数
 *
 */
#define STATE_MQTT_MALFORMED_REMAINING_BYTES                        (-0x030D)

/**
 * @brief SDK读取到尚不支持的MQTT报文类型
 *
 */
#define STATE_MQTT_PACKET_TYPE_UNKNOWN                              (-0x030E)

/**
 * @brief MQTT订阅或取消订阅时, 从服务端得到操作失败的回应
 *
 */
#define STATE_MQTT_SUBACK_RCODE_FAILURE                             (-0x030F)

/**
 * @brief MQTT订阅或取消订阅时, 从服务端得到无法解析的应答
 *
 */
#define STATE_MQTT_SUBACK_RCODE_UNKNOWN                             (-0x0310)

/**
 * @brief MQTT接收消息时, 消息中的Topic与可理解的Topic列表匹配失败
 *
 */
#define STATE_MQTT_TOPIC_COMPARE_FAILED                             (-0x0311)

/**
 * @brief 执行@ref aiot_mqtt_deinit 时, 为等待其他API执行结束, 超过设定的超时时间, MQTT实例销毁失败
 *
 */
#define STATE_MQTT_DEINIT_TIMEOUT                                   (-0x0312)

/**
 * @brief MQTT主动连接服务器相关的日志状态码
 *
 */
#define STATE_MQTT_LOG_CONNECT                                      (-0x0313)

/**
 * @brief MQTT断线后, 自动重连服务器相关的日志状态码
 *
 */
#define STATE_MQTT_LOG_RECONNECTING                                 (-0x0314)

/**
 * @brief MQTT连接服务器超时相关的日志状态码
 *
 */
#define STATE_MQTT_LOG_CONNECT_TIMEOUT                              (-0x0315)

/**
 * @brief MQTT主动与服务器断开连接相关的日志状态码
 *
 */
#define STATE_MQTT_LOG_DISCONNECT                                   (-0x0316)

/**
 * @brief MQTT连接服务器时, 使用的用户名相关的日志状态码
 *
 */
#define STATE_MQTT_LOG_USERNAME                                     (-0x0317)

/**
 * @brief MQTT连接服务器时, 使用的密码相关的日志状态码
 *
 */
#define STATE_MQTT_LOG_PASSWORD                                     (-0x0318)

/**
 * @brief MQTT连接服务器时, 使用的clientId相关的日志状态码
 *
 */
#define STATE_MQTT_LOG_CLIENTID                                     (-0x0319)

/**
 * @brief MQTT连接服务器时, 使用的PSK-TLS密钥相关的日志状态码
 *
 */
#define STATE_MQTT_LOG_TLS_PSK                                      (-0x031A)

/**
 * @brief MQTT发布或订阅或取消订阅时, Topic的长度超出物联网平台的限制, 中止执行
 *
 */
#define STATE_MQTT_TOPIC_TOO_LONG                                   (-0x031B)

/**
 * @brief MQTT发布消息时, Payload的长度超出物联网平台的限制, 中止执行
 *
 */
#define STATE_MQTT_PUB_PAYLOAD_TOO_LONG                             (-0x031C)

/**
 * @brief MQTT连接服务器时, 使用的备用ip地址
 *
 */
#define STATE_MQTT_LOG_BACKUP_IP                                    (-0x031D)

/**
 * @brief 接收到非法的MQTT PINRESP报文
 * 
 */
#define STATE_MQTT_RECV_INVALID_PINRESP_PACKET                      (-0x031E)

/**
 * @brief 接收到非法的MQTT PUBLISH报文
 * 
 */
#define STATE_MQTT_RECV_INVALID_PUBLISH_PACKET                      (-0x031F)

/**
 * @brief 接收到非法的MQTT PUBACK报文
 * 
 */
#define STATE_MQTT_RECV_INVALID_PUBACK_PACKET                       (-0x0320)

/**
 * @brief -0x0400~-0x04FF表达SDK在HTTP模块内的状态码
 *
 */
#define STATE_HTTP_BASE                                             (-0x0400)

/**
 * @brief 解析收到的HTTP报文时, 无法获取有效的状态行, 得不到HTTP StatusCode
 *
 */
#define STATE_HTTP_STATUS_LINE_INVALID                              (-0x0401)

/**
 * @brief 解析收到的HTTP报文时, 报文的body部分已接收完毕, 没有更多数据
 *
 */
#define STATE_HTTP_READ_BODY_FINISHED                               (-0x0402)

/**
 * @brief 执行@ref aiot_http_deinit 时, 等待其他API执行结束超过设定的超时时间, HTTP实例销毁失败
 *
 */
#define STATE_HTTP_DEINIT_TIMEOUT                                   (-0x0403)

/**
 * @brief 因为HTTP认证应答的StatusCode不是200, 认证失败
 *
 */
#define STATE_HTTP_AUTH_CODE_FAILED                                 (-0x0404)

/**
 * @brief 因为HTTP认证应答接收未完成, 认证失败
 *
 */
#define STATE_HTTP_AUTH_NOT_FINISHED                                (-0x0405)

/**
 * @brief 因为HTTP认证应答中未能解析到Token, 认证失败
 *
 */
#define STATE_HTTP_AUTH_TOKEN_FAILED                                (-0x0406)

/**
 * @brief 设备尚未认证, 需要先调用@ref aiot_http_auth 进行设备认证
 *
 */
#define STATE_HTTP_NEED_AUTH                                        (-0x0407)

/**
 * @brief HTTP应答数据接收未完成, 需确认网络是否异常
 *
 */
#define STATE_HTTP_RECV_NOT_FINISHED                                (-0x0408)

/**
 * @brief 内部buffer长度过短, 需调用@ref aiot_http_setopt 配置@ref AIOT_HTTPOPT_HEADER_BUFFER_LEN 选项增大buffer长度
 *
 */
#define STATE_HTTP_HEADER_BUFFER_TOO_SHORT                          (-0x0409)

/**
 * @brief HTTP首部接收异常
 *
 */
#define STATE_HTTP_HEADER_INVALID                                   (-0x040A)

/**
 * @brief HTTP首部发送相关日志使用的状态码
 *
 */
#define STATE_HTTP_LOG_SEND_HEADER                                  (-0x040B)

/**
 * @brief HTTP内容发送相关日志使用的状态码
 *
 */
#define STATE_HTTP_LOG_SEND_CONTENT                                 (-0x040C)

/**
 * @brief HTTP首部接收相关日志使用的状态码
 *
 */
#define STATE_HTTP_LOG_RECV_HEADER                                  (-0x040D)

/**
 * @brief HTTP内容接收相关日志使用的状态码
 *
 */
#define STATE_HTTP_LOG_RECV_CONTENT                                 (-0x040E)

/**
 * @brief HTTP连接断开相关日志使用的状态码
 *
 */
#define STATE_HTTP_LOG_DISCONNECT                                   (-0x040F)

/**
 * @brief HTTP认证交互相关日志使用的状态码
 *
 */
#define STATE_HTTP_LOG_AUTH                                         (-0x0410)

/**
 * @brief 因为HTTP应答报文的内容不符合预期, 认证失败
 *
 */
#define STATE_HTTP_AUTH_NOT_EXPECTED                                (-0x0411)

/**
 * @brief HTTP报文的负载部分为空, 接收已完成
 *
 */
#define STATE_HTTP_READ_BODY_EMPTY                                  (-0x0412)

/**
 * @brief -0x0F00~-0x0FFF表达SDK在系统底层依赖模块内的状态码
 *
 */
#define STATE_PORT_BASE                                             (-0x0F00)

/**
 * @brief 底层依赖函数遇到非法的空指针参数, 执行失败
 *
 */
#define STATE_PORT_INPUT_NULL_POINTER                               (-0x0F01)

/**
 * @brief 底层依赖函数遇到输入参数超出合理值域, 执行失败
 *
 */
#define STATE_PORT_INPUT_OUT_RANGE                                  (-0x0F02)

/**
 * @brief 底层依赖函数遇到申请内存错误, 执行失败
 *
 */
#define STATE_PORT_MALLOC_FAILED                                    (-0x0F03)

/**
 * @brief 底层依赖函数遇到域名地址或IP地址缺失, 执行失败
 *
 */
#define STATE_PORT_MISSING_HOST                                     (-0x0F04)

/**
 * @brief 底层依赖函数遇到TCP客户端建立过程尚未实现, 执行失败
 *
 */
#define STATE_PORT_TCP_CLIENT_NOT_IMPLEMENT                         (-0x0F05)

/**
 * @brief 底层依赖函数遇到TCP服务端建立过程尚未实现, 执行失败
 *
 */
#define STATE_PORT_TCP_SERVER_NOT_IMPLEMENT                         (-0x0F06)

/**
 * @brief 底层依赖函数遇到UDP客户端建立过程尚未实现, 执行失败
 *
 */
#define STATE_PORT_UDP_CLIENT_NOT_IMPLEMENT                         (-0x0F07)

/**
 * @brief 底层依赖函数遇到UDP服务端建立过程尚未实现, 执行失败
 *
 */
#define STATE_PORT_UDP_SERVER_NOT_IMPLEMENT                         (-0x0F08)

/**
 * @brief 底层依赖函数遇到不能理解的网络层设置选项, 执行失败
 *
 */
#define STATE_PORT_NETWORK_UNKNOWN_OPTION                           (-0x0F09)

/**
 * @brief 底层依赖函数遇到不能理解的socket类型, 执行失败
 *
 */
#define STATE_PORT_NETWORK_UNKNOWN_SOCKET_TYPE                      (-0x0F0A)

/**
 * @brief 底层依赖函数遇到域名DNS解析错误, 执行失败
 *
 */
#define STATE_PORT_NETWORK_DNS_FAILED                               (-0x0F0B)

/**
 * @brief 底层依赖函数建立网络连接时遇到socket创建错误, 执行失败
 *
 */
#define STATE_PORT_NETWORK_SOCKET_CREATE_FAILED                     (-0x0F0C)

/**
 * @brief 底层依赖函数建立网络连接时遇到socket配置错误, 执行失败
 *
 */
#define STATE_PORT_NETWORK_SOCKET_CONFIG_FAILED                     (-0x0F0D)

/**
 * @brief 底层依赖函数建立网络连接时遇到bind错误, 执行失败
 *
 */
#define STATE_PORT_NETWORK_SOCKET_BIND_FAILED                       (-0x0F0E)

/**
 * @brief 底层依赖函数遇到TCP连接超时未成功, 执行失败
 *
 */
#define STATE_PORT_NETWORK_CONNECT_TIMEOUT                          (-0x0F0F)

/**
 * @brief 底层依赖函数遇到TCP连接建立错误, 执行失败
 *
 */
#define STATE_PORT_NETWORK_CONNECT_FAILED                           (-0x0F10)

/**
 * @brief 底层依赖函数遇到套接字select错误, 执行失败
 *
 */
#define STATE_PORT_NETWORK_SELECT_FAILED                            (-0x0F11)

/**
 * @brief 底层依赖函数遇到网络层发送数据错误, 执行失败
 *
 */
#define STATE_PORT_NETWORK_SEND_FAILED                              (-0x0F12)

/**
 * @brief 底层依赖函数遇到网络层接收数据错误, 执行失败
 *
 */
#define STATE_PORT_NETWORK_RECV_FAILED                              (-0x0F13)

/**
 * @brief 底层依赖函数发送数据时遇到连接已关闭, 执行失败
 *
 */
#define STATE_PORT_NETWORK_SEND_CONNECTION_CLOSED                   (-0x0F14)

/**
 * @brief 底层依赖函数接收数据时遇到连接已关闭, 执行失败
 *
 */
#define STATE_PORT_NETWORK_RECV_CONNECTION_CLOSED                   (-0x0F15)

/**
 * @brief 底层依赖函数遇到不能理解的安全凭据选项, 执行失败
 *
 */
#define STATE_PORT_TLS_INVALID_CRED_OPTION                          (-0x0F16)

/**
 * @brief 底层依赖函数遇到不合法的最大TLS分片长度配置, 执行失败
 *
 */
#define STATE_PORT_TLS_INVALID_MAX_FRAGMENT                         (-0x0F17)

/**
 * @brief 底层依赖函数遇到不合法的服务端证书, 执行失败
 *
 */
#define STATE_PORT_TLS_INVALID_SERVER_CERT                          (-0x0F18)

/**
 * @brief 底层依赖函数遇到不合法的客户端证书, 执行失败
 *
 */
#define STATE_PORT_TLS_INVALID_CLIENT_CERT                          (-0x0F19)

/**
 * @brief 底层依赖函数遇到不合法的客户端密钥, 执行失败
 *
 */
#define STATE_PORT_TLS_INVALID_CLIENT_KEY                           (-0x0F1A)

/**
 * @brief 底层依赖函数建立TLS连接时遇到socket创建错误, 执行失败
 *
 */
#define STATE_PORT_TLS_SOCKET_CREATE_FAILED                         (-0x0F1B)

/**
 * @brief 底层依赖函数建立TLS连接时遇到socket连接建立错误, 执行失败
 *
 */
#define STATE_PORT_TLS_SOCKET_CONNECT_FAILED                        (-0x0F1C)

/**
 * @brief 底层依赖函数建立TLS连接时遇到握手失败, 执行失败
 *
 */
#define STATE_PORT_TLS_INVALID_RECORD                               (-0x0F1D)

/**
 * @brief 底层依赖函数在TLS连接上, 遇到数据接收错误, 执行失败
 *
 */
#define STATE_PORT_TLS_RECV_FAILED                                  (-0x0F1E)

/**
 * @brief 底层依赖函数在TLS连接上, 遇到数据发送错误, 执行失败
 *
 */
#define STATE_PORT_TLS_SEND_FAILED                                  (-0x0F1F)

/**
 * @brief 底层依赖函数在TLS连接上, 接收数据时遇到连接已关闭, 执行失败
 *
 */
#define STATE_PORT_TLS_RECV_CONNECTION_CLOSED                       (-0x0F20)

/**
 * @brief 底层依赖函数在TLS连接上, 发送数据时遇到连接已关闭, 执行失败
 *
 */
#define STATE_PORT_TLS_SEND_CONNECTION_CLOSED                       (-0x0F21)

/**
 * @brief 底层依赖函数建立TLS连接时遇到PSK配置错误, 执行失败
 *
 */
#define STATE_PORT_TLS_CONFIG_PSK_FAILED                            (-0x0F22)

/**
 * @brief 底层依赖函数建立TLS连接时遇到非法记录以外的握手错误, 执行失败
 *
 */
#define STATE_PORT_TLS_INVALID_HANDSHAKE                            (-0x0F23)

/**
 * @brief DTLS握手时设置PSK配置错误, 执行失败
 *
 */
#define STATE_PORT_DTLS_CONFIG_PSK_FAILED                           (-0x0F24)

/**
 * @brief DTLS握手失败, 执行失败
 *
 */
#define STATE_PORT_DTLS_HANDSHAKE_FAILED                            (-0x0F25)

/**
 * @brief DTLS建立连接时失败, 执行失败
 *
 */
#define STATE_PORT_NETWORK_DTLS_CONNECT_FAILED                      (-0x0F26)

/**
 * @brief 之前的DTLS握手还在进行中
 *
 */
#define STATE_PORT_DTLS_HANDSHAKE_IN_PROGRESS                       (-0x0F27)


#if defined(__cplusplus)
}
#endif

#endif /* #ifndef _AIOT_STATE_API_H_ */


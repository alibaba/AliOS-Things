/**
 * @file dynregmq_private.h
 * @brief dynregmq模块内部的宏定义和数据结构声明, 不面向其它模块, 更不面向用户
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
#ifndef __DYNREGMQ_PRIVATE_H__
#define __DYNREGMQ_PRIVATE_H__

#if defined(__cplusplus)
extern "C" {
#endif

/* 用这种方式包含标准C库的头文件 */
#include "core_stdinc.h"

#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"
#include "aiot_dynregmq_api.h"      /* 内部头文件是用户可见头文件的超集 */

typedef struct {
    uint32_t code;
    uint8_t *content;
    uint32_t content_len;
    uint32_t content_total_len;
} core_mqtt_response_t;

/* 定义dynregmq模块内部的会话句柄结构体, SDK用户不可见, 只能得到void *handle类型的指针 */
typedef struct {

    aiot_sysdep_portfile_t     *sysdep;             /* 底层依赖回调合集的引用指针 */
    aiot_sysdep_network_cred_t *cred;               /* 指向当前连接使用的安全凭据 */

    char       *host;                               /* 会话目标服务器域名 */
    uint16_t    port;                               /* 会话目标服务器端口 */
    char       *product_key;
    char       *product_secret;
    char       *device_name;
    uint8_t     flag_nowhitelist;                   /* 是否使用免白名单功能 */
    char
    *instance_id;                        /* 实例ID，当用户使用自购实例，且使用免白名单方式时，需设置实例ID */

    aiot_dynregmq_recv_handler_t    recv_handler;   /* 组件从协议栈读到内容时, 通知用户的回调 */
    void       *userdata;                           /* 组件调用以上2个 dynregmq_handler 时的入参之一 */

    uint32_t    recv_timeout_ms;                    /* 从协议栈收包时最长等待时间 */
    uint32_t    send_timeout_ms;                    /* 向协议栈写入时最长花费时间 */
    uint32_t    timeout_ms;
    uint32_t    deinit_timeout_ms;

    /*---- 以上都是用户在API可配 ----*/

    /*---- 以下都是DYNREGMQ在内部使用, 用户无感知 ----*/
    void        *mqtt_handle;
    uint8_t     flag_completed;

    uint8_t     exec_enabled;
    uint32_t    exec_count;
    void       *data_mutex;     /* 保护本地的数据结构 */

} dynregmq_handle_t;

#define DYNREGMQ_MODULE_NAME                      "dynregmq"  /* 用于内存统计的模块名字符串 */

#define DYNREGMQ_DEFAULT_TIMEOUT_MS               (5 * 1000)
#define DYNREGMQ_DEFAULT_DEINIT_TIMEOUT_MS        (2 * 1000)
#define DYNREGMQ_DEFAULT_RECV_TIMEOUT             (5 * 1000)
#define DYNREGMQ_DEFAULT_SEND_TIMEOUT             (5 * 1000)

#define DYNREGMQ_DEINIT_INTERVAL_MS               (100)

#if defined(__cplusplus)
}
#endif
#endif  /* __DYNREGMQ_PRIVATE_H__ */


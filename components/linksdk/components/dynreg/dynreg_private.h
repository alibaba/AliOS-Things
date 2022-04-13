/**
 * @file dynreg_private.h
 * @brief dynreg模块内部的宏定义和数据结构声明, 不面向其它模块, 更不面向用户
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
#ifndef __DYNREG_PRIVATE_H__
#define __DYNREG_PRIVATE_H__

#if defined(__cplusplus)
extern "C" {
#endif

/* 用这种方式包含标准C库的头文件 */
#include "core_stdinc.h"

/* TODO: 这一段列出需要包含SDK其它模块头文件, 与上一段落以1个空行隔开 */
#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "core_http.h"
#include "aiot_dynreg_api.h"      /* 内部头文件是用户可见头文件的超集 */

/* TODO: 定义dynreg模块内部的会话句柄结构体, SDK用户不可见, 只能得到void *handle类型的指针 */
typedef struct {

    aiot_sysdep_portfile_t     *sysdep;             /* 底层依赖回调合集的引用指针 */
    aiot_sysdep_network_cred_t *cred;               /* 指向当前连接使用的安全凭据 */

    char       *host;                               /* 会话目标服务器域名 */
    uint16_t    port;                               /* 会话目标服务器端口 */
    char       *product_key;
    char       *product_secret;
    char       *device_name;

    aiot_dynreg_recv_handler_t    recv_handler;       /* 组件从协议栈读到内容时, 通知用户的回调 */
    void *userdata;                                /* 组件调用以上2个 dynreg_handler 时的入参之一 */

    uint32_t    recv_timeout_ms;                       /* 从协议栈收包时最长等待时间 */
    uint32_t    send_timeout_ms;                       /* 向协议栈写入时最长花费时间 */
    uint32_t    timeout_ms;
    uint32_t    deinit_timeout_ms;

    /*---- 以上都是用户在API可配 ----*/

    /*---- 以下都是DYNREG在内部使用, 用户无感知 ----*/
    void *http_handle;
    core_http_response_t response;
    uint32_t    response_body_len;
    uint8_t     exec_enabled;
    uint32_t    exec_count;

    void       *data_mutex;     /* 保护本地的数据结构 */

} dynreg_handle_t;

#define DYNREG_MODULE_NAME                      "dynreg"  /* 用于内存统计的模块名字符串 */

#define DYNREG_DEFAULT_TIMEOUT_MS               (5 * 1000)
#define DYNREG_DEFAULT_DEINIT_TIMEOUT_MS        (2 * 1000)
#define DYNREG_DEFAULT_RECV_TIMEOUT             (5 * 1000)
#define DYNREG_DEFAULT_SEND_TIMEOUT             (5 * 1000)

#define DYNREG_PATH                             "/auth/register/device"

#define DYNREG_DEINIT_INTERVAL_MS               (100)
#define DYNREG_RESPONSE_BODY_LEN                (192)

#if defined(__cplusplus)
}
#endif
#endif  /* __DYNREG_PRIVATE_H__ */


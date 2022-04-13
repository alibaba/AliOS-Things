/**
 * @file ntp_private.h
 * @brief ntp模块内部的宏定义和数据结构声明, 不面向其它模块, 更不面向用户
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
#ifndef __NTP_PRIVATE_H__
#define __NTP_PRIVATE_H__

#if defined(__cplusplus)
extern "C" {
#endif

/* 用这种方式包含标准C库的头文件 */
#include "core_stdinc.h"

/* TODO: 这一段列出需要包含SDK其它模块头文件, 与上一段落以1个空行隔开 */
#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_ntp_api.h"      /* 内部头文件是用户可见头文件的超集 */

/* TODO: 定义ntp模块内部的会话句柄结构体, SDK用户不可见, 只能得到void *handle类型的指针 */
typedef struct {
    aiot_sysdep_portfile_t     *sysdep;             /* 底层依赖回调合集的引用指针 */
    void *mqtt_handle;

    int8_t time_zone;
    uint32_t deinit_timeout_ms;

    aiot_ntp_recv_handler_t    recv_handler;       /* 组件从协议栈读到内容时, 通知用户的回调 */
    aiot_ntp_event_handler_t   event_handler;
    void *userdata;                                 /* 组件调用以上2个 ntp_handler 时的入参之一 */

    /*---- 以上都是用户在API可配 ----*/
    void *data_mutex;

    uint8_t exec_enabled;
    uint32_t exec_count;

} ntp_handle_t;

#define NTP_MODULE_NAME                      "ntp"  /* 用于内存统计的模块名字符串 */

#define NTP_DEFAULT_DEINIT_TIMEOUT_MS        (2 * 1000)
#define NTP_DEFAULT_TIME_ZONE                (0)

#define NTP_REQUEST_TOPIC_FMT                "/ext/ntp/%s/%s/request"
#define NTP_REQUEST_PAYLOAD_FMT              "{\"deviceSendTime\":\"%s\"}"
#define NTP_RESPONSE_TOPIC_FMT               "/ext/ntp/%s/%s/response"

#define NTP_DEINIT_INTERVAL_MS               (100)

#if defined(__cplusplus)
}
#endif
#endif  /* __NTP_PRIVATE_H__ */


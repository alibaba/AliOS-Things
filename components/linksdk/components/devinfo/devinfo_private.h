/**
 * @file devinfo_private.h
 * @brief devinfo模块内部的宏定义和数据结构声明, 不面向其它模块, 更不面向用户
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
#ifndef __DEVINFO_PRIVATE_H__
#define __DEVINFO_PRIVATE_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "core_stdinc.h"

/* TODO: 这一段列出需要包含SDK其它模块头文件, 与上一段落以1个空行隔开 */
#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"
#include "aiot_devinfo_api.h"      /* 内部头文件是用户可见头文件的超集 */

typedef struct {
    aiot_sysdep_portfile_t     *sysdep;             /* 底层依赖回调合集的引用指针 */
    void *mqtt_handle;

    aiot_devinfo_event_handler_t   event_handler;      /* 组件内部运行状态变更时, 通知用户的回调 */
    aiot_devinfo_recv_handler_t    recv_handler;       /* 组件从协议栈读到内容时, 通知用户的回调 */
    void *userdata;                                 /* 组件调用以上2个 devinfo_handler 时的入参之一 */

    uint32_t    deinit_timeout_ms;

    /*---- 以上都是用户在API可配 ----*/

    /*---- 以下都是DEVINFO在内部使用, 用户无感知 ----*/

    void       *data_mutex;     /* 保护本地的数据结构 */
    uint8_t     exec_enabled;
    uint32_t    exec_count;
} devinfo_handle_t;

#define DEVINFO_MODULE_NAME                    "devinfo"  /* 用于内存统计的模块名字符串 */

#define DEVINFO_DEFAULT_DEINIT_TIMEOUT_MS      (2 * 1000)

#define DEVINFO_UPDATE_TOPIC_FMT               "/sys/%s/%s/thing/deviceinfo/update"
#define DEVINFO_UPDATE_REPLY_TOPIC             "/sys/+/+/thing/deviceinfo/update_reply"
#define DEVINFO_DELETE_TOPIC_FMT               "/sys/%s/%s/thing/deviceinfo/delete"
#define DEVINFO_DELETE_REPLY_TOPIC             "/sys/+/+/thing/deviceinfo/delete_reply"

#define DEVINFO_DEINIT_INTERVAL_MS               (100)

#if defined(__cplusplus)
}
#endif
#endif  /* __DEVINFO_PRIVATE_H__ */


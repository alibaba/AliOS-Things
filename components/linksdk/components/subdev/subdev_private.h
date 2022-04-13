/**
 * @file subdev_private.h
 * @brief subdev模块内部的宏定义和数据结构声明, 不面向其它模块, 更不面向用户
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
#ifndef __SUBDEV_PRIVATE_H__
#define __SUBDEV_PRIVATE_H__

#if defined(__cplusplus)
extern "C" {
#endif

/* 用这种方式包含标准C库的头文件 */
#include "core_stdinc.h"

/* TODO: 这一段列出需要包含SDK其它模块头文件, 与上一段落以1个空行隔开 */
#include "core_list.h"
#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_subdev_api.h"      /* 内部头文件是用户可见头文件的超集 */
#include "aiot_mqtt_api.h"

#define SUBDEV_ALINK_ID_MAX_LEN               (11)

/* TODO: 定义subdev模块内部的会话句柄结构体, SDK用户不可见, 只能得到void *handle类型的指针 */
typedef struct {
    aiot_sysdep_portfile_t     *sysdep;             /* 底层依赖回调合集的引用指针 */
    void *mqtt_handle;

    aiot_subdev_event_handler_t   event_handler;      /* 组件内部运行状态变更时, 通知用户的回调 */
    aiot_subdev_recv_handler_t    recv_handler;       /* 组件从协议栈读到内容时, 通知用户的回调 */
    void *userdata;                                 /* 组件调用以上2个 subdev_handler 时的入参之一 */

    /*---- 以上都是用户在API可配 ----*/

    /*---- 以下都是SUBDEV在内部使用, 用户无感知 ----*/

    void       *data_mutex;     /* 保护本地的数据结构 */
    char       alink_id[SUBDEV_ALINK_ID_MAX_LEN];

} subdev_handle_t;

#define SUBDEV_MODULE_NAME                    "subdev"  /* 用于内存统计的模块名字符串 */

#define SUBDEV_TOPIC_TOPO_ADD_REPLY           "/sys/+/+/thing/topo/add_reply"
#define SUBDEV_TOPIC_TOPO_DELETE_REPLY        "/sys/+/+/thing/topo/delete_reply"
#define SUBDEV_TOPIC_TOPO_GET_REPLY           "/sys/+/+/thing/topo/get_reply"
#define SUBDEV_TOPIC_BATCH_LOGIN_REPLY        "/ext/session/+/+/combine/batch_login_reply"
#define SUBDEV_TOPIC_BATCH_LOGOUT_REPLY       "/ext/session/+/+/combine/batch_logout_reply"
#define SUBDEV_TOPIC_SUB_REGISTER_REPLY       "/sys/+/+/thing/sub/register_reply"
#define SUBDEV_TOPIC_PRODUCT_REGISTER_REPLY   "/sys/+/+/thing/proxy/provisioning/product_register_reply"

#define SUBDEV_TOPIC_TOPO_CHANGE_NOTIFY       "/sys/+/+/thing/topo/change"

typedef struct {
    char *topic;
    aiot_mqtt_recv_handler_t handler;
} subdev_topic_map_t;

#if defined(__cplusplus)
}
#endif
#endif  /* __SUBDEV_PRIVATE_H__ */


/**
 * @file shadow_private.h
 * @brief shadow模块内部的宏定义和数据结构声明, 不面向其它模块, 更不面向用户
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
#ifndef __SHADOW_PRIVATE_H__
#define __SHADOW_PRIVATE_H__

#if defined(__cplusplus)
extern "C" {
#endif

/* 用这种方式包含标准C库的头文件 */
#include "core_stdinc.h"

#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"
#include "aiot_shadow_api.h"

/* shadow模块内部的会话句柄结构体, SDK用户不可见, 只能得到void *handle类型的指针 */
typedef struct {
    aiot_sysdep_portfile_t *sysdep;
    void *mqtt_handle;

    aiot_shadow_recv_handler_t recv_handler;
    void *userdata;
} shadow_handle_t;

#define SHADOW_MODULE_NAME                  "shadow"

#define SHADOW_UPDATE_TOPIC_FMT             "/shadow/update/%s/%s"
#define SHADOW_GET_TOPIC                    "/shadow/get/+/+"

#define SHADOW_PAYLOAD_REQ_FMT              "{\"method\":\"%s\",\"state\":{\"reported\":%s},\"version\":%s}"
#define SHADOW_PAYLOAD_CLEAN_FMT            "{\"method\":\"update\",\"state\":{\"desired\":\"null\"},\"version\":%s}"
#define SHADOW_PAYLOAD_GET                  "{\"method\":\"get\"}"

#define SHADOW_JSON_KEY_METHOD              "method"
#define SHADOW_JSON_KEY_PAYLOAD             "payload"
#define SHADOW_JSON_KEY_STATUS              "status"
#define SHADOW_JSON_KEY_TIMESTAMP           "timestamp"
#define SHADOW_JSON_KEY_STATE               "state"
#define SHADOW_JSON_KEY_VERSION             "version"


#if defined(__cplusplus)
}
#endif
#endif  /* __SHADOW_PRIVATE_H__ */


/**
 * @file dm_private.h
 * @brief 数据模型模块内部头文件
 * @date 2020-01-20
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#ifndef __DM_PRIVATE_H__
#define __DM_PRIVATE_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "core_stdinc.h"
#include "core_string.h"
#include "core_log.h"
#include "core_diag.h"
#include "core_global.h"
#include "core_mqtt.h"

#include "aiot_sysdep_api.h"
#include "aiot_state_api.h"
#include "aiot_mqtt_api.h"
#include "aiot_dm_api.h"

/* 模块内部名 */
#define DATA_MODEL_MODULE_NAME          "dm"

/* ALINK请求的JSON格式 */
#define ALINK_REQUEST_FMT               "{\"id\":\"%s\",\"version\":\"1.0\",\"params\":%s,\"sys\":{\"ack\":%s}}"
/* ALINK应答的JSON格式 */
#define ALINK_RESPONSE_FMT              "{\"id\":\"%s\",\"code\":%s,\"data\":%s}"
#define ALINK_JSON_KEY_ID               "id"
#define ALINK_JSON_KEY_CODE             "code"
#define ALINK_JSON_KEY_PARAMS           "params"
#define ALINK_JSON_KEY_DATA             "data"
#define ALINK_JSON_KEY_MESSAGE          "message"

/* 诊断消息类型 */
#define DM_DIAG_MSG_TYPE_REQ            (0x00)
#define DM_DIAG_MSG_TYPE_RSP            (0x01)

#define DM_FREE(ptr)                    do {if (ptr) {dm_handle->sysdep->core_sysdep_free(ptr); ptr = NULL;}} while (0)

/* data-model模块的上下文结构体定义 */
typedef struct {
    aiot_sysdep_portfile_t *sysdep;
    void *mqtt_handle;

    aiot_dm_recv_handler_t recv_handler;
    void *userdata;
    uint8_t post_reply;
} dm_handle_t;

/* data-model内部发送函数原型定义 */
typedef int32_t (*dm_msg_send_func_t)(dm_handle_t *handle, const char *topic, const aiot_dm_msg_t *msg);

/* 包含上行topic和对应处理函数的结构体定义 */
typedef struct {
    char *topic;
    dm_msg_send_func_t func;
} dm_send_topic_map_t;

/* 包含下行topic和对应处理函数的结构体定义 */
typedef struct {
    char *topic;
    aiot_mqtt_recv_handler_t func;
} dm_recv_topic_map_t;

#if defined(__cplusplus)
}
#endif

#endif /* #ifndef __DM_PRIVATE_H__ */


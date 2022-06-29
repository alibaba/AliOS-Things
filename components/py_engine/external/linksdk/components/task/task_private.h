
/**
 * @file task_private.h
 * @brief task模块内部的宏定义和数据结构声明, 不面向其它模块, 更不面向用户
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
#ifndef __TASK_PRIVATE_H__
#define __TASK_PRIVATE_H__

#if defined(__cplusplus)
extern "C" {
#endif

/* 用这种方式包含标准C库的头文件 */
#include "core_stdinc.h"

/* TODO: 这一段列出需要包含SDK其它模块头文件, 与上一段落以1个空行隔开 */
#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"
#include "aiot_task_api.h"

#define TASK_MODULE_NAME                        "TASK"

#define TASK_NOTIFY_TOPIC                       "/sys/+/+/thing/job/notify_reply"
#define TASK_QUERY_RESPONSE_TOPIC               "/sys/+/+/thing/job/get_reply"

#define TASK_QUERY_TASK_TOPIC_FMT               "/sys/%s/%s/thing/job/get"
#define TASK_UPDATE_TASK_TOPIC_FMT              "/sys/%s/%s/thing/job/update"
#define TASK_NOTIFY_REPLY_TOPIC_FMT             "/sys/%s/%s/thing/job/notify_reply"

/* ALINK请求的JSON格式 */
#define TASK_REQUEST_QUERY_TASK_FMT             "{\"id\":\"%s\",\"version\":\"1.0.0\",\"params\":{\"taskId\":\"%s\"}}"
#define TASK_REQUEST_UPDATE_TASK_FMT            "{\"id\":\"%s\",\"version\":\"1.0.0\",\"params\":{\"taskId\":\"%s\",\"status\":\"%s\",\"progress\":%s,\"statusDetails\":%s}}"
#define TASK_REQUEST_UPDATE_TASK_NO_DETAIL_FMT  "{\"id\":\"%s\",\"version\":\"1.0.0\",\"params\":{\"taskId\":\"%s\",\"status\":\"%s\",\"progress\":%s}}"

#define TASK_NOTIFY_REPLY_FMT                   "{\"id\":\"%s\",\"code\":%s,\"data\":{}}"

#define TASK_GET_LIST_REPLY_ARRAY_MAX           10

#define TASK_GET_LIST_REPLY_TASK_ID             "$list"

typedef struct {
    aiot_sysdep_portfile_t      *sysdep;
    aiot_task_recv_handler_t    recv_handler;
    void *mqtt_handle;
    void                        *userdata;
    task_desc_t
    *default_task_desc;   /* handle中用于存储任务描述的数据结构. handle里面只存放一个任务. 如果用户要存放多个,可以自己在外部定义一个列表 */
    void                        *data_mutex;
} task_handle_t;

/* 包含下行topic和对应处理函数的结构体定义 */
typedef struct {
    char *topic;
    aiot_mqtt_recv_handler_t func;
} task_recv_topic_map_t;

typedef struct {
    char *str;
    aiot_task_status_t status;
} task_status_map_t;

typedef struct {
    char *pos;
    int len;
} task_list_json;

#if defined(__cplusplus)
}
#endif
#endif  /* __TASK_PRIVATE_H__ */

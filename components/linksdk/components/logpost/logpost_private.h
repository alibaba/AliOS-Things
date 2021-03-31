/**
 * @file logpost_private.h
 * @brief logpost模块内部的宏定义和数据结构声明, 不面向其它模块, 更不面向用户
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
#ifndef __LOGPOST_PRIVATE_H__
#define __LOGPOST_PRIVATE_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "core_stdinc.h"

#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"
#include "aiot_logpost_api.h"


/* logpost模块内部的会话句柄结构体, SDK用户不可见, 只能得到void *handle类型的指针 */
typedef struct {
    aiot_sysdep_portfile_t *sysdep;
    void *mqtt_handle;
    uint8_t user_log_switch;
    uint8_t sys_log_switch;

    aiot_logpost_event_handler_t event_handler;
    void *userdata;

    /* network info stats */
    uint64_t last_post_time;

} logpost_handle_t;

#define LOGPOST_MODULE_NAME                     "logpost"   /* 用于内存统计的模块名字符串 */

#define LOGPOST_DEFAULT_LOG_ONOFF               (0)

#define LOGPOST_CONTENT_MAXIMUM_LEN             (4096)

#define LOGPOST_NWKSTATS_POST_INTERVAL          (1800000)


/* 上下行topic定义 */
#define LOGPOST_POST_TOPIC_FMT                  "/sys/%s/%s/thing/log/post"
#define LOGPOST_CONFIG_GET_TOPIC_FMT            "/sys/%s/%s/thing/config/log/get"
#define LOGPOST_CONFIG_PUSH_TOPIC               "/sys/+/+/thing/config/log/push"
#define LOGPOST_CONFIG_GET_REPLY_TOPIC          "/sys/+/+/thing/config/log/get_reply"

#define NWKSTAT_RTT_INFO_FMT        "time=%s^rtt=%s"
#define NWKSTAT_CONN_INFO_FMT       "time=%s^conn_type=%s^conn_cost=%s^conn_ret=0"
#define NWKSTAT_CONN_INFO_FMT2      "time=%s^conn_type=%s^conn_cost=%s^conn_ret=0,time=%s^conn_type=%s^conn_cost=0^conn_ret=%s"

#define NWKSTAT_NET_RT              "net_rt"
#define NWKSTAT_NET_CONN            "net_conn"

#if defined(__cplusplus)
}
#endif
#endif  /* __LOGPOST_PRIVATE_H__ */


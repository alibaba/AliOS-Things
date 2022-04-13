/**
 * @file diag_private.h
 * @brief diag模块内部的宏定义和数据结构声明, 不面向其它模块, 更不面向用户
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
#ifndef __DIAG_PRIVATE_H__
#define __DIAG_PRIVATE_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "core_stdinc.h"
#include "core_list.h"

#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_diag_api.h"

typedef struct {
    uint64_t last_check_time;
} diag_mqtt_process_t;

typedef struct {
    uint64_t timestamp;
    int32_t code;
    uint8_t *data;
    uint32_t data_len;
} diag_raw_data_t;

typedef struct {
    uint64_t timestamp;
    uint16_t code;
    char *module_name;
    char *level;
    char *desc;
    uint8_t qos;
    struct core_list_head linked_node;
} diag_desc_node_t;

typedef struct {
    uint8_t is_diag;                                /* 该诊断项是否处于诊断状态 */
    char *level;                                    /* 告警级别 */
    uint64_t start_time;                            /* 统计开始时间 */
    uint64_t stop_time;                             /* 统计结束时间 */
    void *extra_data;                               /* 统计节点附加数据 */
    struct core_list_head linked_node;
} diag_running_state_node_t;

typedef struct {
    uint32_t code;                                 /* 诊断项代号 */
    char *name;                                     /* 诊断项名 */
    uint8_t is_reported;                            /* 当前统计周期内是否上报过 */
    uint64_t report_start_time;                     /* 当前统计周期开始时间 */
    uint32_t alert_counts;                          /* 当前统计周期累计产生告警次数 */
    uint32_t stat_number;                           /* 当前节点数量 */
    uint32_t max_stat_number;                       /* 统计节点最大数量 */
    uint8_t qos;                                    /* 使用MQTT发送告警报文至云端时，使用的QoS值 */
    void *mutex;                                    /* 数据保护锁 */
    struct core_list_head linked_list;
} diag_running_state_t;

typedef void (*diag_running_state_node_extra_clean_t)(void *handle, void *extra_data);
typedef int32_t (*diag_running_state_node_extra_stop_t)(void *handle, diag_running_state_node_t *node, uint32_t stat_idx, uint32_t stat_number, void *extra_data);
typedef int32_t (*diag_report_desc_append_t)(void *handle, diag_running_state_t *running_state, diag_running_state_node_t *node, char **desc);

typedef struct {
    diag_running_state_node_extra_clean_t extra_clean_cb;
    diag_running_state_node_extra_stop_t extra_stop_cb;
    diag_report_desc_append_t desc_append_cb;
} diag_stat_callback_t;

typedef struct {
    aiot_diag_config_t config;
    diag_running_state_t running_state;
    diag_stat_callback_t stat_cb;
} diag_stat_t;

typedef struct {
    uint32_t msgid;
} diag_alink_uplink_extra_data_t;

#define DIAG_STAT_ITEM_NUMBER                              (3)

typedef struct {
    aiot_sysdep_portfile_t     *sysdep;             /* 底层依赖回调合集的引用指针 */
    void *mqtt_handle;

    uint8_t local_report_enabled;
    uint8_t cloud_report_enabled;

    diag_stat_t diag_stat[DIAG_STAT_ITEM_NUMBER];

    uint32_t deinit_timeout_ms;

    aiot_diag_event_handler_t   event_handler;      /* 组件内部运行状态变更时, 通知用户的回调 */
    aiot_diag_recv_handler_t    recv_handler;       /* 组件从协议栈读到内容时, 通知用户的回调 */
    void *userdata;                                 /* 组件调用以上2个 diag_handler 时的入参之一 */

    /*---- 以上都是用户在API可配 ----*/

    /*---- 以下都是DIAG在内部使用, 用户无感知 ----*/

    void       *data_mutex;     /* 保护本地的数据结构 */

    uint8_t diag_status;       /* 本地诊断模块状态, 0: stop, 1: start */
    uint8_t cloud_switch;

    diag_mqtt_process_t mqtt_process;

    uint8_t exec_enabled;
    uint32_t exec_count;

} diag_handle_t;

typedef struct {
    uint32_t stat_idx;
    char *name;
    uint32_t code;
    aiot_diag_config_t def_config;
    uint32_t def_max_stat_number;
    diag_stat_callback_t def_stat_cb;
    uint8_t qos;
} diag_config_t;

#define DIAG_MODULE_NAME                                   "diag"  /* 用于内存统计的模块名字符串 */

#define DIAG_DAFAULT_LOCAL_REPORT_ENABLED                  (1)
#define DIAG_DAFAULT_CLOUD_REPORT_ENABLED                  (1)
#define DIAG_DEFAULT_DEINIT_TIMEOUT_MS                     (2 * 1000)

/* MQTT connection diag default configuration */
#define DIAG_DEFAULT_MQTT_CONN_ENABLED                     (1)
#define DIAG_DEFAULT_MQTT_CONN_INTERVAL_MS                 (30 * 1000)
#define DIAG_DEFAULT_MQTT_CONN_WARNING_THRESHOLD           (200)
#define DIAG_DEFAULT_MQTT_CONN_FATAL_THRESHOLD             (500)
#define DIAG_DEFAULT_MQTT_CONN_MAX_STAT_NUMBER             (20)

/* MQTT heartbeag diag default configuration */
#define DIAG_DEFAULT_MQTT_HB_ENABLED                       (1)
#define DIAG_DEFAULT_MQTT_HB_INTERVAL_MS                   (30 * 1000)
#define DIAG_DEFAULT_MQTT_HB_WARNING_THRESHOLD             (800)
#define DIAG_DEFAULT_MQTT_HB_FATAL_THRESHOLD               (1500)
#define DIAG_DEFAULT_MQTT_HB_MAX_STAT_NUMBER               (20)

/* MQTT alink uplink default configuration */
#define DIAG_DEFAULT_ALINK_UPLINK_ENABLED                  (1)
#define DIAG_DEFAULT_ALINK_UPLINK_INTERVAL_MS              (30 * 1000)
#define DIAG_DEFAULT_ALINK_UPLINK_WARNING_THRESHOLD        (600)
#define DIAG_DEFAULT_ALINK_UPLINK_FATAL_THRESHOLD          (1000)
#define DIAG_DEFAULT_ALINK_UPLINK_MAX_STAT_NUMBER          (20)

#define DIAG_REPORT_TOPIC_FMT                              "/sys/%s/%s/thing/log/post"
#define DIAG_REPORT_PAYLOAD_FMT                            "{\"id\":\"%s\",\"version\":\"1.0\",\"params\":[{\"utcTime\":\"%s\"," \
                                                           "\"logLevel\":\"%s\",\"module\":\"%s\",\"code\":\"%s\",\"traceContext\":\"%s\",\"logContent\":\"%s\"}]}"

#define DIAG_DEINIT_INTERVAL_MS                            (100)
#define DIAG_MQTT_PROCESS_CHECK_INTERVAL_MS                (2000)
#define DIAG_REPORT_LEVEL_WARNING_STR                      "WARN"
#define DIAG_REPORT_LEVEL_FATAL_STR                        "FATAL"

#define DIAG_STATE_MQTT_BASE                               (STATE_MQTT_BASE)
#define DIAG_STATE_DM_BASE                                 (-0x0A00)

/* MQTT connection diag constant */
#define DIAG_MQTT_CONNECTION_STAT_INDEX                    (0)
#define DIAG_MQTT_CONNECTION_NAME_STR                      "DiagMqttConnection"
#define DIAG_TLV_MQTT_CONNECTION                           (0x0010)

/* MQTT heartbeat diag constant */
#define DIAG_MQTT_HEARTBEAT_STAT_INDEX                     (1)
#define DIAG_MQTT_HEARTBEAT_NAME_STR                       "DiagMqttHeartbeat"
#define DIAG_TLV_MQTT_HEARTBEAT                            (0x0020)

/* MQTT alink uplink diag constant */
#define DIAG_ALINK_UPLINK_STAT_INDEX                       (2)
#define DIAG_ALINK_UPLINK_NAME_STR                         "DiagAlinkUplink"
#define DIAG_TLV_ALINK_UPLINK                              (0x0030)
#define DIAG_TLV_ALINK_MSGID                               (0x0031)


/* internal state code */
#define STATE_DIAG_STOP_NODE_NOT_MATCH                     (-0x14FF)

#if defined(__cplusplus)
}
#endif
#endif  /* __DIAG_PRIVATE_H__ */


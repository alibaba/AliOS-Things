/**
 * @file mqtt_upload_private.h
 * @brief MQTT UPLOAD模块内部的宏定义和数据结构声明, 不面向其它模块, 更不面向用户
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
#ifndef __MQTT_UPLOAD_PRIVATE_H__
#define __MQTT_UPLOAD_PRIVATE_H__

#if defined(__cplusplus)
extern "C" {
#endif

/* 用这种方式包含标准C库的头文件 */
#include "core_stdinc.h"
#include "core_string.h"
#include "core_log.h"
#include "core_diag.h"
#include "core_global.h"
#include "core_mqtt.h"

#include "aiot_sysdep_api.h"
#include "aiot_state_api.h"
#include "aiot_mqtt_api.h"

#include "aiot_mqtt_upload_api.h"

/**
 * @brief 文件上传流服务对应的上下文数据
 */
typedef struct {
    char        *file_name;                         /* 上传的文件名 */
    char        *upload_id;                         /* 文件上传的id */
    char        *mode;                              /* 云端文件的写入模式 */
    char        *crc64;                             /* (可选)对文件完整性进行CRC64校验,为空云端则不校验 */
    char        *uuid;                              /* 请求初始化时的UUID*/
    uint8_t     is_rsp;
    uint32_t    file_size;                          /* 上传文件的大小，SDK内不做任何限制 */
    uint32_t    file_offset;                        /* 文件上传的偏移量 */
    uint8_t     *data;                              /* 当前上传的分片文件 */
    uint32_t    block_size;                         /* 当前上传分片的大小 */
    uint32_t    failed_count;                       /* 上传失败计数 */
    uint64_t    send_last_time;                     /* 最近一次发送的时间点 */
    int32_t     status;                             /* 文件上传的状态 */
    int32_t     rsp_code;                           /* 云端应答的错误码 */
    aiot_mqtt_upload_read_handler_t read_data_handler;   /* 读取用户需要上传的数据回调，同时通知用户云端返回的信息 */
    void        *userdata;                                 /* 用户回调接口使用的数据 */
    aiot_mqtt_upload_recv_t packet;
    uint8_t     is_destory;
    struct core_list_head linked_node;
} upload_file_task_handle_t;

/**
 * @brief 定义MQTT UPLOAD模块内部的会话句柄结构体, SDK用户不可见, 只能得到void *handle类型的指针
 */
typedef struct {
    aiot_sysdep_portfile_t      *sysdep;         /* 底层依赖回调合集的引用指针 */
    struct core_list_head       file_list;       /* 文件上传的任务链表 */

    uint32_t    rsp_timeout;                     /* 从发送数据开始，等待云端返回超时的时间 */
    uint32_t    retry_max_value;                 /* 单调上行消息发送失败时，尝试的次数 */
    uint32_t    default_block_size;              /* 文件上传，单包的默认长度 */

    /*---- 以上都是用户在API可配 ----*/
    /*---- 以下都是MQTT_UPLOAD在内部使用, 用户无感知 ----*/
    void       *mqtt_handle;                     /* MQTT 会话句柄 */
    void       *data_mutex;                      /* 保护本地的数据结构 */
    void       *uptask_mutex;                    /* 保证不会并发打开stream */
    void       *send_mutex;
    void       *open_mutex;
} mqtt_upload_handle_t;

#define MQTT_UPLOAD_MODULE_NAME                    "Upload"    /* 用于内存统计的模块名字符串 */

/* MQTT文件上传的topic */
#define MQTT_UPLOAD_UPLOAD_INIT                  "/sys/%s/%s/thing/file/upload/mqtt/init"        /* 请求上传 */
#define MQTT_UPLOAD_UPLOAD_FILE                  "/sys/%s/%s/thing/file/upload/mqtt/send"        /* 分片上传 */
#define MQTT_UPLOAD_UPLOAD_CANCEL                "/sys/%s/%s/thing/file/upload/mqtt/cancel"      /* 设备取消上传 */

#define MQTT_UPLOAD_UPLOAD_INIT_REPLY            "/sys/%s/%s/thing/file/upload/mqtt/init_reply"      /* 请求应答 */
#define MQTT_UPLOAD_UPLOAD_FILE_REPLY            "/sys/%s/%s/thing/file/upload/mqtt/send_reply"      /* 上传应答 */
#define MQTT_UPLOAD_UPLOAD_CANCEL_REPLY          "/sys/%s/%s/thing/file/upload/mqtt/cancel_reply"    /* 取消应答 */

/* MQTT UPLOAD请求的JSON格式 */
#define MQTT_UPLOAD_DEFAULT_BASE_FMT             "{\"id\":\"%s\",\"params\":%s}"

#define MQTT_UPLOAD_OPEN_STREAM_PARAM_FMT        "{\"fileName\":\"%s\",\"fileSize\":%s,\"conflictStrategy\":\"%s\",\"initUid\":\"%s\"}"
#define MQTT_UPLOAD_OPEN_STREAM_PARAM_FMT_MD5    "{\"fileName\":\"%s\",\"fileSize\":%s,\"conflictStrategy\":\"%s\",\"ficMode\":\"%s\",\"ficValue\":\"%s\",\"initUid\":\"%s\"}"
#define MQTT_UPLOAD_SEND_DATA_PARAM_FMT          "{\"uploadId\":\"%s\",\"offset\":%s,\"bSize\":%s,\"isComplete\":%s}"
#define MQTT_UPLOAD_CLOSE_STREAM_PARAM_FMT       "{\"uploadId\":\"%s\"}"

/* 文件分片上报的格式 | Heard H | Heard L | HeaderJson | DATA | CRC16 L | CRC16 H| */
#define MQTT_UPLOAD_SEND_DATA_RAM_FMT                "%.2x%s%s%.2x"

/* 这一段列出用户未用 aiot_MQTT UPLOAD_setopt() 配置时, 各个对外选项的默认值, 必须以 MQTT_UPLOAD_DEFAULT_ 作为前缀 */
#define MQTT_UPLOAD_DEFAULT_SEND_MAX_SIZE         (128 * 1024)    /* MQTT文件最大上传128KB */
#define MQTT_UPLOAD_DEFAULT_SEND_MIN_SIZE         (256)           /* MQTT文件除最后一包外，其他大小不能小于256B */
#define MQTT_UPLOAD_DEFAULT_BLOCK_SIZE            (2 * 1024)      /* MQTT文件上传，默认一包的最大长达 */
#define MQTT_UPLOAD_DEFAULT_RSP_TIMEOUT           (10 * 1000)     /* MQTT上传等待超时时间默认 10s */
#define MQTT_UPLOAD_DEFAULT_RETRY_MAX_COUNT       (5)             /* 默认每次消息重发次数 5次 */
#define MQTT_UPLOAD_DEFAULT_MODE_MAX_SIZE         (32)            /* MQTT文件上传，云端文件覆盖或者追加的模式 */
#define MQTT_UPLOAD_DEFAULT_SUBTOPIC_NUM          (3)             /* MQTT文件上传用到的subtopic的数量 */
#define MQTT_UPLOAD_DEFAULT_UUID_SIZE             (32)            /* 初始化请求时生成的UUID */

#define MQTT_UPLOAD_SYNC_API_SLEEP_MS             (20)            /* 等待云端应答的最小时间*/

#if defined(__cplusplus)
}
#endif
#endif  /* __MQTT_UPLOAD_PRIVATE_H__ */


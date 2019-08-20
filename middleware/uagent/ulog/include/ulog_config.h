/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef U_LOG_CONFIG_H_
#define U_LOG_CONFIG_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "uagent/uagent_type.h"

#define ULOG_SHOW         (UAGENT_FUNC_USER_BASE)

#define ULOG_LEVEL_CHANGE (UAGENT_FUNC_USER_BASE+1)
#define ULOG_LOG_LIST     (UAGENT_FUNC_USER_BASE+2)

#define ULOG_TAG_SELF "ULOG"

/**
 * syslog management command mark
 * NOT RECOMMEND MODIFY THIS VALUE, READ ONLY!!
 */
#define ULOG_CMD_PREFIX "#@"

#define ULOG_HEADER_TYPE_LEN 5

/**
 * syslog protocol, facility local use 0
 * NOT RECOMMEND MODIFY THIS VALUE, READ ONLY!!
 */
#define SYSLOG_FACILITY        128

#define TAG_FILE_NAME_MAX_LEN  16

#define UNDEFINE_FILE_NAME "unknown"

#define MOD_MAX_LEN 10

#define FACILITY_NORMAL_LOG 248

#define FACILITY_NORMAL_LOG_NO_POP_CLOUD 240

#define HTTP_REQ_RETRY 3

#define HTTP_RSP_RETRY  3

#define URL_PREFIX_HTTP "http://"
#define URL_PREFIX_HTTPS "https://"

#define RSP_BUF_SIZE 512

#define HTTP_UP_HDR_SIZE 64

/* 5 is result of cost <%03d> */
#define LOG_PREFIX_LEN 5

typedef enum {
    http_upload_file_operate_fail = -4,
    http_upload_text_empty        = -3,
    http_upload_memory_fail       = -2,
    http_upload_common_fail       = -1,
    http_upload_start             = 0,
    http_upload_finish            = 1,
}http_upload_fail_t;

/**
 * sometimes found the pop log into file always fail, may result from file attribute,
 * or other unknown reason. If the fail operte on the same file keeps 3 times, then
 * destroy the log file and create new one.
 */
#define ULOG_FILE_FAIL_COUNT        3

/**
 * Specify the port of syslog watcher
 * NOT RECOMMEND MODIFY THIS VALUE, READ ONLY!!
 */
#define SYSLOG_WATCHER_DEFAULT_PORT 514

/* Specify if details mode used under sync log */
#ifndef ULOG_CONFIG_SYNC_LOG_DETAILS
#define SYNC_LOG_DETAILS 0
#else
#define SYNC_LOG_DETAILS ULOG_CONFIG_SYNC_LOG_DETAILS
#endif

#ifndef ULOG_CONFIG_SYNC_DETAIL_COLOR
#define SYNC_DETAIL_COLOR 0
#else
#define SYNC_DETAIL_COLOR ULOG_CONFIG_SYNC_DETAIL_COLOR
#endif

#ifndef ULOG_CONFIG_SYNC_ABS_PATH
#define SYNC_ABS_PATH 0
#else
#define SYNC_ABS_PATH ULOG_CONFIG_SYNC_ABS_PATH
#endif

#ifndef ULOG_CONFIG_DEFAULT_DIR_ASYNC
#define LOG_DIR_ASYNC 0
#else
#define LOG_DIR_ASYNC ULOG_CONFIG_DEFAULT_DIR_ASYNC
#endif

#ifndef ULOG_CONFIG_SYSLOG_TIME_FORMAT
#define SYSLOG_TIME_FORMAT 0
#else
#define SYSLOG_TIME_FORMAT ULOG_CONFIG_SYSLOG_TIME_FORMAT
#endif

#ifndef ULOG_CONFIG_EXTREAM_LOG_TAG
#define EXTREAM_LOG_TAG 1
#else
#define EXTREAM_LOG_TAG ULOG_CONFIG_EXTREAM_LOG_TAG
#endif

/**
 * the max length of every trace log to be recorded, include syslog format(if EXTREAM_LOG_TEXT
 * is not set), so 100 is reseanable value when syslog format support, 50 is fit for extream log mode
 * Log Content will be trimmed if it's too long.
 * SYSLOG_SIZE*SYSLOG_SIZE is the log mechanism cost heap size
 */
#ifndef ULOG_CONFIG_LOG_SIZE
#define ULOG_SIZE         256
#else
#define ULOG_SIZE         ULOG_CONFIG_LOG_SIZE
#endif

/**
 * Default size of buffer queue that used to record log under aync mode.
 * May the trace log miss if this value was set too small.
 * More RAM will be costed if it set too large
 * So consider balance of this value and system resouce
 */
#ifndef ULOG_CONFIG_ASYNC_BUF_SIZE
#define DEFAULT_ASYNC_BUF_SIZE    6144
#else
#define DEFAULT_ASYNC_BUF_SIZE    ULOG_CONFIG_ASYNC_BUF_SIZE
#endif

#ifndef ULOG_CONFIG_RESERVED_FS
#define ULOG_RESERVED_FS   0
#else
#define ULOG_RESERVED_FS ULOG_CONFIG_RESERVED_FS
#endif

#ifndef ULOG_CONFIG_RESERVED_FS_SIZE
#define ULOG_RESERVED_FS_SIZE   64
#else
#define ULOG_RESERVED_FS_SIZE ULOG_CONFIG_RESERVED_FS_SIZE
#endif

/**
 * Level Stop Filter of default direction(uart for rtos), any log level qual or lower than this (value higher than it)
 * will be abondoned LOG_EMERG will make none log pop out.
 */
#ifndef ULOG_CONFIG_STOP_FILTER_DEFAULT
#define STOP_FILTER_DEFAULT   LOG_WARNING
#else
#define STOP_FILTER_DEFAULT   ULOG_CONFIG_STOP_FILTER_DEFAULT
#endif

/**
 * Level Stop Filter of udp, any log level qual or lower than this (value higher than it)
 * will be abondoned LOG_EMERG will make none log pop out.
 */
#ifndef ULOG_CONFIG_STOP_FILTER_UDP
#define STOP_FILTER_UDP       LOG_WARNING
#else
#define STOP_FILTER_UDP       ULOG_CONFIG_STOP_FILTER_UDP
#endif

/**
 * switch on pop out log into file system
 */
#ifndef ULOG_CONFIG_POP_FS
#define ULOG_POP_FS_ENABLE       0
#else
#define ULOG_POP_FS_ENABLE ULOG_CONFIG_POP_FS
#endif

#ifndef ULOG_CONFIG_UPLOAD
#define ULOG_UPLOAD_LOG_FILE     0
#else
#define ULOG_UPLOAD_LOG_FILE ULOG_CONFIG_UPLOAD
#endif

/**
 * switch on pop out log via UDP
 */
#ifndef ULOG_CONFIG_POP_UDP
#define ULOG_POP_UDP_ENABLE       0
#else
#define ULOG_POP_UDP_ENABLE ULOG_CONFIG_POP_UDP
#endif

/**
 * Level Stop Filter of File system, any log level qual or lower than this (value higher than it)
 * will be abondoned LOG_EMERG will make none log pop out.
 */
#ifndef ULOG_CONFIG_STOP_FILTER_FS
#define STOP_FILTER_FS       LOG_WARNING
#else
#define STOP_FILTER_FS       ULOG_CONFIG_STOP_FILTER_FS
#endif

/**
 * switch on pop out log into cloud real-time
 */
#ifndef ULOG_CONFIG_POP_CLOUD
#define ULOG_POP_CLOUD_ENABLE       0
#else
#define ULOG_POP_CLOUD_ENABLE ULOG_CONFIG_POP_CLOUD
#endif

/**
 * Level Stop Filter of File system, any log level qual or lower than this (value higher than it)
 * will be abondoned LOG_EMERG will make none log pop out.
 */
#ifndef ULOG_CONFIG_STOP_FILTER_CLOUD
#define STOP_FILTER_CLOUD       LOG_EMERG
#else
#define STOP_FILTER_CLOUD       ULOG_CONFIG_STOP_FILTER_CLOUD
#endif


/**
 * STACK DEPTH of Log Task, will cost 3072 Byte room
 */
#ifndef ULOG_CONFIG_LOG_ROUTINE_TASK_STACK_DEPTH
#define LOG_ROUTINE_TASK_STACK_DEPTH 3072
#else
#define LOG_ROUTINE_TASK_STACK_DEPTH ULOG_CONFIG_LOG_ROUTINE_TASK_STACK_DEPTH
#endif

/**
 * Max log files support in fs, if the recording file more than this value, rolling back mechanism trigged.
 */
#ifndef ULOG_CONFIG_LOCAL_FILE_CNT
#define LOCAL_FILE_CNT 5
#else
#define LOCAL_FILE_CNT ULOG_CONFIG_LOCAL_FILE_CNT
#endif

/**
 * Max log file size locate in local file system, if the recording file larger than this value,
 * new files will be used to be recored.
 */
#ifndef ULOG_CONFIG_LOCAL_FILE_SIZE
#define LOCAL_FILE_SIZE 4096
#else
#define LOCAL_FILE_SIZE ULOG_CONFIG_LOCAL_FILE_SIZE
#endif

#if ULOG_POP_FS_ENABLE
#ifdef CSP_LINUXHOST
#error(modify below PATH as log file located, then disable this statement)
#define FS_PATH              "/workspace/"
#elif defined (AOS_COMP_SPIFFS)
#define FS_PATH              "/spiffs/"
#elif defined (AOS_COMP_FATFS)
#ifdef CONFIG_AOS_FATFS_SUPPORT_MMC
#define FS_PATH              "/sdcard/"
#else
#define FS_PATH              "/fatfs/"
#endif /* CONFIG_AOS_FATFS_SUPPORT_MMC */
#else
#error ("select comp spiffs or fatfs, or disable this function")
#endif
#endif /* ULOG_POP_FS_ENABLE  */

#if ULOG_POP_UDP_ENABLE
#ifdef CONFIG_NO_TCPIP
#error ("pop log via udp not support in no tcpip board")
#endif
#endif

typedef enum {
    ulog_session_std = 0,  /* default out direction, usually uart for rtos, termial for Linux */

#if ULOG_POP_CLOUD_ENABLE
    ulog_session_cloud,      /* to cloud, via uagent */
#endif

#if ULOG_POP_FS_ENABLE
    ulog_session_file,       /* log on local file system */
#endif

#if ULOG_POP_UDP_ENABLE
    ulog_session_udp,        /* syslog via udp, port 514 is default for syslog watcher */
#endif
    ulog_session_size
}ulog_session_type_t;

#define REQ_BUF_SIZE (LOCAL_FILE_SIZE+512)

#endif /*U_LOG_CONFIG_H_*/


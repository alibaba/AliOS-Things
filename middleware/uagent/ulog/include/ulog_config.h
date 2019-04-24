/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef U_LOG_CONFIG_H_
#define U_LOG_CONFIG_H_

/*******************************************************************************
 below header file is legal under C99 or newer. If your compiler version is
 very old, conside below definition:
#ifndef bool
#define bool unsigned char
#endif

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

typedef unsigned short uint16_t
*******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDEBUG
#define CONFIG_LOGMACRO_SILENT
#endif


/**
* syslog management command mark
* NOT RECOMMEND MODIFY THIS VALUE, READ ONLY!!
*/
#define ULOG_CMD_PREFIX "#@"

/**
* syslog protocol, facility local use 0
* NOT RECOMMEND MODIFY THIS VALUE, READ ONLY!!
*/
#define SYSLOG_FACILITY        128

#define TAG_FILE_NAME_MAX_LEN  12

#define UNDEFINE_FILE_NAME "unknown"

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

/* Specify if the MOD will be recored under sync mode, w/o detail mode */
#ifndef ULOG_CONFIG_SYNC_LOG_MOD
#define SYNC_LOG_MOD 0
#else
#define SYNC_LOG_MOD ULOG_CONFIG_SYNC_LOG_MOD
#endif

#if SYNC_LOG_MOD
#define MOD_MAX_LEN 10
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

/**
* If this value is SET then extream log text will be recored, then the log text not support syslog format,
* only pure message will be loged, syslog format cost about 45 bytes for each log.
*/
#ifndef ULOG_CONFIG_EXTREAM_LOG_TEXT
#define EXTREAM_LOG_TEXT 0
#else
#define EXTREAM_LOG_TEXT ULOG_CONFIG_EXTREAM_LOG_TEXT
#endif

#ifndef ULOG_CONFIG_EXTREAM_LOG_ELAPSED_MS
#define EXTREAM_LOG_ELAPSED_MS 0
#else
#define EXTREAM_LOG_ELAPSED_MS ULOG_CONFIG_EXTREAM_LOG_ELAPSED_MS
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
#ifndef ULOG_CONFIG_SYSLOG_SIZE
#define SYSLOG_SIZE         128
#else
#define SYSLOG_SIZE         ULOG_CONFIG_SYSLOG_SIZE
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
* STACK DEPTH of Log Task, will cost 1024 Byte room
*/
#ifndef ULOG_CONFIG_LOG_ROUTINE_TASK_STACK_DEPTH
#define LOG_ROUTINE_TASK_STACK_DEPTH 1024
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
#if !defined (AOS_COMP_VFS)
#error ("select comp vfs, or disable this function")
#else
#if defined (AOS_COMP_SPIFFS)
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
#endif /* !AOS_COMP_VFS  */
#endif /* ULOG_POP_FS_ENABLE  */

#if ULOG_POP_UDP_ENABLE
#ifdef CONFIG_NO_TCPIP
#error ("pop log via udp not support in no tcpip board")
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /*U_LOG_CONFIG_H_*/

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
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

/**
* Module Type, used for distinguish funcion module of whole system, attached in front of TAG in MSG part.
* The log level can be filter based on the module type. User may pass below enum except MOD_SIZE &
* MOD_UNKNOWN.
*/
typedef enum{
    MOD_UNKNOWN,/* log text is "UN" if pass illegal parameter to module type */
    MOD_OS,     /* Log produced in OS , log string is "OS" */
    MOD_NET,    /* Log produced in Net, inlude lwip, bluetooth, etc , log string is "NT" */
    MOD_CLOUD,  /* Log produced in sdk which focus on communication with cloud , log string is "CD" */
    MOD_APP,    /* Log produced in user app, log string is "AP" */
    MOD_SIZE
}MOD_TYPE;

typedef enum{
    SESSION_DIRECTION,  //default out direction, usually uart for rtos, termial for Linux
#ifndef CONFIG_NO_TCPIP
    SESSION_UDP,        //Allow syslog, which support udp, port 514 is default for syslog watcher
#endif

#if defined (AOS_COMP_SPIFFS) && defined (AOS_COMP_VFS)
    SESSION_FILE,       //log on local file system
#endif
    SESSION_USB,

    SESSION_CNT
}SESSION_TYPE;

#define ULOG_CMD_PREFIX "#@"

/**
* If this value is SET then extream log text will be recored, then the log text not support syslog format,
* only pure message will be loged, syslog format cost about 45 bytes for each log.
*/
#ifndef ULOG_CONFIG_EXTREAM_LOG_TEXT
#define EXTREAM_LOG_TEXT 0
#else
#define EXTREAM_LOG_TEXT ULOG_CONFIG_EXTREAM_LOG_TEXT
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
* The minimal depth of queue that used to record under aync mode.
* NOT RECOMMEND MODIFY THIS VALUE, READ ONLY!!
*/
#define MIN_SYSLOG_DEPTH        32

/**
* Default depth of queue that used to record under aync mode.
* May the trace log miss if this depth was set too small.
* More RAM will be costed if it set too large
* So consider balance of this value and system resouce
*/
#ifndef ULOG_CONFIG_DEFAULT_ASYNC_SYSLOG_DEPTH
#define DEFAULT_ASYNC_SYSLOG_DEPTH    64
#else
#define DEFAULT_ASYNC_SYSLOG_DEPTH    ULOG_CONFIG_DEFAULT_ASYNC_SYSLOG_DEPTH
#endif

/**
* Level Stop Filter of default direction(uart for rtos), any log level qual or lower than this (value higher than it)
* will be abondoned LOG_EMERG will make none log pop out.
*/
#ifndef ULOG_CONFIG_STOP_FILTER_DEFAULT
#define STOP_FILTER_DEFAULT   LOG_NOTICE
#else
#define STOP_FILTER_DEFAULT   ULOG_CONFIG_STOP_FILTER_DEFAULT
#endif

/**
* Level Stop Filter of udp, any log level qual or lower than this (value higher than it)
* will be abondoned LOG_EMERG will make none log pop out.
* please keep this value on "LOG_EMERG" if you have no plan to use udp log
*/
#ifndef ULOG_CONFIG_STOP_FILTER_UDP
#define STOP_FILTER_UDP       LOG_WARNING
#else
#define STOP_FILTER_UDP       ULOG_CONFIG_STOP_FILTER_UDP
#endif

/**
* STACK DEPTH of Log Task, will cost 512 Byte room
*/
#ifndef ULOG_CONFIG_LOG_ROUTINE_TASK_STACK_DEPTH
#define LOG_ROUTINE_TASK_STACK_DEPTH 512
#else
#define LOG_ROUTINE_TASK_STACK_DEPTH ULOG_CONFIG_LOG_ROUTINE_TASK_STACK_DEPTH
#endif

/**
* Specify the port of syslog watcher
*/
#ifndef ULOG_CONFIG_SYSLOG_WATCHER_PORT
#define SYSLOG_WATCHER_DEFAULT_PORT 514
#else
#define SYSLOG_WATCHER_DEFAULT_PORT ULOG_CONFIG_SYSLOG_WATCHER_PORT
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
#define LOCAL_FILE_SIZE 1024
#else
#define LOCAL_FILE_CNT ULOG_CONFIG_LOCAL_FILE_CNT
#endif

#ifdef __cplusplus
}
#endif

#endif /*U_LOG_CONFIG_H_*/

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

/* Module Type, used for distinguish funcion module of whole system, attached in front of TAG in MSG part.
   The log level can be filter based on the module type. User may pass below enum except MOD_SIZE & MOD_UNKNOWN. */
typedef enum{
    MOD_UNKNOWN,/* log text is "UN" if pass illegal parameter to module type*/
    MOD_OS,     /* Log produced in OS , log string is "OS" */
    MOD_NET,    /* Log produced in Net, inlude lwip, bluetooth, etc , log string is "NT" */
    MOD_CLOUD,  /* Log produced in sdk which focus on communication with cloud , log string is "CD" */
    MOD_APP,    /* Log produced in user app, log string is "AP" */
    MOD_SIZE
}MOD_TYPE;



/******************************************************************************/
/********************* Below parameters can be modified  **********************/
/******************************************************************************/
/* Most Important Parameter which make decide using Asyncronously(ASYNC) mechanism
   to log or Not(SYNC). Benefit of ASYNC includes:
   Hardly block application when you try to log as the log text is saved in FIFO, which
   only be poped out when the system resource is free.
   REMARK: Currently not support.
*/
#define ASYNC_MODE 1

/* If this value is SET then extream log text will be recored, then the log text not support syslog format,
   only pure message will be loged, syslog format cost about 45 bytes for each log.
*/
#define EXTREAM_LOG_TEXT 0

/*the max length of every trace log to be recorded, include syslog format(if EXTREAM_LOG_TEXT
  is not set), so 100 is reseanable value when syslog format support, 50 is fit for extream log mode
  Log Content will be trimmed if it's too long.
  SYSLOG_SIZE*SYSLOG_SIZE is the log mechanism cost heap size
*/
#define SYSLOG_SIZE         128

//The minimal depth of queue that used to record.
//May the trace log miss if this depth was set too small.
//More RAM will be costed if it set too large
//So consider balance of this value and system resouce
#define MIN_SYSLOG_DEPTH        32

//Level Stop Filter of default direction(uart for rtos), any log level qual or lower than this (value higher than it)
//will be abondoned LOG_EMERG will make none log pop out.
#define       STOP_FILTER_DEFAULT   LOG_NOTICE

//Level Stop Filter of udp, any log level qual or lower than this (value higher than it)
//will be abondoned LOG_EMERG will make none log pop out.
//please keep this value on "LOG_EMERG" if you have no plan to use udp log
#define       STOP_FILTER_UDP       LOG_EMERG

//STACK DEPTH of Log Task, will cost 512 Byte room
#define LOG_ROUTINE_TASK_STACK_DEPTH 512


#ifdef __cplusplus
}
#endif

#endif /*U_LOG_CONFIG_H_*/

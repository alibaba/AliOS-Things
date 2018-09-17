/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _be_osal_h
#define _be_osal_h

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "iot/iot_export.h"
#include "iot/iot_import.h"
#include <utils/be_common.h>
#include "be_osal_net.h"
#include "be_osal_fs.h"

typedef struct {
    uint32_t time;
    uint16_t type;
    uint16_t code;
    unsigned long value;
    unsigned long extra;
} be_osal_input_event_t;


/* system event */
#define  EV_SYS                       0x0001
#define  CODE_SYS_ON_STARTING         1
#define  CODE_SYS_ON_START_COMPLETED  2
#define  CODE_SYS_ON_START_FAILED     4
#define  CODE_SYS_ON_IDLE             3
#define  CODE_SYS_ON_START_UOTA       5
#define  CODE_SYS_ON_ALINK_ONLINE     6
#define  CODE_SYS_ON_ALINK_OFFLINE    7
#define  CODE_SYS_ON_MQTT_READ        8

/* WiFi event */
#define  EV_WIFI                      0x0002
#define  CODE_WIFI_CMD_RECONNECT      1
#define  CODE_WIFI_ON_CONNECTED       2
#define  CODE_WIFI_ON_DISCONNECT      3
#define  CODE_WIFI_ON_PRE_GOT_IP      4
#define  CODE_WIFI_ON_GOT_IP          5

typedef void (*be_osal_event_cb)(be_osal_input_event_t *event, void *data);
typedef void (*be_osal_call_cb)(void *data);
typedef void (*be_osal_task)(void * data);
typedef void (*be_osal_fd_cb_t)(int fd, void *arg);


int32_t	 be_osal_init_yloop(void);
int32_t	 be_osal_start_yloop(void);
int32_t  be_osal_schedule_call(be_osal_call_cb action, void *data);
int32_t  be_osal_post_delayed_action(int32_t ms, be_osal_call_cb action, void *data);
int32_t  be_osal_post_event(uint16_t type, uint16_t code, size_t value);
int32_t  be_osal_register_event_filter(uint16_t type, be_osal_event_cb cb, void *data);
int32_t  be_osal_unregister_event_filter(uint16_t type, be_osal_event_cb cb, void *priv);


int32_t  be_osal_new_task(const char * name,be_osal_task task,void * data,int32_t size,void ** task_handle);
int32_t  be_osal_free_task(void * handle);
void     be_osal_exit_task(int code);
void 	 be_osal_delay(uint32_t ms);
const char *be_osal_task_name();
long long be_osal_time_now(void);
void     be_osal_reboot();

int  be_osal_poll_read_fd(int fd, be_osal_fd_cb_t cb, void* arg);
void be_osal_cancel_poll_read_fd(int fd, be_osal_fd_cb_t cb, void *arg);
void be_osal_cancel_delayed_action(int ms, be_osal_call_cb action, void *arg);


#if defined(__cplusplus)
}
#endif

#endif /* _be_osal_h */

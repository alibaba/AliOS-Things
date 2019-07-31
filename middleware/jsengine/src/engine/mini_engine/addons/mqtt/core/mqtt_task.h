/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_MQTT_TASK_H
#define BE_MQTT_TASK_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "be_jse_module.h"
#include "be_port_osal.h"
#include "be_utils.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum {
    MQTT_START       = 1,
    MQTT_STOP        = 2,
    MQTT_PUBLISH     = 3,
    MQTT_SUBSCRIBE   = 4,
    MQTT_UNSUBSCRIBE = 5,
} MQTT_TSK_CMD_e;

typedef union {
    struct {
        int cmd;
    } common;

    struct {
        int cmd;
        char productKey[16];
        char deviceName[32];
        char deviceSecret[48];
        be_jse_symbol_t* func;
    } start;

    struct {
        int cmd;
        char* topic;
        char* payload;
    } publish;

    struct {
        int cmd;
        char* topic;
        be_jse_symbol_t* func;
    } subscribe;

    struct {
        int cmd;
        char* topic;
        be_jse_symbol_t* func;
    } unsubscribe;

} MQTT_MSG_s;

/* 0 : success */
int mqtt_tsk_start();
int mqtt_tsk_stop();
int mqtt_send_msg(MQTT_MSG_s* pMsg);

#if defined(__cplusplus)
}
#endif

#endif /* BE_MQTT_TASK_H */

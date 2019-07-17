/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_JSE_TASK_H
#define BE_JSE_TASK_H

#include <stdint.h>

typedef void (*bone_engine_call_t)(void *data);

typedef enum {
    JSE_TASK_MSG_CALLBACK = 0, /*默认的JSE回调事件*/
    JSE_TASK_MSG_TYPE_MAX
} jse_task_msg_type_t;

typedef enum {
    JSE_TIMER_ONCE = 0, /*单次定时器*/
    JSE_TIMER_REPEAT    /*循环定时器*/
} jse_timer_type;

typedef struct {
    jse_task_msg_type_t type;
    bone_engine_call_t callback;
    void *param;
} jse_task_msg_t;

int32_t be_jse_task_schedule_call(bone_engine_call_t call, void *arg);

int32_t bone_engine_task_init();

void *be_jse_task_timer_action(uint32_t ms, bone_engine_call_t action,
                               void *arg, jse_timer_type type);

int32_t be_jse_task_cancel_timer(void *timerid);

#endif /* BE_JSE_TASK_H */

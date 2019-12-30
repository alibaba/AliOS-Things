/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef JSE_TASK_H
#define JSE_TASK_H

#include <stdint.h>

typedef void (*bone_engine_call_t)(void *data);

typedef enum {
    JSE_TASK_MSG_CALLBACK = 0, /* default JSE callback */
    JSE_TASK_MSG_TYPE_MAX
} jse_task_msg_type_t;

typedef enum {
    JSE_TIMER_ONCE = 0, /* one shot timer */
    JSE_TIMER_REPEAT    /* repeat timer */
} jse_timer_type;

typedef struct {
    jse_task_msg_type_t type;
    bone_engine_call_t callback;
    void *param;
} jse_task_msg_t;

int32_t jse_task_schedule_call(bone_engine_call_t call, void *arg);

/**
 *
 * JSEngine task initialization
 *
 */
int32_t jsengine_task_init();

/**
 * JSEngine yield task, for asynchronous event process
 *
 */
int32_t jsengine_task_yield(uint32_t timeout);

void *jse_task_timer_action(uint32_t ms, bone_engine_call_t action,
                               void *arg, jse_timer_type type);

int32_t jse_task_cancel_timer(void *timerid);

#endif /* JSE_TASK_H */

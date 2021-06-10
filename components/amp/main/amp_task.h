/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef AMP_TASK_H
#define AMP_TASK_H

#include <aos/kernel.h>
#include <stdint.h>

typedef void (*amp_engine_call_t)(void *data);

typedef enum {
    AMP_TASK_MSG_CALLBACK = 0, /* default JSE callback */
    AMP_TASK_MSG_EXIT = 1,
    AMP_TASK_MSG_TYPE_MAX
} amp_task_msg_type_t;

typedef enum {
    AMP_TIMER_ONCE = 0, /* one shot timer */
    AMP_TIMER_REPEAT    /* repeat timer */
} amp_timer_type_t;

typedef struct {
    amp_task_msg_type_t type;
    amp_engine_call_t callback;
    void *param;
} amp_task_msg_t;

int32_t amp_task_schedule_call(amp_engine_call_t call, void *arg);

/**
 *
 * JSEngine task initialization
 *
 */
int32_t amp_task_init(void);

/**
 * JSEngine yield task, for asynchronous event process
 *
 */
int32_t amp_task_yield(uint32_t timeout);

aos_timer_t *amp_task_timer_action(uint32_t ms, amp_engine_call_t action,
                               void *arg, amp_timer_type_t type);

int32_t amp_task_exit_call(amp_engine_call_t call, void *arg);

void amp_module_free(void);

int32_t amp_module_free_register(void (*callback)(void));

void amp_task_main();

int32_t amp_task_deinit();

#endif /* AMP_TASK_H */

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "fsl_debug_console.h"

void _mytrace_event_create(ktask_t *task, kevent_t *event, const name_t *name, uint32_t flags_init)
{
    PRINTF("_mytrace_event_create TASK 0x%x, event 0x%x, name = %s, %d\r\n", task, event, name, flags_init);
}
void _mytrace_event_get(ktask_t *task, kevent_t *event)
{
    PRINTF("_mytrace_event_get TASK 0x%x, event 0x%x\r\n", task, event);
}
void _mytrace_event_get_blk(ktask_t *task, kevent_t *event, tick_t wait_option)
{
    PRINTF("_mytrace_event_get_blk TASK 0x%x, event 0x%x, wait_option %d\r\n", task, event, wait_option);
}
void _mytrace_event_task_wake(ktask_t *task, ktask_t *task_waked_up, kevent_t *event)
{
    PRINTF("_mytrace_event_task_wake TASK 0x%x, WKTASK 0x%x, EVENT 0x%x\r\n", task, task_waked_up, event);
}
void _mytrace_event_del(ktask_t *task, kevent_t *event)
{
    PRINTF("_mytrace_event_del TASK 0x%x, EVENT 0x%x\r\n", task, event);
}

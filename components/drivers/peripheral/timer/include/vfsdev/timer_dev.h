/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _IO_TIMER_H_
#define _IO_TIMER_H_

#include <stdbool.h>

#define IOC_TIMER_BASE 'T'
#define IOC_TIMER_IRQP_SET IOC_TIMER_BASE + 1 // set timer parameters
#define IOC_TIMER_IRQP_GET IOC_TIMER_BASE + 2 // get timer parameters
#define IOC_TIMER_CONTROL IOC_TIMER_BASE + 3 // start/stop timer
#define IOC_TIMER_RELOAD IOC_TIMER_BASE + 4 // auto reload timer or not

#define IO_TIMER_START 1
#define IO_TIMER_STOP 0

typedef void (*alarm_timer_cb_t)(void *arg);

typedef struct timer_alarm {
    void *arg;
    alarm_timer_cb_t cb;
    unsigned long period;
    bool auto_reload;
} timer_alarm_t;

int vfs_timer_drv_init (void);

#endif //_IO_TIMER_H_

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef SYSTIME_H
#define SYSTIME_H

typedef unsigned long long systick_t;
typedef unsigned long long systime_t;

systick_t sys_tick(void);
systime_t sys_time(void);

systick_t time_to_tick(systime_t);

systime_t tick_to_time(systick_t);

int msleep(systime_t ms);

#else
#error "systime defined"

#endif /* SYSTIME */


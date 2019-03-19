/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef TIMER_H
#define TIMER_H

typedef unsigned long timer_t;

typedef int (*timer_func)(timer_t timer, void *arg);

int timer_create(timer_t *timer, timer_func fun, void *arg, unsigned long delay,
                 unsigned long period, int autorun);

int timer_destory(timer_t *timer);

int timer_start(timer_t *timer);

int timer_stop(timer_t *timer);

int timer_change(timer_t *timer, unsigned long delay, unsigned long period);

int timer_arg_change(timer_t *timer, void *arg);

#endif /* TIMER_H */


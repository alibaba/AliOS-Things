/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include <aos/kernel.h>
#include <ulog/ulog.h>

/* application interface declaration */
void task_new(void);
void task_new_ext(void);
void task_key(void);

void mutex_app(void);
void sem_app(void);
void queue_app(void);

void timer_new(void);
void timer_new_ext(void);
void timer_change(void);

void work_new(void);
void work_cancel(void);
void workqueue_new(void);

void mem_alloc(void);
void mem_realloc(void);

void time_app(void);

/* main entry */
int application_start(int argc, char *argv[])
{
    /* set ulog level */
    aos_set_log_level(AOS_LL_DEBUG);

    task_new();
    task_new_ext();
    task_key();

    sem_app();
    mutex_app();
    queue_app();

    timer_new();
    timer_new_ext();
    timer_change();

    work_new();
    work_cancel();
    workqueue_new();

    mem_alloc();
    mem_realloc();

    time_app();

    return 0;
}

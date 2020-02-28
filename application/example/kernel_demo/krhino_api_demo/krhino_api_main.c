/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include <k_api.h>
#include <ulog/ulog.h>

/**
 * You need to configure the corresponding kernel module before using it,
 * Otherwise the compilation will report an error.
 */

/* application interface declaration */
void task_app(void);
void task_asyn_delete_app(void);

void mutex_app(void);
void sem_app(void);
void event_app(void);

void bufqueue_app(void);
void queue_app(void);

void timer_app(void);

void work_app(void);

/* main entry */
int application_start(int argc, char *argv[])
{
    /* set ulog level */
    aos_set_log_level(AOS_LL_DEBUG);

    task_new();

    /* It may not be possible to run all the use cases at once on a
     * resource-constrained platform */
#if 0
    task_asyn_delete_app();

    mutex_app();
    sem_app();
    event_app();

    bufqueue_app();
    queue_app();

    timer_app();

    work_app();
#endif

    return 0;
}

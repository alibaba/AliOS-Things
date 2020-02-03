/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include <k_api.h>
#include <ulog/ulog.h>

/* application interface declaration */
void task_app(void);
void task_asyn_delete_app(void);
void mutex_app(void);

/**
 * You need to configure the corresponding kernel module before using it,
 * Otherwise the compilation will report an error.
 * In practice, there is no need to use precompiled macros to control whether or not a module is used.
 */
#if (RHINO_CONFIG_SEM > 0)
void sem_app(void);
#endif

#if (RHINO_CONFIG_EVENT_FLAG > 0)
void event_app(void);
#endif

#if (RHINO_CONFIG_BUF_QUEUE > 0)
void bufqueue_app(void);
#endif

#if (RHINO_CONFIG_QUEUE > 0)
void queue_app(void);
#endif

#if ((RHINO_CONFIG_TIMER > 0) && (RHINO_CONFIG_BUF_QUEUE > 0))
void timer_app(void);
#endif

#if ((RHINO_CONFIG_WORKQUEUE > 0) && (RHINO_CONFIG_BUF_QUEUE > 0))
void work_app(void);
#endif

/* main entry */
int application_start(int argc, char *argv[])
{
    /* set ulog level */
    aos_set_log_level(AOS_LL_DEBUG);

    task_app();
    task_asyn_delete_app();
    mutex_app();

#if (RHINO_CONFIG_SEM > 0)
    sem_app();
#endif

#if (RHINO_CONFIG_EVENT_FLAG > 0)
    event_app();
#endif

#if (RHINO_CONFIG_BUF_QUEUE > 0)
    bufqueue_app();
#endif

#if (RHINO_CONFIG_QUEUE > 0)
    queue_app();
#endif

#if ((RHINO_CONFIG_TIMER > 0) && (RHINO_CONFIG_BUF_QUEUE > 0))
    timer_app();
#endif

#if ((RHINO_CONFIG_WORKQUEUE > 0) && (RHINO_CONFIG_BUF_QUEUE > 0))
    work_app();
#endif

    return 0;
}

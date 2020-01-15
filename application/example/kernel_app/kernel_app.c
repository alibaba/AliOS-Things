/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include <ulog/ulog.h>

void task_app(void);
void bufqueue_app(void);

int application_start(int argc, char *argv[])
{
    /* set ulog level */
    aos_set_log_level(AOS_LL_DEBUG);

    task_app();
    bufqueue_app();

    return 0;
}

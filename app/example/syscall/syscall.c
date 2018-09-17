/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <hal/hal.h>

int application_start(int argc, char *argv[])
{
    int cnt = 0;

    while (1) {
        krhino_task_sleep(200);
        printf("syscall cnt %d\r\n", cnt++);
    }

    return 0;
}


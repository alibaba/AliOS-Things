/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/init.h"
#include "board.h"
#include <aos/errno.h>
#include <aos/kernel.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

extern int amp_main(void);

int application_start(int argc, char *argv[])
{
    int count = 0;
    aos_task_t amp_task;

    aos_task_new_ext(&amp_task, "amp_task", amp_main, NULL, 8192, AOS_DEFAULT_APP_PRI - 2);

    while (1) {
        aos_msleep(1000);
    };
}

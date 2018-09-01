/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/aos.h>

static void app_delayed_action(void *arg)
{
    printf("nano %s:%d %s\r\n", __func__, __LINE__, aos_task_name());
}

int application_start(int argc, char *argv[])
{
    do
    {
        app_delayed_action(NULL);
        aos_msleep(1000);
    }while(1);
}


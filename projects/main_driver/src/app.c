/*
* Copyright (C) 2015-2019 Alibaba Group Holding Limited
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "u_api.h"
#include "ulog/ulog.h"
#include "rpc.h"
#include "drivers/u_ld.h"

int application_start(int argc, char *argv[])
{
    printf("%s start to run...\n", RHINO_CONFIG_APP_NAME);

    u_driver_entry("uspace");

    while (1)
    {
        krhino_task_sleep(100);
    }
}


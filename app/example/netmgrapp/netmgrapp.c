/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/aos.h>
#include <netmgr.h>

int application_start(int argc, char *argv[])
{
    netmgr_init();
    netmgr_start(false);
    aos_loop_run();
    return 0;
}

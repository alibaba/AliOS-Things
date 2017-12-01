/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <netmgr.h>
#include "wifimonitor.h"

int application_start(int argc, char *argv[])
{
    aos_set_log_level(AOS_LL_INFO);
    netmgr_init();
    netmgr_start(false);
    wifimonitor_start(false);
    aos_loop_run();
}


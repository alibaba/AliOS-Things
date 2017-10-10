/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <netmgr.h>
#include "wifimonitor.h"

int application_start(int argc, char *argv[])
{
    netmgr_init();
    netmgr_start(false);
    aos_register_event_filter(EV_WIFI, wifimonitor_event_handler, NULL);
    wifimonitor_start(false);
    aos_loop_run();
}


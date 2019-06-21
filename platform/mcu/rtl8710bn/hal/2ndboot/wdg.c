/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "sys.h"
#include "sys_api.h"

void wdg_init(unsigned int ms)
{
    watchdog_init(ms);
    watchdog_start();
}

void wdg_finish(void)
{
    watchdog_stop();
}

void wdg_feed(void)
{
    watchdog_refresh();
}

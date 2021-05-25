/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "rec_sys.h"
#include "sys_api.h"

void rec_wdt_init(unsigned int timeout_ms)
{
    watchdog_init(timeout_ms);
}

void rec_wdt_start()
{
    watchdog_start();
}

void rec_wdt_stop()
{
    watchdog_stop();
}

void rec_wdt_feed()
{
    watchdog_refresh();
}
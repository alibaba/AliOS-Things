/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "bootloader.h"
#include "sys.h"
#include "sys_api.h"

int32_t hal_wdg_init(wdg_dev_t *wdg)
{
    if (wdg == NULL) {
        return -1;
    }

    watchdog_init(wdg->config.timeout);
    watchdog_start();

    return 0;
}

void hal_wdg_reload(wdg_dev_t *wdg)
{
    watchdog_refresh();
}

int32_t hal_wdg_finalize(wdg_dev_t *wdg)
{
    watchdog_stop();
    return 0;
}



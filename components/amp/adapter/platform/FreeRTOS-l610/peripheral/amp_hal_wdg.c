/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdbool.h>
#include "amp_hal_wdg.h"
#include "osi_log.h"

int32_t amp_hal_wdg_init(wdg_dev_t *wdg)
{
    int ret = -1;
    bool r = drvWatchdogStart(wdg->config.timeout);
    if (!r)
    {
        OSI_PRINTFI("can not start wgd!\n");        
    }
    else
    {
    	ret = 0;
    }
    return ret;
}

void amp_hal_wdg_reload(wdg_dev_t *wdg)
{
    int ret = -1;
    bool r = drvWatchdogReload(wdg->config.timeout);
    if (!r)
    {
        OSI_PRINTFI("can not reload wgd!\n");            
    }
    else
    {
    	ret = 0;
    }
    return ret;
}

int32_t amp_hal_wdg_finalize(wdg_dev_t *wdg)
{
     drvWatchdogStop();
    return 0;
}
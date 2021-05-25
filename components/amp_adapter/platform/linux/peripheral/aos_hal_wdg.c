/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/errno.h>
#include "aos_hal_wdg.h"

int32_t aos_hal_wdg_init(wdg_dev_t *wdg)
{
    if(NULL == wdg )
    {
        printf("parameter is invalid!\n\r");
        return -1;
    }
    printf ("[%s] timeout = %d \r\n", __FUNCTION__, wdg->config.timeout);
    return 0;
}

void aos_hal_wdg_reload(wdg_dev_t *wdg)
{
    printf ("[%s] \r\n", __FUNCTION__);
    return;
}

int32_t aos_hal_wdg_finalize(wdg_dev_t *wdg)
{

    printf ("[%s] \r\n", __FUNCTION__);
    return 0;
}
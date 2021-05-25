/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/errno.h>
#include "aos_hal_dac.h"

int32_t aos_hal_dac_init(dac_dev_t *dac)
{
    if(NULL == dac) {
        printf("parameter is invalid!\n\r");
        return -1;
    }
    printf("[%s] \r\n", __FUNCTION__);
    return 0;
}

int32_t aos_hal_dac_start(dac_dev_t *dac, uint32_t channel)
{
    printf("[%s] \r\n", __FUNCTION__);
    return 0;
}

int32_t aos_hal_dac_stop(dac_dev_t *dac, uint32_t channel)
{
    printf("[%s] \r\n", __FUNCTION__);
    return 0;
}

int32_t aos_hal_dac_set_value(dac_dev_t *dac, uint32_t channel, uint32_t data)
{
    printf("[%s] \r\n", __FUNCTION__);
    return 0;
}

int32_t aos_hal_dac_get_value(dac_dev_t *dac, uint32_t channel)
{
    printf("[%s] \r\n", __FUNCTION__);
    return 0;
}

int32_t aos_hal_dac_finalize(dac_dev_t *dac)
{
    printf("[%s] \r\n", __FUNCTION__);
    return 0;
}

/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/errno.h>
#include "aos_hal_adc.h"

int32_t aos_hal_adc_init(adc_dev_t *adc)
{
    if(NULL == adc) {
        printf("parameter is invalid!\n\r");
        return -1;
    }
    printf("[%s] sampling_cycle = %d \r\n", __FUNCTION__, adc->config.sampling_cycle);
    return 0;
}

int32_t aos_hal_adc_value_get(adc_dev_t *adc, uint32_t *output, uint32_t timeout)
{
    printf ("[%s] \r\n", __FUNCTION__);
    return 0;
}

int32_t aos_hal_adc_finalize(adc_dev_t *adc)
{

    printf ("[%s] \r\n", __FUNCTION__);
    return 0;
}
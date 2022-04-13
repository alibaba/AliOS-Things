/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "amp_hal_adc.h"

int32_t amp_hal_adc_init(adc_dev_t *adc)
{
    return hal_adc_init(adc);
}

int32_t amp_hal_adc_value_get(adc_dev_t *adc, void *output, uint32_t timeout)
{
    return hal_adc_value_get(adc, output, timeout);
}

int32_t amp_hal_adc_finalize(adc_dev_t *adc)
{
    return hal_adc_finalize(adc);
}
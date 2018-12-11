/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "hal/hal.h"

#ifdef HAL_ADC_MODULE_ENABLED

int32_t hal_adc_init(adc_dev_t *adc)
{
}

int32_t hal_adc_value_get(adc_dev_t *adc, void *output, uint32_t timeout)
{
}

int32_t hal_adc_finalize(adc_dev_t *adc)
{
}
#endif


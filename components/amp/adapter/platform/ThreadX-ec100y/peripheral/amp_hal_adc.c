/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "amp_defines.h"
#include "ql_adc.h"
#include "amp_hal_adc.h"

#define MOD_STR		"ADC"

int32_t amp_hal_adc_init(adc_dev_t *adc)
{
    int32_t ret = -1;

    if (adc == NULL) {
        amp_error(MOD_STR, "adc is null  \n");
        return ret;
    }

    if (adc->port > 1) {
        amp_error(MOD_STR, "adc port is invalid\r\n");
        return ret;
    }

    ql_adc_init();

    return 0;
}

int32_t amp_hal_adc_value_get(adc_dev_t *adc, void *output, uint32_t timeout)
{
    int32_t ret = -1;
    unsigned short value = 0;

    if (adc == NULL || output == NULL) {
        amp_error(MOD_STR, "adc is null  \n");
        return ret;
    }

    ret = ql_adc_read(adc->port, &value);
    if (ret != ADC_READ_SUCCESS) {
        amp_error(MOD_STR, "adc value read failed! \n");
        return ret;
    }
    *(int32_t *)output = value;
    return ret;
}

int32_t amp_hal_adc_finalize(adc_dev_t *adc)
{
    return 0;
}
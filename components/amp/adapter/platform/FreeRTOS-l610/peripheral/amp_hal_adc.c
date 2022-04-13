/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>

#include "amp_hal_adc.h"
#include "drv_adc.h"

int32_t amp_hal_adc_init(adc_dev_t *adc)
{
    drvAdcInit();
    return 0;
}

int32_t amp_hal_adc_value_get(adc_dev_t *adc, void *output, uint32_t timeout)
{

    int32_t value = 0;
    int ret = -1;
    if (output == NULL)
    {
        return ret; 
    }
	if(adc->port != 3 && adc->port != 1&& adc->port != 2)
	{
		 return ret;
	}
    value = drvAdcGetChannelVolt(adc->port, ADC_SCALE_5V000);
    if (value < 0)
    {
        return ret;
    }
    else
    {
        *(int32_t *)output = value;
	  ret = 0;
        return ret;
    }
}

int32_t amp_hal_adc_finalize(adc_dev_t *adc)
{
    drvAdcDisable();
    return 0;
}
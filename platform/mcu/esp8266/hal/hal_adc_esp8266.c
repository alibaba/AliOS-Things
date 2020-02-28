/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "aos/hal/adc.h"
#include "esp8266/ets_sys.h"

extern uint16_t test_tout();

int32_t hal_adc_init(adc_dev_t *adc)
{
    return 0;
}

int32_t hal_adc_value_get(adc_dev_t *adc, uint32_t *output, uint32_t timeout)
{
    uint16_t val = 0;
    uint16_t *data = (uint16_t *)output;

    if ((adc == NULL) || (output == NULL)) {
    	return -1;
    }

    val = test_tout(0);
    if (val != 0xFFFF) {
        /* The working voltage of ADC is designed according to 1.1v. Later, the actual working
           voltage of ADC is increased to 1.2v, so this scale is added. */
        val = val * 12 / 11;

        if (val > 1023) {
            val = 1023; /* 10-bit precision ADC */
        }
    }

    *data = val;

    return 0;
}

int32_t hal_adc_finalize(adc_dev_t *adc)
{
    return 0;
}

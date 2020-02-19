/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "aos/hal/adc.h"
#include "board.h"
#include <k_api.h>

#if (AOS_HAL_ADC_ENABLED > 0)
/**
 * Convert Analog to Digital, and print out the result
 */
void hal_adc_app_out(void)
{
    uint16_t  val;
    adc_dev_t adc_dev;
    int       ret;
    int       cnt;

    printf("hal_adc_out start\r\n");

    adc_dev.port = HALAPP_ADC;
    hal_adc_init(&adc_dev);

    val = 0;
    cnt = 20;
    while (cnt-- > 0) {
        ret = hal_adc_value_get(&adc_dev, &val, 1000);
        if (ret) {
            printf("%s: get value error, ret %d\r\n", __func__, ret);
            goto out;
        } else {
            printf("adc port %d value 0x%x\r\n", adc_dev.port, val);
        }

        aos_msleep(500);
    }

out:
    hal_adc_finalize(&adc_dev);

    printf("hal_adc_out end\r\n");
}

#endif

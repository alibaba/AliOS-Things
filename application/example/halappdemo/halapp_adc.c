/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "aos/hal/adc.h"
#include "board.h"
#include <k_api.h>

#if (AOS_HAL_ADC_ENABLED > 0)
/**
 * Convert Analog to Digital, and print out the result
 */

static void adc_app_out(uint8_t port)
{
    uint32_t  val[2];
    adc_dev_t adc_dev;
    int       ret;
    int       cnt;

    printf("adc_app_out on port %d start\r\n", port);

    adc_dev.port = port;
    hal_adc_init(&adc_dev);

    cnt = 20;
    while (cnt-- > 0) {
        memset(val, 0, sizeof(val));
        ret = hal_adc_value_get(&adc_dev, &val, 1000);
        if (ret) {
            printf("%s: get value error, ret %d\r\n", __func__, ret);
            goto out;
        } else {
            printf("adc port %d value 0x%03x, 0x%03x\r\n", adc_dev.port, val[0], val[1]);
        }

        aos_msleep(500);
    }

out:
    hal_adc_finalize(&adc_dev);

    printf("adc_app_out on port %d end\r\n", port);
}

void hal_adc_app_out(void)
{
    adc_app_out(HALAPP_ADC);
}
#endif

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "aos/kernel.h"
#include "aos/hal/adc.h"
#include "adc_test.h"

static adc_dev_t adc_dev;
static aos_timer_t gpio_test_timer;

void adc_test_task(void *arg);

void hal_adc_test(void)
{
    int ret = -1;

    printf("*********** adc test start ! ***********\n");

    adc_dev.port = PORT_ADC_TEST;
    ret = hal_adc_init(&adc_dev);
    if (ret != 0) {
        printf("hal_adc_init error !\n");
        return;
    }

    aos_timer_new(&gpio_test_timer, adc_test_task, NULL, 1000, 1);

}

void adc_test_task(void *arg)
{
    int ret = -1;
    uint16_t value = 0;

    printf("adc_test_task\n");


        ret = hal_adc_value_get(&adc_dev, &value, 0xFFFFFFFF);
        if (ret == 0) {
            printf("adc value: %d\n", value);
        } else {
            printf("adc value get error !\n");
        }

}

/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include "exp_adc.h"
#include "aos/kernel.h"
#include "hal_iomux_haas1000.h"
#include "ulog/ulog.h"
#include <stdio.h>

#define TAG "ex_adc"

typedef struct {
    uint8_t   installed;
    adc_dev_t st_adc_info;
} exp_adc_dev_t;

#define ADC_DEFAULT_TIMEOUT 3000
#define ADC_MAX_VALUE       3300
#define ADC_AVERAGE_TIME    32

/*exp adc device*/
static exp_adc_dev_t exp_adc_dev[ADC_PORT_SIZE] = {
    {0, {2, {1000}, NULL}}, // not installed
    {0, {1, {1000}, NULL}},
    {0, {0, {1000}, NULL}},
};

int32_t expansion_board_adc_init(void)
{
    int32_t ret = 0;
    int32_t i   = 0;

    for (i = 0; i < ADC_PORT_SIZE; i++) {
        if (exp_adc_dev[i].installed) {
            continue;
        }
        ret = hal_adc_init(&exp_adc_dev[i].st_adc_info);
        if (ret) {
            LOGE(TAG, "adc port %d init fail %d ", i, ret);
            return -1;
        }
        exp_adc_dev[i].installed = 1;
    }

    return 0;
}

int32_t expansion_board_adc_get_value(uint32_t port, uint32_t *adc_value)
{
    int32_t  ret = 0;
    uint32_t i = 0;
    uint32_t adc_sum = 0;
    uint32_t adc_avrg = 0;
    /*for haas1000 adc max value */
    uint32_t adc_min = ADC_MAX_VALUE;
    uint32_t adc_max = 0;

    if (port >= ADC_PORT_SIZE || NULL == adc_value) {
        LOGE(TAG, "%s %d invalid input port %d", __FILE__, __LINE__, port);
        return -1;
    }

    if (exp_adc_dev[port].installed == 0) {
        LOGE(TAG, "exp adc %d haven't init yet , get value fail", port);
        return -1;
    }
    ret = hal_adc_value_get(&exp_adc_dev[port].st_adc_info, adc_value, ADC_DEFAULT_TIMEOUT);
    if (ret) {
        LOGE(TAG, "Get adc %d port value fail ", port);
        return -1;
    }
    return 0;

}

int32_t board_get_voltage(uint32_t port, uint32_t *output)
{
    int32_t  ret      = 0;
    uint32_t i        = 0;
    uint32_t adc_sum  = 0;
    uint32_t adc_avrg = 0;
    /*for haas1000 adc max value */
    uint32_t adc_min   = ADC_MAX_VALUE;
    uint32_t adc_max   = 0;
    uint32_t adc_value = 0;

    for (i = 0; i < ADC_AVERAGE_TIME + 2; i++)
    {
        ret = expansion_board_adc_get_value(port, &adc_value);
        if (ret) {
            LOGE(TAG, "Get adc %d port value fail ", port);
            return -1;
        }
        adc_sum += adc_value;

        /* the min sampling voltage */
        if (adc_min >= adc_value)
        {
            adc_min = output;
        }
        /* the max sampling voltage */
        if (adc_max <= output)
        {
            adc_max = output;
        }

        aos_msleep(20);
    }
    /*adc sum value reduce adc min value reduse adc max value; and divided by
     * 32*/
    adc_avrg = (adc_sum - adc_min - adc_max) >> 5;
    *output  = adc_avrg;
    return 0;
}

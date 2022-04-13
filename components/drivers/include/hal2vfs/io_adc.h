/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _IO_ADC_H_
#define _IO_ADC_H_

#include <stdbool.h>

#define IOC_ADC_BASE 'A'
#define IOC_ADC_START IOC_ADC_BASE + 1 // start ADC controller with specified config
#define IOC_ADC_STOP IOC_ADC_BASE + 2 // stop ADC controller
#define IOC_ADC_GET_VALUE IOC_ADC_BASE + 3 // get ADC's value with timeout config

typedef struct io_adc_arg {
    unsigned int value;
    unsigned int timeout; // in unit of us
} io_adc_arg_t;

int vfs_dac_drv_init (void);

#endif //_IO_ADC_H_

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/**
 * @file aos/hal/adc_priv.h
 * @brief adc HAL
 * @version since 5.5.0
 */

#ifndef _HAL_ADC_PRIV_H
#define _HAL_ADC_PRIV_H

typedef enum {
	ADC_IRQMODE_NONE            = 0,
	ADC_IRQMODE_DATA            = 1,
	ADC_IRQMODE_LOW             = 2,
	ADC_IRQMODE_HIGH            = 3,
	ADC_IRQMODE_LOW_DATA        = 4,
	ADC_IRQMODE_HIGH_DATA       = 5,
	ADC_IRQMODE_LOW_HIGH        = 6,
	ADC_IRQMODE_LOW_HIGH_DATA   = 7
} adc_irqmode_t;

typedef struct {
    uint32_t irqmode;
    uint32_t irqhigh;
    uint32_t irqlow;
    void (*irq_callback)(void *);
    void *arg;
} adc_priv_config_t;

#endif

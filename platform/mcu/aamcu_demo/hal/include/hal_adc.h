/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __HAL_ADC_AAMCU_H
#define __HAL_ADC_AAMCU_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "board.h"

typedef struct {
    uint32_t channel;
    uint8_t  pin;
} gpio_adc_pin_config_t;

typedef struct {
    uint8_t                port;
    uint32_t               hal_adc;
    gpio_adc_pin_config_t *channel_conf;
    uint8_t                channel_cnt;
} ADC_MAPPING;

extern ADC_MAPPING ADC_MAPPING_TABLE[];

#ifdef __cplusplus
}
#endif

#endif /* __HAL_ADC_AAMCU_H */

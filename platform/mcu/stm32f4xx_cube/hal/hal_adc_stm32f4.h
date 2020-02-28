/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_ADC_STM32F4_H
#define __HAL_ADC_STM32F4_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"

#ifdef HAL_ADC_MODULE_ENABLED

#define HAL_ADC_1   ((uint32_t)ADC1)
#define HAL_ADC_2   ((uint32_t)ADC2)
#ifdef ADC3
#define HAL_ADC_3   ((uint32_t)ADC3)
#define HAL_ADC_CNT ((uint32_t)3)
#else
#define HAL_ADC_CNT ((uint32_t)2)
#endif

#define HAL_ADC_CHANNEL_0   ((uint32_t)ADC_CHANNEL_0)
#define HAL_ADC_CHANNEL_1   ((uint32_t)ADC_CHANNEL_1)
#define HAL_ADC_CHANNEL_2   ((uint32_t)ADC_CHANNEL_2)
#define HAL_ADC_CHANNEL_3   ((uint32_t)ADC_CHANNEL_3)
#define HAL_ADC_CHANNEL_4   ((uint32_t)ADC_CHANNEL_4)
#define HAL_ADC_CHANNEL_5   ((uint32_t)ADC_CHANNEL_5)
#define HAL_ADC_CHANNEL_6   ((uint32_t)ADC_CHANNEL_6)
#define HAL_ADC_CHANNEL_7   ((uint32_t)ADC_CHANNEL_7)
#define HAL_ADC_CHANNEL_8   ((uint32_t)ADC_CHANNEL_8)
#define HAL_ADC_CHANNEL_9   ((uint32_t)ADC_CHANNEL_9)
#define HAL_ADC_CHANNEL_10  ((uint32_t)ADC_CHANNEL_10)
#define HAL_ADC_CHANNEL_11  ((uint32_t)ADC_CHANNEL_11)
#define HAL_ADC_CHANNEL_12  ((uint32_t)ADC_CHANNEL_12)
#define HAL_ADC_CHANNEL_13  ((uint32_t)ADC_CHANNEL_13)
#define HAL_ADC_CHANNEL_14  ((uint32_t)ADC_CHANNEL_14)
#define HAL_ADC_CHANNEL_15  ((uint32_t)ADC_CHANNEL_15)
#define HAL_ADC_CHANNEL_16  ((uint32_t)ADC_CHANNEL_16)
#define HAL_ADC_CHANNEL_17  ((uint32_t)ADC_CHANNEL_17)
#define HAL_ADC_CHANNEL_18  ((uint32_t)ADC_CHANNEL_18)
#define HAL_ADC_CHANNEL_CNT ((uint32_t)19)


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

#endif /* HAL_ADC_MODULE_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* __HAL_ADC_STM32F4_H */

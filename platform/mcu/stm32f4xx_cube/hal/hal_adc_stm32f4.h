/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_ADC_STM32F4_H
#define __HAL_ADC_STM32F4_H

#ifdef __cplusplus
 extern "C" {
#endif

#define HAL_ADC_1   ((uint8_t)0)
#define HAL_ADC_2   ((uint8_t)1)
#define HAL_ADC_3   ((uint8_t)2)
#define HAL_ADC_CNT ((uint8_t)3)

#define HAL_ADC_CHANNEL_0   ((uint8_t)0)
#define HAL_ADC_CHANNEL_1   ((uint8_t)1)
#define HAL_ADC_CHANNEL_2   ((uint8_t)2)
#define HAL_ADC_CHANNEL_3   ((uint8_t)3)
#define HAL_ADC_CHANNEL_4   ((uint8_t)4)
#define HAL_ADC_CHANNEL_5   ((uint8_t)5)
#define HAL_ADC_CHANNEL_6   ((uint8_t)6)
#define HAL_ADC_CHANNEL_7   ((uint8_t)7)
#define HAL_ADC_CHANNEL_8   ((uint8_t)8)
#define HAL_ADC_CHANNEL_9   ((uint8_t)9)
#define HAL_ADC_CHANNEL_10  ((uint8_t)10)
#define HAL_ADC_CHANNEL_11  ((uint8_t)11)
#define HAL_ADC_CHANNEL_12  ((uint8_t)12)
#define HAL_ADC_CHANNEL_13  ((uint8_t)13)
#define HAL_ADC_CHANNEL_14  ((uint8_t)14)
#define HAL_ADC_CHANNEL_15  ((uint8_t)15)
#define HAL_ADC_CHANNEL_16  ((uint8_t)16)
#define HAL_ADC_CHANNEL_17  ((uint8_t)17)
#define HAL_ADC_CHANNEL_18  ((uint8_t)18)
#define HAL_ADC_CHANNEL_CNT (HAL_ADC_CHANNEL_18 + 1)

typedef struct {
    uint8_t channel;
    uint8_t pin;
} gpio_adc_pin_config_t;

typedef struct {
    uint8_t                port;
    uint8_t                hal_adc;
    gpio_adc_pin_config_t *channel_conf;
    uint8_t                channel_cnt;
} ADC_MAPPING;

extern ADC_MAPPING ADC_MAPPING_TABLE[];

#ifdef __cplusplus
}
#endif

#endif /* __HAL_ADC_STM32F4_H */

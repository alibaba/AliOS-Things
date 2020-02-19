/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_ADC_STM32F4_H
#define __HAL_ADC_STM32F4_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "board.h"
#include "stm32f4xx_hal.h"
#include "aos/list.h"

#define HAL_ADC_1       ((uint8_t)0)
#define HAL_ADC_2       ((uint8_t)1)
#define HAL_ADC_3       ((uint8_t)2)
#define HAL_ADC_CNT     ((uint8_t)3)
#define HAL_ADC_INVALID ((uint8_t)0xff)

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

typedef struct {
    /* logical adc port */
    uint8_t hal_adc_port;
    /* logical gpio pin */
	uint8_t hal_gpio_pin;
    uint32_t hal_adc_channel;
} ADC_MAPPINT;

typedef struct {
    uint8_t           inited;
    uint8_t           port;
	ADC_HandleTypeDef hadc;
} stm32_adc_dev_t;

/* define by user in board.h */
extern ADC_MAPPINT ADC_MAPPINT_TABLE[];

#define entryof_stm32_adc_dev(phadc) \
    aos_container_of(phadc,stm32_adc_dev_t,hadc)

ADC_MAPPINT *hal_adc_get_mapping_entry(uint8_t adc_port);

#ifdef __cplusplus
}
#endif

#endif /* __HAL_UART_STM32L4_H */

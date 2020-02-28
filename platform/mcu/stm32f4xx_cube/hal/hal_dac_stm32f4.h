/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_DAC_STM32F4_H
#define __HAL_DAC_STM32F4_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"

#ifdef HAL_ADC_MODULE_ENABLED

#define HAL_DAC_1            ((uint32_t)DAC1) /**< Maps to DAC1 */
#define HAL_DAC_CNT          ((uint32_t)1) /**< Total DACs */

#define HAL_DAC_CHANNEL_1    ((uint32_t)DAC1_CHANNEL_1) /**< Maps to DACx_CHANNEL_1 */
#define HAL_DAC_CHANNEL_2    ((uint32_t)DAC1_CHANNEL_2) /**< Maps to DACx_CHANNEL_2 */
#define HAL_DAC_CHANNEL_CNT  ((uint32_t)2) /**< Total DAC channels */

/**
 * @brief The structure describes how the DAC channel and external
 *        Pin are related.
 */
typedef struct {
    uint32_t channel; /**< The logic DAC channel */
    uint8_t pin;     /**< The logic pin */
} gpio_dac_pin_config_t;

/**
 * @brief DAC mapping table structure, it's defined by user program
 *        and used by hal adc driver.
 */
typedef struct {
    uint8_t                port;         /**< User defined DAC logic port number */
    uint32_t               hal_dac;      /**< DAC logic port number */
    gpio_dac_pin_config_t *channel_conf; /**< Poiter to channel mapping table */
    uint8_t                channel_cnt;  /**< the number of entries in channel mapping table */
} DAC_MAPPING;

extern DAC_MAPPING DAC_MAPPING_TABLE[];

#endif /* HAL_ADC_MODULE_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* __HAL_UART_STM32L4_H */


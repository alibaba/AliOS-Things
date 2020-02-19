/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "board.h"

#ifdef HAL_ADC_MODULE_ENABLED

#include <k_api.h>

#include "aos/errno.h"
#include "aos/hal/adc.h"
#include "stm32f4xx_hal.h"
#include "hal_adc_stm32f4.h"
#include "stm32f4xx_hal_adc.h"

static stm32_adc_dev_t stm32_adc_dev[HAL_ADC_CNT];

ADC_MAPPINT *hal_adc_get_mapping_entry(uint8_t adc_port)
{
    ADC_MAPPINT *entry;

    if (adc_port >= HAL_ADC_CNT) {
        return NULL;
    }

    for (int i = 0; i < PORT_ADC_SIZE; i++) {
        entry = &ADC_MAPPINT_TABLE[i];
        if (entry->hal_adc_port == adc_port) {
            return entry;
        }
    }

    return NULL;
}

int32_t hal_adc_init(adc_dev_t *adc)
{
    ADC_HandleTypeDef      *hadc;
    ADC_MAPPINT            *entry;
    ADC_ChannelConfTypeDef  channel_conf;

    int32_t ret = -1;

    if (adc == NULL) {
        return -EINVAL;
    }

    if (adc->port >= HAL_ADC_CNT) {
        return -ENODEV;
    }

    if (stm32_adc_dev[adc->port].inited != 0) {
        return -EIO;
    }

    hadc = &stm32_adc_dev[adc->port].hadc;
    memset(hadc, 0, sizeof(ADC_HandleTypeDef));

    switch(adc->port) {
        case HAL_ADC_1: hadc->Instance = ADC1; break;
        case HAL_ADC_2: hadc->Instance = ADC2; break;
        case HAL_ADC_3: hadc->Instance = ADC3; break;
        default: return -EIO;
    }

    stm32_adc_dev[adc->port].port = adc->port;

    hadc->Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
    hadc->Init.Resolution = ADC_RESOLUTION_12B;
    hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc->Init.ScanConvMode = DISABLE;
    hadc->Init.EOCSelection = DISABLE;
    hadc->Init.NbrOfConversion = 1;
    hadc->Init.ContinuousConvMode = DISABLE;
    hadc->Init.DiscontinuousConvMode = DISABLE;
    hadc->Init.NbrOfDiscConversion = 0;
    hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;

    if (HAL_OK != HAL_ADC_Init(hadc)) {
        return -EIO;
    }

    memset(&channel_conf, 0, sizeof(ADC_ChannelConfTypeDef));

    entry = hal_adc_get_mapping_entry(adc->port);
    if (entry == NULL) {
        HAL_ADC_DeInit(hadc);
        return -EIO;
    }

    channel_conf.Channel = entry->hal_adc_channel;
    channel_conf.Rank = 1;
    channel_conf.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    channel_conf.Offset = 0;
    if (HAL_OK != HAL_ADC_ConfigChannel(hadc, &channel_conf)) {
        HAL_ADC_DeInit(hadc);
        return -EIO;
    }

    stm32_adc_dev[adc->port].inited = 1;

    return 0;
}

int32_t hal_adc_value_get(adc_dev_t *adc, void *output, uint32_t timeout)
{
    ADC_HandleTypeDef *hadc;
    uint16_t          *value = (uint16_t*)output;

    if (adc == NULL) {
        return -EINVAL;
    }

    if (adc->port >= HAL_ADC_CNT) {
        return -ENODEV;
    }

    if (stm32_adc_dev[adc->port].inited != 1) {
        return -EIO;
    }

    hadc = &stm32_adc_dev[adc->port].hadc;

    HAL_ADC_Start(hadc);

    if (HAL_OK != HAL_ADC_PollForConversion(hadc, timeout)) {
        return -EIO;
    }

    *value = HAL_ADC_GetValue(hadc);

    return 0;
}

int32_t hal_adc_finalize(adc_dev_t *adc)
{
    ADC_HandleTypeDef *hadc;

    if (adc == NULL) {
        return -EINVAL;
    }

    if (adc->port >= HAL_ADC_CNT) {
        return -ENODEV;
    }

    if (stm32_adc_dev[adc->port].inited != 1) {
        return -EIO;
    }

    hadc = &stm32_adc_dev[adc->port].hadc;

    if (HAL_OK != HAL_ADC_DeInit(hadc)) {
        return -EIO;
    }

    stm32_adc_dev[adc->port].inited = 0;

    return 0;

}

#endif


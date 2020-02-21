/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "stm32f4xx_hal.h"

#ifdef HAL_ADC_MODULE_ENABLED

#include <stdio.h>
#include <stdlib.h>
#include <k_api.h>
#include "board.h"
#include "aos/errno.h"
#include "aos/hal/adc.h"
#include "stm32f4xx_hal.h"
#include "hal_adc_stm32f4.h"
#include "stm32f4xx_hal_adc.h"

typedef struct {
    ADC_HandleTypeDef hadc;
    uint8_t           inited;
} stm32_adc_dev_t;

static stm32_adc_dev_t stm32_adc_dev[PORT_ADC_SIZE];

static const uint32_t adc_channel_map[HAL_ADC_CHANNEL_CNT] = {
    ADC_CHANNEL_0, ADC_CHANNEL_1, ADC_CHANNEL_2, ADC_CHANNEL_3,
    ADC_CHANNEL_4, ADC_CHANNEL_5, ADC_CHANNEL_6, ADC_CHANNEL_7,
    ADC_CHANNEL_8, ADC_CHANNEL_9, ADC_CHANNEL_10, ADC_CHANNEL_11,
    ADC_CHANNEL_12, ADC_CHANNEL_13, ADC_CHANNEL_14, ADC_CHANNEL_15,
    ADC_CHANNEL_16, ADC_CHANNEL_17, ADC_CHANNEL_18
    };

static int config_gpio(uint8_t pin)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_TypeDef    *GPIOx;

    /* Only adc channle 0 ~ channel 16 are gpio mapped */
    GPIOx = hal_gpio_typedef(pin);
    if (NULL == GPIOx) {
       return -1;
    }

    hal_gpio_enable_clk(pin);

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));

    GPIO_InitStruct.Pin = hal_gpio_pin(pin);
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

    return 0;
}

static ADC_MAPPING *get_mapping_entry(uint8_t port)
{
    int i;

    for (i = 0; i < PORT_ADC_SIZE; i++) {
        if (port == ADC_MAPPING_TABLE[i].port) {
            return &ADC_MAPPING_TABLE[i];
        }
    }

    return NULL;
}

int32_t hal_adc_init(adc_dev_t *adc)
{
    ADC_HandleTypeDef       *hadc;
    ADC_ChannelConfTypeDef   sconf;
    stm32_adc_dev_t         *pdev;
    ADC_MAPPING             *map_entry;
    gpio_adc_pin_config_t   *channel_conf;
    int                      channel, pin;
    int                      i, entry_cnt;

    int32_t ret = -1;

    if (adc == NULL) {
        return -EINVAL;
    }

    if (adc->port >= PORT_ADC_SIZE) {
        return -ENODEV;
    }

    pdev = &stm32_adc_dev[adc->port];
    if (pdev->inited != 0) {
        return -EIO;
    }

    hadc = &pdev->hadc;
    memset(hadc, 0, sizeof(ADC_HandleTypeDef));

    map_entry = get_mapping_entry(adc->port);
    entry_cnt = map_entry->channel_cnt;
    channel_conf = map_entry->channel_conf;

    switch(map_entry->hal_adc) {
        case HAL_ADC_1: hadc->Instance = ADC1; break;
        case HAL_ADC_2: hadc->Instance = ADC2; break;
        case HAL_ADC_3: hadc->Instance = ADC3; break;
        default: return -EIO;
    }

    // TODO: support more than one channel, to this end, DMA must
    //       be configured to transfer the converted data.
    //       To be simple, here we just support one channel at once.
    hadc->Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
    hadc->Init.Resolution = ADC_RESOLUTION_12B;
    hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc->Init.ScanConvMode = DISABLE;
    hadc->Init.EOCSelection = DISABLE;
    hadc->Init.NbrOfConversion = entry_cnt;
    hadc->Init.ContinuousConvMode = DISABLE;
    hadc->Init.DiscontinuousConvMode = DISABLE;
    hadc->Init.NbrOfDiscConversion = 0;
    hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;

    if (HAL_OK != HAL_ADC_Init(hadc)) {
        return -EIO;
    }

    for (i = 0; i < entry_cnt; i++, channel_conf++) {
        if ((channel_conf->channel < HAL_ADC_CHANNEL_16) && (channel_conf->pin < HAL_GPIO_CNT)) {
            config_gpio(channel_conf->pin);
            memset(&sconf, 0, sizeof(ADC_ChannelConfTypeDef));
            sconf.Channel = adc_channel_map[channel_conf->channel];
            sconf.Rank = i + 1;
            sconf.SamplingTime = ADC_SAMPLETIME_480CYCLES;
            sconf.Offset = 0;
            if (HAL_OK != HAL_ADC_ConfigChannel(hadc, &sconf)) {
                HAL_ADC_DeInit(hadc);
                return -EIO;
            }
        } else {
            return -EIO;
        }
    }

    // TODO: Config DMA for output data.

    pdev->inited = 1;

    printf("hal_init_done\r\n");

    return 0;
}

int32_t hal_adc_value_get(adc_dev_t *adc, void *output, uint32_t timeout)
{
    ADC_HandleTypeDef *hadc;
    stm32_adc_dev_t   *pdev;
    uint16_t          *value = (uint16_t*)output;

    if (adc == NULL) {
        return -EINVAL;
    }

    if (adc->port >= PORT_ADC_SIZE) {
        return -ENODEV;
    }

    pdev = &stm32_adc_dev[adc->port];

    if (pdev->inited != 1) {
        return -EIO;
    }

    hadc = &pdev->hadc;


    HAL_ADC_Start(hadc);

    if (HAL_OK != HAL_ADC_PollForConversion(hadc, timeout)) {
        return -EIO;
    }

    *value = HAL_ADC_GetValue(hadc);

    // TODO: read data from DMA destination buffer into output buffer

    return 0;
}

int32_t hal_adc_finalize(adc_dev_t *adc)
{
    GPIO_TypeDef          *GPIOx;
    ADC_HandleTypeDef     *hadc;
    stm32_adc_dev_t       *pdev;
    ADC_MAPPING           *map_entry;
    gpio_adc_pin_config_t *channel_conf;
    int                   channel, pin;
    int                   i, entry_cnt;

    if (adc == NULL) {
        return -EINVAL;
    }

    if (adc->port >= HAL_ADC_CNT) {
        return -ENODEV;
    }

    pdev = &stm32_adc_dev[adc->port];

    if (pdev->inited != 1) {
        return -EIO;
    }

    /* deinit gpio */
    map_entry = get_mapping_entry(adc->port);
    entry_cnt = map_entry->channel_cnt;
    channel_conf = map_entry->channel_conf;

    for (i = 0; i < entry_cnt; i++, channel_conf++) {
        if ((channel_conf->channel <= HAL_ADC_CHANNEL_16) && (channel_conf->pin < HAL_GPIO_CNT)) {
            GPIOx = hal_gpio_typedef(channel_conf->pin);
            if (NULL == GPIOx) {
               return -EIO;
            }

            HAL_GPIO_DeInit(GPIOx, hal_gpio_pin(channel_conf->pin));
        }
    }

    hadc = &pdev->hadc;
    if (HAL_OK != HAL_ADC_DeInit(hadc)) {
        return -EIO;
    }
    pdev->inited = 0;

    return 0;
}

#endif

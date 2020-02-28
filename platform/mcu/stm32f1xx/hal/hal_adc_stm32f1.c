/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "stm32f1xx_hal.h"

#ifdef HAL_ADC_MODULE_ENABLED

#include <stdio.h>
#include <stdlib.h>
#include <k_api.h>
#include "board.h"
#include "aos/errno.h"
#include "aos/hal/adc.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_adc.h"
#include "hal_adc_stm32f1.h"
#include "hal_gpio_stm32f1.h"

typedef struct {
    ADC_HandleTypeDef hadc;
    uint8_t           inited;
} stm32_adc_dev_t;

static stm32_adc_dev_t stm32_adc_dev[PORT_ADC_SIZE];

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

    hadc->Instance = (ADC_TypeDef*)map_entry->hal_adc;

    // TODO: support more than one channel, to this end, DMA must
    //       be configured to transfer the converted data.
    //       To be simple, here we just support one channel at once.
    hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc->Init.ScanConvMode = DISABLE;
    hadc->Init.NbrOfConversion = 1;
    hadc->Init.ContinuousConvMode = DISABLE;
    hadc->Init.DiscontinuousConvMode = DISABLE;
    hadc->Init.NbrOfDiscConversion = 0;
    hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;

    if (HAL_OK != HAL_ADC_Init(hadc)) {
        return -EIO;
    }

    for (i = 0; i < entry_cnt; i++, channel_conf++) {
        if ((channel_conf->channel < HAL_ADC_CHANNEL_16) && (channel_conf->pin < HAL_GPIO_CNT)) {
            config_gpio(channel_conf->pin);
        } else {
            return -EIO;
        }
    }

    pdev->inited = 1;

    printf("hal_init_done\r\n");

    return 0;
}

int32_t hal_adc_value_get(adc_dev_t *adc, uint32_t *output, uint32_t timeout)
{
    ADC_HandleTypeDef       *hadc;
    ADC_ChannelConfTypeDef   sconf;
    stm32_adc_dev_t         *pdev;
    ADC_MAPPING             *map_entry;
    gpio_adc_pin_config_t   *channel_conf;
    int                      i, entry_cnt;

    uint32_t *value = (uint32_t*)output;

    if (adc == NULL || value == NULL) {
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

    map_entry = get_mapping_entry(adc->port);
    entry_cnt = map_entry->channel_cnt;
    channel_conf = map_entry->channel_conf;

    for (i = 0; i < entry_cnt; i++, channel_conf++) {
        if (channel_conf->channel < HAL_ADC_CHANNEL_16) {
            memset(&sconf, 0, sizeof(ADC_ChannelConfTypeDef));
            sconf.Channel = channel_conf->channel;
            sconf.Rank = 1;
            sconf.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;
            if (HAL_OK != HAL_ADC_ConfigChannel(hadc, &sconf)) {
                return -EIO;
            }

            HAL_ADC_Start(hadc);

            if (HAL_OK != HAL_ADC_PollForConversion(hadc, timeout)) {
                return -EIO;
            }

            *value++ = HAL_ADC_GetValue(hadc);

        } else {
            return -EIO;
        }
    }

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

#endif /* HAL_ADC_MODULE_ENABLED */


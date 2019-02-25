/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx_hal.h"
#include "hal_adc_stm32f4.h"
#include "stm32f4xx_hal_adc.h"
#include "hal/soc/soc.h"

#ifdef HAL_ADC_MODULE_ENABLED

/* Init and deInit function for adc1 */
static int32_t adc1_init(adc_dev_t *uart);
static int32_t adc1_DeInit(void);

/* function used to transform hal para to stm32l4 para */
int32_t get_adc_Instance(adc_dev_t *adc, ADC_TypeDef **ADCx);
uint16_t get_adc_channel(uint8_t port);

/* handle for adc */
ADC_HandleTypeDef adc1_handle;
ADC_ChannelConfTypeDef adc1_sConfig;

int32_t hal_adc_init(adc_dev_t *adc)
{
    int32_t ret = -1;

    if (adc == NULL) {
        return -1;
    }

    /*init adc handle*/
    memset(&adc1_handle, 0, sizeof(adc1_handle));
    memset(&adc1_sConfig, 0, sizeof(adc1_sConfig));

    switch (adc->port) {
        case PORT_ADC1:
            adc->priv = &adc1_handle;
            ret = adc1_init(adc);
            break;

    /* if ohter adc exist add init code here */

        default:
            break;
    }

    return ret;
}

int32_t hal_adc_value_get(adc_dev_t *adc, void *output, uint32_t timeout)
{
    int32_t ret = -1;
    uint16_t *value = (uint16_t *)output;

    ret = HAL_ADC_PollForConversion((ADC_HandleTypeDef*)adc->priv, timeout);
    if (ret == 0) {
        *value = HAL_ADC_GetValue((ADC_HandleTypeDef*)adc->priv);
    }

    return ret;
}

int32_t hal_adc_finalize(adc_dev_t *adc)
{
    int32_t ret = -1;

    if (adc == NULL) {
        return -1;
    }

    switch (adc->port) {
        case PORT_ADC1:
            ret = adc1_DeInit();
            break;
    /* if other adc exist add Deinit code here */

        default:
            break;
    }

    return ret;
}

int32_t adc1_init(adc_dev_t *adc)
{
    ADC_TypeDef *adc_instance = NULL;
    int32_t ret = 0;

    ret = get_adc_Instance(adc, &adc_instance);
    adc1_handle.Instance = adc_instance;

    /* Initialize other parameters in struction ADC_InitTypeDef */

    ret = HAL_ADC_Init(&adc1_handle);
    if (ret != 0) {
        return -1;
    }

    adc1_sConfig.Channel = get_adc_channel(adc->port);
    /* Initialize other parameters in struction ADC_ChannelConfTypeDef */

    ret = HAL_ADC_ConfigChannel(&adc1_handle, &adc1_sConfig);

    return ret;
}

int32_t adc1_DeInit(void)
{
    int32_t ret = -1;

    /* adc1 deinitialization */
    ret = HAL_ADC_DeInit(&adc1_handle);

    return ret;
}

int32_t get_adc_Instance(adc_dev_t *adc, ADC_TypeDef **ADCx)
{
    /* Get adc instance according to adc->port */
    return 0;
}

uint16_t get_adc_channel(uint8_t port)
{
    /* Get adc channel according to adc->port */
    return 0;
}
#endif


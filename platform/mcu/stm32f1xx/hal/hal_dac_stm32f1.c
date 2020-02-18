/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>

#include "aos/hal/dac.h"

#include "stm32f1xx_hal.h"

#ifdef HAL_DAC_MODULE_ENABLED

#if defined (STM32F100xB) || defined (STM32F100xE) || defined (STM32F101xE) || defined (STM32F101xG) || defined (STM32F103xE) || defined       (STM32F103xG) || defined (STM32F105xC) || defined (STM32F107xC)

#include "hal_dac_stm32f1.h"
#include "stm32f1xx_hal_dac.h"

typedef struct {
     uint8_t           inited;
     DAC_HandleTypeDef hdac;
}stm32_dac_t;

static stm32_dac_t stm32_dac[HAL_DAC_SIZE];

int32_t hal_dac_init(dac_dev_t *dac)
{
    DAC_HandleTypeDef      *hdac;
    DAC_ChannelConfTypeDef  sConfig;

    if (NULL == dac) {
        return -EIO;
    }

    /* Invalid port number */
    if (dac->port >= HAL_DAC_SIZE) {
        return -EIO;
    }

    /* Already initialized */
    if (stm32_dac[dac->port].inited != 0) {
        return 0;
    }

    hdac = &stm32_dac[dac->port].hdac;

    if (dac->port == HAL_DAC_1) {
        hdac->Instance = DAC;
    } else {
        return -EIO;
    }


    HAL_DAC_Init(hdac);

    memset(&sConfig, 0, sizeof(sConfig));
    sConfig.DAC_Trigger = DAC_TRIGGER_SOFTWARE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;

    HAL_DAC_ConfigChannel(hdac, &sConfig, DAC1_CHANNEL_1);
    HAL_DAC_ConfigChannel(hdac, &sConfig, DAC1_CHANNEL_2);

    stm32_dac[dac->port].inited = 1;

    return 0;
}

int32_t hal_dac_start(dac_dev_t *dac, uint32_t channel)
{
    DAC_HandleTypeDef *hdac;

    if (NULL == dac) {
        return -EIO;
    }

    /* Invalid port number */
    if (dac->port >= HAL_DAC_SIZE) {
        return -EIO;
    }

    if (stm32_dac[dac->port].inited != 1) {
        return -EIO;
    }

    hdac = &stm32_dac[dac->port].hdac;
    if (HAL_OK != HAL_DAC_Start(hdac, channel)) {
        return -EIO;
    }

    return 0;
}

int32_t hal_dac_stop(dac_dev_t *dac, uint32_t channel)
{
    DAC_HandleTypeDef *hdac;

    if (NULL == dac) {
        return -EIO;
    }

    /* Invalid port number */
    if (dac->port >= HAL_DAC_SIZE) {
        return -EIO;
    }

    if (stm32_dac[dac->port].inited != 1) {
        return -EIO;
    }

    hdac = &stm32_dac[dac->port].hdac;
    if (HAL_OK != HAL_DAC_Stop(hdac, channel)) {
        return -EIO;
    }

    return 0;
}

int32_t hal_dac_set_value(dac_dev_t *dac, uint32_t channel, uint32_t data)
{
    DAC_HandleTypeDef *hdac;

    if (NULL == dac) {
        return -EIO;
    }

    /* Invalid port number */
    if (dac->port >= HAL_DAC_SIZE) {
        return -EIO;
    }

    if (stm32_dac[dac->port].inited != 1) {
        return -EIO;
    }

    hdac = &stm32_dac[dac->port].hdac;
    if (HAL_OK != HAL_DAC_SetValue(hdac, channel, DAC_ALIGN_12B_R, data)) {
        return -EIO;
    }

    return 0;
}

int32_t hal_dac_get_value(dac_dev_t *dac, uint32_t channel)
{
    DAC_HandleTypeDef *hdac;

    if (NULL == dac) {
        return 0;
    }

    /* Invalid port number */
    if (dac->port >= HAL_DAC_SIZE) {
        return 0;
    }

    if (stm32_dac[dac->port].inited != 1) {
        return 0;
    }

    hdac = &stm32_dac[dac->port].hdac;

    return HAL_DAC_GetValue(hdac, channel);
}

int32_t hal_dac_finalize(dac_dev_t *dac)
{
    DAC_HandleTypeDef *hdac;

    if (NULL == dac) {
        return -EIO;
    }

    /* Invalid port number */
    if (dac->port >= HAL_DAC_SIZE) {
        return -EIO;
    }

    if (stm32_dac[dac->port].inited != 1) {
        return -EIO;
    }

    hdac = &stm32_dac[dac->port].hdac;

    HAL_DAC_Stop(hdac, DAC1_CHANNEL_1);
    HAL_DAC_Stop(hdac, DAC1_CHANNEL_2);

    if (HAL_OK != HAL_DAC_DeInit(hdac)) {
        return -EIO;
    }

    stm32_dac[dac->port].inited = 0;
    memset(&stm32_dac[dac->port], 0, sizeof(stm32_dac_t));

    return 0;
}
#endif /* (STM32F100xB) || defined (STM32F100xE) || defined (STM32F101xE) || defined (STM32F101xG) || defined (STM32F103xE) || defined       (STM32F103xG) || defined (STM32F105xC) || defined (STM32F107xC) */

#endif  /* HAL_DAC_MODULE_ENABLED */


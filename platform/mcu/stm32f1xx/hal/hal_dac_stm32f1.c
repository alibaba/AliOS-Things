/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "stm32f1xx_hal.h"

#ifdef HAL_DAC_MODULE_ENABLED

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "aos/errno.h"
#include "aos/hal/dac.h"
#include "stm32f1xx_hal_dac.h"
#include "hal_dac_stm32f1.h"
#include "hal_gpio_stm32f4.h"

typedef struct {
    DAC_HandleTypeDef hdac;
    uint8_t           inited;
} stm32_dac_t;

static stm32_dac_t stm32_dac[PORT_DAC_SIZE];

static int config_gpio(uint8_t pin)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_TypeDef    *GPIOx;

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

static DAC_MAPPING *get_mapping_entry(uint8_t port)
{
    int i;

    for (i = 0; i < PORT_DAC_SIZE; i++) {
        if (port == DAC_MAPPING_TABLE[i].port) {
            return &DAC_MAPPING_TABLE[i];
        }
    }

    return NULL;
}

int32_t hal_dac_init(dac_dev_t *dac)
{
    DAC_HandleTypeDef      *hdac;
    DAC_MAPPING            *map_entry;
    gpio_dac_pin_config_t  *dac_conf;
    DAC_ChannelConfTypeDef  sConfig;
    int                     i;

    if (NULL == dac) {
        return -EIO;
    }

    /* Invalid port number */
    if (dac->port >= PORT_DAC_SIZE) {
        return -EIO;
    }

    /* Already initialized */
    if (stm32_dac[dac->port].inited != 0) {
        return 0;
    }

    map_entry = get_mapping_entry(dac->port);
    hdac = &stm32_dac[dac->port].hdac;

    hdac->Instance = (DAC_TypeDef*)map_entry->hal_dac;

    // init dac gpio
    dac_conf = map_entry->channel_conf;
    for (i = 0; i < map_entry->channel_cnt; i++, dac_conf++) {
        config_gpio(dac_conf->pin);
    }

    HAL_DAC_Init(hdac);

    memset(&sConfig, 0, sizeof(sConfig));
    sConfig.DAC_Trigger = DAC_TRIGGER_SOFTWARE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;

    dac_conf = map_entry->channel_conf;
    for (i = 0; i < map_entry->channel_cnt; i++, dac_conf++) {
        HAL_DAC_ConfigChannel(hdac, &sConfig, dac_conf->channel);
    }

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
    if (dac->port >= PORT_DAC_SIZE) {
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
    if (dac->port >= PORT_DAC_SIZE) {
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
    if (dac->port >= PORT_DAC_SIZE) {
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
    if (dac->port >= PORT_DAC_SIZE) {
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
    DAC_HandleTypeDef      *hdac;
    DAC_MAPPING            *map_entry;
    gpio_dac_pin_config_t  *dac_conf;
    int                     i;

    if (NULL == dac) {
        return -EIO;
    }

    /* Invalid port number */
    if (dac->port >= PORT_DAC_SIZE) {
        return -EIO;
    }

    if (stm32_dac[dac->port].inited != 1) {
        return -EIO;
    }

    hdac = &stm32_dac[dac->port].hdac;
    map_entry = get_mapping_entry(dac->port);
    dac_conf = map_entry->channel_conf;
    for (i = 0; i < map_entry->channel_cnt; i++, dac_conf++) {
        HAL_DAC_Stop(hdac, dac_conf->channel);
    }

    if (HAL_OK != HAL_DAC_DeInit(hdac)) {
        return -EIO;
    }

    stm32_dac[dac->port].inited = 0;
    memset(&stm32_dac[dac->port], 0, sizeof(stm32_dac_t));

    return 0;
}

#endif  /* HAL_DAC_MODULE_ENABLED */


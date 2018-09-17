/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "hal/hal.h"
#include "stm32l4xx_hal.h"
#include "hal_spi_stm32l4.h"
#include "stm32l4xx_hal_spi.h"

#ifdef HAL_SPI_MODULE_ENABLED 
/* Init and deInit function for spi */
static int32_t spi_init(spi_dev_t *spi);
static int32_t spi_DeInit(spi_dev_t *spi);

/* function used to transform hal para to stm32l4 para */
static int32_t spi_mode_transform(uint32_t mode_hal, uint32_t *mode_stm32l4);
static int32_t spi_freq_transform(uint32_t freq_hal, uint32_t *BaudRatePrescaler_stm32l4_stm32l4);

/* handle for spi */
SPI_HandleTypeDef spi1_handle = {0};
SPI_HandleTypeDef spi2_handle = {0};
SPI_HandleTypeDef spi3_handle = {0};

int32_t hal_spi_init(spi_dev_t *spi)
{
    int32_t ret = -1;

    if (spi == NULL) {
        return -1;
    }

    switch (spi->port) {
        case PORT_SPI1:
            spi->priv = &spi1_handle;
            ret = spi_init(spi);
            break;

        case PORT_SPI2:
            spi->priv = &spi2_handle;
            ret = spi_init(spi);
            break;

        case PORT_SPI3:
            spi->priv = &spi3_handle;
            ret = spi_init(spi);
            break;

    /* if ohter spi exist add init code here */

        default:
            break;
    }

    return ret;
}

static inline SPI_HandleTypeDef *get_priv(uint8_t port)
{
    SPI_HandleTypeDef *hspi = NULL;

    switch (port) {
        case PORT_SPI1:
            hspi = &spi1_handle;
            break;
        case PORT_SPI2:
            hspi = &spi2_handle;
            break;
        case PORT_SPI3:
            hspi = &spi3_handle;
            break;

        default:
            return NULL;
    }

    return hspi;
}


int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret = -1;

    SPI_HandleTypeDef *pstspi = NULL;

    if (NULL == spi || NULL == data) {
        return ret;
    }

    pstspi = get_priv(spi->port);

    ret = HAL_SPI_Transmit(pstspi, (uint8_t *)data, size, timeout);

    return ret;
}

int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret = -1;

    SPI_HandleTypeDef *pstspi = NULL;

    if (NULL == spi || NULL == data) {
        return ret;
    }

    pstspi = get_priv(spi->port);


    ret = HAL_SPI_Receive(pstspi, (uint8_t *)data, size, timeout);

    return ret;
}

int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
        uint16_t size, uint32_t timeout)
{
    int32_t ret = -1;

    SPI_HandleTypeDef *pstspi = NULL;

    if ((NULL == spi) || (NULL == tx_data) || (NULL == rx_data)) {
        return ret;
    }

    pstspi = get_priv(spi->port);


    ret = HAL_SPI_TransmitReceive(pstspi, (uint8_t *)tx_data, (uint8_t *)rx_data, size, timeout);

    return ret;
}

int32_t hal_spi_finalize(spi_dev_t *spi)
{
    int32_t ret = -1;

    if (NULL == spi) {
        return -1;
    }

    switch (spi->port) {
        case PORT_SPI1:
        case PORT_SPI2:
        case PORT_SPI3:
            ret = spi_DeInit(spi);
           break;
    /* if other spi exist add Deinit code here */

        default:
            break;
    }

    return ret;
}

int32_t spi_init(spi_dev_t *spi)
{
    int32_t ret = -1;

    SPI_HandleTypeDef *handle = (SPI_HandleTypeDef *)spi->priv;

    if (HAL_SPI_GetState(handle) == HAL_SPI_STATE_RESET) {

        switch (spi->port) {
            case PORT_SPI1:
                handle->Instance = SPI1;
                handle->Init.Mode = SPI_MODE_MASTER;
                handle->Init.Direction = SPI_DIRECTION_1LINE;
                handle->Init.DataSize = SPI_DATASIZE_8BIT;
                handle->Init.CLKPolarity = SPI_POLARITY_LOW;
                handle->Init.CLKPhase = SPI_PHASE_1EDGE;
                handle->Init.NSS = SPI_NSS_HARD_OUTPUT;
                handle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
                handle->Init.FirstBit = SPI_FIRSTBIT_MSB;
                handle->Init.TIMode = SPI_TIMODE_DISABLE;
                handle->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
                handle->Init.CRCPolynomial = 7;
                handle->Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
                handle->Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
                break;

            case PORT_SPI2:
                handle->Instance = SPI2;
                handle->Init.Mode = SPI_MODE_MASTER;
                handle->Init.Direction = SPI_DIRECTION_2LINES;
                handle->Init.DataSize = SPI_DATASIZE_16BIT;
                handle->Init.CLKPolarity = SPI_POLARITY_LOW;
                handle->Init.CLKPhase = SPI_PHASE_1EDGE;
                handle->Init.NSS = SPI_NSS_HARD_OUTPUT;
                handle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
                handle->Init.FirstBit = SPI_FIRSTBIT_MSB;
                handle->Init.TIMode = SPI_TIMODE_DISABLE;
                handle->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
                handle->Init.CRCPolynomial = 7;
                handle->Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
                handle->Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
                break;

            case PORT_SPI3:
                handle->Instance = SPI3;
                handle->Init.Mode = SPI_MODE_MASTER;
                handle->Init.Direction = SPI_DIRECTION_1LINE;
                handle->Init.DataSize = SPI_DATASIZE_8BIT;
                handle->Init.CLKPolarity = SPI_POLARITY_LOW;
                handle->Init.CLKPhase = SPI_PHASE_1EDGE;
                handle->Init.NSS = SPI_NSS_HARD_OUTPUT;
                handle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
                handle->Init.FirstBit = SPI_FIRSTBIT_MSB;
                handle->Init.TIMode = SPI_TIMODE_DISABLE;
                handle->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
                handle->Init.CRCPolynomial = 7;
                handle->Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
                handle->Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
                break;

                /* if ohter spi exist add init code here */

            default:
                break;
        }

        ret = spi_mode_transform(spi->config.mode, &handle->Init.Mode);
        ret = spi_freq_transform(spi->config.freq, &handle->Init.BaudRatePrescaler);
        if (ret != 0) {
            return -1;
        }

        /* Initialize other parameters in struction SPI_InitTypeDef */

        /* init spi */
        ret = HAL_SPI_Init(handle);
    }

    return ret;
}

int32_t spi_DeInit(spi_dev_t *spi)
{
    int32_t ret = -1;

    SPI_HandleTypeDef *handle = (SPI_HandleTypeDef *)spi->priv;

    if (HAL_SPI_GetState(handle) != HAL_SPI_STATE_RESET) {
        /* spi deinitialization */
        ret = HAL_SPI_DeInit(handle);
    }
    return ret;
}

static int32_t spi_mode_transform(uint32_t mode_hal, uint32_t *mode_stm32l4)
{
    uint32_t mode = 0;
    int32_t	ret = 0;

    if(mode_hal == HAL_SPI_MODE_MASTER)
    {
        mode = SPI_MODE_MASTER;
    }
    else if(mode_hal == HAL_SPI_MODE_SLAVE)
    {
        mode = SPI_MODE_SLAVE;
    }
    else
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *mode_stm32l4 = mode;
    }

    return ret;
}

static int32_t spi_freq_transform(uint32_t freq_hal, uint32_t *BaudRatePrescaler_stm32l4)
{
    int32_t	ret = 0;

    /* calc BaudRatePrescaler according to freq_hal */

    return ret;
}
#endif

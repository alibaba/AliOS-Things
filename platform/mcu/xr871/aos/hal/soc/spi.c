/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hal/soc/soc.h"
#include "driver/chip/hal_spi.h"

/**
 * Initialises the SPI interface for a given SPI device
 *
 * @param[in]  spi  the spi device
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
int32_t hal_spi_init(spi_dev_t *spi)
{
    HAL_Status status = HAL_ERROR;
    SPI_Global_Config spi_param = {
		.mclk	  = 24000000,
		.cs_level = 0,
    };
    SPI_Config spi_config = {
        .mode = spi->config.mode == HAL_SPI_MODE_MASTER ? SPI_CTRL_MODE_MASTER : SPI_CTRL_MODE_SLAVE,
        .opMode = SPI_OPERATION_MODE_POLL,
        .firstBit = SPI_TCTRL_FBS_MSB,
        .sclk = spi->config.freq,
        .sclkMode = SPI_SCLK_Mode0,
	};
    if (spi == NULL)
        return -1;
    status = HAL_SPI_Init(spi->port, &spi_param);
    if (status != HAL_OK)
        return -1;

	status = HAL_SPI_Open(spi->port, SPI_TCTRL_SS_SEL_SS0, &spi_config, 1);
    if (status != HAL_OK)
        return -1;
    return 0;
}

/**
 * Spi send
 *
 * @param[in]  spi      the spi device
 * @param[in]  data     spi send data
 * @param[in]  size     spi send data size
 * @param[in]  timeout  timeout in ms
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    if (spi == NULL || data == NULL)
        return -1;
    HAL_SPI_CS(spi->port, 1);
    HAL_Status status = HAL_SPI_Transmit(spi->port, data, size);
    HAL_SPI_CS(spi->port, 0);
	if (status != HAL_OK) {
        printf("hal spi send failed\n");
        return -1;
    }
    return 0;
}

/**
 * spi_recv
 *
 * @param[in]   spi      the spi device
 * @param[out]  data     spi recv data
 * @param[in]   size     spi recv data size
 * @param[in]   timeout  timeout in ms
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
    if (spi == NULL || data == NULL)
        return -1;
    HAL_SPI_CS(spi->port, 1);
    HAL_Status status = HAL_SPI_Receive(spi->port, data, size);
    HAL_SPI_CS(spi->port, 0);
	if (status != HAL_OK) {
        printf("hal spi recv failed\n");
        return -1;
    }
    return 0;
}

/**
 * spi send data and recv
 *
 * @param[in]  spi      the spi device
 * @param[in]  tx_data  spi send data
 * @param[in]  rx_data  spi recv data
 * @param[in]  tx_size  spi data to be sent
 * @param[in]  rx_size  spi data to be recv
 * @param[in]  timeout  timeout in ms
 *
 * @return  0, on success;  EIO : if the SPI device could not be initialised
 */
int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data, uint16_t size, uint32_t timeout)
{
    if (spi == NULL || tx_data == NULL || rx_data == NULL)
        return -1;
    HAL_SPI_CS(spi->port, 1);
    HAL_Status status = HAL_SPI_TransmitReceive(spi->port, tx_data, rx_data, size);
    HAL_SPI_CS(spi->port, 0);
	if (status != HAL_OK) {
        printf("hal spi send/recv failed\n");
        return -1;
    }
    return 0;
}

/**
 * De-initialises a SPI interface
 *
 *
 * @param[in]  spi the SPI device to be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred
 */
int32_t hal_spi_finalize(spi_dev_t *spi)
{
    HAL_SPI_Close(spi->port);
    HAL_SPI_Deinit(spi->port);
    return 0;
}

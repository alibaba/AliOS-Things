/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef CH395_SPI_H
#define CH395_SPI_H

#include "stdint.h"
#include "aos/hal/spi.h"

int32_t hal_ch395_spi_init(spi_dev_t *spi);

int32_t hal_spi_send_ch395(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout);

int32_t hal_spi_send_and_recv_ch395_normal(spi_dev_t *spi, uint8_t *tx_data, uint16_t tx_size, uint8_t *rx_data,
							  uint16_t rx_size, uint32_t timeout);

int32_t hal_spi_send_and_recv_ch395_exist(spi_dev_t *spi, uint8_t *tx_data, uint16_t tx_size, uint8_t *rx_data,
							  uint16_t rx_size, uint32_t timeout);

#endif

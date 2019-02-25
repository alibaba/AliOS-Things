/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "aos/hal/spi.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_spi.h"

#ifdef HAL_SPI_MODULE_ENABLED

int32_t hal_spi_init(spi_dev_t *spi)
{
}

int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
}

int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{

}

int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
                          uint16_t size, uint32_t timeout)
{
}

int32_t hal_spi_finalize(spi_dev_t *spi)
{
}

#endif

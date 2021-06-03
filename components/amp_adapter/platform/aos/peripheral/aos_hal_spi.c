/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/errno.h>
#include <vfsdev/spi_dev.h>
#include "aos/hal/spi.h"
#include "aos_hal_spi.h"

int32_t aos_hal_spi_init(spi_dev_t *spi)
{
    return hal_spi_init(spi);
}

int32_t aos_hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    return hal_spi_send(spi, data, size, timeout);
}

int32_t aos_hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
    return hal_spi_recv(spi, data, size, timeout);
}

int32_t aos_hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
                          uint16_t size, uint32_t timeout)
{
    return hal_spi_send_recv(spi, tx_data, rx_data, size, timeout);
}

int32_t aos_hal_spi_finalize(spi_dev_t *spi)
{
    return hal_spi_finalize(spi);
}



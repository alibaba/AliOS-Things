/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "amp_defines.h"
#include "ql_spi.h"
#include "amp_hal_spi.h"


#define MOD_STR		"SPI"

int32_t amp_hal_spi_init(spi_dev_t *spi)
{
    int32_t ret = -1;
    QL_SPI_MODE_E spi_mode;
    QL_SPI_CLK_E spi_clk;

    if (spi == NULL) {
        return ret;
    }

    if (spi->port >= QL_SPI_PORT_MAX) {
        return ret;
    }

    switch (spi->config.mode) {
        case HAL_SPI_MODE_MASTER:
            spi_mode = QL_SPI_MODE0;
            break;
        case HAL_SPI_MODE_SLAVE:
            spi_mode = QL_SPI_MODE1;
            break;
        default: break;
    }

    ret = ql_spi_init(spi->port, spi_mode, QL_SPI_CLK_3_25MHZ);
    if (ret != 0) {
        return ret;
    }

    return ret;
}

int32_t amp_hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret = -1;

    if (spi == NULL || data == NULL) {
        return ret;
    }
    ret = ql_spi_write(spi->port, data, size);
    if (ret != 0) {
        return ret;
    }

    return ret;
}

int32_t amp_hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret = -1;

    if (spi == NULL || data == NULL) {
        return ret;
    }
    ret = ql_spi_read(spi->port, data, size);
    if (ret != 0) {
        return ret;
    }

    return ret;
}

int32_t amp_hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
                          uint16_t size, uint32_t timeout)
{
    int32_t ret = -1;

    if (spi == NULL || tx_data == NULL || rx_data == NULL) {
        return ret;
    }
    ret = ql_spi_write_read(spi->port, rx_data, tx_data, size);
    if (ret != 0) {
        return ret;
    }

    return ret;
}

int32_t amp_hal_spi_finalize(spi_dev_t *spi)
{
    return 0;
}



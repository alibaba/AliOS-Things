/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/errno.h>
#include <vfsdev/spi_dev.h>
#include "amp_hal_spi.h"


int32_t amp_hal_spi_init(spi_dev_t *spi)
{
    uint32_t flags = 0;
    int32_t ret = 0;
    int32_t port = 0;
    int32_t *p_fd = NULL;
    char name[16] = {0};

    if (!spi || spi->priv)
        return -EINVAL;

    port = spi->port;
    if (spi->config.mode != HAL_SPI_MODE_MASTER) {
        printf ("spi only support master mode\r\n");
        return -EINVAL;
    }

    p_fd = (int32_t *)malloc(sizeof(int32_t));
    if (!p_fd)
        return -ENOMEM;
    *p_fd = -1;

    snprintf(name, sizeof(name), "/dev/spi%d", port);

    *p_fd = open(name, 0);

    if (*p_fd < 0) {
        printf ("open %s failed, fd:%d\r\n", name, *p_fd);
        ret = -EIO;
	goto out;
    }
    // set SPI's parameters and clock frequency
    ret = ioctl(*p_fd, IOC_SPI_SET_CFLAG, SPI_MODE_0 | SPI_MSB | SPI_MASTER | SPI_DATA_8BIT);
    if (ret) {
        printf ("ioctl set CFLAGon %s failed, ret:%d\r\n", name, ret);
        goto out;
    }

    ret = ioctl(*p_fd, IOC_SPI_SET_FREQ, spi->config.freq);
    if (ret) {
        printf ("ioctl on %s failed, ret:%d\r\n", name, ret);
        goto out;
    }

out:
    if (!ret) {
        spi->priv = p_fd;
    } else {
        if (*p_fd >= 0)
            close(*p_fd);
        free(p_fd);
        p_fd = NULL;
    }
    return ret;
}

int32_t amp_hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;
    int32_t fd = -1;
    int32_t *p_fd = NULL;

    if (!spi || !data || !size) {
        return -EINVAL;
    }
    p_fd = (int32_t *)spi->priv;

    if (!p_fd || *p_fd < 0)
        return -EIO;

    ret = write(*p_fd, data, size);

    return ret == size ? 0 : ret;
}

int32_t amp_hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;
    int32_t fd = -1;
    int32_t *p_fd = NULL;

    if (!spi || !data || !size) {
        return -EINVAL;
    }
    p_fd = (int32_t *)spi->priv;

    if (!p_fd || *p_fd < 0)
        return -EIO;

    ret = read(*p_fd, data, size);

    return ret == size ? 0 : ret;
}

int32_t amp_hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
                          uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;
    int32_t fd = -1;
    int32_t *p_fd = NULL;
    ioc_spi_transfer_t tranptr;
    if (!spi || !tx_data || !rx_data || !size) {
        return -EINVAL;
    }
    p_fd = (int32_t *)spi->priv;

    if (!p_fd || *p_fd < 0)
        return -EIO;
    tranptr.tx_buf = tx_data;
    tranptr.rx_buf = rx_data;
    tranptr.tx_buf = size;
    tranptr.rx_size = size;

    ret = ioctl(*p_fd, IOC_SPI_SEND_RECV, &tranptr);
    return ret;
}

int32_t amp_hal_spi_finalize(spi_dev_t *spi)
{
    int32_t ret = 0;
    int32_t port = 0;
    int32_t *p_fd = NULL;

    if (!spi || !spi->priv)
        return -EINVAL;

    p_fd = (int32_t *)spi->priv;

    if (*p_fd < 0)
        return -EIO;

    if (*p_fd >= 0)
        close(*p_fd);
    else
        ret = -EALREADY;

    spi->priv = NULL;

    *p_fd = -1;
    free(p_fd);

    return ret;
}



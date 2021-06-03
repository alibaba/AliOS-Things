/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <aos/errno.h>
#include <vfsdev/i2c_dev.h>
#include "aos_hal_i2c.h"

int32_t aos_hal_i2c_init(i2c_dev_t *i2c)
{
    int32_t ret = 0;
    int32_t port = 0;
    int32_t *p_fd = NULL;
    char name[16] = {0};
    io_i2c_control_u c;

    if (!i2c || i2c->priv)
        return -EINVAL;

    port = i2c->port;
    p_fd = (int32_t *)malloc(sizeof(int32_t));
    if (!p_fd)
        return -ENOMEM;
    *p_fd = -1;

    snprintf(name, sizeof(name), "/dev/i2c%d", port);

    *p_fd = open(name, 0);

    if (*p_fd < 0) {
        printf ("open %s failed, fd:%d\r\n", name, *p_fd);
        ret = -EIO;
        goto out;
    }

    c.c.addr = i2c->config.dev_addr;            /* sensor's address */
    c.c.addr_width = i2c->config.address_width; /* 7-bit address */
    c.c.role = 1;                               /* master mode */
    ret = ioctl(*p_fd, IOC_I2C_SET_CONFIG, (unsigned long)&c);
    if (ret) {
        printf ("set i2c config on %s failed\r\n", name);
        goto out;
    }

    c.freq = i2c->config.freq;
    ret = ioctl(*p_fd, IOC_I2C_SET_FREQ, (unsigned long)&c);
    if (ret) {
        printf ("set i2c config on %s failed\r\n", name);
        goto out;
    }
out:
    if (!ret) {
        i2c->priv = p_fd;
    } else {
        if (*p_fd >= 0)
            close(*p_fd);
        free(p_fd);
        p_fd = NULL;
    }
    return ret;
}

int32_t aos_hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    int32_t port = 0;
    int32_t *p_fd = NULL;
    int32_t ret = -1;
    io_i2c_data_t d;

    if (!i2c)
        return -EINVAL;

    p_fd = (int32_t *)i2c->priv;

    if (!p_fd || *p_fd < 0)
        return -EIO;

    d.addr = dev_addr;
    d.data = (unsigned char *)data;
    d.length = size;
    d.maddr = 0;
    d.mlength = 0;
    d.timeout = timeout;

    ret = ioctl(*p_fd, IOC_I2C_MASTER_TX, (unsigned long)&d);

    return ret;
}

int32_t aos_hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    int32_t port = 0;
    int32_t *p_fd = NULL;
    int32_t ret = -1;
    io_i2c_data_t d;

    if (!i2c)
        return -EINVAL;

    p_fd = (int32_t *)i2c->priv;

    if (!p_fd || *p_fd < 0)
        return -EIO;

    d.addr = dev_addr;
    d.data = data;
    d.length = size;
    d.maddr = 0;
    d.mlength = 0;
    d.timeout = timeout;

    ret = ioctl(*p_fd, IOC_I2C_MASTER_RX, (unsigned long)&d);

    return 0;
}

int32_t aos_hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    // slave operation is not supported
    return -ENOTSUP;
}

int32_t aos_hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
    // slave operation is not supported
    return -ENOTSUP;
}

int32_t aos_hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    int32_t port = 0;
    int32_t *p_fd = NULL;
    int32_t ret = -1;
    io_i2c_data_t d;

    if (!i2c)
        return -EINVAL;

    p_fd = (int32_t *)i2c->priv;

    if (!p_fd || *p_fd < 0)
        return -EIO;

    d.addr = dev_addr;
    d.data = (unsigned char *)data;
    d.length = size;
    d.maddr = mem_addr;
    d.mlength = mem_addr_size;
    d.timeout = timeout;


    ret = ioctl(*p_fd, IOC_I2C_MEM_TX, (unsigned long)&d);
    return 0;
}

int32_t aos_hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    int32_t port = 0;
    int32_t *p_fd = NULL;
    int32_t ret = -1;
    io_i2c_data_t d;

    if (!i2c)
        return -EINVAL;

    p_fd = (int32_t *)i2c->priv;

    if (!p_fd || *p_fd < 0)
        return -EIO;

    d.addr = dev_addr;
    d.data = data;
    d.length = size;
    d.maddr = mem_addr;
    d.mlength = mem_addr_size;
    d.timeout = timeout;

    ret = ioctl(*p_fd, IOC_I2C_MEM_RX, (unsigned long)&d);
    return 0;
}

int32_t aos_hal_i2c_finalize(i2c_dev_t *i2c)
{
    int32_t ret = 0;
    int32_t port = 0;
    int32_t *p_fd = NULL;

    if (!i2c || !i2c->priv)
        return -EINVAL;

    p_fd = (int32_t *)i2c->priv;

    if (*p_fd < 0)
        return -EIO;

    if (*p_fd >= 0)
        close(*p_fd);
    else
        ret = -EALREADY;

    i2c->priv = NULL;

    *p_fd = -1;
    free(p_fd);

    return ret;
}

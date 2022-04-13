/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "sensor_drv_api.h"
#include <aos/errno.h>
#include <vfsdev/i2c_dev.h>

static int g_i2c_fd[4] = {-1, -1, -1, -1};

int32_t vfs_i2c_init(i2c_dev_t *i2c)
{
    int32_t ret = 0;
    int32_t port = 0;
    int32_t *p_fd = NULL;
    char name[16] = {0};
    io_i2c_control_u i2c_ctrl;

    if (!i2c) {
        printf ("i2c is null\r\n");
        return -EINVAL;
    }

    port = i2c->port;
    // p_fd = (int32_t *)malloc(sizeof(int32_t));
    // if (!p_fd)
    //     return -ENOMEM;
    // *p_fd = -1;

    p_fd = &g_i2c_fd[port];
    if (*p_fd >= 0) {
        printf ("i2c port[%d] isn't ready\r\n",port);
        return -EALREADY;
    }

    snprintf(name, sizeof(name), "/dev/i2c%d", port);

    *p_fd = open(name, 0);
    if (*p_fd < 0) {
        printf ("open %s failed, fd:%d\r\n", name, *p_fd);
        ret = -EIO;
        goto out;
    }
    // printf("p_fd is %d\r\n",*p_fd);
    // printf("g_i2c_fd[%d] is %d\r\n",port,g_i2c_fd[port]);

    i2c_ctrl.c.addr = i2c->config.dev_addr;            /* sensor's address */
    i2c_ctrl.c.addr_width = i2c->config.address_width; /* 7-bit address */
    i2c_ctrl.c.role = 1;                               /* master mode */
    ret = ioctl(*p_fd, IOC_I2C_SET_CONFIG, (unsigned long)&i2c_ctrl);
    if (ret) {
        printf ("set i2c config on %s failed\r\n", name);
        goto out;
    }

    i2c_ctrl.freq = i2c->config.freq;
    ret = ioctl(*p_fd, IOC_I2C_SET_FREQ, (unsigned long)&i2c_ctrl);
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
        // free(p_fd);
        // p_fd = NULL;
    }
    return ret;
}

static int32_t vfs_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    int32_t *p_fd = NULL;
    int32_t ret = -1;
    io_i2c_data_t i2c_data;

    if (!i2c) {
        return -EINVAL;
    }

    p_fd = &g_i2c_fd[i2c->port];
    if (!p_fd || (*p_fd < 0)) {
        printf ("i2c port[%d] has not been initialized\r\n",i2c->port);
        return -EINVAL;
    }

    i2c_data.addr = dev_addr;
    i2c_data.data = data;
    i2c_data.length = size;
    i2c_data.maddr = 0;
    i2c_data.mlength = 0;
    i2c_data.timeout = timeout;

    ret = ioctl(*p_fd, IOC_I2C_MASTER_TX, (unsigned long)&i2c_data);

    return ret;
}

static int32_t vfs_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    int32_t port = 0;
    int32_t *p_fd = NULL;
    int32_t ret = -1;
    io_i2c_data_t i2c_data;

    if (!i2c) {
        printf ("i2c is null\r\n");
        return -EINVAL;
    }

    p_fd = &g_i2c_fd[i2c->port];
    if (!p_fd || (*p_fd < 0)) {
        printf ("i2c port[%d] has not been initialized\r\n",i2c->port);
        return -EINVAL;
    }

    i2c_data.addr = dev_addr;
    i2c_data.data = data;
    i2c_data.length = size;
    i2c_data.maddr = 0;
    i2c_data.mlength = 0;
    i2c_data.timeout = timeout;

    ret = ioctl(*p_fd, IOC_I2C_MASTER_RX, (unsigned long)&i2c_data);

    return 0;
}

static int32_t vfs_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    int32_t port = 0;
    int32_t *p_fd = NULL;
    int32_t ret = -1;
    io_i2c_data_t i2c_data;

    if (!i2c)
        return -EINVAL;

    p_fd = &g_i2c_fd[i2c->port];
    if (!p_fd || (*p_fd < 0)) {
        printf ("i2c port[%d] has not been initialized\r\n",i2c->port);
        return -EINVAL;
    }

    i2c_data.addr = dev_addr;
    i2c_data.data = data;
    i2c_data.length = size;
    i2c_data.maddr = mem_addr;
    i2c_data.mlength = mem_addr_size;
    i2c_data.timeout = timeout;


    ret = ioctl(*p_fd, IOC_I2C_MEM_TX, (unsigned long)&i2c_data);
    return 0;
}

static int32_t vfs_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    int32_t port = 0;
    int32_t *p_fd = NULL;
    int32_t ret = -1;
    io_i2c_data_t i2c_data;

    if (!i2c)
        return -EINVAL;

    p_fd = &g_i2c_fd[i2c->port];
    if (!p_fd || (*p_fd < 0)) {
        printf ("i2c port[%d] has not been initialized\r\n",i2c->port);
        return -EINVAL;
    }

    i2c_data.addr = dev_addr;
    i2c_data.data = data;
    i2c_data.length = size;
    i2c_data.maddr = mem_addr;
    i2c_data.mlength = mem_addr_size;
    i2c_data.timeout = timeout;

    ret = ioctl(*p_fd, IOC_I2C_MEM_RX, (unsigned long)&i2c_data);
    return 0;
}

static int32_t vfs_i2c_finalize(i2c_dev_t *i2c)
{
    int32_t ret = 0;
    int32_t port = 0;
    int32_t *p_fd = NULL;

    if (!i2c || !i2c->priv)
        return -EINVAL;

    // p_fd = (int32_t *)i2c->priv;
    // if (*p_fd < 0)
    //     return -EIO;

    p_fd = &g_i2c_fd[i2c->port];
    if (!p_fd || (*p_fd < 0)) {
        printf ("i2c port[%d] has not been initialized\r\n",i2c->port);
        return -EINVAL;
    }

    if (*p_fd >= 0)
        close(*p_fd);
    else
        ret = -EALREADY;

    i2c->priv = NULL;

    *p_fd = -1;
    free(p_fd);

    return ret;
}


int32_t sensor_i2c_init(i2c_dev_t *i2c)
{
    int32_t ret = 0;
    if (i2c == NULL) {
        return -1;
    }

    ret = vfs_i2c_init(i2c);

    return ret;
}

int32_t sensor_i2c_read(i2c_dev_t *i2c, uint16_t reg, uint8_t *data,
                        uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;
    if (i2c == NULL) {
        return -1;
    }

#if SENSOR_CONFIG_I2C_ENABLE
    ret = hal_i2c_mem_read(i2c, i2c->config.dev_addr, reg, I2C_REG_LEN, data,
                           size, timeout);
#else
    ret = vfs_i2c_mem_read(i2c, i2c->config.dev_addr, reg, I2C_REG_LEN, data,
                           size, timeout);
#endif

    return ret;
}

int32_t sensor_i2c_write(i2c_dev_t *i2c, uint16_t reg, uint8_t *data,
                         uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;
    if (i2c == NULL) {
        return -1;
    }
#if SENSOR_CONFIG_I2C_ENABLE
    ret = hal_i2c_mem_write(i2c, i2c->config.dev_addr, reg, I2C_REG_LEN, data,
                            size, timeout);
#else
    ret = vfs_i2c_mem_write(i2c, i2c->config.dev_addr, reg, I2C_REG_LEN, data,
                            size, timeout);
#endif
    return ret;
}

int32_t sensor_i2c_master_send(i2c_dev_t *i2c, uint8_t *data,
                         uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;
    if (i2c == NULL) {
        return -1;
    }
#if SENSOR_CONFIG_I2C_ENABLE
    ret = hal_i2c_master_send(i2c, i2c->config.dev_addr, data,
                            size, timeout);
#else
    ret = vfs_i2c_master_send(i2c, i2c->config.dev_addr, data,
                            size, timeout);
#endif
    return ret;
}

int32_t sensor_i2c_master_recv(i2c_dev_t *i2c, uint8_t *data,
                         uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;
    if (i2c == NULL) {
        return -1;
    }
#if SENSOR_CONFIG_I2C_ENABLE
    ret = hal_i2c_master_recv(i2c, i2c->config.dev_addr, data,
                            size, timeout);
#else
    ret = vfs_i2c_master_recv(i2c, i2c->config.dev_addr, data,
                            size, timeout);
#endif
    return ret;
}

int32_t sensor_spi_read(spi_dev_t *spi, uint8_t *tx, uint8_t *rx, uint16_t size,
                        uint32_t timeout)
{
    int32_t ret = 0;
    if (spi == NULL) {
        return -1;
    }
#if SENSOR_CONFIG_SPI_ENABLE
    ret = hal_spi_send_recv(spi, tx, rx, size, timeout);
#endif
    return ret;
}
int32_t sensor_spi_write(spi_dev_t *spi, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    int32_t ret = 0;
    if (spi == NULL) {
        return -1;
    }
#if SENSOR_CONFIG_SPI_ENABLE
    ret = hal_spi_send(spi, data, size, timeout);
#endif
    return ret;
}

int32_t sensor_io_read(sensor_io_dev_t* dev, uint8_t* reg, uint8_t* data, uint16_t size,uint32_t timeout)
{
    int ret = -1;
    if(NULL == dev){
        return -1;
    }

    if(NULL == reg){
        return -1;
    }

    if(NULL == data){
        return -1;
    }
    switch(dev->io_port){
        case    I2C_PORT:{
            uint16_t   addr = *((uint16_t*)reg);
            ret = sensor_i2c_read(&(dev->dev_i2c), addr, data, size, timeout);
            break;
        }

        case    SPI_PORT:{
            ret = sensor_spi_read(&(dev->dev_spi), reg, data, size, timeout);
            break;
        }

        default:
            break;
    }

    return ret;
}

int32_t sensor_io_write(sensor_io_dev_t* dev, uint8_t* reg, uint8_t *data, uint16_t size, uint32_t timeout)
{
    int ret = -1;
    if(NULL == dev){
        return -1;
    }
    if(NULL == data){
        return -1;
    }

    switch(dev->io_port){
        case    I2C_PORT:{
            if(NULL == reg){
                return -1;
            }
            uint16_t   addr = *((uint16_t*)reg);

            ret = sensor_i2c_write(&(dev->dev_i2c), addr, data, size, timeout);
            break;
        }

        case    SPI_PORT:{
            ret = sensor_spi_write(&(dev->dev_spi), data, size, timeout);
            break;
        }

        default:
            break;
    }

    return ret;
}

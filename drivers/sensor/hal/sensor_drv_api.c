/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "sensor_drv_api.h"

int32_t sensor_i2c_read(i2c_dev_t *i2c, uint16_t reg, uint8_t *data,
                        uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;
    if (i2c == NULL) {
        return -1;
    }
#ifdef SENSOR_I2C_ENABLE
    ret = hal_i2c_mem_read(i2c, i2c->config.dev_addr, reg, I2C_REG_LEN, data,
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
#ifdef SENSOR_I2C_ENABLE
    ret = hal_i2c_mem_write(i2c, i2c->config.dev_addr, reg, I2C_REG_LEN, data,
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
#ifdef SENSOR_SPI_ENABLE
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
#ifdef SENSOR_SPI_ENABLE
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


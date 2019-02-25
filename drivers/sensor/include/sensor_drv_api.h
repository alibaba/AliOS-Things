/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef SENSOR_DRV_API_H
#define SENSOR_DRV_API_H


#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "aos/hal/i2c.h"
#include "sensor/sensor.h"
#include "aos/hal/spi.h"

typedef struct  _sensor_io_dev_t{
    dev_io_port_e           io_port;
    union{
        i2c_dev_t           dev_i2c;
        spi_dev_t           dev_spi;
    };
}sensor_io_dev_t;

typedef struct _sensor_io_func_st
{
    int (*read)(uint16_t reg, uint8_t *data, uint16_t size);
    int (*write)(uint16_t reg, uint8_t *data, uint16_t size);
} sensor_io_func_st;

extern int32_t sensor_i2c_read(i2c_dev_t *i2c, uint16_t reg, uint8_t *data,
                               uint16_t size, uint32_t timeout);
extern int32_t sensor_i2c_write(i2c_dev_t *i2c, uint16_t reg, uint8_t *data,
                                uint16_t size, uint32_t timeout);
extern int32_t sensor_spi_read(spi_dev_t *spi, uint8_t *tx, uint8_t *rx,
                               uint16_t size, uint32_t timeout);
extern int32_t sensor_spi_write(spi_dev_t *spi, uint8_t *data, uint16_t size,
                                uint32_t timeout);

extern int32_t sensor_io_read(sensor_io_dev_t* dev, uint8_t* reg, uint8_t* data, uint16_t size,uint32_t timeout);
extern int32_t sensor_io_write(sensor_io_dev_t* dev, uint8_t* reg, uint8_t *data, uint16_t size, uint32_t timeout);
#endif /* SENSOR_DRV_API_H */

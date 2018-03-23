/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "hal/soc/soc.h"
#include "hal/soc/i2c.h"
#include "hal/sensor.h"

int32_t sensor_i2c_read(i2c_dev_t *i2c, uint16_t reg, uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;
    if(i2c == NULL){
        return -1;
    }
    ret = hal_i2c_mem_read(i2c, i2c->config.dev_addr, reg, I2C_REG_LEN, data, size, AOS_WAIT_FOREVER);
    return ret;
}    

int32_t sensor_i2c_write(i2c_dev_t *i2c, uint16_t reg, uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;
    if(i2c == NULL){
        return -1;
    }
    ret = hal_i2c_mem_write(i2c, i2c->config.dev_addr, reg, I2C_REG_LEN, data, size, AOS_WAIT_FOREVER);
    return ret;
}    



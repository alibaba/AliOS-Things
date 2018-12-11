/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/hal/i2c.h"

#ifdef HAL_I2C_MODULE_ENABLED

int32_t hal_i2c_init(i2c_dev_t *i2c)
{

}

int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{

}

int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{

}

int32_t hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
{

}

int32_t hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{

}

int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{

}

int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{

}

int32_t hal_i2c_finalize(i2c_dev_t *i2c)
{

}

#endif /* HAL_I2C_MODULE_ENABLED */


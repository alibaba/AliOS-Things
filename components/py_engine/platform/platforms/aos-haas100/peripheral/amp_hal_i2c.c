/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "amp_hal_i2c.h"

int32_t amp_hal_i2c_init(i2c_dev_t *i2c)
{
    return hal_i2c_init(i2c);
}

int32_t amp_hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    return hal_i2c_master_send(i2c, dev_addr, data, size, timeout);
}

int32_t amp_hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    return hal_i2c_master_recv(i2c, dev_addr, data, size, timeout);
}

int32_t amp_hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    return hal_i2c_slave_send(i2c, data, size, timeout);
}

int32_t amp_hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
    return hal_i2c_slave_recv(i2c, data, size, timeout);
}

int32_t amp_hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    return hal_i2c_mem_write(i2c, dev_addr, mem_addr, mem_addr_size, data, size, timeout);
}

int32_t amp_hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    return hal_i2c_mem_read(i2c, dev_addr, mem_addr, mem_addr_size, data, size, timeout);
}

int32_t amp_hal_i2c_finalize(i2c_dev_t *i2c)
{
    return hal_i2c_finalize(i2c);
}

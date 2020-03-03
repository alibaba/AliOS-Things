/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <k_api.h>
#include <stdint.h>
#include <stdbool.h>
#include "board.h"
#include "aos/hal/i2c.h"
#include "i2c_api.h"
#include "pinmap.h"


/*I2C pin location:
* I2C0:
*   - S0:  PA_18(SCL)/PA_23(SDA).
*   - S1:  PA_22(SCL)/PA_19(SDA).
*/

i2c_t   i2c_pa = {0};
i2c_t   i2c_pb = {0};

typedef struct i2c_resource {
    i2c_t * dev;
    PinName scl_io;
    PinName sda_io;
} i2c_resource_t;

static i2c_resource_t g_dev[PORT_I2C_SIZE] = {
    {&i2c_pa,PA_18,PA_23},
    {&i2c_pb,PA_22,PA_19}
};

int32_t hal_i2c_init(i2c_dev_t *i2c)
{
    int32_t ret = 0;
    if(NULL == i2c || (PORT_I2C_1 != i2c->port && PORT_I2C_2 != i2c->port)) {
        return (-1);
    }

    i2c_resource_t * resource = &g_dev[i2c->port];

#ifdef I2C_MASTER_DEVICE
    i2c_init(resource->dev, resource->sda_io ,resource->scl_io);
    i2c_frequency(resource->dev,i2c->config.freq);

#else   //I2C_SLAVE_DEVICE
    i2c_init(resource->dev, resource->sda_io ,resource->scl_io);
    i2c_frequency(resource->dev,i2c->config.freq);
    i2c_slave_address(resource->dev, 0, i2c->config.dev_addr, 0xFF);
    i2c_slave_mode(resource->dev, 1);

#endif

    return ret;
}

int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;
    uint16_t len;
    (void)timeout;

    if(NULL == i2c || (NULL == data) ||(PORT_I2C_1 != i2c->port && PORT_I2C_2 != i2c->port)) {
        return (-1);
    }
    i2c_resource_t * resource = &g_dev[i2c->port];
    len = (uint16_t)i2c_write(resource->dev, dev_addr, data, size , 1);
    if(len != size){
        return -1;
    }

    return ret;
}

int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;
    uint16_t len;
    (void)timeout;

    if(NULL == i2c || (NULL == data) || (PORT_I2C_1 != i2c->port && PORT_I2C_2 != i2c->port)) {
        return (-1);
    }
    i2c_resource_t * resource = &g_dev[i2c->port];
    len = i2c_read(resource->dev, dev_addr, data, size, 1);
    if(len != size){
        return -1;
    }

    return ret;
}

int32_t hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;
    (void)timeout;

    if(NULL == i2c || (NULL == data) || (PORT_I2C_1 != i2c->port && PORT_I2C_2 != i2c->port)) {
        return (-1);
    }
    i2c_resource_t * resource = &g_dev[i2c->port];

    i2c_slave_write(resource->dev,data,size);

    return ret;
}

int32_t hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;
    (void)timeout;

    if(NULL == i2c || (NULL == data) || (PORT_I2C_1 != i2c->port && PORT_I2C_2 != i2c->port)) {
        return (-1);
    }
    i2c_resource_t * resource = &g_dev[i2c->port];

    i2c_slave_read(resource->dev, data, size) ;

    return ret;
}

int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    int32_t ret = 0;

    return ret;
}

int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    int32_t ret = 0;

    return ret;
}

int32_t hal_i2c_finalize(i2c_dev_t *i2c)
{
    int32_t ret = 0;

    return ret;
}



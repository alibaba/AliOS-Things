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
*   - S0:  PA_25(SCL)/PA_26(SDA).
*   - S1:  PB_5(SCL)/PB_6(SDA).
*/

typedef enum{
    I2C_NUM_0 = 0,  /*!< I2C port 0 */
    I2C_NUM_1 ,     /*!< I2C port 1 */
    I2C_NUM_MAX
} i2c_port_t;

i2c_t   i2c_pa = {0};
i2c_t   i2c_pb = {0};

typedef struct i2c_resource {
    i2c_t * dev;
    PinName scl_io;
    PinName sda_io;
} i2c_resource_t;

static i2c_resource_t g_dev[I2C_NUM_MAX] = {
    {&i2c_pa,PA_25,PA_26},
    {&i2c_pb,PB_5,PB_6}
};

int32_t hal_i2c_init(i2c_dev_t *i2c)
{
    int32_t ret = 0;
    if(NULL == i2c || (MICO_I2C_1 != i2c->port && MICO_I2C_2 != i2c->port)) {
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

    if(NULL == i2c || (NULL == data) ||(MICO_I2C_1 != i2c->port && MICO_I2C_2 != i2c->port)) {
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

    if(NULL == i2c || (NULL == data) || (MICO_I2C_1 != i2c->port && MICO_I2C_2 != i2c->port)) {
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

    if(NULL == i2c || (NULL == data) || (MICO_I2C_1 != i2c->port && MICO_I2C_2 != i2c->port)) {
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

    if(NULL == i2c || (NULL == data) || (MICO_I2C_1 != i2c->port && MICO_I2C_2 != i2c->port)) {
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



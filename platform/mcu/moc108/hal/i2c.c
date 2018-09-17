/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "hal_peripherals.h"
#include "hal/soc/soc.h"

#define I2C0_SELF_SLAVE_ADDR    0x72
#define PRI_IRQ_I2C2            (21)
#define IRQ_I2C2                (5)
#define PRI_IRQ_I2C1            (24)
#define IRQ_I2C1                (2)
#define GPIO_I2C_FM_SCL_PIN     20
#define GPIO_2F_I2C2_SCL_PIN    0

extern void I2C0_InterruptHandler(void);
extern void I2C_FM_InterruptHandler(void);

int32_t hal_i2c_init(i2c_dev_t *i2c)
{
    int hz = 100000;
    intc_service_register(IRQ_I2C1, PRI_IRQ_I2C1, I2C_FM_InterruptHandler);
    if(i2c->config.address_width == I2C_ADDRESS_WIDTH_10BIT || i2c->config.address_width == I2C_ADDRESS_WIDTH_16BIT)
    {
        return -1;
    }
    switch(i2c->config.freq)
    {
        case I2C_LOW_SPEED_MODE:
            return -1;
        case I2C_STANDARD_SPEED_MODE:
            hz = 100000;
            break;
        case I2C_HIGH_SPEED_MODE:
            hz = 500000;
            break;
        default:
            return -1;
    }
    if(i2c->port == I2C_2)
    {
        I2C0_Init(I2C0_SELF_SLAVE_ADDR, hz);//self slave addr
        intc_service_register(IRQ_I2C2, PRI_IRQ_I2C2, I2C0_InterruptHandler);
    }
    else if(i2c->port == I2C_1)
    {
        i2c_fm_init(hz);
        intc_service_register(IRQ_I2C1, PRI_IRQ_I2C1, I2C_FM_InterruptHandler);
    }
    else
        return -1;
    return 0;
}

int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    unsigned int ret = 0;
    if(data == NULL)
    {
        return -1;
    }

    if(i2c->port == I2C_2)
    {
        /* Send data */
        ret = I2C0_Master_Tx(dev_addr, data, size);
    }
    else if(i2c->port == I2C_1)
    {
        /* Send data */
        ret = I2C_FM_Master_Tx(dev_addr, data, size);
    }
    else
        return -1;
    return ret;
}

int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    unsigned int ret = 0;
    if ( data == NULL )
    {
        return -1;
    }

    if ( i2c->port == I2C_2 )
    {
        /* Send data */
        ret = I2C0_Master_Rx( dev_addr, data, size );
    }
    else if ( i2c->port == I2C_1 )
    {
        /* Send data */
        ret = I2C_FM_Master_Rx( dev_addr, data, size );
    }
    else
        return -1;
    return ret;
}

int32_t hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    unsigned int ret = 0;

    return ret;
}

int32_t hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
    unsigned int ret = 0;

    return ret;
}

int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    unsigned int ret = 0;

    return ret;
};

int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    unsigned int ret = 0;

    return ret;
};

int32_t hal_i2c_finalize(i2c_dev_t *i2c)
{
    unsigned int ret = 0;

    return ret;
}
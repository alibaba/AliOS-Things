/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include "stm32f1xx_hal.h"
#ifdef HAL_I2C_MODULE_ENABLED
#include "hal_i2c_stm32f1.h"
#include "k_api.h"
#include "errno.h"
#include "aos/hal/i2c.h"
#include "board.h"

typedef struct {
    uint8_t   inited;
    I2C_HandleTypeDef hal_i2c_handle;
}stm32_i2c_t;

static stm32_i2c_t stm32_i2c[PORT_I2C_SIZE];

static i2c_mapping_t* get_i2c_mapping(const PORT_I2C_TYPE port)
{
    int8_t i = 0;
    i2c_mapping_t* rc = NULL;
    for (i = 0; i < PORT_I2C_SIZE; i++)
    {
        if (i2c_mapping[i].i2c_func_p == port) {
            rc = &i2c_mapping[i];
            break;
        }
    }
    return rc;
}

I2C_HandleTypeDef * i2c_get_handle(const uint8_t port)
{
    I2C_HandleTypeDef *handle = NULL;
    if (port < PORT_I2C_SIZE) {
        handle = &stm32_i2c[port].hal_i2c_handle;
    } else
    {
        handle = NULL;
    }
    return handle;
}

int32_t hal_i2c_init(i2c_dev_t *i2c)
{
    int32_t rc = -1;
    if (NULL != i2c) {
        i2c_mapping_t* mapping = get_i2c_mapping(i2c->port);

        if (mapping != NULL) {
            memset(&stm32_i2c[i2c->port], 0, sizeof(stm32_i2c_t));
            I2C_HandleTypeDef *psti2chandle = &stm32_i2c[i2c->port].hal_i2c_handle;

            psti2chandle->Instance = mapping->i2c_physic_p;
            psti2chandle->Init.ClockSpeed = i2c->config.freq;
            psti2chandle->Init.DutyCycle = I2C_DUTYCYCLE_2;
            psti2chandle->Init.OwnAddress1 = 0;
            psti2chandle->Init.AddressingMode =
                (i2c->config.address_width == I2C_HAL_ADDRESS_WIDTH_7BIT) ?
                I2C_ADDRESSINGMODE_7BIT : I2C_ADDRESSINGMODE_10BIT;
            psti2chandle->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
            psti2chandle->Init.OwnAddress2 = 0;
            psti2chandle->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
            psti2chandle->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
            if (HAL_I2C_Init(psti2chandle) == HAL_OK)
            {
                stm32_i2c[i2c->port].inited = 1;
                rc = 0;
            }
        }

    }
    return rc;
}

int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
    uint16_t size, uint32_t timeout)
{
    int ret = -1;
    I2C_HandleTypeDef *psti2c = NULL;

    if (NULL == i2c || NULL == data) {
        return ret;
    }

    psti2c = i2c_get_handle(i2c->port);
    if (NULL != psti2c) {
        ret = HAL_I2C_Master_Transmit(psti2c, dev_addr, (uint8_t *)data, size, timeout);
    }

    return ret;
}

int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
    uint16_t size, uint32_t timeout)
{
    int ret = -1;
    I2C_HandleTypeDef *psti2c = NULL;

    if (NULL == i2c || NULL == data) {
        return ret;
    }

    psti2c = i2c_get_handle(i2c->port);

    if (NULL != psti2c) {
        ret = HAL_I2C_Master_Receive(psti2c, dev_addr, data, size, timeout);
    }

    return ret;
}

int32_t hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    int ret = -1;
    I2C_HandleTypeDef *psti2c = NULL;

    if (NULL == i2c || NULL == data) {
        return ret;
    }

    psti2c = i2c_get_handle(i2c->port);
    if (NULL != psti2c) {
        ret = HAL_I2C_Slave_Transmit(psti2c, (uint8_t *)data, size, timeout);
    }

    return ret;
}

int32_t hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
    int ret = -1;
    I2C_HandleTypeDef *psti2c = NULL;

    if (NULL == i2c || NULL == data) {
        return ret;
    }

    psti2c = i2c_get_handle(i2c->port);
    if (NULL != psti2c) {
        ret = HAL_I2C_Slave_Receive(psti2c, data, size, timeout);
    }

    return ret;
}

int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
    uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
    uint32_t timeout)
{
    int ret = -1;
    I2C_HandleTypeDef *psti2c = NULL;

    if (NULL == i2c || NULL == data) {
        return ret;
    }

    psti2c = i2c_get_handle(i2c->port);
    if (NULL != psti2c) {
        ret = HAL_I2C_Mem_Write(psti2c, dev_addr, mem_addr,
            (uint16_t)mem_addr_size, (uint8_t *)data, size, timeout);
    }

    return ret;
};

int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
    uint16_t mem_addr_size, uint8_t *data, uint16_t size,
    uint32_t timeout)
{
    int ret = -EINVAL;
    I2C_HandleTypeDef *psti2c = NULL;

    if (NULL == i2c || NULL == data) {
        return ret;
    }

    psti2c = i2c_get_handle(i2c->port);
    if (NULL != psti2c) {
        if (HAL_OK == HAL_I2C_Mem_Read(psti2c, dev_addr, mem_addr,
            (uint16_t)mem_addr_size, data, size, timeout)) {
            ret = 0;
        } else {
            ret = -1;
        }
    }
    return ret;
};

int32_t hal_i2c_finalize(i2c_dev_t *i2c)
{
    int32_t rc = -1;
    if (NULL != i2c) {
        i2c_mapping_t* mapping = get_i2c_mapping(i2c->port);

        if (mapping != NULL) {
            I2C_HandleTypeDef *psti2chandle = &stm32_i2c[i2c->port].hal_i2c_handle;
            if (HAL_I2C_DeInit(psti2chandle) == HAL_OK) {
                stm32_i2c[i2c->port].inited = 0;
                rc = 0;
            }
        }
    }
    return rc;
}

#endif


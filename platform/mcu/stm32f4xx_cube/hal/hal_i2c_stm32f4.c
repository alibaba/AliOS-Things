/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hal/hal.h"
#include "k_types.h"
#include "errno.h"
#include "hal_i2c_stm32f4.h"
#ifdef HAL_I2C_MODULE_ENABLED

#ifndef AOS_PORT_I2C1
#define AOS_PORT_I2C1 1
#endif

#ifndef AOS_PORT_I2C2
#define AOS_PORT_I2C2 2
#endif

#ifndef AOS_PORT_I2C3
#define AOS_PORT_I2C3 3
#endif

#ifndef AOS_PORT_I2C4
#define AOS_PORT_I2C4 4
#endif

/* Init and deInit function for i2c1 */
static void I2C1_Init(void);
static void I2C1_DeInit(void);

/* Init and deInit function for i2c2 */
static void I2C2_Init(void);
static void I2C2_DeInit(void);

/* handle for i2c */
static I2C_HandleTypeDef I2c1Handle = {0};
static I2C_HandleTypeDef I2c2Handle = {0};
static I2C_HandleTypeDef I2c3Handle = {0};
static I2C_HandleTypeDef I2c4Handle = {0};


int32_t hal_i2c_init(i2c_dev_t *i2c)
{
    int32_t ret = -1;
	
    if (i2c == NULL) {
       return -1;
    }

    switch (i2c->port) {
        case AOS_PORT_I2C1:
            I2C1_Init();
            i2c->priv = &I2c1Handle;
            ret = 0;
            break;
        case AOS_PORT_I2C2:
            I2C2_Init();
            i2c->priv = &I2c2Handle;
            ret = 0;
            break;
        default:
            break;
    }

    return ret;
}

static inline I2C_HandleTypeDef *get_priv(uint8_t port)
{
    I2C_HandleTypeDef *hi2c = NULL;

    switch (port) {
    case AOS_PORT_I2C1:
        hi2c = &I2c1Handle;
        break;
    case AOS_PORT_I2C2:
        hi2c = &I2c2Handle;
        break;
    case AOS_PORT_I2C3:
        hi2c = &I2c3Handle;
        break;
    case AOS_PORT_I2C4:
        hi2c = &I2c4Handle;
        break;
    default:
        return NULL;
    }

    return hi2c;
}

int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    int ret = -1;
    I2C_HandleTypeDef *psti2c = NULL;

    if (NULL == i2c || NULL == data){
        return ret;
    }

    psti2c = get_priv(i2c->port);

    ret = HAL_I2C_Master_Transmit(psti2c, dev_addr, (uint8_t *)data, size, timeout);

    return ret;
}

int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    int ret = -1;
    I2C_HandleTypeDef *psti2c = NULL;

    if (NULL == i2c || NULL == data){
        return ret;
    }

    psti2c = get_priv(i2c->port);

    ret = HAL_I2C_Master_Receive(psti2c, dev_addr, data, size, timeout);

    return ret;
}

int32_t hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    int ret = -1;
    I2C_HandleTypeDef *psti2c = NULL;

    if (NULL == i2c || NULL == data){
        return ret;
    }

    psti2c = get_priv(i2c->port);

    ret = HAL_I2C_Slave_Transmit(psti2c, (uint8_t *)data, size, timeout);

    return ret;
}

int32_t hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
    int ret = -1;
    I2C_HandleTypeDef *psti2c = NULL;

    if (NULL == i2c || NULL == data){
        return ret;
    }

    psti2c = get_priv(i2c->port);

    ret = HAL_I2C_Slave_Receive(psti2c, data, size, timeout);

    return ret;
}

int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    int ret = -1;
    I2C_HandleTypeDef *psti2c = NULL;

    if (NULL == i2c || NULL == data){
        return ret;
    }

    psti2c = get_priv(i2c->port);

    ret = HAL_I2C_Mem_Write(psti2c, dev_addr, mem_addr,
          (uint16_t)mem_addr_size, (uint8_t *)data, size, timeout);

    return ret;
};

int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    int ret = -1;
    I2C_HandleTypeDef *psti2c = NULL;

    if (NULL == i2c || NULL == data){
        return ret;
    }

    psti2c = get_priv(i2c->port);

    ret = HAL_I2C_Mem_Read(psti2c, dev_addr, mem_addr,
          (uint16_t)mem_addr_size, data, size, timeout);

    return ret;
};

int32_t hal_i2c_finalize(i2c_dev_t *i2c)
{
    int32_t ret = -1;

    if (i2c == NULL) {
        return -1;
    }

    switch (i2c->port) {
        case AOS_PORT_I2C1:
            I2C1_DeInit();
            ret = 0;
            break;				
        case AOS_PORT_I2C2:
            I2C2_DeInit();
            ret = 0;
            break;				
        default:
            break;
    }

    return ret;
}

void I2C1_Init(void)
{
    if (HAL_I2C_GetState(&I2c1Handle) == HAL_I2C_STATE_RESET) {
        I2c1Handle.Instance              = I2C1_INSTANCE;
        I2c1Handle.Init.ClockSpeed       = 400000;
        I2c1Handle.Init.DutyCycle        = I2C_DUTYCYCLE_2;
        I2c1Handle.Init.OwnAddress1      = I2C1_OWN_ADDRESS1;
        I2c1Handle.Init.AddressingMode   = I2C1_ADDRESSING_MODE;
        I2c1Handle.Init.DualAddressMode  = I2C1_DUAL_ADDRESS_MODE;
        I2c1Handle.Init.OwnAddress2      = I2C1_OWNADDRESS2;
        I2c1Handle.Init.GeneralCallMode  = I2C1_GENERAL_CALL_MODE;
        I2c1Handle.Init.NoStretchMode    = I2C1_NO_STRETCH_MODE;
        
        /* Init the I2C */
        HAL_I2C_Init(&I2c1Handle);
    }
}

void I2C1_DeInit(void)
{
    if (HAL_I2C_GetState(&I2c1Handle) != HAL_I2C_STATE_RESET) {
        /* DeInit the I2C */
        HAL_I2C_DeInit(&I2c1Handle);
    }
}

void I2C2_Init(void)
{
    if (HAL_I2C_GetState(&I2c2Handle) == HAL_I2C_STATE_RESET) {
        I2c2Handle.Instance              = I2C2_INSTANCE;
        I2c2Handle.Init.ClockSpeed       = 100000;
        I2c2Handle.Init.DutyCycle        = I2C_DUTYCYCLE_16_9;
        I2c2Handle.Init.OwnAddress1      = I2C2_OWN_ADDRESS1;
        I2c2Handle.Init.AddressingMode   = I2C2_ADDRESSING_MODE;
        I2c2Handle.Init.DualAddressMode  = I2C2_DUAL_ADDRESS_MODE;
        I2c2Handle.Init.OwnAddress2      = I2C2_OWNADDRESS2;
        I2c2Handle.Init.GeneralCallMode  = I2C2_GENERAL_CALL_MODE;
        I2c2Handle.Init.NoStretchMode    = I2C2_NO_STRETCH_MODE;

        /* Init the I2C */
        HAL_I2C_Init(&I2c2Handle);
    }
}

void I2C2_DeInit(void)
{
    if (HAL_I2C_GetState(&I2c2Handle) != HAL_I2C_STATE_RESET) {
        /* DeInit the I2C */
        HAL_I2C_DeInit(&I2c2Handle);
    }
}
#endif

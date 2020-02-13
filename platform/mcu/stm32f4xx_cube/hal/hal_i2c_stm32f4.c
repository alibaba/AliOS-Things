/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "stm32f4xx_hal.h"
#ifdef HAL_I2C_MODULE_ENABLED
#include "hal_i2c_stm32f4.h"

#include "aos/hal/i2c.h"
#include "k_api.h"
#include "errno.h"

typedef struct {
    uint8_t   inited;
    I2C_HandleTypeDef hal_i2c_handle;
}stm32_i2c_t;

static stm32_i2c_t stm32_i2c[MAX_I2C_BUS_NUM] = {0};

static int32_t stm32_i2c_param_transform(i2c_dev_t *i2c)
{
    switch (i2c->config.address_width) {
        case I2C_HAL_ADDRESS_WIDTH_7BIT:
            stm32_i2c[i2c->port - 1].hal_i2c_handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
            break;
        case I2C_HAL_ADDRESS_WIDTH_10BIT:
            stm32_i2c[i2c->port - 1].hal_i2c_handle.Init.AddressingMode = I2C_ADDRESSINGMODE_10BIT;
            break;
        default:
            stm32_i2c[i2c->port - 1].hal_i2c_handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
            printf("invalid i2c addr width %d , use default 7 bit \r\n", i2c->config.address_width);
            break;
    }

    switch (i2c->config.freq) {
        case I2C_BUS_BIT_RATES_100K:
            stm32_i2c[i2c->port - 1].hal_i2c_handle.Init.ClockSpeed = 100000;
            break;
        case I2C_BUS_BIT_RATES_400K:
            stm32_i2c[i2c->port - 1].hal_i2c_handle.Init.ClockSpeed = 400000;
            stm32_i2c[i2c->port - 1].hal_i2c_handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
            break;
        case I2C_BUS_BIT_RATES_3400K:
            printf("i2c freq %d is unsupported in stm32f4\r\n", i2c->config.freq);
            return -1;
        default:
            printf("invalid i2c freq %d  \r\n", i2c->config.freq);
            return -1;
    }

    if (i2c->config.mode == I2C_MODE_MASTER) {
        stm32_i2c[i2c->port - 1].hal_i2c_handle.Mode = HAL_I2C_MODE_MASTER;
    } else if (i2c->config.mode == I2C_MODE_SLAVE) {
        stm32_i2c[i2c->port - 1].hal_i2c_handle.Mode = HAL_I2C_MODE_SLAVE;
        stm32_i2c[i2c->port - 1].hal_i2c_handle.Init.OwnAddress1 = i2c->config.dev_addr;
    } else {
        stm32_i2c[i2c->port - 1].hal_i2c_handle.Mode = HAL_I2C_MODE_MASTER;
        printf("invalid i2c work mode %d use I2C_MODE_MASTER by default \r\n", i2c->config.mode);
    }

    return 0;
}

static void * get_i2c_instance_by_port(int32_t port)
{
    if (port >= MAX_I2C_BUS_NUM || port <= 0 )
        return NULL;

    return i2c_mapping_table[port - 1];
}
/*
void hal_i2c_pre_init()
{
    int32_t i = 0;
    void *  I2cPhyPort = NULL;

    for (i = 1; i <= MAX_I2C_BUS_NUM; i++) {
        I2cPhyPort = get_i2c_instance_by_port(i);
        if (NULL == I2cPhyPort) {
            continue;
        }
        stm32_i2c[i - 1].hal_i2c_handle.Instance = I2cPhyPort;
        stm32_i2c[i - 1].hal_i2c_handle.Init.ClockSpeed       = 400000;
        stm32_i2c[i - 1].hal_i2c_handle.Init.DutyCycle        = I2C_DUTYCYCLE_2;
        stm32_i2c[i - 1].hal_i2c_handle.Init.OwnAddress1      = 0;
        stm32_i2c[i - 1].hal_i2c_handle.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
        stm32_i2c[i - 1].hal_i2c_handle.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
        stm32_i2c[i - 1].hal_i2c_handle.Init.OwnAddress2      = 0;
        stm32_i2c[i - 1].hal_i2c_handle.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
        stm32_i2c[i - 1].hal_i2c_handle.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
    }
}*/

void hal_i2c_pre_init(int port)
{
    int32_t i = port;
    void *  I2cPhyPort = NULL;

    if(i >= 1 && i <= MAX_I2C_BUS_NUM){
        I2cPhyPort = get_i2c_instance_by_port(i);
        if (NULL == I2cPhyPort) {
            return;
        }
        stm32_i2c[i - 1].hal_i2c_handle.Instance = I2cPhyPort;
        stm32_i2c[i - 1].hal_i2c_handle.Init.ClockSpeed       = 400000;
        stm32_i2c[i - 1].hal_i2c_handle.Init.DutyCycle        = I2C_DUTYCYCLE_2;
        stm32_i2c[i - 1].hal_i2c_handle.Init.OwnAddress1      = 0;
        stm32_i2c[i - 1].hal_i2c_handle.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
        stm32_i2c[i - 1].hal_i2c_handle.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
        stm32_i2c[i - 1].hal_i2c_handle.Init.OwnAddress2      = 0;
        stm32_i2c[i - 1].hal_i2c_handle.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
        stm32_i2c[i - 1].hal_i2c_handle.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
    }

}


int32_t hal_i2c_init(i2c_dev_t *i2c)
{
    int32_t ret = -1;

    if (i2c == NULL) {
       return ret;
    }

    if (NULL == get_i2c_instance_by_port(i2c->port)) {
        return ret;
    }

    if (stm32_i2c[i2c->port - 1].inited) {
        return 0;
    }

    hal_i2c_pre_init(i2c->port);

    ret = stm32_i2c_param_transform(i2c);
    if (ret) {
        return -1;
    }

    if (HAL_I2C_GetState(&stm32_i2c[i2c->port - 1].hal_i2c_handle) != HAL_I2C_STATE_RESET) {
        printf("i2c port %d is not in reset status ,init fail\r\n", i2c->port);
        return -1;
    }

    /* Init the I2C */
    ret = HAL_I2C_Init(&stm32_i2c[i2c->port - 1].hal_i2c_handle);
    if (ret) {
        printf("i2c port %d init fail ret is %d \r\n", i2c->port, ret);
        return -1;
    }

    stm32_i2c[i2c->port - 1].inited = 1;

    return 0;
}

int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    int ret = -1;
    I2C_HandleTypeDef *psti2c = NULL;

    if (NULL == i2c || NULL == data){
        return ret;
    }

    if (NULL == get_i2c_instance_by_port(i2c->port)) {
        return ret;
    }

    if (!stm32_i2c[i2c->port - 1].inited) {
        return ret;
    }

    psti2c = &stm32_i2c[i2c->port - 1].hal_i2c_handle;

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

    if (NULL == get_i2c_instance_by_port(i2c->port)) {
        return ret;
    }

    if (!stm32_i2c[i2c->port - 1].inited) {
        return ret;
    }

    psti2c = &stm32_i2c[i2c->port - 1].hal_i2c_handle;

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

    if (NULL == get_i2c_instance_by_port(i2c->port)) {
        return ret;
    }

    if (!stm32_i2c[i2c->port - 1].inited) {
        return ret;
    }

    psti2c = &stm32_i2c[i2c->port].hal_i2c_handle;

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

    if (NULL == get_i2c_instance_by_port(i2c->port)) {
        return ret;
    }

    if (!stm32_i2c[i2c->port - 1].inited) {
        return ret;
    }

    psti2c = &stm32_i2c[i2c->port - 1].hal_i2c_handle;

    ret = HAL_I2C_Slave_Receive(psti2c, data, size, timeout);

    return ret;
}

int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    int ret = -1;
    uint64_t time_tick = 0;
    uint32_t real_timeout = 0;
    I2C_HandleTypeDef *psti2c = NULL;

    if (NULL == i2c || NULL == data){
        return ret;
    }

    if (NULL == get_i2c_instance_by_port(i2c->port)) {
        return ret;
    }

    if (!stm32_i2c[i2c->port - 1].inited) {
        return ret;
    }

    if (timeout == AOS_WAIT_FOREVER) {
        real_timeout = timeout;
    } else {
        time_tick = krhino_ms_to_ticks(timeout);
        real_timeout = (uint32_t)time_tick;
    }

    psti2c = &stm32_i2c[i2c->port - 1].hal_i2c_handle;

    if (timeout == AOS_WAIT_FOREVER) {
        real_timeout = timeout;
    } else {
        time_tick = krhino_ms_to_ticks(timeout);
        real_timeout = (uint32_t)time_tick;
    }

    ret = HAL_I2C_Mem_Write(psti2c, dev_addr, mem_addr,
          (uint16_t)mem_addr_size, (uint8_t *)data, size, real_timeout);

    return ret;
};

int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    int ret = -1;
    uint64_t time_tick = 0;
    uint32_t real_timeout = 0;
    I2C_HandleTypeDef *psti2c = NULL;

    if (NULL == i2c || NULL == data){
        printf("%s invalid input \r\n", __func__);
        return ret;
    }

    if (NULL == get_i2c_instance_by_port(i2c->port)) {
        printf("get_i2c_instance_by_port fail \r\n");
        return ret;
    }

    if (!stm32_i2c[i2c->port - 1].inited) {
        printf("i2c port %d have not init yet in call %s \r\n", i2c->port, __func__);
        return ret;
    }

    psti2c = &stm32_i2c[i2c->port - 1].hal_i2c_handle;

    if (timeout == AOS_WAIT_FOREVER) {
        real_timeout = timeout;
    } else {
        time_tick = krhino_ms_to_ticks(timeout);
        real_timeout = (uint32_t)time_tick;
    }

    ret = HAL_I2C_Mem_Read(psti2c, dev_addr, mem_addr,
          (uint16_t)mem_addr_size, data, size, real_timeout);
    if (ret) {
        printf("i2c port %d read dev 0x%x meme_addr 0x%x mem_addr_size %d timeout 0x%x \r\n",
            i2c->port, dev_addr, mem_addr, mem_addr_size, real_timeout);
        return -1;
    }
    return 0;
};

int32_t hal_i2c_finalize(i2c_dev_t *i2c)
{
    int32_t ret = -1;

    if (NULL == get_i2c_instance_by_port(i2c->port)) {
        return ret;
    }

    if (!stm32_i2c[i2c->port - 1].inited) {
        return 0;
    }

    if (HAL_I2C_GetState(&stm32_i2c[i2c->port - 1].hal_i2c_handle) != HAL_I2C_STATE_RESET) {
        /* DeInit the I2C */
        ret = HAL_I2C_DeInit(&stm32_i2c[i2c->port - 1].hal_i2c_handle);
    }

    stm32_i2c[i2c->port - 1].inited = 0;

    return ret;
}

#endif

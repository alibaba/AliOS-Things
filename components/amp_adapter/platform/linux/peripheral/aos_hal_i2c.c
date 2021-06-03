/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <aos/errno.h>
#include "aos_hal_i2c.h"

#define BUF_LEN 16
uint8_t buf[BUF_LEN];

int32_t aos_hal_i2c_init(i2c_dev_t *i2c)
{
    if(NULL == i2c) {
        printf("parameter is invalid!\n\r");
        return -1;
    }
    memset(buf, 0, BUF_LEN);

    printf("[%s] freq = %d slave_addr = 0x%X\r\n",
           __FUNCTION__, i2c->config.freq, i2c->config.dev_addr);
    return 0;
}

int32_t aos_hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    uint32_t i;
    printf("[%s] \r\n", __FUNCTION__);

    for (i = 0; i < size; i++) {
        buf[i] = data[i];
        printf("[%d] 0x%x \r\n", i, buf[i]);
    }
    return 0;
}

int32_t aos_hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    uint32_t i;
    printf("[%s] \r\n", __FUNCTION__);

    for (i = 0; i < size; i++) {
        data[i] = buf[i];
        printf("[%d] 0x%x \r\n", i, data[i]);
    }
    return 0;
}

int32_t aos_hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    printf("[%s] \r\n", __FUNCTION__);
    return 0;
}

int32_t aos_hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
    printf("[%s] \r\n", __FUNCTION__);
    return 0;
}

int32_t aos_hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    uint32_t i;

    printf("[%s] \r\n", __FUNCTION__);

    for (i = 0; i < size; i++) {
        buf[i] = data[i];
        printf("[%d] 0x%x \r\n", i, buf[i]);
    }
    return 0;
}

int32_t aos_hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    uint32_t i;

    printf("[%s] \r\n", __FUNCTION__);

    for (i = 0; i < size; i++) {
        data[i] = buf[i];
        printf("[%d] 0x%x \r\n", i, data[i]);
    }

    return 0;
}

int32_t aos_hal_i2c_finalize(i2c_dev_t *i2c)
{
    printf("[%s] \r\n", __FUNCTION__);
    return 0;
}

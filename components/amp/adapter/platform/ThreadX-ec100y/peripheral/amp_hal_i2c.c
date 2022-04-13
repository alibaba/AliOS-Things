/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "amp_defines.h"
#include "ql_iic.h"
#include "amp_hal_i2c.h"


#define MOD_STR		"I2C"

int32_t amp_hal_i2c_init(i2c_dev_t *i2c)
{
    int32_t ret = -1;
    QL_I2C_MODE_E freq_mode;

    if (i2c == NULL) {
        return ret;
    }
    /* only one i2c port */
    if (i2c->port > 0)
        return ret;

    switch (i2c->config.freq) {
        case I2C_BUS_BIT_RATES_100K:
            freq_mode = STANDARD_MODE;
            break;
        case I2C_BUS_BIT_RATES_400K:
            freq_mode = FAST_MODE;
            break;
        case I2C_BUS_BIT_RATES_3400K:
            freq_mode = HS_MODE;
            break;
        default: break;
    }

    ret = ql_i2c_init(i2c->port, freq_mode);
    if (ret != 0) {
        return ret;
    }

    return ret;
}

int32_t amp_hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    int32_t ret = -1;
    uint8_t mem_addr, i;
    uint8_t wrtBuff[size - 1];

    if (i2c == NULL || data == NULL) {
        amp_error(MOD_STR, "i2c is null or data is null\r\n");
        return ret;
    }

    mem_addr = data[0];
    for(i = 0; i < size - 1; i++) {
        wrtBuff[i] = data[i+1];
    }
    
    ret = ql_i2c_write(i2c->port, dev_addr, mem_addr, wrtBuff, size - 1);
    if (ret != 0) {
        amp_error(MOD_STR, "i2c write error!\r\n");
        return ret;
    }

    return ret;
}

int32_t amp_hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    int32_t ret = -1;
    uint8_t mem_addr, i;
    uint8_t wrtBuff[size - 1];

    if (i2c == NULL || data == NULL) {
        amp_error(MOD_STR, "i2c is null or data is null\r\n");
        return ret;
    }

    mem_addr = data[0];
    for(i = 0; i < size - 1; i++) {
        wrtBuff[i] = data[i+1];
    }
    
    ret = ql_i2c_read(i2c->port, dev_addr, mem_addr, wrtBuff, size - 1);
    if (ret != 0) {
        amp_error(MOD_STR, "i2c write error!\r\n");
        return ret;
    }

    return ret;
}

int32_t amp_hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    return 0;
}

int32_t amp_hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
    return 0;
}

int32_t amp_hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    int32_t ret = -1;
    uint8_t  i;

    if (i2c == NULL || data == NULL) {
        amp_error(MOD_STR, "i2c is null or data is null\r\n");
        return ret;
    }

    
    ret = ql_i2c_write(i2c->port, dev_addr, mem_addr, data, size);
    if (ret != 0) {
        amp_error(MOD_STR, "i2c write error!\r\n");
        return ret;
    }

    return ret;
}

int32_t amp_hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    int32_t ret = -1;
    uint8_t  i;
    uint8_t wrtBuff[size - 1];
  
    if (i2c == NULL || data == NULL) {
      amp_error(MOD_STR, "i2c is null or data is null\r\n");
      return ret;
    }
  
    
    ret = ql_i2c_read(i2c->port, dev_addr, mem_addr, data, size);
    
    if (ret != 0) {
      amp_error(MOD_STR, "i2c write error!\r\n");
      return ret;
    }
  
    return ret;
}

int32_t amp_hal_i2c_finalize(i2c_dev_t *i2c)
{
    return 0;
}

/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include <k_api.h>
#include "ulog/ulog.h"
#include "aos/hal/i2c.h"
#include "pca9544.h"
#include "mux_i2c.h"

/*********************************************************
 * @fun    haas_mux_i2c_init
 * @breif  mux i2c initialization
 * @param  i2c:the pointer for i2c configuration
 * @param  port: port for muxer
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t haas_mux_i2c_init(i2c_dev_t *i2c, uint8_t port)
{
    int32_t ret = 0;

    PCA9544_DEV_CFG_T dev_cfg;

    dev_cfg.dev_addr = PCA9544_BASE_ADDR;
    dev_cfg.pca9544_ch = port;

    if(i2c == NULL)
    {
        printf("i2c param is null\r\n");
        return -1;
    }

    if((i2c->port != DFT_MCU_I2C_PORT))
    {
        printf("The i2c port is not correct for mux_i2c\r\n");
        i2c->port = 1;
    }

    ret = pca9544_init(i2c, &dev_cfg);
    if (ret) {
        printf("mux_i2c init error since pca9544 init fail\r\n");
        return -1;

    }

    ret = pca9544_set_chan(dev_cfg.pca9544_ch);
    if (ret) {
        printf("mux_i2c init error since pca9544 init fail\r\n");
        return -1;

    }

    return ret;
}

/*********************************************************
 * @fun    haas_mux_i2c_reg_write
 * @param[in]  i2c            the i2c device
 * @param[in]  dev_addr       device address
 * @param[in]  reg_addr       register address
 * @param[in]  reg_num        register num
 * @param[in]  data           i2c master send data
 * @param[in]  size           i2c master send data size
 * @param[in]  timeout        timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                            if you want to wait forever
 *
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t haas_mux_i2c_reg_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t reg_addr,
                          uint16_t reg_num, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    int32_t ret = 0;

    if(i2c == NULL)
    {
        printf("i2c param is null\r\n");
        return -1;
    }

    if((i2c->port != DFT_MCU_I2C_PORT))
    {
        printf("The i2c port is not correct for mux_i2c\r\n");
        i2c->port = DFT_MCU_I2C_PORT;
    }

    ret = hal_i2c_mem_write(i2c, dev_addr, reg_addr, reg_num, data, size, timeout);
    if (ret) {
        printf("mux_i2c reg write failed\r\n");
        return -1;
    }


}

/*********************************************************
 * @fun    haas_mux_i2c_reg_read
 *
 * @param[in]   i2c            the i2c device
 * @param[in]   dev_addr       device address
 * @param[in]   reg_addr       register address
 * @param[in]   reg_num        register number
 * @param[out]  data           i2c master send data
 * @param[in]   size           i2c master send data size
 * @param[in]  timeout         timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                             if you want to wait forever
 *
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t haas_mux_i2c_reg_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t reg_addr,
                          uint16_t reg_num, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    int32_t ret = 0;

    if(i2c == NULL)
    {
        printf("i2c param is null\r\n");
        return -1;
    }

    if((i2c->port != DFT_MCU_I2C_PORT))
    {
        printf("The i2c port is not correct for mux_i2c\r\n");
        i2c->port = DFT_MCU_I2C_PORT;
    }

    ret = hal_i2c_mem_read(i2c, dev_addr, reg_addr, reg_num, data, size, timeout);
    if (ret) {
        printf("mux_i2c reg write failed\r\n");
        return -1;
    }

}

/********************************************************
 * @fun    haas_mux_i2c_data_send
 *
 * @param[in]  i2c            the i2c device
 * @param[in]  dev_addr       device address
 * @param[in]  data           i2c master send data
 * @param[in]  size           i2c master send data size
 * @param[in]  timeout        timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                            if you want to wait forever
 *
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t haas_mux_i2c_data_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
    uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;

    if(i2c == NULL)
    {
        printf("i2c param is null\r\n");
        return -1;
    }

    if((i2c->port != DFT_MCU_I2C_PORT))
    {
        printf("The i2c port is not correct for mux_i2c\r\n");
        i2c->port = DFT_MCU_I2C_PORT;
    }


    ret = hal_i2c_master_send(i2c, dev_addr, data, size, timeout);
    if (ret) {
        printf("mux_i2c data send failed\r\n");
        return -1;
    }

}

/********************************************************
 * @fun    haas_mux_i2c_data_recv
 *
 * @param[in]  i2c            the i2c device
 * @param[in]  dev_addr       device address
 * @param[in]  data           i2c master recv data
 * @param[in]  size           i2c master recv data size
 * @param[in]  timeout        timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                            if you want to wait forever
 *
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t haas_mux_i2c_data_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                             uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;

    if(i2c == NULL)
    {
        printf("i2c param is null\r\n");
        return -1;
    }

    if((i2c->port != DFT_MCU_I2C_PORT))
    {
        printf("The i2c port is not correct for mux_i2c\r\n");
        i2c->port = DFT_MCU_I2C_PORT;
    }

    ret = hal_i2c_master_recv(i2c, dev_addr, data, size, timeout);
    if (ret)
    {
        printf("mux_i2c data recv failed\r\n");
        return -1;
    }
}

/********************************************************
 * @fun    haas_mux_i2c_deinit
 * @param  null
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t haas_mux_i2c_deinit()
{
    int32_t ret = 0;

    pca9544_deinit();

    return ret;
}

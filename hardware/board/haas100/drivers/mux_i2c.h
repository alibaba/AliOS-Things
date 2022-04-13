/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef MUX_I2C_H
#define MUX_I2C_H


/* Muxer I2C Port definition */
#define HAAS_MUX_I2C_PORT2   0x06
#define HAAS_MUX_I2C_PORT3   0x07
#define DFT_MCU_I2C_PORT     1

/*********************************************************
 * @fun    haas_mux_i2c_init
 * @breif  mux i2c initialization
 * @param  i2c:the pointer for i2c configuration
 * @param  port: port for muxer
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t haas_mux_i2c_init(i2c_dev_t *i2c, uint8_t port);

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
                          uint32_t timeout);

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
                          uint32_t timeout);

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
   uint16_t size, uint32_t timeout);

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
    uint16_t size, uint32_t timeout);

/********************************************************
 * @fun    haas_mux_i2c_deinit
 * @param  null
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t haas_mux_i2c_deinit();

#endif /* MUX_I2C_H */

/**
 * @file i2c.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef HAL_I2C_H
#define HAL_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup hal_i2c I2C
 *  i2c hal API.
 *
 *  @{
 */

#include <stdint.h>

/* Define the wait forever timeout macro */
#define HAL_WAIT_FOREVER 0xFFFFFFFFU

#define I2C_MODE_MASTER 1 /**< i2c communication is master mode */
#define I2C_MODE_SLAVE  2 /**< i2c communication is slave mode */

#define I2C_MEM_ADDR_SIZE_8BIT  1 /**< i2c memory address size 8bit */
#define I2C_MEM_ADDR_SIZE_16BIT 2 /**< i2c memory address size 16bit */

/*
 * Specifies one of the standard I2C bus bit rates for I2C communication
 */
#define I2C_BUS_BIT_RATES_100K  100000
#define I2C_BUS_BIT_RATES_400K  400000
#define I2C_BUS_BIT_RATES_3400K 3400000

/* Addressing mode */
#define I2C_HAL_ADDRESS_WIDTH_7BIT  0   /**< 7 bit mode */
#define I2C_HAL_ADDRESS_WIDTH_10BIT 1   /**< 10 bit mode */

/* This struct define i2c config args */
typedef struct {
    uint32_t address_width; /**< Addressing mode: 7 bit or 10 bit */
    uint32_t freq;          /**< CLK freq */
    uint8_t  mode;          /**< master or slave mode */
    uint16_t dev_addr;      /**< slave device addr */
} i2c_config_t;

/* This struct define i2c main handle */
typedef struct {
    uint8_t       port;   /**< i2c port */
    i2c_config_t  config; /**< i2c config */
    void         *priv;   /**< priv data */
} i2c_dev_t;

/**
 * Initialises an I2C interface
 * Prepares an I2C hardware interface for communication as a master or slave
 *
 * @param[in]  i2c  the device for which the i2c port should be initialised
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_i2c_init(i2c_dev_t *i2c);

/**
 * I2c master send
 *
 * @param[in]  i2c       the i2c device
 * @param[in]  dev_addr  device address
 * @param[in]  data      i2c send data
 * @param[in]  size      i2c send data size
 * @param[in]  timeout   timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout);

/**
 * I2c master recv
 *
 * @param[in]   i2c       the i2c device
 * @param[in]   dev_addr  device address
 * @param[out]  data      i2c receive data
 * @param[in]   size      i2c receive data size
 * @param[in]   timeout   timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                        if you want to wait forever
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout);

/**
 * I2c slave send
 *
 * @param[in]  i2c      the i2c device
 * @param[in]  data     i2c slave send data
 * @param[in]  size     i2c slave send data size
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout);

/**
 * I2c slave receive
 *
 * @param[in]   i2c      tthe i2c device
 * @param[out]  data     i2c slave receive data
 * @param[in]   size     i2c slave receive data size
 * @param[in]  timeout   timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout);

/**
 * I2c mem write
 *
 * @param[in]  i2c            the i2c device
 * @param[in]  dev_addr       device address
 * @param[in]  mem_addr       mem address
 * @param[in]  mem_addr_size  mem address
 * @param[in]  data           i2c master send data
 * @param[in]  size           i2c master send data size
 * @param[in]  timeout        timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                            if you want to wait forever
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout);

/**
 * I2c master mem read
 *
 * @param[in]   i2c            the i2c device
 * @param[in]   dev_addr       device address
 * @param[in]   mem_addr       mem address
 * @param[in]   mem_addr_size  mem address
 * @param[out]  data           i2c master send data
 * @param[in]   size           i2c master send data size
 * @param[in]  timeout         timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                             if you want to wait forever
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout);

/**
 * Deinitialises an I2C device
 *
 * @param[in]  i2c  the i2c device
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_i2c_finalize(i2c_dev_t *i2c);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* HAL_I2C_H */


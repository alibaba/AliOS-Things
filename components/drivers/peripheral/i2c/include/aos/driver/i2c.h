/**
 * @file i2c.h
 * @copyright Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _AOS_I2C_H
#define _AOS_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <aos/kernel.h>

/** @addtogroup aos_i2c_device I2C
 *  aos i2c API.
 *
 *  @{
 */


/* Define the wait forever timeout macro */
#define AOS_WAIT_FOREVER 0xFFFFFFFFU

/*
 * I2C bus's clock rate
 */
#define I2C_BUS_CLK_10K  10000
#define I2C_BUS_CLK_100K  100000
#define I2C_BUS_CLK_400K  400000
#define I2C_BUS_CLK_1000K  1000000
#define I2C_BUS_CLK_3400K 3400000

/* Slave device's address mode */
#define I2C_SLAVE_ADDR_WIDTH_7BIT  0     /**< 7 bit mode */
#define I2C_SLAVE_ADDR_WIDTH_10BIT 1     /**< 10 bit mode */

#define I2C_MEM_ADDR_SIZE_8BIT  1        /**< i2c memory address size 8bit */
#define I2C_MEM_ADDR_SIZE_16BIT 2        /**< i2c memory address size 16bit */

typedef uint32_t* i2c_dev_handle_t;

typedef struct i2c_slave_config {
    uint16_t addr;          /**< slave device's address */
    uint32_t clk;           /**< clock during i2c transaction */
    uint32_t addr_width;    /**< device's address mode: 7 bit or 10 bit */
} i2c_slave_config_t;


/**
 * setup new I2C channel with specified slave device's config to communicate with slave device
 *
 * @param[in]  id  host I2C controller's channel ID
 * @param[in]  config slave device's address, address width and clock settings, \ref i2c_slave_config_t
 *
 * @return  handle of the i2c device, used for later i2c operations on I2C slave device, when open operation success;
            NULL when open operation fails
 */
i2c_dev_handle_t aos_i2c_open (uint32_t id, i2c_slave_config_t *config);

/**
 * close target I2C communication channel
 *
 * @param[in]  dev  handle of the i2c device to be operated, must be the return value of aos_i2c_open
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
aos_status_t aos_i2c_close (i2c_dev_handle_t dev);

/**
 * set I2C bus's clock when communicate with target I2C slave
 *
 * @param[in]  dev  handle of the i2c device to be operated, must be the return value of aos_i2c_open
 * @param[in]  clk  target clock to be used
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
aos_status_t aos_i2c_clk_set (i2c_dev_handle_t dev,  uint32_t clk);

/**
 * set slave device's address witdh on target I2C channel
 *
 * @param[in]  dev  handle of the i2c device to be operated, must be the return value of aos_i2c_open
 * @param[in]  addr_width  slave device's address width, 7-bit or 10-bit only
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
aos_status_t aos_i2c_addr_width_set (i2c_dev_handle_t dev,  uint32_t addr_width);

/**
 * set slave device's address on target I2C channel
 *
 * @param[in]  dev  handle of the i2c device to be operated, must be the return value of aos_i2c_open
 * @param[in]  addr  slave device's address width
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
aos_status_t aos_i2c_slave_addr_set (i2c_dev_handle_t dev,  uint16_t  addr);

/**
 * issue I2C write operation on target I2C channel
 *
 * @param[in]  dev  handle of the i2c device to be operated, must be the return value of aos_i2c_open
 * @param[in]  buf  pointer to the buffer to be sent
 * @param[in]  len  number of data to be sent
 * @param[in]  timeout  in unit of ms, current i2c transaction's timeout value
 *             use AOS_WAIT_FOREVER for endless wait until operation success or fails
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
int32_t aos_i2c_master_send (i2c_dev_handle_t dev, char *buf, uint32_t len, uint32_t timeout);

/**
 * issue I2C read operation on target I2C channel
 *
 * @param[in]  dev  handle of the i2c device to be operated, must be the return value of aos_i2c_open
 * @param[in]  buf  pointer to the buffer where to save the received data
 * @param[in]  len  number of data to be read
 * @param[in]  timeout  in unit of ms, current i2c transaction's timeout value
 *             use AOS_WAIT_FOREVER for endless wait until operation success or fails
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
int32_t aos_i2c_master_recv (i2c_dev_handle_t dev, char *buf, uint32_t len, uint32_t timeout);

/**
 * issue I2C memory/register write operation on target I2C channel
 *
 * @param[in]  dev  handle of the i2c device to be operated, must be the return value of aos_i2c_open
 * @param[in]  addr address of target memory/register to be write
 * @param[in]  addr_len  address's length, either I2C_MEM_ADDR_SIZE_8BIT or I2C_MEM_ADDR_SIZE_16BIT
 * @param[in]  buf  pointer to the buffer to be written
 * @param[in]  len  number of data to be written
 * @param[in]  timeout  in unit of ms, current i2c transaction's timeout value
 *             use AOS_WAIT_FOREVER for endless wait until operation success or fails
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
int32_t aos_i2c_mem_write (i2c_dev_handle_t dev, uint16_t addr, uint8_t addr_len, uint8_t *buf, uint32_t len, uint32_t timeout);

/**
 * issue I2C memory/register read operation on target I2C channel
 *
 * @param[in]  dev  handle of the i2c device to be operated, must be the return value of aos_i2c_open
 * @param[in]  addr address of target memory/register to be read
 * @param[in]  addr_len  address's length, , either I2C_MEM_ADDR_SIZE_8BIT or I2C_MEM_ADDR_SIZE_16BIT
 * @param[in]  buf  pointer to the buffer for read operation
 * @param[in]  len  number of data to be read
 * @param[in]  timeout  in unit of ms, current i2c transaction's timeout value
 *             use AOS_WAIT_FOREVER for endless wait until operation success or fails
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
int32_t aos_i2c_mem_read (i2c_dev_handle_t dev, uint16_t addr, uint8_t addr_len, uint8_t *buf, uint32_t len, uint32_t timeout);

/**
 * issue I2C memory/register read operation on target I2C channel
 *
 * @param[in]  dev  handle of the i2c device to be operated, must be the return value of aos_i2c_open
 * @param[in]  level  level of current power management level
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
aos_status_t aos_i2c_pm_ctrl (i2c_dev_handle_t dev, int level);

/**
 * get the number of I2C hardware module
 *
 * @param[in]  dev  handle of the i2c device to be operated, must be the return value of aos_i2c_open
 * @param[in]  level  level of current power management level
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
int32_t aos_i2c_get_num(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _AOS_I2C_H */

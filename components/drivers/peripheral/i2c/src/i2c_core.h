/**
 * @file i2c_core.h
 * @copyright Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _I2C_CORE_H
#define _I2C_CORE_H

#ifdef __cplusplus
extern "C" {
#endif
#include "aos/driver/i2c.h"
#include <aos/list.h>
#include <aos/kernel.h>

#define AOS_I2C_MAGIC 0x12c12c12

/* This struct define i2c main handle */
typedef struct {
    i2c_slave_config_t  config;            /**< i2c slave device's config */
    aos_mutex_t         lock;             /**< lock for i2c slave device's operations */
    uint32_t            i2c_dev_handle;    /**< use &i2c_dev_handle as i2c device's handle */
    dlist_t             node;              /**<  ode in i2c channel's slave device list */
    void                *master;           /**<  pointer to i2c controller's struct */
} i2c_slave_dev_t;

/**
 * i2c master controller's settings
 * all slave devices in the same I2C bus are connected to the list named with slave
 * each time when an i2c transaction request is receiived, we check whether last i2c transaction on the same device or not
 * if yes, skip slave device address/clock/device address bit mode setting, issue i2c tx/rx directly
 * if current request is on a new device compared with the last one, check if target clock equals with clk or not, if not
 * set clock; then set device's address and address bit mode, and then issue tx/rx request to driver
*/
typedef struct i2c_master_dev {
    uint32_t    id;          /**< i2c master controller's id */
    aos_mutex_t lock;        /**< used to lock all operations on current i2c master device */
    dlist_t     slave;       /**< node in i2c channel's slave device list */
    uint32_t    clk;         /**< used to log i2c host's current clock setting, if current i2c transaction equals with
                                clk, skip slave clk setting procedure */
    uint16_t    addr;        /**< used to log i2c host's current slave address, if current i2c transaction equals with
                                slave address, skip slave slave address setting procedure */
    uint32_t    addr_width;  /**< used to log i2c host's current address width mode setting, if current i2c transaction equals with
                                address width mode, skip slave address width mode setting procedure */
    csi_iic_t   csi_dev;     /**< CSI I2C device */
    dlist_t     host;        /**< node in host i2c controller list*/
    uint32_t    init;        /**< whether i2c controller is initialized or not >*/
} i2c_master_dev_t;


#ifdef __cplusplus
}
#endif

#endif /* _I2C_CORE_H */

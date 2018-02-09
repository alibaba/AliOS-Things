/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef YUNOS_BSP_I2C_H
#define YUNOS_BSP_I2C_H

#include <stdint.h>
#include <bsp/yunos_bsp_errno.h>
#include <i2c.h>

typedef enum {
    DRV_I2C_INT_TYPE_RX_OVER         , ///< receive buffer was completely filled
    DRV_I2C_INT_TYPE_RX_FULL         , ///< transmit buffer reaches or goes above the threshold value
    DRV_I2C_INT_TYPE_TX_OVER         , ///< transmit buffer was completely filled
    DRV_I2C_INT_TYPE_TX_EMPTY        , ///< transmit buffer is at or below the threshold value
    DRV_I2C_INT_TYPE_RX_DONE         , ///< transmission is done
    DRV_I2C_INT_TYPE_START_DET       , ///< Indicates whether a start condition has occurred on the I2C interface
    DRV_I2C_INT_TYPE_WRITE_STOP      , ///< Indicates whether a write stop condition has occurred on the I2C interface
    DRV_I2C_INT_TYPE_READ_STOP         ///< Indicates whether a read stop condition has occurred on the I2C interface
} ENUM_DRV_I2C_INT_TYPE;

struct i2c_drv_cb_s {
    ENUM_DRV_I2C_INT_TYPE type;        ///< type for driver porting i2c interrupt source
    uint8_t               dev_id;      ///< i2c bsp id
};

typedef enum {
    BSP_I2C_USEMODE_INT,
    BSP_I2C_USEMODE_QUERY
} ENUM_BSP_I2C_USEMODE_TYPE;

typedef enum {
    BSP_I2C_FIFO_WRITE  ,
    BSP_I2C_FIFO_READ
} ENUM_BSP_I2C_FIFO_TYPE;

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/**
 * initialization i2c module
 * @param[in]    count      get count of i2c modules
 * @return       0-success, other-error
 */
int yunos_bsp_i2c_init(uint8_t *count);

/**
 * uninitialization i2c module
 * @return      0-success, other-error
 */
int yunos_bsp_i2c_uninit(void);

/**
 * init i2c register state and enable i2c module
 * @param[in]    id         bus id
 * @return       0-success, other-error
 */
int yunos_bsp_i2c_open(uint8_t id);

/**
 * disable i2c module
 * @param[in]    id         bus id
 * @return       0-success, other-error
 */
int yunos_bsp_i2c_close(uint8_t id);

/**
 * set i2c configuration
 * @param[in]   id         bus id
 * @param[in]   i2c_config configuration
 * @return      0-success, other-error
 */
int yunos_bsp_i2c_setconfig(uint8_t id, struct i2c_config_s *i2c_config);

/**
 * read i2c fifo data
 * @param[in]   id         bus id
 * @param[in]   buffer     datastore space
 * @param[in]   buflen     data length
 * @return      0-success, other-error
 */
int yunos_bsp_i2c_fifo_read(uint8_t id, char *buffer, uint8_t buflen);

/**
 * write i2c fifo data
 * @param[in]   id          bus id
 * @param[in]   buffer      datastore space
 * @param[in]   buflen      data length
 * @return      0-success , other-error
 */
int yunos_bsp_i2c_fifo_write(uint8_t id, char const *buffer, uint8_t buflen);

/**
 * send i2c start signal for write and read
 * @param[in]   id         bus id
 * @param[in]   type       the type for write or read
 * @return      0-success, other-error
 */
int yunos_bsp_i2c_start(uint8_t id, ENUM_BSP_I2C_FIFO_TYPE type);

/**
 * if i2c bsp trigger interrupt, callback function id called
 * @param[in]   id         bus id
 * @param[in]   cb         callback function
 * @return      0-success, other-error
 */
int yunos_bsp_i2c_set_interrupt_cb(uint8_t id, void (*cb)(struct i2c_drv_cb_s*));

/**
 * set i2c fifo threshold
 * @param[in]   id         bus id
 * @param[in]   type       the type for write or read
 * @param[in]   threshold  threshold for transmit-receive
 * @return      0-success, other-error
 */
int yunos_bsp_i2c_fifo_interrupt_threshold(uint8_t id, ENUM_BSP_I2C_FIFO_TYPE type, uint8_t threshold);

/**
 * examine remain fifo space for write and read
 * @param[in]   id         bus id
 * @param[in]   type       the type for write or read
 * @param[in]   size       remain size
 * @return      0-success, other-error
 */
int yunos_bsp_i2c_fifo_remain(uint8_t id, ENUM_BSP_I2C_FIFO_TYPE type, uint8_t* size);

/**
 * inquire bsp i2c use mode for interrupt or query
 * @param[in]   id         bus id
 * @return      ENUM_BSP_I2C_USEMODE_TYPE
 */
ENUM_BSP_I2C_USEMODE_TYPE yunos_bsp_i2c_get_usemode(uint8_t id);

//bool yunos_bsp_i2c_status(uint8_t id, enum I2C_STATUS bit);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* YUNOS_BSP_I2C_H */

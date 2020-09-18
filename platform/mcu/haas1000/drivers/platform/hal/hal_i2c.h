/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_I2C_H__
#define __HAL_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "hal_gpio.h"
#include "hal_sensor_eng.h"

// For 10-bit address devices: OR the following mask with the real address
#define HAL_I2C_10BITADDR_MASK              (1 << 15)

enum HAL_I2C_ACTION_AFTER_WRITE_T {
    HAL_I2C_STOP_AFTER_WRITE = 0,
    HAL_I2C_RESTART_AFTER_WRITE,
};

enum HAL_I2C_ID_T {
    HAL_I2C_ID_0 = 0,
#if (CHIP_HAS_I2C > 1)
    HAL_I2C_ID_1,
#endif

    HAL_I2C_ID_NUM,
};

enum HAL_I2C_API_MODE_T {
    HAL_I2C_API_MODE_TASK = 0,
    HAL_I2C_API_MODE_SIMPLE,
    HAL_I2C_API_MODE_SENSOR_ENGINE,
};

enum HAL_I2C_INT_STATUS_T {
    HAL_I2C_INT_STATUS_GEN_CALL_MASK = 0x1<<11,
    HAL_I2C_INT_STATUS_START_DET_MASK = 0x1<<10,
    HAL_I2C_INT_STATUS_STOP_DET_MASK = 0x1<<9,
    HAL_I2C_INT_STATUS_ACTIVITY_MASK = 0x1<<8,
    HAL_I2C_INT_STATUS_RX_DONE_MASK = 0x1<<7,
    HAL_I2C_INT_STATUS_TX_ABRT_MASK = 0x1<<6,
    HAL_I2C_INT_STATUS_RD_REQ_MASK = 0x1<<5,
    HAL_I2C_INT_STATUS_TX_EMPTY_MASK = 0x1<<4,
    HAL_I2C_INT_STATUS_TX_OVER_MASK = 0x1<<3,
    HAL_I2C_INT_STATUS_RX_FULL_MASK = 0x1<<2,
    HAL_I2C_INT_STATUS_RX_OVER_MASK = 0x1<<1,
    HAL_I2C_INT_STATUS_RX_UNDER_MASK = 0x1<<0,
    /* FIXME : same as i2cip definitions
    #define I2CIP_INT_STATUS_GEN_CALL_SHIFT (11)
    #define I2CIP_INT_STATUS_START_DET_SHIFT (10)
    #define I2CIP_INT_STATUS_STOP_DET_SHIFT (9)
    #define I2CIP_INT_STATUS_ACTIVITY_SHIFT (8)
    #define I2CIP_INT_STATUS_RX_DONE_SHIFT  (7)
    #define I2CIP_INT_STATUS_TX_ABRT_SHIFT (6)
    #define I2CIP_INT_STATUS_RD_REQ_SHIFT (5)
    #define I2CIP_INT_STATUS_TX_EMPTY_SHIFT (4)
    #define I2CIP_INT_STATUS_TX_OVER_SHIFT (3)
    #define I2CIP_INT_STATUS_RX_FULL_SHIFT (2)
    #define I2CIP_INT_STATUS_RX_OVER_SHIFT (1)
    #define I2CIP_INT_STATUS_RX_UNDER_SHIFT (0)
     */
};

enum HAL_I2C_ERRCODE_T {
    HAL_I2C_ERRCODE_SLVRD_INTX      = 1<<15,
    HAL_I2C_ERRCODE_SLV_ARBLOST     = 1<<14,
    HAL_I2C_ERRCODE_SLVFLUSH_TXFIFO = 1<<13,
    HAL_I2C_ERRCODE_ARB_LOST        = 1<<12,
    HAL_I2C_ERRCODE_MASTER_DIS      = 1<<11,
    HAL_I2C_ERRCODE_10B_RD_NORSTRT  = 1<<10,
    HAL_I2C_ERRCODE_SBYTE_NORSTRT   = 1<<9,
    HAL_I2C_ERRCODE_HS_NORSTRT      = 1<<8,
    HAL_I2C_ERRCODE_SBYTE_ACKDET    = 1<<7,
    HAL_I2C_ERRCODE_HS_ACKDET       = 1<<6,
    HAL_I2C_ERRCODE_GCALL_READ      = 1<<5,
    HAL_I2C_ERRCODE_GCALL_NOACK     = 1<<4,
    HAL_I2C_ERRCODE_TXDATA_NOACK    = 1<<3,
    HAL_I2C_ERRCODE_10ADDR2_NOACK   = 1<<2,
    HAL_I2C_ERRCODE_10ADDR1_NOACK   = 1<<1,
    HAL_I2C_ERRCODE_7B_ADDR_NOACK   = 1<<0,

    HAL_I2C_ERRCODE_INV_PARAM       = 1<<31,
    HAL_I2C_ERRCODE_IN_USE          = 1<<30,
    HAL_I2C_ERRCODE_FIFO_ERR        = 1<<29,
    HAL_I2C_ERRCODE_SYNC_TIMEOUT    = 1<<28,
    HAL_I2C_ERRCODE_ACT_TIMEOUT     = 1<<27,
    HAL_I2C_ERRCODE_TFNF_TIMEOUT    = 1<<26,
    HAL_I2C_ERRCODE_TFE_TIMEOUT     = 1<<25,
    HAL_I2C_ERRCODE_RFNE_TIMEOUT    = 1<<24,

    /* FIXME : same as i2cip definitions
    #define I2CIP_TX_ABRT_SOURCE_ABRT_SLVRD_INTX_SHIFT (15)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_SLV_ARBLOST_SHIFT (14)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_SLVFLUSH_TXFIFO_SHIFT (13)
    #define I2CIP_TX_ABRT_SOURCE_ARB_LOST_SHIFT (12)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_MASTER_DIS_SHIFT (11)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_10B_RD_NORSTRT_SHIFT (10)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_SBYTE_NORSTRT_SHIFT (9)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_HS_NORSTRT_SHIFT (8)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_SBYTE_ACKDET_SHIFT (7)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_HS_ACKDET_SHIFT (6)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_GCALL_READ_SHIFT (5)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_GCALL_NOACK_SHIFT (4)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK_SHIFT (3)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_10ADDR2_NOACK_SHIFT (2)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_10ADDR1_NOACK_SHIFT (1)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK_SHIFT (0)
    */
};

struct HAL_I2C_CONFIG_T {
    /* mode
     SIMPLE_MODE : pulling when reading or writing (always sync), or use dma external;master or slave;only enable slave related irq;
     TASK_MODE : task to driven reading or writing (sync or async, dma or non-dma);only master
     */
    enum HAL_I2C_API_MODE_T mode;

    uint8_t use_sync;
    uint8_t use_dma;
    uint8_t as_master;
    uint32_t speed;
    uint32_t addr_as_slave;
};

typedef void (*HAL_I2C_SENSOR_ENG_HANDLER_T)(enum HAL_I2C_ID_T id, const uint8_t *buf, uint32_t len);

struct HAL_I2C_SENSOR_ENGINE_CONFIG_T {
    enum HAL_SENSOR_ENGINE_ID_T id;
    enum HAL_SENSOR_ENGINE_TRIGGER_T trigger_type;
    enum HAL_GPIO_PIN_T trigger_gpio;
    uint32_t period_us;
    uint16_t target_addr;
    uint8_t *write_buf;
    uint16_t write_txn_len;
    uint8_t *read_buf;
    uint16_t read_txn_len;
    uint16_t txn_cnt;
    uint8_t read_burst_cnt;
    HAL_I2C_SENSOR_ENG_HANDLER_T handler;
};

struct HAL_GPIO_I2C_CONFIG_T {
    enum HAL_GPIO_PIN_T scl;
    enum HAL_GPIO_PIN_T sda;
    uint32_t speed;
};

uint32_t hal_i2c_open(enum HAL_I2C_ID_T id, const struct HAL_I2C_CONFIG_T *cfg);
uint32_t hal_i2c_close(enum HAL_I2C_ID_T id);
uint32_t hal_i2c_reopen(enum HAL_I2C_ID_T id, const struct HAL_I2C_CONFIG_T *cfg);

/* for master task mode */
typedef void (*HAL_I2C_TRANSFER_HANDLER_T)(enum HAL_I2C_ID_T id, uint32_t transfer_id,
                                            const uint8_t *tx_buf, uint32_t tx_len,
                                            const uint8_t *rx_buf, uint32_t rx_len,
                                            enum HAL_I2C_ERRCODE_T errcode);
uint32_t hal_i2c_send(enum HAL_I2C_ID_T id, uint32_t device_addr, uint8_t *buf, uint32_t reg_len, uint32_t value_len,
        uint32_t transfer_id, HAL_I2C_TRANSFER_HANDLER_T handler);
uint32_t hal_i2c_recv(enum HAL_I2C_ID_T id, uint32_t device_addr, uint8_t *buf, uint32_t reg_len, uint32_t value_len,
        uint8_t restart_after_write, uint32_t transfer_id, HAL_I2C_TRANSFER_HANDLER_T handler);
// New I2C task mode APIs
uint32_t hal_i2c_task_send(enum HAL_I2C_ID_T id, uint16_t device_addr, const uint8_t *tx_buf, uint16_t tx_len,
        uint32_t transfer_id, HAL_I2C_TRANSFER_HANDLER_T handler);
uint32_t hal_i2c_task_recv(enum HAL_I2C_ID_T id, uint16_t device_addr, const uint8_t *tx_buf, uint16_t tx_len,
        uint8_t *rx_buf, uint16_t rx_len,
        uint32_t transfer_id, HAL_I2C_TRANSFER_HANDLER_T handler);
uint32_t hal_i2c_task_msend(enum HAL_I2C_ID_T id, uint16_t device_addr, const uint8_t *tx_buf, uint16_t tx_item_len,
        uint16_t item_cnt, uint32_t transfer_id, HAL_I2C_TRANSFER_HANDLER_T handler);
uint32_t hal_i2c_task_mrecv(enum HAL_I2C_ID_T id, uint16_t device_addr, const uint8_t *tx_buf, uint16_t tx_item_len,
        uint8_t *rx_buf, uint16_t rx_item_len, uint16_t item_cnt,
        uint32_t transfer_id, HAL_I2C_TRANSFER_HANDLER_T handler);
/* for master task mode end */

/* for slave and simple master mode */
typedef void (*HAL_I2C_INT_HANDLER_T)(enum HAL_I2C_ID_T id, enum HAL_I2C_INT_STATUS_T status, uint32_t errocode);
uint32_t hal_i2c_slv_write(enum HAL_I2C_ID_T id, uint8_t *buf, uint32_t buf_len, uint32_t *act_len);
uint32_t hal_i2c_slv_read(enum HAL_I2C_ID_T id, uint8_t *buf, uint32_t buf_len, uint32_t *act_len);
uint32_t hal_i2c_set_interrupt_handler(enum HAL_I2C_ID_T id, HAL_I2C_INT_HANDLER_T handler);
uint32_t hal_i2c_mst_write(enum HAL_I2C_ID_T id, uint32_t device_addr, const uint8_t *buf, uint32_t buf_len, uint32_t *act_len, uint32_t restart, uint32_t stop, uint32_t yield);
uint32_t hal_i2c_mst_read(enum HAL_I2C_ID_T id, uint32_t device_addr, uint8_t *buf, uint32_t buf_len, uint32_t *act_len, uint32_t restart, uint32_t stop, uint32_t yield);
// New I2C simple mode APIs
uint32_t hal_i2c_simple_send(enum HAL_I2C_ID_T id, uint16_t device_addr, const uint8_t *tx_buf, uint16_t tx_len);
uint32_t hal_i2c_simple_recv(enum HAL_I2C_ID_T id, uint16_t device_addr, const uint8_t *tx_buf, uint16_t tx_len, uint8_t *rx_buf, uint16_t rx_len);
/* for slave and simple master mode end */

/* sensor engine mode */
uint32_t hal_i2c_sensor_engine_start(enum HAL_I2C_ID_T id, const struct HAL_I2C_SENSOR_ENGINE_CONFIG_T *cfg);
uint32_t hal_i2c_sensor_engine_stop(enum HAL_I2C_ID_T id);
/* sensor engine end */

int hal_gpio_i2c_open(const struct HAL_GPIO_I2C_CONFIG_T *cfg);
int hal_gpio_i2c_close(void);
uint32_t hal_gpio_i2c_send(uint32_t device_addr, const uint8_t *buf, uint32_t reg_len, uint32_t value_len);
uint32_t hal_gpio_i2c_recv(uint32_t device_addr, uint8_t *buf, uint32_t reg_len, uint32_t value_len, uint8_t restart_after_write);
// New GPIO I2C APIs
uint32_t hal_gpio_i2c_simple_send(uint32_t device_addr, const uint8_t *tx_buf, uint16_t tx_len);
uint32_t hal_gpio_i2c_simple_recv(uint32_t device_addr, const uint8_t *tx_buf, uint16_t tx_len, uint8_t *rx_buf, uint16_t rx_len);

#ifdef __cplusplus
}
#endif

#endif /* __HAL_I2C_H__ */

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#ifndef _BT_HAL_HCIT_H_
#define _BT_HAL_HCIT_H_

typedef void (*TG_BT_HAL_HCIT_CB)(void);

/**
 * @brief hci transport initial function, open uart port
 * @param[in]: None.
 * @return: None
 */
void tg_bt_hal_hcit_init(void);

/**
 * @brief hci transport de-initial function, close uart port
 * @param[in]: None.
 * @return: None
 */
void tg_bt_hal_hcit_deinit(void);

/**
 * @brief hci transport set uart baudrate
 * @param[in] baudRate:  uart baudrate, eg. 115200
 * @return: None
 */
void tg_bt_hal_hcit_set_baudrate(uint32_t baudRate);

/**
 * @brief hci transport set uart flow enable or disable
 * @param[in] flow:  1 means flow enable, 0 means flow disable.
 * @return: None
 */
void tg_bt_hal_hcit_set_flow(uint32_t flow);

/**
 * @brief hci transport set rx receive data indcation callback
 * @param[in] ind:  callback function pointer when uart received data.
 * @return: None
 */
void tg_bt_hal_hcit_set_rx_ind_callback(TG_BT_HAL_HCIT_CB ind);

/**
 * @brief hci transport uart tx
 * @param[in] p_buf:  data pointer write to uart.
 * @param[in] len:  length want write to uart.
 * @param[in] tx_cb: tx callback when uart tx completed or failed
 * @return: tx length
 */
int32_t tg_bt_hal_hcit_tx(uint8_t *p_buf, uint16_t len, TG_BT_HAL_HCIT_CB tx_cb);

/**
 * @brief hci transport uart rx
 * @param[in] p_buf:  data pointer read from uart.
 * @param[in] len:  length want read from uart.
 * @return: rx length
 */
int32_t tg_bt_hal_hcit_rx(uint8_t *p_buf, uint16_t len);

#endif

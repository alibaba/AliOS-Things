/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#pragma once

#ifndef MICO_PREBUILT_LIBS
#include "platform_peripheral.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/
#ifndef UART_RX_FIFO_SIZE
#define UART_RX_FIFO_SIZE (3000)
#endif
/******************************************************
 *                    Constants
 ******************************************************/
/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    MICO_BT_PIN_POWER,
    MICO_BT_PIN_RESET,
    MICO_BT_PIN_HOST_WAKE,
    MICO_BT_PIN_DEVICE_WAKE,
    MICO_BT_PIN_MAX,
} mico_bt_control_pin_t;

typedef enum
{
    MICO_BT_PIN_UART_TX,
    MICO_BT_PIN_UART_RX,
    MICO_BT_PIN_UART_CTS,
    MICO_BT_PIN_UART_RTS,
} mico_bt_uart_pin_t;

typedef enum
{
    PATCHRAM_DOWNLOAD_MODE_NO_MINIDRV_CMD,
    PATCHRAM_DOWNLOAD_MODE_MINIDRV_CMD,
} mico_bt_patchram_download_mode_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct
{
    uint32_t                          patchram_download_baud_rate;
    mico_bt_patchram_download_mode_t  patchram_download_mode;
    uint32_t                          featured_baud_rate;
} platform_bluetooth_config_t;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

extern const platform_bluetooth_config_t    mico_bt_config;

#ifndef MICO_PREBUILT_LIBS
/* Variables to be defined by the Bluetooth supporting platform */
extern const platform_gpio_t*               mico_bt_control_pins[];
extern const platform_gpio_t*               mico_bt_uart_pins[];
extern const platform_uart_t*               mico_bt_uart_peripheral;
extern       platform_uart_driver_t*        mico_bt_uart_driver;
extern const platform_uart_config_t         mico_bt_uart_config;
#endif

/******************************************************
 *               Function Declarations
 ******************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif

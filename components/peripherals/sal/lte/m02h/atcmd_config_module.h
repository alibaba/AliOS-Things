/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef _ATCMD_CONFIG_MODULE
#define _ATCMD_CONFIG_MODULE

#include "aos/hal/uart.h"

/* prefix postfix delimiter */
#define AT_RECV_PREFIX "\r\n"
#define AT_RECV_SUCCESS_POSTFIX "OK\r\n"
#define AT_RECV_FAIL_POSTFIX "ERROR\r\n"
#define AT_SEND_DELIMITER "\r"

/* send wait prompt default '>' */
#define AT_SEND_DATA_WAIT_PROMPT 1

/* uart config  */
#define AT_UART_BAUDRATE 115200
#define AT_UART_DATA_WIDTH DATA_WIDTH_8BIT
#define AT_UART_PARITY NO_PARITY
#define AT_UART_STOP_BITS STOP_BITS_1
#define AT_UART_FLOW_CONTROL FLOW_CONTROL_DISABLED
#define AT_UART_MODE MODE_TX_RX

typedef struct {
   uart_dev_t            uart_dev;
} sal_device_config_t;

#endif

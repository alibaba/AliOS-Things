/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _ATCMD_CONFIG_MODULE
#define _ATCMD_CONFIG_MODULE

#include "aos/hal/uart.h"

/* uart config */
#define AT_UART_BAUDRATE     115200
#define AT_UART_DATA_WIDTH   DATA_WIDTH_8BIT
#define AT_UART_PARITY       NO_PARITY
#define AT_UART_STOP_BITS    STOP_BITS_1
#define AT_UART_FLOW_CONTROL FLOW_CONTROL_DISABLED
#define AT_UART_MODE         MODE_TX_RX

/* AT worker config */
#define AT_WORKER_STACK_SIZE 4096
#define AT_WORKER_PRIORITY   (AOS_DEFAULT_APP_PRI - 2)
#endif

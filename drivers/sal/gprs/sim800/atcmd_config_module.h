/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _ATCMD_CONFIG_MODULE
#define _ATCMD_CONFIG_MODULE

#include "aos/hal/uart.h"

/**
 * AT related platform-dependent things are here, including:
 *   1. AT command;
 *   2. AT response code;
 *   3. AT delimiter;
 *   4. AT event;
 *   5. Uart port used by AT;
 *   6. ...
 */

// AT command
#define AT_CMD_ENET_SEND "AT+ENETRAWSEND"
#define AT_CMD_ENTER_ENET_MODE "AT+ENETRAWMODE=ON"
#define AT_CMD_EHCO_OFF "AT+UARTE=OFF"
#define AT_CMD_TEST "AT"

// Delimiter
#define AT_RECV_PREFIX "\r\n"
#define AT_RECV_SUCCESS_POSTFIX "OK\r\n"
#define AT_RECV_FAIL_POSTFIX "ERROR\r\n"
#define AT_SEND_DELIMITER "\r"

// send wait prompt default '>'
#define AT_SEND_DATA_WAIT_PROMPT 1

// AT event
#define AT_EVENT_ENET_DATA "+ENETEVENT:"

// uart config
#define AT_UART_BAUDRATE 115200
#define AT_UART_DATA_WIDTH DATA_WIDTH_8BIT
#define AT_UART_PARITY NO_PARITY
#define AT_UART_STOP_BITS STOP_BITS_1
#define AT_UART_FLOW_CONTROL FLOW_CONTROL_DISABLED
#define AT_UART_MODE MODE_TX_RX
#endif

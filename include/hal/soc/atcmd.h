/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _ATCMD_H_
#define _ATCMD_H_

/**
 * AT related platform-dependent definituons are here, including:
 *   1. AT command;
 *   2. AT response code;
 *   3. AT delimiter;
 *   4. AT event;
 *   5. Uart port used by AT;
 *   6. ...
 */

/**
 * You will need to define below things in your platform header,
 * which is to be included by this file.
 *
 * -------------------------------------------------------------
 * AT commands (mk3060 as example):
 *   #define AT_CMD_ENET_SEND "AT+ENETRAWSEND"
 *   #define AT_CMD_ENTER_ENET_MODE "AT+ENETRAWMODE=ON"
 *   #define AT_CMD_TEST "AT"
 *   #define AT_CMD_CONNECT_AP "AT+WJAP"
 *   #define AT_CMD_OBTAIN_MAC "AT+WMAC?"
 *   #define AT_CMD_OBTAIN_IP "AT+WJAPIP?"
 *   #define AT_CMD_EHCO_OFF "AT+UARTE=OFF"
 *
 * AT response:
 *   #define AT_RSP_SUCCESS "OK"
 *   #define AT_RSP_FAIL "ERROR"
 *
 * Delimiter:
 *   #define AT_RECV_DELIMITER "\r\n"
 *   #define AT_SEND_DELIMITER "\r"
 *
 * AT events:
 *   #define AT_EVENT_GOT_IP "+WEVENT:STATION_UP\r\n"
 *
 * AT uart:
 *   #define AT_UART_PORT 1
 *   #define AT_UART_LINUX_DEV "/dev/ttyUSB1"
 * -------------------------------------------------------------
 */

#ifdef AOS_ATCMD
#include <atcmd_config_platform.h>
#include <atcmd_config_module.h>
#endif

#endif // #ifndef _ATCMD_H_

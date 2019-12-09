/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/**
 * This file is auto generated code, for easy configuration of the 
 * SAL device interface. Please **DO** modify the value for the C macro 
 * whereever necessary, according to your platform case. For example,
 * modify the value of AT_UART_BAUDRATE if you are not using 115200;
 * or modify the value of AT_RECV_PREFIX if you are not using "\r\n" as
 * the prefix of receiving your AT content. 
 *
 * Please **DO NOT** modify the name of the C macroes.
 */

#ifndef _ATCMD_CONFIG_MODULE
#define _ATCMD_CONFIG_MODULE

#include "aos/hal/uart.h"

/* Subject to change - prefix, postfix, delimiter */
#define AT_RECV_PREFIX "\r\n"
#define AT_RECV_SUCCESS_POSTFIX "OK\r\n"
#define AT_RECV_FAIL_POSTFIX "ERROR\r\n"
#define AT_SEND_DELIMITER "\r"

/* Subject to change - send wait prompt default '>' */
#define AT_SEND_DATA_WAIT_PROMPT 0

typedef struct {
   uart_dev_t            uart_dev;
} sal_device_config_t;

#endif

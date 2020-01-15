/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef _ATCMD_CONFIG_MODULE
#define _ATCMD_CONFIG_MODULE

#include "aos/hal/uart.h"

/* send wait prompt default '>' */
#define AT_SEND_DATA_WAIT_PROMPT 1

typedef struct {
   uart_dev_t            uart_dev;
} mal_device_config_t;

#endif

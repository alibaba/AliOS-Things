/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _ATCMD_CONFIG_PLATFORM_H_
#define _ATCMD_CONFIG_PLATFORM_H_

#ifdef CENTRALIZE_MAPPING
#include "board.h"

// AT uart
#define AT_UART_PORT PORT_UART_AT
#else
#define AT_UART_PORT 1
#endif

#endif

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _ATCMD_CONFIG_PLATFORM_H_
#define _ATCMD_CONFIG_PLATFORM_H_

extern int get_devloperkit_atuart(void);

// AT uart
#define AT_UART_PORT get_devloperkit_atuart()

#endif

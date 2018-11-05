/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "hal/hal.h"
#include "k_config.h"
#include "board.h"
#if defined(DEV_SAL_WILC1000)
#include <lwip/tcpip.h>
#endif

#if defined(DEV_SAL_WILC1000)
struct netif lwip_netif;

void same5x_wilc1000_init(void)
{
    /*ethernet interface init*/
    tcpip_init(NULL, NULL);
}
#endif
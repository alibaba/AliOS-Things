/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include "type.h"
#include "clk.h"
#include "uart.h"
#include "gpio.h"
#include "cache.h"
#include "delay.h"
#include "watchdog.h"

#include "aos/hal/uart.h"

extern void SysTickCfg(void);
extern unsigned int Image$$RW_XMEM$$ZI$$Base;

uart_dev_t uart_0;

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    SwUartSend((unsigned char *)data, 1);
    return 0;
}

void board_init(void)
{
    ClkPorRcToDpll(0);
    CacheInit();
    ClkModuleEn(ALL_MODULE_CLK_SWITCH);
    ClkModuleGateEn(ALL_MODULE_CLK_GATE_SWITCH);

    //Disable Watchdog
    WaitMs(200);
    WdgDis();

    memset(&Image$$RW_XMEM$$ZI$$Base, 0, 0x20020000 - ((uint32_t)&Image$$RW_XMEM$$ZI$$Base));

    GpioFuartRxIoConfig(1);
    GpioFuartTxIoConfig(1);

    FuartInit(115200,8,0,1);

    SysTickCfg();
}

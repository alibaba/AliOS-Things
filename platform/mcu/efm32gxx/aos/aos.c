/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <k_api.h>
#include <aos/log.h>

#include "aos/hal/timer.h"
#include "network/hal/base.h"
#include <aos/aos.h>

#include "board.h"

#include "port_mcu.h"

extern uart_dev_t uart_0;

void platform_init(void);

void hal_reboot(void)
{
  NVIC_SystemReset();
}

kinit_t kinit;

int main(void)
{
    uint32_t core_frequency = 0;
    platform_init();

    aos_init();
    hal_uart_init(&uart_0);

    core_frequency = SystemCoreClockGet();

    SysTick_Config(core_frequency / RHINO_CONFIG_TICKS_PER_SECOND);

    aos_components_init(&kinit);
#ifndef AOS_BINS
    application_start(kinit.argc, kinit.argv);  /* jump to app/example entry */
#endif

    while(1);
}

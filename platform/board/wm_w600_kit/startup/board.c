/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "k_api.h"
#include "aos/kernel.h"

#include "wm_regs.h"
#include "wm_io.h"
#include "wm_gpio.h"
#include "wm_params.h"
#include "wm_netif2.0.3.h"

void hal_reboot(void)
{
    tls_sys_reset();  
}

void board_init(void)
{
    tls_sys_clk_set(0);

}

void OS_CPU_SysTickHandler(void)
{
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();   
}

void SystemInit()
{

}


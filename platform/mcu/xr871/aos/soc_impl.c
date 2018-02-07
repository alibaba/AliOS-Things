/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos.h>
#include <k_api.h>
#include <k_soc.h>
#include <driver/chip/hal_chip.h>

void soc_driver_init()
{
    hal_init();

    printf("soc_driver_init done\n");
}

void soc_system_init()
{
    printf("%s %d\n", __func__, __LINE__);
    platform_init();
}

void soc_systick_init()
{
    /* enable system tick */
    extern uint32_t SystemCoreClock;
    SysTick_Config(SystemCoreClock/RHINO_CONFIG_TICKS_PER_SECOND);
}

void SysTick_Handler(void)
{
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
}

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx.h"

#define AOS_START_STACK 2048

#define WIFI_PRODUCT_INFO_SIZE                      ES_WIFI_MAX_SSID_NAME_SIZE

ktask_t *g_aos_init;

static kinit_t kinit;

extern int application_start(int argc, char **argv);
extern int aos_framework_init(void);
extern void board_init(void);


static void hal_init()
{
    board_init();
}

static void var_init()
{
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
}

extern void hw_start_hal(void);

static void sys_init(void)
{
    
    init_architecture();
    init_platform();

#ifdef BOOTLOADER
    main();
#else
    hal_init();
    hw_start_hal();

    var_init();
    aos_kernel_init(&kinit);
#endif
}


static void sys_start(void)
{
    aos_init();
    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)sys_init, 1);
    aos_start();
}

#if defined (__CC_ARM) /* Keil / armcc */
int main(void)
{
    sys_start();
    return 0;
}
#else

extern uint32_t g_pfnVectors[];

void entry_main(void)
{
    SCB->VTOR = (unsigned long) &g_pfnVectors;
    /* Enable CPU Cycle counting */
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    init_clocks();
    sys_start();
}
#endif


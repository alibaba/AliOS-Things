/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include "aos/init.h"
#include <k_api.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include "project.h"

#define AOS_START_STACK 4096

//#define WIFI_PRODUCT_INFO_SIZE                      ES_WIFI_MAX_SSID_NAME_SIZE

ktask_t *g_aos_init;

aos_task_t task;

static kinit_t kinit;

extern int application_start(int argc, char **argv);
extern void board_init(void);
extern int default_UART_Init(void);

static void var_init()
{
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
}

extern void hw_start_hal(void);

void SysTick_IRQ(void)
{
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
}

static void sys_init(void)
{
    Cy_SysTick_Init(CY_SYSTICK_CLOCK_SOURCE_CLK_CPU, CYDEV_CLK_HFCLK0__HZ/RHINO_CONFIG_TICKS_PER_SECOND);
    Cy_SysTick_SetCallback(0, SysTick_IRQ);

    default_UART_Init();

#ifdef BOOTLOADER
    main();
#else
    hw_start_hal();
    board_init();
    var_init();
    aos_components_init(&kinit);
#ifndef AOS_BINS
    application_start(kinit.argc, kinit.argv);  /* jump to app/example entry */
#endif
#endif
}


static void sys_start(void)
{
    aos_init();
    //aos_task_new_ext(&task, "aos-init", sys_init, 0, AOS_START_STACK, AOS_DEFAULT_APP_PRI);
    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)sys_init, 1);

    aos_start();
}

#if defined (__CC_ARM) || defined(__ICCARM__) /* Keil / armcc */
int main(void)
{
    __enable_irq(); /* Enable global interrupts. */

    sys_start();
    return 0;
}
#else
void entry_main(void)
{
    sys_start();
}
#endif


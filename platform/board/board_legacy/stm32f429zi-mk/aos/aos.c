/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/init.h"
#include "aos/kernel.h"

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#define AOS_START_STACK 1536

static ktask_t demo_task_obj;
cpu_stack_t demo_task_buf[AOS_START_STACK];


ktask_t *g_aos_init;

static kinit_t kinit;

extern int application_start(int argc, char **argv);
extern void board_init(void);

static void var_init()
{
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
}

extern void hw_start_hal(void);

#include "aos/hal/uart.h"
#include "hal_uart_stm32f4.h"
#include "board.h"

static void sys_init(void)
{
    stm32_soc_init();

    stm32_peripheral_init();
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
    aos_heap_set();

    aos_init();
    //krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)sys_init, 1);
    krhino_task_create(&demo_task_obj, "aos-init", 0,AOS_DEFAULT_APP_PRI,
        0, demo_task_buf, AOS_START_STACK, (task_entry_t)sys_init, 1);

    aos_start();
}

int main(void)
{
    sys_start();
    return 0;
}



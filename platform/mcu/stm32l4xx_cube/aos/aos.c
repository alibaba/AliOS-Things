/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/init.h"
#include "aos/kernel.h"

#include <k_api.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef TINY_ENGINE
#define AOS_START_STACK 1536 + 1024*14
#else
#define AOS_START_STACK 1536
#endif

static ktask_t demo_task_obj;
cpu_stack_t demo_task_buf[AOS_START_STACK];


ktask_t *g_aos_init;

static kinit_t kinit;

extern int application_start(int argc, char **argv);
extern void board_init(void);
#ifdef AOS_COMP_CPLUSPLUS
extern void cpp_init(void);
#endif

static void var_init()
{
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
}

extern void hw_start_hal(void);

#include "aos/hal/uart.h"
#include "hal/hal_uart_stm32l4.h"
#include "board.h"
#if defined (AOS_OTA_RECOVERY_TYPE)
#include "rec_clear_ota_flag.h"
#endif

static void sys_init(void)
{
    stm32_soc_init();
    stm32_soc_peripheral_init();
#ifdef BOOTLOADER
    main();
#else
    hw_start_hal();
    board_init();
    var_init();
#ifdef AOS_COMP_CPLUSPLUS
    cpp_init();
#endif
#if defined (AOS_OTA_RECOVERY_TYPE)
    sys_clear_ota_flag();
#endif
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



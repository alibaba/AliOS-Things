/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include "hal/soc/uart.h"
#include "board.h"

extern int application_start(int argc, char **argv);
extern int aos_framework_init(void);
extern void board_init(void);
extern void hw_start_hal(void);

static void sys_init(void)
{
    kinit_t kinit;

    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
    stm32_soc_peripheral_init();
#ifdef BOOTLOADER
    main();
#else
    hw_start_hal();
    board_init();
    aos_kernel_init(&kinit);
#endif
}


static void sys_start(void)
{
    aos_heap_set();
    
    stm32_soc_init();
    
    aos_init();

    aos_task_new("aos-init", sys_init, NULL,1024 * 4);
    
    aos_start();
}

int main(void)
{
    sys_start();
    return 0;
}



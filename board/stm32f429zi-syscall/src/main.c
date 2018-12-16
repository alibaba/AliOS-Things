/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <stdlib.h>

#include "aos/hal/uart.h"
#include "hal_uart_stm32f4.h"
#include "board.h"

#define AOS_START_STACK 1536

static ktask_t demo_task_obj;
cpu_stack_t demo_task_buf[AOS_START_STACK];

extern int application_start(int argc, char **argv);

extern void board_init(void);

extern void hw_start_hal(void);


static void sys_init(void)
{
    stm32_soc_init();

    //hw_start_hal();

    aos_run_app();
}


int main(void)
{
    aos_heap_set();

    aos_init();

    krhino_task_create(&demo_task_obj, "aos-init", 0,AOS_DEFAULT_APP_PRI,
                        0, demo_task_buf, AOS_START_STACK,
                        (task_entry_t)sys_init, 1);

    aos_start();

    return 0;
}

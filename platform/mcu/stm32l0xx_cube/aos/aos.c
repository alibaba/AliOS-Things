/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
/**
  ******************************************************************************
  * @file    aos.c
  * @author  MCU China FAE team
  * @version 1.0
  * @date    23/11/2018
  * @brief   aos porting layer
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  ******************************************************************************
  */

#include <aos/aos.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include "board.h"

#ifdef TINY_ENGINE
#define AOS_START_STACK 1536 + 1024*14
#else
#define AOS_START_STACK 256
#endif

static ktask_t demo_task_obj;
cpu_stack_t demo_task_buf[AOS_START_STACK];

extern int application_start(int argc, char **argv);
extern int aos_framework_init(void);
extern void board_init(void);

extern void hw_start_hal(void);
extern void     aos_heap_set(void);


static void sys_init(void)
{

    stm32_soc_init();
#ifdef BOOTLOADER
    main();
#else
    board_init();
    application_start(0,NULL);
#endif
}


static void sys_start(void)
{
    aos_heap_set();
    krhino_init();
    //krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)sys_init, 1);
    krhino_task_create(&demo_task_obj, "aos-init", 0,2, 
        0, demo_task_buf, AOS_START_STACK, (task_entry_t)sys_init, 1);
    
    krhino_start();
}

int main(void)
{
    sys_start();
    return 0;
}




/**
  ******************************************************************************
  * @file    startup.c
  * @author  MCU China FAE team
  * @version 1.0
  * @date    05/01/2019
  * @brief   aos porting layer
  ******************************************************************************
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  ******************************************************************************
  */
#include "aos/init.h"
#include "board.h"
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

/*
main task stask size(byte)
*/
#define OS_MAIN_TASK_STACK 1024
#define OS_MAIN_TASK_PRI 32

/*  For user config
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
*/
static kinit_t kinit = {0, NULL, 1};
static ktask_t *g_main_task;

extern void board_init(void);

#ifndef AOS_BINS
extern int application_start(int argc, char **argv);
#endif

void trace_start(void)
{
    printf("trace config close!!!\r\n");
}

static void sys_init(void)
{
    /* user code start*/
    stm32_soc_peripheral_init();
    /*insert driver to enable irq for example: starting to run tick time.
     drivers to trigger irq is forbidden before aos_start, which will start core schedule.
    */
    /*user_trigger_irq();*/  //for example

    /*aos components init including middleware and protocol and so on !*/
    /*If only run kernel, not use aos_components_init*/
    aos_components_init(&kinit);

    #ifndef AOS_BINS
    application_start(kinit.argc, kinit.argv);  /* jump to app/example entry */
    #endif
}

int main(void)
{
    /*irq initialized is approved here.But irq triggering is forbidden, which will enter CPU scheduling.
    Put them in sys_init which will be called after aos_start.
    Irq for task schedule should be enabled here, such as PendSV for cortex-M4.
    */
    board_init();   //including aos_heap_set();  flash_partition_init();

    /*kernel init, malloc can use after this!*/
    krhino_init();

    /*main task to run */
    krhino_task_dyn_create(&g_main_task, "main_task", 0, OS_MAIN_TASK_PRI, 0, OS_MAIN_TASK_STACK, (task_entry_t)sys_init, 1);

    /*kernel start schedule!*/
    krhino_start();

    /*never run here*/
    return 0;
}

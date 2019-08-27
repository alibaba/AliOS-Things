/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include "aos/init.h"
#include "board.h"
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "lega_cm4.h"
#include "systick_delay.h"
/*
main task stask size(byte)
*/
#define OS_MAIN_TASK_STACK (3072/4)
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

static void sys_init(void)
{
    board_after_init();

    /*aos components init including middleware and protocol and so on !*/
    aos_components_init(&kinit);

    #ifndef AOS_BINS
    application_start(kinit.argc, kinit.argv);  /* jump to app/example entry */
    #endif
}

#define ALWAYS_ON_REGFILE           0x40000A00
#define REG_AHB_BUS_CTRL            *((volatile uint32_t *)(ALWAYS_ON_REGFILE + 0x90))

void HCLK_SW_IRQHandler(void)
{
    SYS_CRM_CLR_HCLK_REC = 0x1;
}

void delay_nop(unsigned int dly)
{
    volatile unsigned int i;
    for(i=dly; i>0; i--)
    {
    }
}

void ahb_sync_brid_open(void)
{
    unsigned int is_using_sync_down = (REG_AHB_BUS_CTRL & (0x1<<1));
    if(!is_using_sync_down)
    {
        REG_AHB_BUS_CTRL |= (0x1<<1); //0x40000A90 bit1 sw_use_hsync

        __enable_irq();
        NVIC_EnableIRQ(24);
        __asm volatile("DSB");
        __asm volatile("WFI");
        __asm volatile("ISB");

        delay_nop(50);
    }
}

int main(void)
{
    ahb_sync_brid_open();
    lega_flash_alg_cache_flush();

    board_init();
    /*kernel init, malloc can use after this!*/
    krhino_init();

    /*main task to run */
    krhino_task_dyn_create(&g_main_task, "main_task", 0, OS_MAIN_TASK_PRI, 0, OS_MAIN_TASK_STACK, (task_entry_t)sys_init, 1);

    /*kernel start schedule!*/
    krhino_start();

    /*never run here*/
    return 0;
}
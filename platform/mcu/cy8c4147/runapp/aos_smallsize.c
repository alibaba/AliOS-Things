/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include "project.h"
#include <k_api.h>
#include <stdio.h>
#include <aos\aos.h>
#include <uart_port.h>

#define AOS_START_STACK 1024

ktask_t *g_aos_init;
aos_task_t task;
static kinit_t kinit;

void board_init(void);
int default_UART_Init(void);

void PendSV_Handler(void);
int aos_kv_init(void);

static void var_init()
{
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
}

void SysTick_IRQ(void)
{
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();	
}

static void sys_init(void)
{
    default_UART_Init();
	
    board_init();
    var_init();
    aos_kv_init(); 
    
    while(1)
    {
        UART_API(_UartPutString("hello\r\n"));
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    }
}

static void sys_start(void)
{
    kstat_t stat;
    
    aos_init();
    
    CySysTickInit();
    CySysTickSetReload(CYDEV_BCLK__SYSCLK__HZ/RHINO_CONFIG_TICKS_PER_SECOND);
    CyIntSetSysVector(CY_INT_PEND_SV_IRQN, PendSV_Handler);
    CySysTickEnable();
    CySysTickSetCallback(0, SysTick_IRQ);
         
    stat = krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)sys_init, 1);
    if(stat != RHINO_SUCCESS)
    {
        return;
    }
       
    aos_start();
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
	  
    sys_start();
    return 0;
}


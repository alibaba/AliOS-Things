/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include <stdio.h>
#include <k_api.h>
#include "uart_port.h"

#define DEMO_TASK_STACKSIZE    256 //256*cpu_stack_t = 1024byte
#define DEMO_TASK_PRIORITY     20

static ktask_t demo_task_obj;
cpu_stack_t demo_task_buf[DEMO_TASK_STACKSIZE];

void demo_task(void *arg)
{
    (void) arg;
    
    int count = 0;
    default_UART_Init();
    
    printf("demo_task here!\n");
    
    while (1)
    {
        printf("hello world! count %d\n", count++);
        LED10_Write(~LED10_Read());
        //sleep 1 second
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    };
}

void PendSV_Handler(void);

void SysTick_IRQ(void)
{
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();	
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    for(;;)
    {
        /* Place your application code here. */
        krhino_init();
        krhino_task_create(&demo_task_obj, "demo_task", 0,DEMO_TASK_PRIORITY, 
        50, demo_task_buf, DEMO_TASK_STACKSIZE, demo_task, 1);
        
        CySysTickInit();
        CySysTickSetReload(CYDEV_BCLK__SYSCLK__HZ/RHINO_CONFIG_TICKS_PER_SECOND);
        CyIntSetSysVector(CY_INT_PEND_SV_IRQN, PendSV_Handler);
        CySysTickEnable();
        CySysTickSetCallback(0, SysTick_IRQ);
        
        krhino_start();
    }
}

/* [] END OF FILE */

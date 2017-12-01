/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

#pragma arm
void *cpu_task_stack_init(cpu_stack_t *base, size_t size, void *arg, task_entry_t entry)
{
    cpu_stack_t *stk;
    uint32_t temp = (uint32_t)(base + size);

    /*stack need 8 bytes align*/
    temp &= 0xfffffff8;
    stk = (cpu_stack_t *)temp;

    *(--stk)  = (uint32_t)entry;                /* Entry Point   */
    *(--stk)  = (uint32_t)krhino_task_deathbed; /* LR            */
    *(--stk)  = (uint32_t)0;                    /* R12           */
    *(--stk)  = (uint32_t)0;                    /* R11           */
    *(--stk)  = (uint32_t)0;                    /* R10           */
    *(--stk)  = (uint32_t)0;                    /* R9            */
    *(--stk)  = (uint32_t)0;                    /* R8            */
    *(--stk)  = (uint32_t)0;                    /* R7 :          */
    *(--stk)  = (uint32_t)0;                    /* R6            */
    *(--stk)  = (uint32_t)0;                    /* R5            */
    *(--stk)  = (uint32_t)0;                    /* R4            */
    *(--stk)  = (uint32_t)0;                    /* R3            */
    *(--stk)  = (uint32_t)0;                    /* R2            */
    *(--stk)  = (uint32_t)0;                    /* R1            */
    *(--stk)  = (uint32_t)arg;                  /* R0 argument   */
    *(--stk)  = (uint32_t)0x13;                 /* CPSR SVC mode */

    return stk;
}

void fiq_pre_proc(void)
{
    g_intrpt_nested_level[cpu_cur_get()]++;
}

void fiq_end_proc(void)
{
    g_intrpt_nested_level[cpu_cur_get()]--;
}

/*-----------------------------------------------------------*/

uint32_t platform_is_in_irq_enable( void )
{
#define ARM968_IF_MASK      0xC0
#define ARM968_IRQ_ENABLE   0x80

    uint32_t interrupt;

    __asm volatile(
        "MRS interrupt,CPSR\n"
        "AND interrupt,interrupt,#0xC0\n"
    );

    return (!(interrupt & ARM968_IRQ_ENABLE));
}
/*-----------------------------------------------------------*/

uint32_t platform_is_in_fiq_enable( void )
{
#define ARM968_IF_MASK      0xC0
#define ARM968_FIQ_ENABLE   0x40

    uint32_t interrupt;

    __asm volatile(
        "MRS interrupt,CPSR\n"
        "AND interrupt,interrupt,#0xC0\n"
    );

    return (!(interrupt & ARM968_FIQ_ENABLE));
}

/*-----------------------------------------------------------*/

uint32_t platform_is_in_irq_context( void )
{
#define ARM968_IRQ_MODE      0x12

    uint32_t mode;

    __asm volatile(
        "MRS mode,CPSR\n"
        "AND mode,mode,#0x1f\n"
    );

    return (ARM968_IRQ_MODE == mode);
}
/*-----------------------------------------------------------*/

uint32_t platform_is_in_fiq_context( void )
{
#define ARM968_FIQ_MODE      0x11

    uint32_t mode;

    __asm volatile(
        "MRS mode,CPSR\n"
        "AND mode,mode,#0x1f\n"
    );

    return (ARM968_FIQ_MODE == mode);
}


uint32_t platform_is_in_interrupt_context( void )
{
    return ((platform_is_in_fiq_context())
            || (platform_is_in_irq_context()));
}

/*-----------------------------------------------------------*/
int portDISABLE_FIQ(void)
{
    unsigned long temp;
    unsigned long mask;

    __asm volatile(
        "mrs	mask, cpsr \n"
        "orr	temp, mask, #0x40\n"
        "msr	cpsr_c, temp"
    );

    return (!!(mask & 0x40));
}

int portDISABLE_IRQ(void)
{
    unsigned long temp;
    unsigned long mask;

    __asm volatile(
        "mrs	mask, cpsr \n"
        "orr	temp, mask, #0x80\n"
        "msr	cpsr_c, temp"
    );

    return (!!(mask & 0x80));
}

void portENABLE_IRQ(void)
{
    unsigned long temp;
    __asm volatile(
        "mrs	temp, cpsr \n"
        "bic	temp, temp, #0x80\n"
        "msr	cpsr_c, temp"
    );
}

void portENABLE_FIQ(void)
{
    unsigned long temp;
    __asm volatile(
        "mrs	temp, cpsr \n"
        "bic	temp, temp, #0x40\n"
        "msr	cpsr_c, temp"
    );
}

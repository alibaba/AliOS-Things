/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <k_api.h>

#define RISCV_MSTATUS_MIE               (1<<3)        /*machine-level interrupt bit*/
#define RISCV_MSTATUS_MPIE              (1<<7)        /*machine-level pre interrupt bit*/
#define RISCV_MSTATUS_MPP               (0x3<<11)     /*machine-level MPP bit*/

#define RISCV_MSTATUS_MPP_MPIE          (RISCV_MSTATUS_MPIE | RISCV_MSTATUS_MPP)

volatile uint8_t g_task_switch_required = 0;

/* The global pointer defined in linker script. */
extern uint32_t __global_pointer;

void *cpu_task_stack_init(cpu_stack_t *stack_base, size_t stack_size, void *arg,
                          task_entry_t entry)
{
    cpu_stack_t *stk;
    uint32_t     temp = (uint32_t)(stack_base + stack_size);

    /* stack aligned by 16 byte */
    temp &= 0xfffffff0;
    stk = (cpu_stack_t *)temp;

    /*
        x9 - x8
        x31 - x18
        x1          (ra)
        x3          (gp)
        x4          (tp)
        x5 - x7     (t0 - t2)
        x10         (a0)
        x17 - x11
        lpcount[0-1], lpend[0-1], lpstart[0-1]
        mepc
        mstatus
     */

    *(stk - 16) = (cpu_stack_t)arg;                  /* Reg R0 : argument */
    *(stk - 24) = (cpu_stack_t)(&__global_pointer);  /* gp */
    *(stk - 21) = (cpu_stack_t)krhino_task_deathbed; /* ra */
    *(stk - 2) = (cpu_stack_t)entry;                 /* mepc */
    *(stk - 1) = (cpu_stack_t)(RISCV_MSTATUS_MPIE | RISCV_MSTATUS_MPP); /* mstatus SET MIE and MPP */

    stk -= 38;

    return stk;
}

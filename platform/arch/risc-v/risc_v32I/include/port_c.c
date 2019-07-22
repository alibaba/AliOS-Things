/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "typedef.h"

extern int fpu_reg_count(void);

void *cpu_task_stack_init(cpu_stack_t *stack_base, size_t stack_size, void *arg,
                          task_entry_t entry)
{
    cpu_stack_t *stk;
    cpu_stack_t  task_addr;
    uint32_t     temp = (uint32_t)(stack_base + stack_size);

    /* stack aligned by 16 byte */
    temp &= 0xfffffff0;
    stk = (cpu_stack_t *)temp;

    /*
        mstatus
        epc
        x1          (ra)
        x3          (gp)
        x4          (tp)
        x5-7        (t0-2)
        x8          (s0/fp)
        x9          (s1)
        x10-11      (a0-1)
        x12-17      (a2-7)
        x18-27      (s2-11)
        x28-31      (t3-6)
    */

    *(stk - 22) = (cpu_stack_t)arg;                  /* Reg R0 : argument */
    *(stk - 30) = (cpu_stack_t)krhino_task_deathbed; /* ra */
    *(stk - 31) = (cpu_stack_t)entry;                /* epc */
    *(stk - 32) = (cpu_stack_t)(RISCV_MSTATUS_MPIE | RISCV_MSTATUS_MPP); /* mstatus SET MIE and MPP */

    stk -= 32;

    return stk;
}

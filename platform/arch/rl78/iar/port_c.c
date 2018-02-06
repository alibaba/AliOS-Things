/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

void *cpu_task_stack_init(cpu_stack_t *stack_base, size_t stack_size,
                          void *arg, task_entry_t entry)
{
    cpu_stack_t *stk;
    stk   = (cpu_stack_t *)(stack_base + stack_size);
   *--stk = (cpu_stack_t)((uint32_t)arg >> 16);
   *--stk = (cpu_stack_t)arg;
   *--stk = (cpu_stack_t)(((uint32_t)entry >> 16) & 0x000F) | 0x8600;
   *--stk = (cpu_stack_t)entry;
   *--stk = 0x1100;
   *--stk = 0x3322;
   *--stk = 0x5544;
   *--stk = 0x7766;
   *--stk = 0x0F00;

    return stk;
}


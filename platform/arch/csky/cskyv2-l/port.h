/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef PORT_H
#define PORT_H

#include <k_types.h>
#include <k_task.h>

size_t cpu_intrpt_save(void);
void   cpu_intrpt_restore(size_t psr);
void   cpu_intrpt_switch(void);
void   cpu_task_switch(void);
void   cpu_first_task_start(void);

void  *cpu_task_stack_init(cpu_stack_t *base, size_t size,
                           void *arg, task_entry_t entry);
RHINO_INLINE uint8_t cpu_cur_get(void)
{
    return 0;
}
#define CPSR_ALLOC() size_t psr

#define RHINO_CPU_INTRPT_DISABLE() { psr = cpu_intrpt_save(); }
#define RHINO_CPU_INTRPT_ENABLE()  { cpu_intrpt_restore(psr); }

#endif /* PORT_H */


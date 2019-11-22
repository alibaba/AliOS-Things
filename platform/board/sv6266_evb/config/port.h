/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef PORT_H
#define PORT_H

#include "mcu_def.h"
//extern size_t cpu_intrpt_save(void);
//extern void   cpu_intrpt_restore(size_t cpsr);
void   cpu_intrpt_switch(void);
void   cpu_task_switch(void);
void   cpu_first_task_start(void);
void  *cpu_task_stack_init(cpu_stack_t *base, size_t size, void *arg, task_entry_t entry);

RHINO_INLINE uint8_t cpu_cur_get(void)
{
    return 0;
}

#define CPSR_ALLOC() size_t psw

#define RHINO_CPU_INTRPT_DISABLE()	do { \
									psw=cpu_intrpt_save(); \
									} while(0)
#define RHINO_CPU_INTRPT_ENABLE()	do { \
									cpu_intrpt_restore(psw); \
									} while(0)
#endif /* PORT_H */


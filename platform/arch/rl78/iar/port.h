/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef PORT_H
#define PORT_H
#include  <intrinsics.h>


void     cpu_intrpt_switch(void);
void     cpu_first_task_start(void);
void    *cpu_task_stack_init(cpu_stack_t *base, size_t size, void *arg, task_entry_t entry);

RHINO_INLINE uint8_t cpu_cur_get(void)
{
    return 0;
}

#define cpu_task_switch()  __break() 

#define CPSR_ALLOC() uint32_t cpsr

#define RHINO_CPU_INTRPT_DISABLE() do { cpsr = __get_interrupt_state(); \
                                    __disable_interrupt();            } while (0)
#define RHINO_CPU_INTRPT_ENABLE()   do { __set_interrupt_state(cpsr);    } while (0)

#endif /* PORT_H */


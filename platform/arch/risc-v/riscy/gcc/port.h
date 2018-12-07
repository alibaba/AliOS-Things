/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef PORT_H
#define PORT_H

/* Is task switch required or not. */
extern volatile uint8_t g_task_switch_required;

size_t cpu_intrpt_save(void);
void   cpu_intrpt_restore(size_t cpsr);
void   cpu_task_switch(void);
void   cpu_first_task_start(void);
void * cpu_task_stack_init(cpu_stack_t *base, size_t size, void *arg,
                           task_entry_t entry);

RHINO_INLINE uint8_t cpu_cur_get(void)
{
    return 0;
}

RHINO_INLINE void cpu_intrpt_switch(void)
{
    g_task_switch_required = 1;
}

#define CPSR_ALLOC() size_t cpsr

#define RHINO_CPU_INTRPT_DISABLE() \
    {                              \
        cpsr = cpu_intrpt_save();  \
    }
#define RHINO_CPU_INTRPT_ENABLE() \
    {                             \
        cpu_intrpt_restore(cpsr); \
    }

#endif /* PORT_H */

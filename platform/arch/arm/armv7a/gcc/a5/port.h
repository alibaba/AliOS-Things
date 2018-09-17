/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef PORT_H
#define PORT_H

#include "k_vector.h"
#include "k_cache.h"
#include "k_mmu_sd.h"

size_t cpu_intrpt_save(void);
void   cpu_intrpt_restore(size_t cpsr);
void   cpu_intrpt_switch(void);
void   cpu_task_switch(void);
void   cpu_first_task_start(void);
void  *cpu_task_stack_init(cpu_stack_t *base, size_t size, void *arg, task_entry_t entry);

RHINO_INLINE uint8_t cpu_cur_get(void)
{
    return 0;
}

/*  Cortex-A5 processor: The cache line length is eight words.  */
#define RHINO_CACHE_LINE_SIZE       32

#define CPSR_ALLOC()                size_t cpsr
#define RHINO_CPU_INTRPT_DISABLE()  do{ cpsr = cpu_intrpt_save(); }while(0)
#define RHINO_CPU_INTRPT_ENABLE()   do{ cpu_intrpt_restore(cpsr); }while(0)

#endif /* PORT_H */


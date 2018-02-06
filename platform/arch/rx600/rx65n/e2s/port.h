/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef PORT_H
#define PORT_H
#include "k_types.h"
#include "k_api.h"
#include <stddef.h>
#include <machine.h>

/*size_t cpu_intrpt_save(void);
void   cpu_intrpt_restore(size_t cpsr);*/
void   cpu_intrpt_switch(void);
void   cpu_task_switch(void);
void   cpu_first_task_start(void);
void  *cpu_task_stack_init(cpu_stack_t *base, size_t size, void *arg, task_entry_t entry);

RHINO_INLINE uint8_t cpu_cur_get(void)
{
    return 0;
}

#define CPSR_ALLOC()   uint8_t cpsr
#define  IPL_BOUNDARY  12u

#ifdef   IPL_BOUNDARY
#define  RHINO_CPU_INTRPT_DISABLE()     do { cpsr = get_ipl(); \
                                             set_ipl(IPL_BOUNDARY); } while (0)
#else
#define  RHINO_CPU_INTRPT_DISABLE()     do { cpsr = get_ipl(); \
                                             set_ipl(12); } while (0)
#endif
#define  RHINO_CPU_INTRPT_ENABLE()      do { set_ipl(cpsr);} while (0)


#endif


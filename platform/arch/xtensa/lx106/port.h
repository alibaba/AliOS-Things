/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef CPU_H
#define CPU_H
#include <xtensa/tie/xt_core.h>
#include <xtensa/hal.h>
#include <xtensa/config/core.h>
#include <xtensa/config/system.h>       /* required for XSHAL_CLIB */
#include <xtensa/xtruntime.h>

typedef void (* _xt_isr)(void *arg);

typedef struct __xt_isr_entry {
	_xt_isr	handler;
    void *	arg;
} _xt_isr_entry;

size_t cpu_intrpt_save(void);
void   cpu_intrpt_restore(size_t cpsr);
void   cpu_intrpt_switch(void);
void   cpu_task_switch(void);
void   cpu_first_task_start(void);
void  *cpu_task_stack_init(cpu_stack_t *base, size_t size, void *arg, task_entry_t entry);

#define CPSR_ALLOC() size_t cpsr

#define RHINO_CPU_INTRPT_DISABLE() { cpsr = XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL); }
#define RHINO_CPU_INTRPT_ENABLE()  { XTOS_RESTORE_JUST_INTLEVEL(cpsr); }

RHINO_INLINE uint8_t cpu_cur_get(void)
{
    return 0;
}

void _xt_isr_attach(uint8_t i, _xt_isr func, void *arg);

void ResetCcountVal(uint32_t cnt_val);

uint32_t xthal_get_ccount(void);

#endif /* CPU_H */


/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "mcu_def.h"
#include "soc_defs.h"
#include "sys/intc.h"
#include "sys/systick.h"

extern void systick_isr();
void   cpu_first_task_start(void)
{
	/* enable SWI for context switch */
	intc_irq_set_priority(IRQ_SWI, LOWEST);
	intc_irq_enable(IRQ_SWI);

	/* enable systick */
	intc_register_isr(IRQ_UTIMER_0, systick_isr);
	intc_irq_set_priority(IRQ_UTIMER_0, HIGHEST);
	intc_irq_enable(IRQ_UTIMER_0);

	/* start  timer */
	systick_set_period((1000*1000)/RHINO_CONFIG_TICKS_PER_SECOND);
	systick_start();

	/* start first task */
	__asm__ __volatile__
	(
		"b\t CtxRestore"
		:
		:
		:
	);
}


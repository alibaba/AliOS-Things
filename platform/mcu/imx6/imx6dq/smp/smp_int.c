/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include <string.h>
#include "gic.h"
#include "interrupt.h"


#define OS_SMP_CROSSCORE_INTNO        SW_INTERRUPT_3

extern volatile uint64_t g_cpu_flag;
extern void register_interrupt_routine(uint32_t irq_id, irq_hdlr_t isr);

void  os_crosscore_isr(void *arg) {
    //krhino_intrpt_enter();

    // do nothing here, only intended to invoke krhino_intrpt_exit to switch task

    //krhino_intrpt_exit();
}

/*Initialize the crosscore interrupt on this core. Call this once
on each active core.*/
void os_crosscore_int_init() {
    register_interrupt_routine(OS_SMP_CROSSCORE_INTNO, os_crosscore_isr);
}


void cpu_signal(uint8_t cpu_num)
{
     if(g_cpu_flag & (1UL << cpu_num))
     {
         gic_send_sgi(OS_SMP_CROSSCORE_INTNO, 1UL << cpu_num, kGicSgiFilter_UseTargetList);
     }
}



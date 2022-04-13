/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include <string.h>
#include "k_api.h"
#include "mt_irq_define.h"
#include "mt_gic_v3.h"

extern volatile uint64_t g_cpu_flag;

void  os_crosscore_sched(int n, void *arg)
{
    __asm volatile ("dsb sy":::"memory");
}
void  os_crosscore_freeze(int n, void *arg) {
    while(1);
}

/*Initialize the crosscore interrupt on this core. Call this once
on each active core.*/
void os_crosscore_int_init() {
#if 0 //ask ali
    irq_request(OS_SMP_CROSSCORE_INTNO, os_crosscore_sched, NULL);
    irq_enable(OS_SMP_CROSSCORE_INTNO);
    irq_request(OS_SMP_FREEZE_INTNO, os_crosscore_freeze, NULL);
    irq_enable(OS_SMP_FREEZE_INTNO);
#endif
     request_irq(OS_SMP_CROSSCORE_INTNO, os_crosscore_sched, NULL, "cross cpu sched", NULL);
     request_irq(OS_SMP_FREEZE_INTNO, os_crosscore_freeze,  NULL, "cross cpu freeze", NULL);
#if (RHINO_CONFIG_FIQ_AS_NMI > 0)
    irq_set_nmi(OS_SMP_FREEZE_INTNO);
#endif
}

void cpu_signal(uint8_t cpu_num)
{
    if (g_cpu_flag & (1UL << cpu_num)) {
        gic_send_sgi(0, cpu_num, OS_SMP_CROSSCORE_INTNO);
    }
}

void cpu_freeze_others()
{
    uint8_t cpu_num;

    for ( cpu_num = 0 ; cpu_num < RHINO_CONFIG_CPU_NUM ; cpu_num++ )
    {
        if((g_cpu_flag & (1UL << cpu_num)) && cpu_num != cpu_cur_get())
        {
#if 0 //ask for ali
            gic_send_sgi(OS_SMP_FREEZE_INTNO, 1UL << cpu_num, 0);
#endif
        }
    }
}



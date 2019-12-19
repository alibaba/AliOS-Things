/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include "soc_defs.h"

#if (RHINO_CONFIG_HW_COUNT > 0)
void soc_hw_timer_init(void)
{
}

hr_timer_t soc_hr_hw_cnt_get(void)
{
    return 0;
    //return *(volatile uint64_t *)0xc0000120;
}

lr_timer_t soc_lr_hw_cnt_get(void)
{
    return 0;
}
#endif /* RHINO_CONFIG_HW_COUNT */

#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
void soc_intrpt_stack_ovf_check(void)
{
}
#endif

#if (RHINO_CONFIG_DYNTICKLESS > 0)
void soc_tick_interrupt_set(tick_t next_ticks,tick_t elapsed_ticks)
{
}

tick_t soc_elapsed_ticks_get(void)
{
    return 0;
}
#endif

extern uint32_t _eflash;
extern uint32_t ilm_remain;
extern uint32_t _end;
extern uint32_t dlm_remain;
k_mm_region_t g_mm_region[] = {{(uint8_t*)&_eflash,(size_t)&ilm_remain},{(uint8_t*)&_end,(size_t)&dlm_remain}};
int           g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);

size_t soc_get_cur_sp()
{
	return __nds32__get_current_sp();
}

void soc_err_proc(kstat_t err)
{
	(void)err;
}

void soc_asserted( const char *func, int line )
{
	printf("<!!!OS Assert!!!> func = %s, line=%d\n", func, line);
	__nds32__gie_dis();
	while(1);
}

void trace_start(void)
{
    printf("trace config close!!!\r\n");
}

krhino_err_proc_t g_err_proc = soc_err_proc;


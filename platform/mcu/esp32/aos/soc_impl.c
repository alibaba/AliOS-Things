/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

extern void ets_printf(char *format, ...);
extern void hal_reboot(void);

size_t soc_get_cur_sp()
{
    volatile size_t dummy = (size_t)&dummy;
    return dummy;
}

#if (RHINO_CONFIG_HW_COUNT > 0)
hr_timer_t soc_hr_hw_cnt_get(void)
{
    return 0;
}
#endif

#define SYS_DYN_POOL_SIZE_1 0x18000

extern uint8_t _heap_start, _heap_size;
const k_mm_region_t g_mm_region[] = {
    {(uint8_t*)0x3ffe8000, SYS_DYN_POOL_SIZE_1},
    {&_heap_start, (uint32_t)&_heap_size},
};
int g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);

void soc_err_proc(kstat_t err)
{
    printf("kernel panic,err %d!\n",err);
    hal_reboot();
}

krhino_err_proc_t g_err_proc = soc_err_proc;

/////////////////////////////////////////////////////////////////
//stub

void espaos_task_get_snaps_shot_all()
{
    ets_printf("call funciont %s\n", __FUNCTION__);
}

void espos_yield_from_isr()
{

}

int heap_caps_add_region_with_caps()
{
    return 0;
}

void sc_ack_send()
{
    ets_printf("call stub funciont %s\n", __FUNCTION__);
}

void sc_ack_send_stop()
{
    ets_printf("call stub funciont %s\n", __FUNCTION__);
}


int uECC_secp256r1()
{
    return 0;
}

int uECC_verify()
{
    return 0;
}

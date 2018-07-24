/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <iodefine.h>
#include <rx_platform.h>
uint8_t HeapStart[0x25000];
k_mm_region_t g_mm_region[] = {
{
   (uint8_t *)HeapStart, sizeof(HeapStart)},
};

int g_region_num = sizeof(g_mm_region)/sizeof(k_mm_region_t);

void soc_err_proc(kstat_t err)
{
   (void)err;
    while(1);
}

krhino_err_proc_t g_err_proc = soc_err_proc;

void hal_reboot(void)
{
	R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);

	SYSTEM.SWRR = 0XA501;
	while (1)
	{

	}
}


#if (RHINO_CONFIG_TASK_STACK_CUR_CHECK > 0)
size_t soc_get_cur_sp()
{
    size_t sp = 0;
#if defined (__GNUC__)&&!defined(__CC_ARM)
    asm volatile(
        "mov %0,sp\n"
        :"=r"(sp));
#endif
    return sp;
}
static void soc_print_stack()
{
    void    *cur, *end;
    int      i=0;
    int     *p;

    end   = krhino_cur_task_get()->task_stack_base + krhino_cur_task_get()->stack_size;
    cur = (void *)soc_get_cur_sp();
    p = (int*)cur;
    while(p < (int*)end) {
        if(i%4==0) {
            printf("\r\n%08x:",(uint32_t)p);
        }
        printf("%08x ", *p);
        i++;
        p++;
    }
    printf("\r\n");
    return;
}
#endif

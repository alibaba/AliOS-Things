/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include <k_api.h>
#include "aos/init.h"
#include <aos/kernel.h>
#include <stdio.h>
#include <stdlib.h>

#define AOS_START_STACK 768
extern void hal_init(void);

ktask_t *g_aos_init;
krhino_err_proc_t g_err_proc = soc_err_proc;
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
#define HEAP_BUFFER_SIZE 1024*48
uint8_t g_heap_buf[HEAP_BUFFER_SIZE];
k_mm_region_t g_mm_region[] = {{g_heap_buf, HEAP_BUFFER_SIZE}};
int           g_region_num  = sizeof(g_mm_region) / sizeof(k_mm_region_t);

uint32_t __heap_start = (uint32_t)g_heap_buf;
uint32_t __heap_end = (uint32_t)(g_heap_buf + HEAP_BUFFER_SIZE);

void soc_err_proc(kstat_t err)
{
    printf("kernel panic,err %d!\n", err);
}


static kinit_t kinit;

void board_cli_init(void)
{
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
}
void sys_init_func(void)
{
    //test_case_task_start();
    hal_init();
    board_cli_init();
    aos_components_init(&kinit);
#ifndef AOS_BINS
    application_start(kinit.argc, kinit.argv);  /* jump to app/example entry */
#endif
}

int main(void)
{
    printf("alios start\n");
    aos_init();
    (void)krhino_task_dyn_create(&g_aos_init, "aos-init", 0, 6, 0, AOS_START_STACK, (task_entry_t)sys_init_func, 1);

    aos_start();
    return 0;
}


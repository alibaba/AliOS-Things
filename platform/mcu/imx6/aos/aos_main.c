/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos_main.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <aos/init.h>

extern void time_init_global_tick(void);
extern void platform_init(void);

#define AOS_START_STACK 2048
ktask_t *g_aos_init;

static kinit_t kinit = {
    .argc = 0,
    .argv = NULL,
    .cli_enable = 1
};

#if (RHINO_CONFIG_CPU_NUM > 1)
extern void os_load_slavecpu(void);
extern void smp_cpu_init(void);

static ktask_t *g_cpu1_task1;
static ktask_t *g_cpu2_task1;

static void cpu1_task1_main(void)
{
    int cnt = 0;
    int cpu;

    while (1) {
        krhino_task_sleep(201);
        cpu = cpu_cur_get();
        printf("cpu %d cnt 0x%x\r\n", cpu, cnt);
        cnt++;

    }
}

static void cpu2_task1_main(void)
{
    int cnt = 0;
    int cpu;

    while (1) {
        krhino_task_sleep(200);
        cpu = cpu_cur_get();
        printf("cpu %d cnt 0x%x\r\n", cpu, cnt);
        cnt++;

    }
}
#endif

unsigned int qq_tick;
static void sys_init(void)
{
    int ret = 0;

    hal_uart_init(NULL);

    printf("sys_init in\r\n");

	time_init_global_tick();

    aos_components_init(&kinit);

#if (RHINO_CONFIG_CPU_NUM > 1)
    ret = krhino_task_cpu_dyn_create(&g_cpu1_task1, "cpu1_task1", 0, 2, 0, AOS_START_STACK, cpu1_task1_main, 1, 1);
    if (ret == RHINO_SUCCESS) {
        printf("create cpu1_task1 success\r\n");
    }
    ret = krhino_task_cpu_dyn_create(&g_cpu2_task1, "cpu2_task1", 0, 2, 0, AOS_START_STACK, cpu2_task1_main, 2, 1);
    if (ret == RHINO_SUCCESS) {
        printf("create cpu1_task1 success\r\n");
    }
    os_load_slavecpu();
#endif

    application_start(0, NULL);
}

void sys_start(void)
{
    int ret;

	platform_init();

#if (RHINO_CONFIG_CPU_NUM > 1)
    smp_cpu_init();
#endif

    k_vectable_set();
	//isr_stk_init();
    aos_init();

#if (RHINO_CONFIG_CPU_NUM > 1)
    /*bind 0 core*/
    ret = krhino_task_cpu_dyn_create(&g_aos_init, "aos-init", 0, 2, 0, AOS_START_STACK, sys_init, 0,1);
#else
    ret = krhino_task_dyn_create(&g_aos_init, "aos-init", 0, 2, 0, AOS_START_STACK, sys_init, 1);
#endif

    if (RHINO_SUCCESS != ret) {
        printf("create aos_init task failed\r\n");
    }

    aos_start();
}


/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <aos/kernel.h>
#include <aos/init.h>

#define AOS_START_STACK 2048

extern void time_init_global_tick(void);
extern void platform_init(void);

extern int aos_run_app(void);

ktask_t *g_aos_init;

static void sys_init(void)
{
    int ret = 0;
    hal_uart_init(NULL);

    time_init_global_tick();

    aos_run_app();
}

void sys_start(void)
{
    int ret;

    platform_init();

    k_vectable_set();

    aos_init();

    ret = krhino_task_dyn_create(&g_aos_init,
                                 "aos-init",
                                 0, 2, 0,
                                 AOS_START_STACK,
                                 sys_init, 1);

    aos_start();
}


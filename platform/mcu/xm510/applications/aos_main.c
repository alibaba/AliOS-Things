/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <aos/init.h>
#include "aos_main.h"
#include <hal/wifi.h>

extern void os_hw_interrupt_init(void);
extern void test_certificate(void);
extern void cpp_init(void);

#define AOS_START_STACK (64 * 1024)
ktask_t *g_aos_init;

static kinit_t kinit = {
    .argc = 0,
    .argv = NULL,
    .cli_enable = 1
};

extern void xm_cli_init(void);
extern void xm_driver_init(void);

static void sys_init(void *arg)
{
    int ret = 0;

    xm_driver_init();
    xm_cli_init();

    //test_certificate();
    application_start(kinit.argc, kinit.argv);

    while(1) {
        aos_msleep(20000);
    }
}

void sys_start(void)
{
    mmu_init();

    k_vectable_set();

    os_hw_interrupt_init();

    os_hw_timer_init();

    aos_init();

    cpp_init();

    soc_driver_init();

    //aos_components_init(&kinit);
    tcpip_init(NULL, NULL);

    vfs_init();
#ifdef AOS_COMP_CLI
    aos_cli_init();
#endif

#ifdef AOS_COMP_ULOG
    ulog_init();
#endif
    dumpsys_cli_init();

#ifdef AOS_COMP_KV
    aos_kv_init();
#endif

#ifdef AOS_LOOP
    vfs_device_init();
    aos_loop_init();
#endif

    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, 50, 0, AOS_START_STACK, sys_init, 1);

    aos_start();
}

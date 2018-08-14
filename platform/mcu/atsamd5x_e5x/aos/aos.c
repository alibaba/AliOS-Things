/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <aos/kernel.h>
#include "atmel_start.h"
#include <k_api.h>
#include <hal/soc/soc.h>
#include <hal/wifi.h>

#define AOS_START_STACK 1024

ktask_t *g_aos_init;
static kinit_t kinit;

static void sys_init(void);
extern uart_dev_t uart_cli;

#if defined(DEV_SAL_MK3060)
extern hal_wifi_module_t aos_wifi_module_mk3060;
#endif

static void sys_init(void)
{
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;

    atmel_start_init();
    hal_uart_init(&uart_cli);

#if defined(DEV_SAL_MK3060)
    hal_wifi_register_module(&aos_wifi_module_mk3060);
    hal_wifi_init();
#endif

    board_init();
    aos_kernel_init(&kinit);

    application_start(0, NULL);	
}

int main(void)
{
    aos_heap_set();
    aos_init();

    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)sys_init, 1);
    aos_start();

    return 0;
}


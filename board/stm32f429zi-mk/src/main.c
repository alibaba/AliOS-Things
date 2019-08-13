/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include "aos/kernel.h"
#include "k_api.h"

#include "aos/hal/uart.h"
#include "hal_uart_stm32f4.h"
#include "board.h"

#include "aos/cli.h"
#include "aos/init.h"
#include "ulog/ulog.h"

#define AOS_START_STACK 1536

static ktask_t demo_task_obj;
cpu_stack_t demo_task_buf[AOS_START_STACK];

static kinit_t kinit;

extern int lwip_tcpip_init(void);
extern int application_start(int argc, char **argv);

int g_proc_var; /**< for test purpose */

static void var_init()
{
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
}

static void sys_init(void)
{
    stm32_soc_init();

    var_init();

    aos_components_init(&kinit);

    LOG("strart lwip_tcpip_init\r\n");

#if (ENABLE_LWIP_INIT > 0)
    lwip_tcpip_init();
#endif

    g_proc_var = 0x5a5a;
    LOG("kernel set g_proc_var to 0x%x\r\n", g_proc_var);

    application_start(0, NULL);
}


int main(void)
{
    aos_heap_set();

    aos_init();

    krhino_task_create(&demo_task_obj, "aos-init", 0,AOS_DEFAULT_APP_PRI,
                        0, demo_task_buf, AOS_START_STACK,
                        (task_entry_t)sys_init, 1);

    aos_start();

    return 0;
}

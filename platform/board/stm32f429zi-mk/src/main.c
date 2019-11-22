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
#include "cli/cli_api.h"

#define AOS_START_STACK 1536

static ktask_t demo_task_obj;
cpu_stack_t demo_task_buf[AOS_START_STACK];

static kinit_t kinit;

extern int lwip_tcpip_init(void);
extern int application_start(int argc, char **argv);

int g_proc_var; /**< for test purpose */


static void ifconfig_task_main(void *arg)
{
    LOG("strart lwip_tcpip_init\r\n");
    lwip_tcpip_init();
}

static void ifconfig(char *pbuffer, int outlen, int argc, char **argv)
{
    ktask_t *task;
    krhino_task_dyn_create(&task, "ifconfig-task", NULL, 20, 5, 256,
                           ifconfig_task_main, 1);
}

static void kernel_read_g_proc_var(char *pbuffer, int outlen, int argc, char **argv)
{
    printf("kernel read g_proc_var: %d\r\n", g_proc_var);
}

static void kernel_set_g_proc_var(char *pbuffer, int outlen, int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: kernel_set_g_proc_var val\r\n");
        return;
    }

    g_proc_var = atoi(argv[1]);

    printf("kernel set g_proc_var: %d\r\n", g_proc_var);
}

static struct cli_command_st kernel_cmds[] = {
    {"ifconfig", "start net interface",ifconfig},
    {"kernel_read_g_proc_var", "kernel read g_proc_var", kernel_read_g_proc_var},
    {"kernel_set_g_proc_var", "kernel set g_proc_var", kernel_set_g_proc_var},
};

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

    aos_msleep(1000);

    cli_register_commands(kernel_cmds, sizeof(kernel_cmds)/sizeof(kernel_cmds[0]));

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

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos_main.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <aos/init.h>

#define AOS_START_STACK 2048

ktask_t *g_aos_init;

extern void board_init(void);

static kinit_t kinit = {
    .argc = 0,
    .argv = NULL,
    .cli_enable = 1
};

static void sys_init(void)
{
    int i = 0;

    soc_system_init();

#ifdef BOOTLOADER
    main();
#else

    board_init();

    aos_components_init(&kinit);
#ifndef AOS_BINS
    application_start(kinit.argc, kinit.argv);  /* jump to app/example entry */
#endif

#endif
}

void sys_start(void)
{
    aos_init();

    soc_driver_init();

    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, sys_init, 1);

    aos_start();
}

#if defined (AOS_COMP_CLI)
/*use in panic*/

void panic_cli_board_config(void)
{
}
#endif

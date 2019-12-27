/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos_main.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <aos/init.h>
<<<<<<< HEAD
=======
#include <cli.h>

#include "ate_app.h"
#include "cmd_evm.h"
#include "cmd_rx_sensitivity.h"
#include "arm_arch.h"
>>>>>>> 9ce6b4409 (BugID:23266431:AOS code update for folder adjustment)

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

<<<<<<< HEAD
    aos_components_init(&kinit);
=======
#if ATE_APP_FUN
    if (get_ate_mode_state()) {
#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
        cpu_pwrmgmt_init();
#endif

#ifdef AOS_COMP_CLI
        cli_service_init(&kinit);
        aos_cli_register_commands(&cli_cmd_rftest[0], sizeof(cli_cmd_rftest) / sizeof(struct cli_command));
#endif

    } else
#endif
    {
        aos_components_init(&kinit);
>>>>>>> 89b29036d (BugID:18744724:unify the use of macro)
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

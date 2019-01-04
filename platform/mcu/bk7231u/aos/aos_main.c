/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos_main.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <aos/init.h>
#include <aos/aos.h>

#include "ate_app.h"
#include "cmd_evm.h"
#include "cmd_rx_sensitivity.h"

#define AOS_START_STACK 2048

ktask_t *g_aos_init;

extern void board_init(void);

static kinit_t kinit = {
    .argc = 0,
    .argv = NULL,
    .cli_enable = 1
};

#if ATE_APP_FUN 
static void tx_evm_cmd_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int ret = do_evm(NULL, 0, argc, argv);
    if(ret)
    {
        printf("tx_evm bad parameters\r\n");
    }
}

static void rx_sens_cmd_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int ret = do_rx_sensitivity(NULL, 0, argc, argv);
    if(ret)
    {
        printf("rx sensitivity bad parameters\r\n");
    }
}

static const struct cli_command cli_cmd_rftest[] = {
	{"txevm",       "txevm [-m] [-c] [-l] [-r] [-w]", tx_evm_cmd_test},
	{"rxsens",      "rxsens [-m] [-d] [-c] [-l]",    rx_sens_cmd_test},
};
#endif

static void sys_init(void)
{
    int i = 0;

    soc_system_init();

#ifdef BOOTLOADER
    main();
#else

    board_init();

#if (RHINO_CONFIG_CPU_PWR_MGMT > 0)
	cpu_pwrmgmt_init();
#endif

#if ATE_APP_FUN 
	if(get_ate_mode_state()) 
	{
    	#ifdef CONFIG_AOS_CLI
		cli_service_init(&kinit);
    	#endif
		
		aos_cli_register_commands(&cli_cmd_rftest[0],
			sizeof(cli_cmd_rftest) / sizeof(struct cli_command));
	} 
	else
#endif
	{
    	aos_kernel_init(&kinit);
    }

#endif
}

void sys_start(void)
{
    aos_init();

    soc_driver_init();

    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, sys_init, 1);

    aos_start();
}


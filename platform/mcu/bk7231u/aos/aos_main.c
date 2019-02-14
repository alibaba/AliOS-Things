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

static void efuse_cmd_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint8_t addr, data;
    
    if(argc == 3)
    {
        if (strncmp(argv[1], "-r", 2) == 0) {
            hexstr2bin(argv[2], &addr, 1);
            aos_cli_printf("efuse read: addr-0x%02x, data-0x%02x\r\n",
                        addr, wifi_read_efuse(addr));
        } 
    } 
    else if(argc == 4) 
    {
        if(strncmp(argv[1], "-w", 2) == 0)  {
            hexstr2bin(argv[2], &addr, 1);
            hexstr2bin(argv[3], &data, 6);
            aos_cli_printf("efuse write: addr-0x%02x, data-0x%02x, ret:%d\r\n",
                        addr, data, wifi_write_efuse(addr, data));
        }
    }
    else {
        printf("efuse [-r addr] [-w addr data]\r\n");
    }
}

static void efuse_mac_cmd_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint8_t mac[6];
    
    if (argc == 1)
    {
        if(wifi_get_mac_address_from_efuse(mac))
            aos_cli_printf("MAC address: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    else if(argc == 2)
    {
        if (strncmp(argv[1], "-r", 2) == 0) {
            if(wifi_get_mac_address_from_efuse(mac))
                aos_cli_printf("MAC address: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
                        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        } 
    } 
    else if(argc == 3) 
    {
        if(strncmp(argv[1], "-w", 2) == 0)  {
            hexstr2bin(argv[2], mac, 6);
            //if(wifi_set_mac_address_to_efuse(mac))
                aos_cli_printf("Set MAC address: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
                        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        }
    }
    else {
        printf("efusemac [-r] [-w] [mac]\r\n");
    }
}


static const struct cli_command cli_cmd_rftest[] = {
	{"txevm",       "txevm [-m] [-c] [-l] [-r] [-w]", tx_evm_cmd_test},
	{"rxsens",      "rxsens [-m] [-d] [-c] [-l]",    rx_sens_cmd_test},
	{"efuse",       "efuse [-r addr] [-w addr data]", efuse_cmd_test},
    {"efusemac",    "efusemac [-r] [-w] [mac]",       efuse_mac_cmd_test},
};
#endif

static void sys_init(void)
{
    int i = 0;

    soc_system_init();

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
}

void sys_start(void)
{
    aos_init();

    soc_driver_init();

    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, sys_init, 1);

    aos_start();
}


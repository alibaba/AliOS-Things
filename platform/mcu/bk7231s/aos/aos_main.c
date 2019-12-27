/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos_main.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <aos/init.h>
#include <cli.h>

#include "ate_app.h"
#include "cmd_evm.h"
#include "cmd_rx_sensitivity.h"
#include "arm_arch.h"

#define AOS_START_STACK 2048

ktask_t *g_aos_init;

extern void board_init(void);

static kinit_t kinit = {
    .argc = 0,
    .argv = NULL,
    .cli_enable = 1
};

#if ATE_APP_FUN 
#ifdef AOS_COMP_CLI
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

static void reg_write_read_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    UINT32 reg_addr, reg_value;
    if(strncmp(argv[1], "-r", 2) == 0) {
        if(argc != 3) {
            printf("bkreg -r addr\r\n");
            return;
        }

        hexstr2bin(argv[2], &reg_addr, 4);
        reg_addr = ntohl(reg_addr);
        aos_cli_printf("bkreg R: addr:0x%08x, value:0x%08x\r\n", reg_addr, REG_READ(reg_addr));
    }
    else if(strncmp(argv[1], "-w", 2) == 0) {
        if(argc != 4) {
            printf("bkreg -w addr value\r\n");
            return;
        }

        hexstr2bin(argv[2], &reg_addr, 4);
        reg_addr = ntohl(reg_addr);

        hexstr2bin(argv[3], &reg_value, 4);
        reg_value = ntohl(reg_value);

        REG_WRITE(reg_addr, reg_value);

        extern INT32 rwnx_cal_save_trx_rcbekn_reg_val(void);
        // when write trx and rc beken regs, updata registers save.
        if( (reg_addr & 0xfff0000) == 0x1050000)
            rwnx_cal_save_trx_rcbekn_reg_val();

        aos_cli_printf("bkreg W: addr:0x%08x, value:0x%08x - check:0x%08x\r\n",
            reg_addr, reg_value, REG_READ(reg_addr));
    }
    else {
        printf("bkreg -w/r addr [value]\r\n");
    }
}

static const struct cli_command cli_cmd_rftest[] = {
	{"txevm",       "txevm [-m] [-c] [-l] [-r] [-w]", tx_evm_cmd_test},
	{"rxsens",      "rxsens [-m] [-d] [-c] [-l]",    rx_sens_cmd_test},
	{"efuse",       "efuse [-r addr] [-w addr data]", efuse_cmd_test},
    {"efusemac",    "efusemac [-r] [-w] [mac]",       efuse_mac_cmd_test},
    {"bkreg",       "bkreg -w/r addr [value]",        reg_write_read_test},
};
#endif
#endif

static void sys_init(void)
{
    int i = 0;

    soc_system_init();

    board_init();

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
	cpu_pwrmgmt_init();
#endif

#if ATE_APP_FUN 
	if(get_ate_mode_state()) 
	{
        #ifdef AOS_COMP_CLI
		cli_service_init(&kinit);
		aos_cli_register_commands(&cli_cmd_rftest[0],
			sizeof(cli_cmd_rftest) / sizeof(struct cli_command));
    	#endif
	} 
	else
#endif
	{
        aos_components_init(&kinit);
#ifndef AOS_BINS
        application_start(kinit.argc, kinit.argv);  /* jump to app/example entry */
#endif
    }		
}

void sys_start(void)
{
    aos_init();

    soc_driver_init();

    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, sys_init, 1);

    aos_start();
}


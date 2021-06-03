/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include "uservice/uservice.h"
#include "uservice/eventid.h"

#include "ble_netconfig.h"

int ble_netcfg_callback(BLECFG_EVENT event, BLECFG_STATE state, void *event_data)
{
    return 0;
}

static void netcfg_comp_example(int argc, char **argv)
{
    int ret;

    ret = BleCfg_stack_init(ble_netcfg_callback);
    if (ret) {
        printf("error: BLE Net Config initial! ret %x\r\n", ret);
    }
    ret = BleCfg_stack_start();
    if (ret) {
        printf("error: BLE Net Config start! ret %x\r\n", ret);
    }
}

#if AOS_COMP_CLI
ALIOS_CLI_CMD_REGISTER(netcfg_comp_example, netcfg_example, netcfg component base example);
#endif

/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include "netmgr.h"
#include "u_api.h"
#include "ulog/ulog.h"
#include "aos/kernel.h"

#define STATE_NETWORK_CONNECTED  5

#if (RHINO_CONFIG_UCLI_TASK > 0)
#include "aos/cli.h"

static void help_cmd(char *buf, int len, int argc, char **argv)
{
    printf("==== %s CLI Cmd ====\r\n", RHINO_CONFIG_APP_NAME);
    cli_cmds_show();
}

static struct cli_command ucmds[] = {
    {"help", "print this", help_cmd},
};

#endif
extern int demo_main(int argc, char *argv[]);

void wait_connect()
{
    int ret;
    netmgr_hdl_t hdl = -1;
    printf("wait wifi connecting ...\r\n");

    do {
        ret = netmgr_service_init();
        if(ret == 0) {
            break;
        }
        printf("netmgr_service_init failed: %d\r\n", ret);
        aos_msleep(200);
    } while (ret < 0);

    do {
        hdl = netmgr_get_dev("/dev/wifi0");
        if(hdl >= 0) {
            break;
        }
        printf("netmgr_get_dev failed: %d\r\n", hdl);
        aos_msleep(200);
    } while (hdl < 0);

    do {
        aos_msleep(200);
        ret = netmgr_get_state(hdl);
    } while (ret != STATE_NETWORK_CONNECTED);

    netmgr_service_deinit();
    printf("wifi connected!\r\n");
}

int application_start(int argc, char *argv[])
{
    printf("LinkSDK demo start\r\n");


#if (RHINO_CONFIG_UCLI_TASK > 0)
    aos_cli_register_commands(ucmds, sizeof(ucmds) / sizeof(ucmds[0]));
#endif
    wait_connect();

    demo_main(argc, argv);
    return 0;
}


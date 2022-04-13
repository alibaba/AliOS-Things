/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "u_api.h"
#include "ulog/ulog.h"
#include "netmgr.h"
#include "netmgr_wifi.h"

#include <aos/kernel.h>
#include <uservice/uservice.h>
#include <uservice/eventid.h>
#include <netmgr_rpc.h>
#include "iperf_cli.h"
#include "ping_cli.h"

#ifdef AOS_COMP_DEVICEVFS
#include <drivers/u_ld.h>
#endif

static aos_hdl_t sem_finished;
extern void wifi_handle_cmd(int argc, char **argv);

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

lr_timer_t soc_lr_hw_cnt_get(void)
{
    return 0;
}

static void wifi_event_cb(uint32_t event_id, const void *param, void *context)
{
    if (event_id != EVENT_WIFI_DHCP_SUCCESS) {
        return;
    }
    aos_sem_signal(&sem_finished);
}

void wifi_config(void)
{
    int retn;

    event_service_init(NULL);
    netmgr_service_init(NULL);
    netmgr_rpc_service_start();
    event_subscribe(EVENT_WIFI_DHCP_SUCCESS, wifi_event_cb, NULL);

    netmgr_wifi_auto_reconnect(1);

    retn = aos_sem_wait(&sem_finished, 0);
    if (0 != retn) {
        printf("[%s]sem(0x%p) wait failed", __FUNCTION__, sem_finished.hdl);
        return;
    }

    printf("[%s]wifi config success!", __FUNCTION__);
}

extern int ulwip_service_init(void);
#ifdef AOS_BOARD_VEXPRESSA9_MK
extern void lwip_tcpip_init(void);
#endif
int application_start(int argc, char *argv[])
{
    printf("user space lwip start\r\n");

#if (RHINO_CONFIG_UCLI_TASK > 0)
    aos_cli_register_commands(ucmds, sizeof(ucmds) / sizeof(ucmds[0]));
#endif

    ulwip_service_init();

#if AOS_BOARD_HAAS100_MK > 0
    u_driver_entry("uspace");
    wifi_config();
#endif

#ifdef AOS_BOARD_VEXPRESSA9_MK
    printf("lwip_tcpip_init\r\n");
    lwip_tcpip_init();
#endif

#ifdef WITH_LWIP_PING
    ping_cli_register();
#endif
#ifdef WITH_LWIP_IPERF
    iperf_cli_register();
#endif

    while (1) {
        aos_msleep(100000);
    }
    netmgr_rpc_service_stop();
    return 0;
}

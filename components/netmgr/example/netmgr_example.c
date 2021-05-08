/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "netmgr.h"
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include <uservice/uservice.h>
#include <uservice/eventid.h>

static void wifi_event_cb(uint32_t event_id, const void *param, void *context)
{
    printf("wifi event cb\r\n");

    printf("Got IP\r\n");
}

static void netmgr_comp_example(int argc, char **argv)
{
    printf("netmgr test \r\n");

    event_service_init(NULL);

    netmgr_service_init(NULL);

    event_subscribe(EVENT_NETMGR_DHCP_SUCCESS, wifi_event_cb, NULL);
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(netmgr_comp_example, netmgr_example, netmgr component base example)
#endif

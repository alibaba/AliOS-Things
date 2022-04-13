/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
/**
 * @file main.c
 *
 * This file includes the entry code of link sdk related demo
 *
 */

#include <string.h>
#include <stdio.h>
#include <aos/kernel.h>
#include "ulog/ulog.h"
#include "netmgr.h"
#include <uservice/uservice.h>
#include <uservice/eventid.h>
#include "auto_app.h"

extern int demo_main(int argc, char *argv[]);

static int _ip_got_finished = 0;

static void entry_func(void *data) {
    demo_main(0 , NULL);
}
static void wifi_event_cb(uint32_t event_id, const void *param, void *context)
{
    if (event_id != EVENT_NETMGR_DHCP_SUCCESS) {
        return;
    }
    if(_ip_got_finished != 0) {
        return;
    }
    _ip_got_finished = 1;
    aos_task_new("link_dmeo", entry_func, NULL, 6*1024);
}

int application_start(int argc, char *argv[])
{
    aos_set_log_level(AOS_LL_DEBUG);
    event_service_init(NULL);

    netmgr_service_init(NULL);

    event_subscribe(EVENT_NETMGR_DHCP_SUCCESS, wifi_event_cb, NULL);
    auto_app_init();

    while(1) {
        aos_msleep(1000);
    };

    return 0;
}

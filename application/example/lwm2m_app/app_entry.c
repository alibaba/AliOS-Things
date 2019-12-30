/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/kernel.h>
#include <aos/yloop.h>
#include <netmgr.h>

static int lwm2m_client_started = 0;

extern int lwm2m_client(void);

static void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

    if(!lwm2m_client_started)
    {
       aos_task_new("lwm2m_client", (void (*)(void *))lwm2m_client, NULL, 1024 * 8);
       lwm2m_client_started = 1;
    }
}

int application_start(int argc, char *argv[])
{
    netmgr_init();

    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);

    netmgr_start(false);

    aos_loop_run();

    return 0;
}

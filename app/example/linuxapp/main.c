/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/aos.h>
#include <aos/network.h>
#include <strings.h>

#ifdef AOS_NETMGR
#include <netmgr.h>
#endif

struct cookie {
    int flag;
};

static void app_delayed_action(void *arg)
{
    struct cookie *cookie = arg;
    LOG("%s - %s", __func__, aos_task_name());
    if (cookie->flag != 0) {
        aos_post_delayed_action(10000, app_delayed_action, arg);
    }
    else {
        aos_schedule_call(app_delayed_action, arg);
    }
    cookie->flag ++;
}

static void handle_event(input_event_t *event, void *arg)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

    struct hostent *hent = gethostbyname("www.taobao.com");
    LOG("%s - %s", __func__, aos_task_name());
    if(hent) {
        LOG("%s - %s", __func__, hent->h_name);
    }

    aos_post_delayed_action(1000, app_delayed_action, arg);
}

int application_start(void)
{
    struct cookie *cookie = aos_malloc(sizeof(*cookie));
    bzero(cookie, sizeof(*cookie));

    aos_register_event_filter(EV_WIFI, handle_event, cookie);

#ifdef AOS_NETMGR
    netmgr_init();
    netmgr_start(true);
#endif

#ifdef CONFIG_CMD_BENCHMARKS
    extern void benchmark_cli_init();
    benchmark_cli_init();
#endif

    aos_loop_run();
    /* never return */

    return 0;
}


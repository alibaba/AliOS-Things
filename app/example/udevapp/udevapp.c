/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "aos/yloop.h"

#include "netmgr.h"

#ifdef CSP_LINUXHOST
#include <signal.h>
#endif /* CSP_LINUXHOST */

#include "udev.h"

/* using default pk & dn if not store in kv */
#define DEFAULT_PRODUCT_KEY     "a1MZxOdcBnO"
#define DEFAULT_DEVICE_NAME     "test_01"

static char udev_started = 0;

static void udev_app(void *unused)
{
    int pk_len = 0, dn_len = 0;
    char pk[16] = {0}, dn[16] = {0};
    pk_len = sizeof(pk);
    dn_len = sizeof(dn);

    LOGI("udevapp", "starting udev_init\n");

    if (aos_kv_get("productkey", pk, &pk_len) != 0 || aos_kv_get("devicename", dn, &dn_len) != 0){
        LOGI("udevapp", "found no pk&dn in kv, using default pk: "DEFAULT_PRODUCT_KEY", dn: "DEFAULT_DEVICE_NAME"\r\n");
        udev_init(DEFAULT_PRODUCT_KEY, DEFAULT_DEVICE_NAME);
        return;
    }

    LOGI("udevapp", "found pk&dn in kv, pk: %s, dn: %s\r\n", pk, dn);

    udev_init(pk, dn);

    return;
}

static void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

    if (!udev_started) {
        aos_task_new("udev_app", udev_app, NULL, 2048);
        udev_started = 1;
    }
}

int application_start(int argc, char **argv)
{
#ifdef CSP_LINUXHOST
    signal(SIGPIPE, SIG_IGN);
#endif /* CSP_LINUXHOST */

#ifdef WITH_SAL
    sal_add_dev(NULL, NULL);
    sal_init();
#endif /* WITH_SAL */

    aos_set_log_level(AOS_LL_DEBUG);

    netmgr_init();

    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);

#ifdef CSP_LINUXHOST
    aos_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0xdeaddead);
#endif /* CSP_LINUXHOST */

    netmgr_start(false);

    aos_loop_run();

    return 0;
}

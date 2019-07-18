/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <time.h>
#include <unistd.h>
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "aos/yloop.h"

#include "netmgr.h"
#include "qxwz_types.h"
#include "qxwz_sdk.h"

#define QXWZ_APPKEY          "770302"
#define QXWZ_APPSECRET       "99094b805092d2e7e08f63bcde97a2a7a70bded81a1e0b0f6728dede7fc01902"
#define QXWZ_DEVICE_ID       "AliOS-Things2.1.1"
#define QXWZ_DEVICE_TYPE     "uLocation_qianxun"

static qxwz_started = 0;

static const qxwz_usr_config_t s_config = {
    QXWZ_APPKEY,     /* appKey */
    QXWZ_APPSECRET,  /* appSecret */
    QXWZ_DEVICE_ID,  /* device_ID */
    QXWZ_DEVICE_TYPE /* device_Type */
};

static void wifi_service_event(input_event_t *event, void *priv_data)
{
    int ret;
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

    netmgr_ap_config_t config;
    memset(&config, 0, sizeof(netmgr_ap_config_t));
    netmgr_get_ap_config(&config);
    LOG("wifi_service_event config.ssid %s", config.ssid);
    if (strcmp(config.ssid, "adha") == 0 || strcmp(config.ssid, "aha") == 0) {
        return;
    }

    if (!qxwz_started){
        qxwz_started = 1;
        uloc_qianxun_service(&s_config);
    }
}


int application_start(int argc, char *argv[])
{
    printf("ulocation qianxun app!\n");

#ifdef WITH_SAL
    sal_add_dev(NULL, NULL);
    sal_init();
#endif

    netmgr_init();
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
    netmgr_start(0);

    aos_loop_run();
    return 0;
}

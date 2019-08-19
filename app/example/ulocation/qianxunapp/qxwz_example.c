/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <time.h>
#include <unistd.h>
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "aos/yloop.h"
#include "ulocation/ulocation.h"

#include "netmgr.h"

#define ULOCATION_QXWZ_APPKEY          "770302"
#define ULOCATION_QXWZ_APPSECRET       "99094b805092d2e7e08f63bcde97a2a7a70bded81a1e0b0f6728dede7fc01902"
#define ULOCATION_QXWZ_DEVICE_ID       "AliOS-Things2.1.2"
#define ULOCATION_QXWZ_DEVICE_TYPE     "uLocation_qianxun"

#define QXWZ_TASK_STACK_SIZE 4096
#define QXWZ_TASK_PRIO (AOS_DEFAULT_APP_PRI - 4)

aos_task_t g_qianxun_task;

static ulocation_gga_info_t location;

static const ulocation_qxwz_usr_config_t config = {
    ULOCATION_QXWZ_APPKEY,     /* appKey */
    ULOCATION_QXWZ_APPSECRET,  /* appSecret */
    ULOCATION_QXWZ_DEVICE_ID,  /* device_ID */
    ULOCATION_QXWZ_DEVICE_TYPE /* device_Type */
};

static int32_t qxwz_service_sample()
{
    return ulocation_qianxun_service(&config, &location);
}

static void wifi_service_event(input_event_t *event, void *priv_data)
{
    int ret;
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

    LOG("wifi_service_event!\n");

    ret = aos_task_new_ext(&g_qianxun_task, "qxwz_service_task", qxwz_service_sample,
                           NULL, QXWZ_TASK_STACK_SIZE, QXWZ_TASK_PRIO);
    if (ret != 0) {
        return;
    }
}


int application_start(int argc, char *argv[])
{
    printf("ulocation qianxun app!\n");

#ifdef WITH_SAL
    sal_add_dev(NULL, NULL);
    sal_init();
#endif

    aos_set_log_level(AOS_LL_DEBUG);
    netmgr_init();
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
    netmgr_start(0);

    aos_loop_run();
    return 0;
}

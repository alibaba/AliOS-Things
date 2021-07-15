/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <string.h>
#include <aos/kernel.h>
#include <aos/debug.h>

#include "wifi_provisioning.h"
#include "exports/iot_export_linkkit.h"
#include "sl_wifi_prov.h"

#define SL_EVT_REPORT_CLOUD         (1 << 0)
#define SL_EVT_NOT_REPORT_CLOUD     (1 << 1)

#define EVT_LINKKIT_QUIT            (1 << 0)

#define TAG "SL_PROV"

static int sl_dev_ap_start(wifi_prov_cb cb);
static int sl_smartconfig_start(wifi_prov_cb cb);
static void sl_dev_ap_stop();
static void sl_smartconfig_stop();

static wifi_prov_cb prov_cb;
static uint32_t current_method = 0;
static aos_event_t evt_report_to_cloud;
static aos_event_t evt_linkkit_quit;
static int linkkit_running = 0;
static int callback_running = 0;
static int8_t last_smartconfig_running = 0, last_dev_ap_running = 0;

static wifi_prov_t awss_dp_priv = {
    .name        = "sl_dev_ap",
    .start       = sl_dev_ap_start,
    .stop        = sl_dev_ap_stop,
};

static wifi_prov_t awss_sf_priv = {
    .name        = "sl_smartconfig",
    .start       = sl_smartconfig_start,
    .stop        = sl_smartconfig_stop,
};

static void linkkit_start(void *arg)
{
    int res = 0;
    int cnt = 0;
    iotx_linkkit_dev_meta_info_t master_meta_info;
    int domain_type = 0, dynamic_register = 0, post_reply_need = 0;
    int master_devid;
    int connect_cnt = 0;

    linkkit_running = 1;

    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    HAL_GetProductKey(master_meta_info.product_key);
    HAL_GetDeviceName(master_meta_info.device_name);
    HAL_GetProductSecret(master_meta_info.product_secret);
    HAL_GetDeviceSecret(master_meta_info.device_secret);

    IOT_SetLogLevel(IOT_LOG_INFO);

    domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    /* Choose Login Method */
    dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* post reply doesn't need */
    post_reply_need = 1;
    IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_reply_need);

    /* Create Master Device Resources */
    do {
        master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
        if (master_devid >= 0) {
            break;
        }
        printf("IOT_Linkkit_Open failed! retry after %d ms\n", 2000);
        aos_msleep(2000);
    } while (linkkit_running && ++connect_cnt < 3);

    if (connect_cnt == 3) {
        goto END;
    }

    connect_cnt = 0;
    /* Start Connect Aliyun Server */
    do {
        res = IOT_Linkkit_Connect(master_devid);
        if (res >= 0) {
            break;
        }
        printf("IOT_Linkkit_Connect failed! retry after %d ms\n", 5000);
        aos_msleep(5000);
    } while (linkkit_running && ++connect_cnt < 5);

    cnt = 0;
    while (linkkit_running && cnt++ < 100) {
        IOT_Linkkit_Yield(300);
    }

    IOT_Linkkit_Close(master_devid);

END:
    linkkit_running = 0;
    aos_event_set(&evt_linkkit_quit, EVT_LINKKIT_QUIT, AOS_EVENT_OR);
    return;
}

void sl_report_to_cloud(int report)
{
    if (!callback_running) {
        return;
    }

    if (report) {
        aos_event_set(&evt_report_to_cloud, SL_EVT_REPORT_CLOUD, AOS_EVENT_OR);

        aos_task_t tsk;
        aos_task_new_ext(&tsk, "linkkit", linkkit_start, NULL, 1024*8, AOS_DEFAULT_APP_PRI);
    } else {
        aos_event_set(&evt_report_to_cloud, SL_EVT_NOT_REPORT_CLOUD, AOS_EVENT_OR);
    }
}

void test_report_to_cloud(void) {
     aos_task_t tsk;
     
     aos_task_new_ext(&tsk, "linkkit", linkkit_start, NULL, 1024*8, AOS_DEFAULT_APP_PRI);
}
static int sl_prov_cb(int success, char *ssid, char *password, int timeout)
{
    if (current_method == 0) {
        return -1;
    }
    callback_running = 1;

    aos_event_set(&evt_report_to_cloud, 0, AOS_EVENT_AND);

    if (success) {
        wifi_prov_result_t res;
        strncpy(res.ssid, ssid, sizeof(res.ssid) - 1);
        strncpy(res.password, password, sizeof(res.password) - 1);

        prov_cb(current_method, WIFI_RPOV_EVENT_GOT_RESULT, &res);
    } else {
        prov_cb(current_method, WIFI_PROV_EVENT_TIMEOUT, NULL);
    }

    unsigned flags;
    aos_event_get(&evt_report_to_cloud, SL_EVT_REPORT_CLOUD | SL_EVT_NOT_REPORT_CLOUD, AOS_EVENT_OR_CLEAR, &flags, timeout);
    if (flags & SL_EVT_REPORT_CLOUD) {
        callback_running = 0;
        return 0;
    }

    callback_running = 0;
    return -1;
}

static void sl_prov_thread(void *args)
{
    wifi_prov_t *priv = (wifi_prov_t *)args;
    int ret;

    if (priv->method_id == awss_dp_priv.method_id) {    
        awss_dev_ap_start(sl_prov_cb, 300000);
        last_dev_ap_running = 1;
    } else if (priv->method_id == awss_sf_priv.method_id) {
        ret = awss_start(sl_prov_cb);
        last_smartconfig_running = 1;
    } else {
        LOGE(TAG, "wifi prov method id mismatch: %d", priv->method_id);
        current_method = 0;
    }
}

static void start_prepare()
{
    if (last_smartconfig_running) {
        awss_stop();
    }

    if (last_dev_ap_running) {
        awss_dev_ap_stop();
    }

    if (linkkit_running) {
        unsigned int flags;

        linkkit_running = 0;
        aos_event_get(&evt_linkkit_quit, EVT_LINKKIT_QUIT, AOS_EVENT_OR_CLEAR, &flags, AOS_WAIT_FOREVER);
    }
}

/**
    User starts the smartconfig process
*/
static int sl_smartconfig_start(wifi_prov_cb cb)
{
    start_prepare();

    int ret;
    awss_config_press();
    prov_cb = cb;
    ret = aos_task_new("slsfprov", sl_prov_thread, &awss_sf_priv, 4096);
    CHECK_RET_WITH_RET(ret == 0, -1);

    current_method = awss_sf_priv.method_id;
    return 0;
}

static void sl_smartconfig_stop()
{
    if (!callback_running) {
        LOGD(TAG, "call awss stop\n");
        awss_stop();
        last_smartconfig_running = 1;
    }
}

/**
    User starts the dev_ap process
*/
static int sl_dev_ap_start(wifi_prov_cb cb)
{    
    start_prepare();

    int ret;
    awss_config_press();
    prov_cb = cb;
    ret = aos_task_new("sldpprov", sl_prov_thread, &awss_dp_priv, 4096);
    CHECK_RET_WITH_RET(ret == 0, -1);

    current_method = awss_dp_priv.method_id;
    return 0;
}

static void sl_dev_ap_stop()
{
    if (!callback_running) {
        LOGD(TAG, "call awss_dev_ap stop\n");
        awss_dev_ap_stop();
        last_dev_ap_running = 1;
    }
}

int wifi_prov_sl_register()
{
    aos_event_new(&evt_report_to_cloud, 0);
    aos_event_new(&evt_linkkit_quit, 0);

    wifi_prov_method_register(&awss_sf_priv);
    wifi_prov_method_register(&awss_dp_priv);

    return 0;
}

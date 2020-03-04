/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "aos/cli.h"
#include "app_entry.h"
#include "aos/kernel.h"
#include "aos/yloop.h"
#include "linkkit/infra/infra_compat.h"
#include "linkkit/infra/infra_defs.h"
#include "linkkit/dev_model_api.h"
#include "netmgr.h"
#include "ulocation/ulocation.h"
#include "ulog/ulog.h"
#include "app_entry.h"
#include "aos/hal/adc.h"

#ifdef WITH_SAL
#include <atcmd_config_module.h>
#endif

static volatile char g_wifi_connect = 0; /* wifi connect flag */

/* linkkit initialize callback */
static int user_initialized(const int devid)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    LOG("Device Initialized, Devid: %d", devid);

    /* Set the linkkit initialize success flag */
    if (user_example_ctx->master_devid == devid) {
        user_example_ctx->master_initialized = 1;
    }

    return 0;
}


/* wifi connect success callback */
static void wifi_service_event(input_event_t *event, void *priv_data)
{
    netmgr_ap_config_t config;

    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

    memset(&config, 0, sizeof(netmgr_ap_config_t));
    netmgr_get_ap_config(&config);
    LOG("wifi_service_event config.ssid %s", config.ssid);
    if (strcmp(config.ssid, "adha") == 0 || strcmp(config.ssid, "aha") == 0) {
        return;
    }

#ifdef EN_COMBO_NET
    if (awss_running) {
        awss_success_notify();
    }
#endif

    /* Set wifi connect flag */
    if (g_wifi_connect == 0) {
        g_wifi_connect = 1;
    }
}

void user_post_locationinfo(void)
{
    int ret                  = 0;
    int contain_elem_flg     = 0;
    char *event_id = "LocationInfo";

    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    char property_payload[128];
    char event_payload[512];

    memset(property_payload, 0, sizeof(property_payload));
    memset(event_payload,    0, sizeof(event_payload));

    snprintf(event_payload + strlen(event_payload),
             sizeof(event_payload) - strlen(event_payload),
             "%s", "{");

#ifdef ULOCATION_WIFI
    char mmac[128];
    char macs[512];

    ret = ulocation_update_wifiinfo(mmac, macs);
    if (ret != 0) {
        LOGE("uLocation", "ulocation update wifi info failed!");
        return ret;
    }

    {
        LOGD("uLocation", "get wifi info\n");
        if (!contain_elem_flg) {
            contain_elem_flg = 1;
        } else {
            snprintf(event_payload + strlen(event_payload),
                     sizeof(event_payload) - strlen(event_payload),
                     "%s", ",");
        }

        snprintf(event_payload + strlen(event_payload),
                 sizeof(event_payload) - strlen(event_payload),
                 "\"mmac\":\"%s\",", mmac);

        snprintf(event_payload + strlen(event_payload),
                 sizeof(event_payload) - strlen(event_payload),
                 "\"macs\":\"%s\"", macs);
    }
#endif

#ifdef ULOCATION_GPRS
    char cdma[4];
    char bts[128];
    char nearbts[128*4];

    ret = ulocation_update_gprsinfo(cdma, bts, nearbts);
    if (ret != 0) {
        LOGE("uLocation", "ulocation update gprs info failed!");
        return ret;
    }

    {
        LOGD("uLocation", "get cell info: \n cdma: %s: \n bts: %s, \n nbts: %s \n", cdma, bts, nearbts);

        contain_elem_flg = 1;

        snprintf(event_payload + strlen(event_payload),
                 sizeof(event_payload) - strlen(event_payload),
                 "\"cdma\":\"%s\",", cdma);

        snprintf(event_payload + strlen(event_payload),
                 sizeof(event_payload) - strlen(event_payload),
                 "\"bts\":\"%s\",", bts);

        snprintf(event_payload + strlen(event_payload),
                 sizeof(event_payload) - strlen(event_payload),
                 "\"nearbts\":\"%s\"", nearbts);
    }
#endif

    snprintf(event_payload + strlen(event_payload),
             sizeof(event_payload) - strlen(event_payload),
             "%s", "}");
    if (strlen(event_payload) > 2) {
        LOGD("uLocation", "Post event Message : %s", event_payload);

        ret = IOT_Linkkit_TriggerEvent(user_example_ctx->master_devid, event_id, strlen(event_id),
                                    event_payload, strlen(event_payload));

        LOGD("uLocation", "Post event Message ID: %d", ret);
    }

#ifdef ULOCATION_GPS
    location_t gps;
    ret = ulocation_update_gpsinfo(&gps);
    if (ret != 0) {
        LOGE("uLocation", "ulocation update gps info failed!");
        return ret;
    }
    if (gps.outdoor.longitude != 0) {
        LOGD("uLocation","get gps info\n");
        snprintf(property_payload + strlen(property_payload),
                 sizeof(property_payload) - strlen(property_payload),
                 "{\"GeoLocation\":{\"value\":{\"Longitude\":%f,\"Latitude\":%f,\"Altitude\":%f,\"CoordinateSystem\":2}}}",
                 gps.outdoor.longitude, gps.outdoor.latitude, gps.outdoor.altitude);

        LOGD("uLocation", "Post property Message : %s", property_payload);
        ret = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                                 (unsigned char *)property_payload, strlen(property_payload));
        // return;
    }
    LOGD("uLocation", "GPS location data is empty!");
#endif

}

/* uLocation cloud test, include the following functions */
/* 1. wait wifi conect                               */
/* 2. link aliyun server                             */
/* 3. uLocation start                                    */
/* 4. handle linkkit event                           */
void ulocation_demo(void* arg)
{
    int ret = 0;

    iotx_linkkit_dev_meta_info_t master_meta_info;

    user_example_ctx_t* user_ctx = user_example_get_ctx();

    (void)arg;

    /* uLocation start */
    ret = ulocation_init(ULOCATION_SCENARIO_OUTDOOR, 5000);
    if (ret != 0) {
        LOGE("uLocation", "uLocation init failed!");
        return ret;
    }

    /* Linkkit start */
    ret = linkkit_init();
    if (ret != 0){
        return;
    }

    /* Register linkkit initialize callback */
    IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, user_initialized);

    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, PRODUCT_KEY, strlen(PRODUCT_KEY));
    memcpy(master_meta_info.product_secret, PRODUCT_SECRET, strlen(PRODUCT_SECRET));
    memcpy(master_meta_info.device_name, DEVICE_NAME, strlen(DEVICE_NAME));
    memcpy(master_meta_info.device_secret, DEVICE_SECRET, strlen(DEVICE_SECRET));

    #if defined(USE_ITLS)
    {
        char url[128] = {0};
        int port = 1883;
        snprintf(url, 128, "%s.itls.cn-shanghai.aliyuncs.com",master_meta_info.product_key);
        IOT_Ioctl(IOTX_IOCTL_SET_MQTT_DOMAIN, (void *)url);
        IOT_Ioctl(IOTX_IOCTL_SET_CUSTOMIZE_INFO, (void *)"authtype=id2");
        IOT_Ioctl(IOTX_IOCTL_SET_MQTT_PORT, &port);
    }
    #endif

        /* Create Master Device Resources */
        do {
            user_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
            if (user_ctx->master_devid >= 0) {
                break;
            }
            printf("IOT_Linkkit_Open failed! retry after %d ms\n", 2000);
            HAL_SleepMs(2000);
        } while (1);

        /* Start Connect Aliyun Server */
        do {
            ret = IOT_Linkkit_Connect(user_ctx->master_devid);
            if (ret >= 0) {
                break;
            }
            printf("IOT_Linkkit_Connect failed! retry after %d ms\n", 5000);
            HAL_SleepMs(5000);
        } while (1);

        /* post location info */
        while (1) {
            user_post_locationinfo();
            aos_msleep(10000);
        }

    IOT_Linkkit_Close(user_ctx->master_devid);

    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
}

int application_start(int argc, char **argv)
{
    int ret;

#ifdef WITH_SAL
    sal_device_config_t data = {0};
    data.uart_dev.port = 1;
    data.uart_dev.config.baud_rate = 115200;
    data.uart_dev.config.data_width = DATA_WIDTH_8BIT;
    data.uart_dev.config.parity = NO_PARITY;
    data.uart_dev.config.stop_bits  = STOP_BITS_1;
    data.uart_dev.config.flow_control = FLOW_CONTROL_DISABLED;
    data.uart_dev.config.mode = MODE_TX_RX;
    sal_add_dev("sim800", &data);
    sal_init();
#endif
    /* Set debug log show */
    aos_set_log_level(AOS_LL_DEBUG);
    set_iotx_info();

    /* Wifi initialize                                         */
    /* User can use the following cli commands to connect wifi */
    /* Clear wifi command:    netmgr clear                     */
    /* Connect wifi command:  netmgr connect ssid passwd       */
#ifdef ULOCATION_WIFI
    netmgr_init();
#endif
    /* Register wifi connect callback */
    // aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);

    /* Creat task for uLocation cloud test */
    ret = aos_task_new("ulocation_demo", ulocation_demo, NULL, 8192);
    if (ret != 0) {
        return -1;
    }

    /* Connect wifi with saved ssid and passwd */
#ifdef ULOCATION_WIFI
    netmgr_start(true);
#endif
    /* Enter yloop */
    aos_loop_run();

    return 0;
}



/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/init.h"
#include "board.h"
#include <aos/errno.h>
#include <aos/kernel.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#include "wifi_provisioning.h"
#include "smartliving/sl_wifi_prov.h"
#include "smartliving/iot_export.h"
#include "netmgr.h"
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include <uservice/uservice.h>
#include <uservice/eventid.h>

#include "aos/kv.h"

#define WIFI_SSID_KV_KEY "wifi_ssid"
#define WIFI_PASSWORD_KV_KEY "wifi_password"

static int from_zero_config = 0;

static void wifi_event_cb(uint32_t event_id, const void *param, void *context)
{
    int from_zc = *(int *)context;

    printf("wifi event cb\r\n");

    printf("Got IP from_zc:%d\r\n", from_zc);
    if(from_zc)
        sl_report_to_cloud(1);
    else
        test_report_to_cloud();
}

static void netmgr_comp_enable(const char *ssid, const char *password, int from_zc)
{
    netmgr_hdl_t  fd;
    netmgr_connect_params_t* params; 
    printf("netmgr test \r\n");
    if(!ssid || !password) {
        return;
    }
    event_service_init(NULL);

    netmgr_service_init(NULL);

    from_zero_config = from_zc;
    event_subscribe(EVENT_NETMGR_DHCP_SUCCESS, wifi_event_cb, &from_zero_config);

    printf("%s-%d ssid:%s paaswd:%s\r\n", __FUNCTION__, __LINE__, ssid, password);
    params = aos_malloc(sizeof(netmgr_connect_params_t));

    fd = netmgr_get_dev("/dev/wifi0");
    params->type = NETMGR_TYPE_WIFI;
    memcpy(params->params.wifi_params.ssid, ssid, strlen(ssid)+1);
    memcpy(params->params.wifi_params.pwd, password, strlen(password)+1);
    netmgr_connect(fd, params);
    aos_free(params);

    return; 
}


void helloworld_cb(uint32_t method_id, wifi_prov_event_t event, wifi_prov_result_t *result) {
     int ret;

     if(event==WIFI_PROV_EVENT_TIMEOUT) {
        printf("%s event timeout\r\n", __FUNCTION__);
        return;
     }
     printf("%s-%d ssid:%s paaswd:%s\r\n", __FUNCTION__, __LINE__, result->ssid, result->password);
     ret = aos_kv_set(WIFI_SSID_KV_KEY, result->ssid, strlen(result->ssid),1);
     if(ret) {
        printf("aos_kv_set %s ret:%d\r\n", WIFI_SSID_KV_KEY, ret);
     }
     ret = aos_kv_set(WIFI_PASSWORD_KV_KEY, result->password, strlen(result->password), 1);
     if(ret) {
        printf("aos_kv_set %s ret:%d\r\n", WIFI_PASSWORD_KV_KEY, ret);
     }
     netmgr_comp_enable(result->ssid, result->password, 1);
     return;
}

int application_start(int argc, char *argv[])
{
    int count = 0;
    unsigned int method_id;
    char ssid[32+1] = {0};
    char password[64+1]={0};
    int len;
    int ret;
    #if 0 /*zeroconfig*/
    char *devinfo_dn = "H0SgxCuqLjXgWczbrjfc";
    char *devinfo_ds = "3c6368c6239ffc0f591ef8db647b2612";
    char *devinfo_pk = "a1e701VItwp";
    char *devinfo_ps = "9czOzNHtHszt1Ko6";
    #else /*smartconfig*/
    char *devinfo_dn = "l8Pmr1wxmNDtDmyOROHw";
    char *devinfo_ds = "d781f10127ced6b9cb13fa8f8fe2003a";
    char *devinfo_pk = "a1aGVQcqbBZ";
    char *devinfo_ps = "q3mmLZnFoZryiKJ2";
    #endif
    printf("nano entry here!\r\n"); 
    wifi_prov_sl_register();
    /*
        configure 三元组
     */
    ret = aos_kv_set(DEVINFO_DN, devinfo_dn, strlen(devinfo_dn), 1);
    if(ret) {
            printf("aos_kv_set %s ret:%d\r\n", DEVINFO_DN, ret);
    }
    ret = aos_kv_set(DEVINFO_DS, devinfo_ds, strlen(devinfo_ds), 1);
    if(ret) {
            printf("aos_kv_set %s ret:%d\r\n", DEVINFO_DS, ret);
    }
    ret = aos_kv_set(DEVINFO_PK, devinfo_pk, strlen(devinfo_pk), 1);
    if(ret) {
            printf("aos_kv_set %s ret:%d\r\n", DEVINFO_PK, ret);
    }
    ret = aos_kv_set(DEVINFO_PS, devinfo_ps, strlen(devinfo_ps), 1);
    if(ret) {
            printf("aos_kv_set %s ret:%d\r\n", DEVINFO_PS, ret);
    }

    /*
        get wifi password and ssid
     */
    len = sizeof(ssid);
    ret = aos_kv_get(WIFI_SSID_KV_KEY, ssid, &len);
    if(ret) {
            printf("aos_kv_get %s ret:%d\r\n", WIFI_SSID_KV_KEY, ret);
            goto zero_config;
    }
    len = sizeof(password);
    ret = aos_kv_get(WIFI_PASSWORD_KV_KEY, password, &len);
    if(ret) {
            printf("aos_kv_get %s ret:%d\r\n", WIFI_PASSWORD_KV_KEY, ret);
            goto zero_config;
    }

    /*
        enable wifi
     */
    netmgr_comp_enable(ssid, password, 0);
    goto loop;

    /*
        zero config
     */
zero_config:
    method_id = wifi_prov_get_method_id("sl_smartconfig");
    wifi_prov_start(method_id, helloworld_cb, 1000000);

loop:
    while (1) {
        aos_msleep(10000);
    };
}

/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ulog/ulog.h>
#include <aos/kernel.h>
#include <stdint.h>
#include <network/network.h>
#include <aos/yloop.h>
#include <netmgr/netmgr.h>
#include <umesh2/core/umesh.h>

/*for get mac addr*/
#include "hal/wifi.h"

static void *net_handle = NULL;
static netmgr_ap_config_t config;
static int connect_ap = 0 ;
static int finish = 0;

static void umesh_zero_finish_func(const uint8_t *dest_mac, umesh_zero_config_state_t result, void *context)
{
    int i;
    LOG("recv finish:");
    for (i = 0; i < 6; i++) {
        printf("%02x ", dest_mac[i]);
    }
    printf("result = %d \r\n", result);
    finish = 1;
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

    memset(&config, 0, sizeof(netmgr_ap_config_t));
    netmgr_get_ap_config(&config);
    LOG("wifi_service_event config.ssid %s", config.ssid);
    connect_ap = 1;
}

static void get_ap_info(const char *ssid, const char *pwd, const uint8_t *bssid, uint8_t *ext, uint8_t len, void *ctx)
{
    int i;
    LOG("++++++++++get ssid = %s, pwd = %s ++++++++++++", ssid, pwd);
    printf("bssid:");
    for (i = 0; i < 6; i++) {
        printf("%02x ", bssid[i]);
    }
    printf("\r\n");
    printf("ext info:");
    for (i = 0; i < len; i++) {
        printf("%02x ", ext[i]);
    }
    finish = 1;
    printf("\r\n");

}


static void app_main_entry(void *arg)
{
    int ret;
    uint8_t test_ext[20] = {1, 2, 3, 4, 5, 6, 7, 8};
    net_handle = umesh_network_init();
    if (net_handle == NULL) {
        LOG("umesh init failed");
        return;
    }

    while (!umesh_is_connected(net_handle)) {
        LOG("----------Waiting for networking-----------");
        aos_msleep(1000);
    }

    umesh_zero_slave_register_receiver(net_handle, get_ap_info, net_handle);
    while (finish == 0) {
        if (connect_ap == 1) {
            ret = umesh_zero_master_start_config(net_handle, config.ssid, config.pwd, config.bssid, test_ext, 20,
                                                 umesh_zero_finish_func, net_handle);

            LOG("zero start, ret = %d", ret);
        }

        aos_msleep(5000);
    }

    aos_msleep(10000);
err:
    LOG("-----------------test end--------------------");
    if (net_handle != NULL) {
        ret = umesh_network_deinit(net_handle);
        LOG("umesh_network_deinit,ret = %d", ret);
        net_handle = NULL;
    }

    return;
}

int application_start(int argc, char **argv)
{

    LOG("application_start");

    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
    netmgr_init();
    netmgr_start(false);

    aos_set_log_level(AOS_LL_DEBUG);
    aos_task_new("meshappmain", app_main_entry, NULL, 4096);
    aos_loop_run();
    return 0;
}

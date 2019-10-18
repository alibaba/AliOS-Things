/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network/hal/base.h"
#include <hal/wifi.h>


static uint8_t fixmac[6] = {0x0b, 0x23, 0x03, 0x10, 0xcb, 0x63};
extern char net_ipaddr[16], net_mask[16], net_gw[16];
extern int esp8266_wifi_got_ip(const char *ssid, const char *psw);

static int wifi_init(hal_wifi_module_t *m)
{
    printf("wifi init success!!\n");
    return 0;
};

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    (void)m;

    memcpy(mac, fixmac, 6);
    printf("wifi_get_mac_addr!!\n");

};

static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    (void)init_para;

    int ret = esp8266_wifi_got_ip(init_para->wifi_ssid, init_para->wifi_key);

    if (ret < 0) {
        return ret;
    }

//config ap
    hal_wifi_ip_stat_t ip_stat;

    memcpy(ip_stat.ip, net_ipaddr, sizeof(net_ipaddr));
    memcpy(ip_stat.mask, net_mask, sizeof(net_mask));
    memcpy(ip_stat.gate, net_gw, sizeof(net_gw));
    memcpy(ip_stat.mac, fixmac, 6);

    if (m->ev_cb->stat_chg != NULL) {
        m->ev_cb->stat_chg(m, NOTIFY_STATION_UP, NULL);
    }

    if (m->ev_cb->ip_got != NULL) {
        m->ev_cb->ip_got(m, &ip_stat, NULL);
    }

    return 0;
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    (void)init_para_adv;

    return 0;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    (void)wifi_type;

    memcpy(out_net_para->ip, net_mask, 4);
    memcpy(out_net_para->mask, net_mask, 4);
    memcpy(out_net_para->gate, net_gw, 4);
    memcpy(out_net_para->mac, fixmac, 6);

    return 0;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    out_stat->is_connected = 1;
    return 0;
}

static void start_scan(hal_wifi_module_t *m)
{

}

static void start_scan_adv(hal_wifi_module_t *m)
{
}

static int power_off(hal_wifi_module_t *m)
{
    return 0;
}

static int power_on(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend_station(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{

    return 0;
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
    return 0;
}

static void start_monitor(hal_wifi_module_t *m)
{

}

static void stop_monitor(hal_wifi_module_t *m)
{

}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{

}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{

}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    return 0;
}

static int get_wireless_info(hal_wifi_module_t *m, void *wireless_info)
{
    hal_wireless_info_t *info = (hal_wireless_info_t *)wireless_info;
    signed char rssi;

    LOGD("get wireless info\r\n");

    if (info == NULL)
        return -1;

    if (csi_wifi_connection_get_rssi(&rssi) < 0) {
        return -1;
    }
    if (rssi > 0)
        rssi -= 128;
    info->rssi = rssi;

    return 0;
}

hal_wifi_module_t hobbit_wifi_esp8266 = {
    .base.name           = "csky_hobbit_wifi",
    .init                =  wifi_init,
    .get_mac_addr        =  wifi_get_mac_addr,
    .start               =  wifi_start,
    .start_adv           =  wifi_start_adv,
    .get_ip_stat         =  get_ip_stat,
    .get_link_stat       =  get_link_stat,
    .start_scan          =  start_scan,
    .start_scan_adv      =  start_scan_adv,
    .power_off           =  power_off,
    .power_on            =  power_on,
    .suspend             =  suspend,
    .suspend_station     =  suspend_station,
    .suspend_soft_ap     =  suspend_soft_ap,
    .set_channel         =  set_channel,
    .start_monitor       =  start_monitor,
    .stop_monitor        =  stop_monitor,
    .register_monitor_cb =  register_monitor_cb,
    .register_wlan_mgnt_monitor_cb = register_wlan_mgnt_monitor_cb,
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame,

    .get_wireless_info   = get_wireless_info,
};


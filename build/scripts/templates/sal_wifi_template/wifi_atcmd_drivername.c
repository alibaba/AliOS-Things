/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "network/hal/base.h"
#include <hal/wifi.h>

void at_wevent_handler(void *arg, char *buf, int buflen)
{
}

static int wifi_init(hal_wifi_module_t *m)
{
    return 0;
};

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
};

static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    return 0;
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    return 0;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    return 0;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
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

hal_wifi_module_t aos_wifi_module_@drivername@ = {
    .base.name           = "aos_wifi_module_@drivername@",
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
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame
};


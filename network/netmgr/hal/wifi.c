/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "network/hal/wifi.h"

static AOS_DLIST_HEAD(g_wifi_module);

hal_wifi_module_t *hal_wifi_get_default_module(void)
{
    hal_wifi_module_t *m = 0;

    if (dlist_empty(&g_wifi_module)) {
        return 0;
    }

    m = dlist_first_entry(&g_wifi_module, hal_wifi_module_t, base.list);

    return m;
}

void hal_wifi_register_module(hal_wifi_module_t *module)
{
    dlist_add_tail(&module->base.list, &g_wifi_module);

}

int hal_wifi_init(void)
{
    int          err = 0;
    dlist_t *t;

    /* do low level init */
    dlist_for_each(t, &g_wifi_module) {
        hal_wifi_module_t *m = (hal_wifi_module_t *)t;
        m->init(m);
    }

    return err;
}

int hal_wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if (m && m->get_mac_addr) {
        m->get_mac_addr(m, mac);
        return 0;
    }

    return -1;
}

int hal_wifi_set_mac_addr(hal_wifi_module_t *m, const uint8_t *mac)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if (m && m->set_mac_addr) {
        m->set_mac_addr(m, mac);
        return 0;
    }

    return -1;
}

int hal_wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if (m == NULL)
        return -1;

    return m->start(m, init_para);
}

int  hal_wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    return m->start_adv(m, init_para_adv);
}

int  hal_wifi_get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    return m->get_ip_stat(m, out_net_para, wifi_type);
}

int  hal_wifi_get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    return m->get_link_stat(m, out_stat);
}

int hal_wifi_scan_ap_list(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat, uint8_t *ap_num)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    return m->scan_ap_list(m, out_stat, ap_num);
}

void hal_wifi_start_scan(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    m->start_scan(m);
}

void hal_wifi_start_scan_adv(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    m->start_scan_adv(m);
}

int hal_wifi_power_off(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    return m->power_off(m);
}

int hal_wifi_power_on(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    return m->power_on(m);
}

int hal_wifi_suspend(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    return m->suspend(m);
}

int hal_wifi_suspend_station(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    return m->suspend_station(m);
}

int hal_wifi_suspend_soft_ap(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    return m->suspend_soft_ap(m);
}

int hal_wifi_set_channel(hal_wifi_module_t *m, int ch)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    return m->set_channel(m, ch);
}

int hal_wifi_get_channel(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    return m->get_channel(m);
}

int hal_wifi_get_channel_list(hal_wifi_module_t *m, const uint8_t **chnlist)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    return m->get_channel_list(m, chnlist);
}

void hal_wifi_start_wifi_monitor(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    m->start_monitor(m);
}

void hal_wifi_stop_wifi_monitor(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    m->stop_monitor(m);
}

void hal_wifi_register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    m->register_monitor_cb(m, fn);
}

void hal_wifi_install_event(hal_wifi_module_t *m, const hal_wifi_event_cb_t *cb)
{
    if (NULL == m)
        return;
    m->ev_cb = cb;
}

void hal_wlan_register_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    m->register_wlan_mgnt_monitor_cb(m, fn);
}

int hal_wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    return m->wlan_send_80211_raw_frame(m, buf, len);
}

int hal_get_wireless_info(hal_wifi_module_t *m, void *info)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if (m == NULL || m->get_wireless_info == NULL)
        return -1;

    return m->get_wireless_info(m, info);
}

void hal_wifi_start_debug_mode(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if (m->start_debug_mode == NULL) {
        return;
    }

    m->start_debug_mode(m);
}

void hal_wifi_stop_debug_mode(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if (m->stop_debug_mode == NULL) {
        return;
    }

    m->stop_debug_mode(m);
}

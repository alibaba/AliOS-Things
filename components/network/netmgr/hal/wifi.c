/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#ifdef CONFIG_MODERN_NETMGR
#include "lwip/netif.h"
#endif /* CONFIG_MODERN_NETMGR */
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

int hal_wifi_set_module_base(hal_wifi_module_t *module, char *os, char *partner,
                                 char* app_net, char* type, char *project, char* cloud)
{
    dlist_t *t;
    int found = 0;

    dlist_for_each(t, &g_wifi_module) {
        if(module ==  (hal_wifi_module_t *)t) {
            found = 1;
            break;
        }
    }

    if(1 == found) {
        module->base.os = os;
        module->base.partner = partner;
        module->base.app_net = app_net;
        module->base.type = type;
        module->base.project = project;
        module->base.cloud = cloud;
        return 0;
    } else {
        return -1;
    }
}

void hal_wifi_register_module(hal_wifi_module_t *module)
{
    /* reset module base information */
    module->base.os = NULL;
    module->base.partner = NULL;
    module->base.app_net = NULL;
    module->base.type = NULL;
    module->base.project = NULL;
    module->base.cloud = NULL;

    dlist_add_tail(&module->base.list, &g_wifi_module);

}

int hal_wifi_init(void)
{
    int          err = 0;
    dlist_t *t;

    /* do low level init */
    dlist_for_each(t, &g_wifi_module) {
        hal_wifi_module_t *m = (hal_wifi_module_t *)t;
#if ((WIFI_CONFIG_SUPPORT_LOWPOWER > 0) && (WIFI_CONFIG_LISTENSET_BINIT > 0))
        m->set_listeninterval(m, WIFI_CONFIG_LISTEN_INTERVAL);
#endif
        m->init(m);
#if ((WIFI_CONFIG_SUPPORT_LOWPOWER > 0) && (WIFI_CONFIG_LISTENSET_BINIT == 0))
        m->set_listeninterval(m, WIFI_CONFIG_LISTEN_INTERVAL);
#endif
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

int hal_wifi_connect(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->connect) {
        return m->connect(m, init_para);
    }

    return -1;
}

int hal_wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->start) {
        return m->start(m, init_para);
    }

    return -1;
}

int  hal_wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->start_adv) {
        return m->start_adv(m, init_para_adv);
    }

    return -1;
}

int  hal_wifi_set_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *in_net_para, hal_wifi_type_t wifi_type)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->set_ip_stat) {
        return m->set_ip_stat(m, in_net_para, wifi_type);
    }

    return -1;
}

int  hal_wifi_get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

#ifdef CONFIG_MODERN_NETMGR
    /* Wifi service got ip address, so no need to get ip address from wifi driver */
    if(m) {
        struct netif* net_if = hal_wifi_get_netif(m, wifi_type);

        if(net_if == NULL) {
            return -1;
        }

        memset(out_net_para, 0, sizeof(hal_wifi_ip_stat_t));
        snprintf(out_net_para->ip, sizeof(out_net_para->ip), "%u.%u.%u.%u",
               ((uint8_t *)&net_if->ip_addr.addr)[0],
               ((uint8_t *)&net_if->ip_addr.addr)[1],
               ((uint8_t *)&net_if->ip_addr.addr)[2],
               ((uint8_t *)&net_if->ip_addr.addr)[3]);

        snprintf(out_net_para->mask, sizeof(out_net_para->mask), "%u.%u.%u.%u",
               ((uint8_t *)&net_if->netmask.addr)[0],
               ((uint8_t *)&net_if->netmask.addr)[1],
               ((uint8_t *)&net_if->netmask.addr)[2],
               ((uint8_t *)&net_if->netmask.addr)[3]);

        snprintf(out_net_para->gate, sizeof(out_net_para->gate), "%u.%u.%u.%u",
               ((uint8_t *)&net_if->gw.addr)[0],
               ((uint8_t *)&net_if->gw.addr)[1],
               ((uint8_t *)&net_if->gw.addr)[2],
               ((uint8_t *)&net_if->gw.addr)[3]);
        return 0;
    }
#else
    if(m && m->get_ip_stat) {
        return m->get_ip_stat(m, out_net_para, wifi_type);
    }
#endif
    return -1;
}

int  hal_wifi_get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->get_link_stat) {
        return m->get_link_stat(m, out_stat);
    }

    return -1;
}

int hal_wifi_scan_ap_list(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat, uint8_t *ap_num)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->scan_ap_list) {
        return m->scan_ap_list(m, out_stat, ap_num);
    }

    return -1;
}

int hal_wifi_start_scan(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->start_scan) {
        return m->start_scan(m);
    }

    return -1;
}

int hal_wifi_start_specified_scan(hal_wifi_module_t *m, ap_list_t *ap_list, int ap_num)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->start_specified_scan) {
        return m->start_specified_scan(m, ap_list, ap_num);
    }

    return -1;
}

int hal_wifi_start_scan_adv(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->start_scan_adv) {
        return m->start_scan_adv(m);
    }

    return -1;
}

int hal_wifi_power_off(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->power_off) {
        return m->power_off(m);
    }

    return -1;
}

int hal_wifi_power_on(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->power_on) {
        return m->power_on(m);
    }

    return -1;
}

int hal_wifi_suspend(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->suspend) {
        return m->suspend(m);
    }

    return -1;
}

int hal_wifi_disconnect(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->disconnect) {
        return m->disconnect(m);
    }

    return -1;
}

int hal_wifi_suspend_station(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->suspend_station) {
        return m->suspend_station(m);
    }

    return -1;
}

int hal_wifi_suspend_soft_ap(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->suspend_soft_ap) {
        return m->suspend_soft_ap(m);
    }

    return -1;
}

int hal_wifi_cancel(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->cancel) {
        return m->cancel(m);
    }

    return -1;
}

int hal_wifi_set_channel(hal_wifi_module_t *m, int ch)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->set_channel) {
        return m->set_channel(m, ch);
    }

    return -1;
}

int hal_wifi_get_channel(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->get_link_stat) {
        hal_wifi_link_stat_t out;
        if(0 == m->get_link_stat(m, &out)) {
            return out.channel;
        }
    }

    return -1;
}

int hal_wifi_get_channel_list(hal_wifi_module_t *m, const uint8_t **chnlist)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->get_channel_list) {
        return m->get_channel_list(m, chnlist);
    }

    return -1;
}

void hal_wifi_start_wifi_monitor(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m) {
        m->start_monitor(m);
    }
}

void hal_wifi_stop_wifi_monitor(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m) {
        m->stop_monitor(m);
    }
}

void hal_wifi_register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m) {
        m->register_monitor_cb(m, fn);
    }
}

void hal_wifi_install_event(hal_wifi_module_t *m, const hal_wifi_event_cb_t *cb)
{
    if (m) {
        m->ev_cb = cb;
    }
}

void hal_wlan_register_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->register_wlan_mgnt_monitor_cb) {
        m->register_wlan_mgnt_monitor_cb(m, fn);
    }
}

void hal_wlan_start_mgnt_monitor(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->start_mgnt_monitor) {
        m->start_mgnt_monitor(m);
    }
}

void hal_wlan_stop_mgnt_monitor(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->stop_mgnt_monitor) {
        m->stop_mgnt_monitor(m);
    }
}

int hal_wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->wlan_send_80211_raw_frame) {
        return m->wlan_send_80211_raw_frame(m, buf, len);
    }
    return -1;
}

int hal_get_wireless_info(hal_wifi_module_t *m, void *info)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if (m && m->get_wireless_info) {
        return m->get_wireless_info(m, info);
    }

    return -1;
}

void hal_wifi_start_debug_mode(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if (m && m->start_debug_mode) {
        m->start_debug_mode(m);
    }
}

void hal_wifi_stop_debug_mode(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if (m && m->stop_debug_mode) {
        m->stop_debug_mode(m);
    }
}

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)

int hal_wifi_set_listeninterval(hal_wifi_module_t *m, uint8_t listen_interval)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if (m && m->set_listeninterval) {
        return m->set_listeninterval(m, listen_interval);
    }

    return -1;
}

int hal_wifi_enter_powersave(hal_wifi_module_t *m, uint8_t recvDTIMs)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if (m && m->enter_powersave) {
        return m->enter_powersave(m, recvDTIMs);
    }

    return -1;
}

int hal_wifi_exit_powersave(hal_wifi_module_t *m)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if (m && m->exit_powersave) {
        return m->exit_powersave(m);
    }

    return -1;
}

#endif

struct netif* hal_wifi_get_netif(hal_wifi_module_t *m, hal_wifi_type_t mode)
{
    if (m == NULL) {
        m = hal_wifi_get_default_module();
    }

    if(m && m->get_netif) {
        return m->get_netif(m, mode);
    }

    return NULL;
}

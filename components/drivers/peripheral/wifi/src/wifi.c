/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

//#if defined(CONFIG_SAL) || defined(CONFIG_TCPIP)
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <aos/hal/wifi.h>
#include <vfsdev/wifi_dev.h>

#define WIFI_DRIVER(dev)  ((wifi_driver_t *)dev->drv_ops)

int hal_wifi_init(netdev_t *dev)
{
    int ret = -1;

    printf("%s:%d\n", __func__, __LINE__);
    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->init) {
       printf("%s:%d\n", __func__, __LINE__);
       ret = WIFI_DRIVER(dev)->init(dev);
    }

    return ret;
}

int hal_wifi_deinit(netdev_t *dev)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->deinit) {
        ret = WIFI_DRIVER(dev)->deinit(dev);
    }

    return ret;
}

int hal_wifi_get_base_info(netdev_t *dev, wifi_base_info_t* info)
{
    if(dev&& WIFI_DRIVER(dev)) {
        wifi_base_info_t* base = (wifi_base_info_t*)&(WIFI_DRIVER(dev)->base);

        if(base->partner) {
            info->partner = base->partner;
        } else {
            info->partner = NULL;
        }

        if(base->project) {
            info->project = base->project;
        } else {
            info->project = NULL;
        }

        if(base->app_net) {
            info->app_net = base->app_net;
        } else {
            info->app_net = NULL;
        }

        if(base->cloud) {
            info->cloud = base->cloud;
        } else {
            info->cloud = NULL;
        }

        if(base->os) {
            info->os = base->os;
        } else {
            info->os = NULL;
        }

        if(base->type) {
            info->type = base->type;
        } else {
            info->type = NULL;
        }
        return 0;
    } else {
        return -1;
    }
}
int hal_wifi_set_mode(netdev_t *dev, wifi_mode_t mode)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->set_mode) {
        ret = WIFI_DRIVER(dev)->set_mode(dev, mode);
    }

    return ret;
}


int hal_wifi_get_mode(netdev_t *dev, wifi_mode_t *mode)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->get_mode) {
        ret = WIFI_DRIVER(dev)->get_mode(dev, mode);
    }

    return ret;
}


int hal_wifi_install_event_cb(netdev_t *dev, wifi_event_func *evt_cb)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->install_event_cb) {
        ret = WIFI_DRIVER(dev)->install_event_cb(dev, evt_cb);
    }

    return ret;
}

int hal_wifi_cancel_connect(netdev_t *dev)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->cancel_connect) {
        ret = WIFI_DRIVER(dev)->cancel_connect(dev);
    }

    return ret;
}

int hal_wifi_set_mac_addr(netdev_t *dev, const uint8_t *mac)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->set_mac_addr) {
        ret = WIFI_DRIVER(dev)->set_mac_addr(dev, mac);
    }

    return ret;
}

int hal_wifi_get_mac_addr(netdev_t *dev, uint8_t *mac)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->get_mac_addr) {
        ret = WIFI_DRIVER(dev)->get_mac_addr(dev, mac);
    }

    return ret;
}

int hal_wifi_set_lpm(netdev_t *dev, wifi_lpm_mode_t mode)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->set_lpm) {
        ret = WIFI_DRIVER(dev)->set_lpm(dev, mode);
    }

    return ret;
}

int hal_wifi_get_lpm(netdev_t *dev, wifi_lpm_mode_t *mode)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->get_lpm) {
        ret = WIFI_DRIVER(dev)->get_lpm(dev, mode);
    }

    return ret;
}

int hal_wifi_notify_ip_state2drv(netdev_t *dev, wifi_ip_stat_t *out_net_para, wifi_mode_t wifi_type)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->notify_ip_state2drv) {
        ret = WIFI_DRIVER(dev)->notify_ip_state2drv(dev, out_net_para, wifi_type);
    }

    return ret;
}

int hal_wifi_start_scan(netdev_t *dev, wifi_scan_config_t *config, bool block)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->start_scan) {
        ret = WIFI_DRIVER(dev)->start_scan(dev, config, block);
    }

    return ret;
}

int hal_wifi_connect(netdev_t *dev, wifi_config_t *config)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->connect) {
        ret = WIFI_DRIVER(dev)->connect(dev, config);
    }

    return ret;
}

int hal_wifi_disconnect(netdev_t *dev)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->disconnect) {
        ret = WIFI_DRIVER(dev)->disconnect(dev);
    }

    return ret;
}

int hal_wifi_sta_get_link_status(netdev_t *dev, wifi_ap_record_t *ap_info)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->sta_get_link_status) {
        ret = WIFI_DRIVER(dev)->sta_get_link_status(dev, ap_info);
    }

    return ret;
}

int hal_wifi_ap_get_sta_list(netdev_t *dev, wifi_sta_list_t *sta)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->ap_get_sta_list) {
        ret = WIFI_DRIVER(dev)->ap_get_sta_list(dev, sta);
    }

    return ret;
}

int hal_wifi_start_monitor(netdev_t *dev, wifi_promiscuous_cb_t cb)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->start_monitor) {
        ret = WIFI_DRIVER(dev)->start_monitor(dev, cb);
    }

    return ret;
}

int hal_wifi_stop_monitor(netdev_t *dev)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->stop_monitor) {
        ret = WIFI_DRIVER(dev)->stop_monitor(dev);
    }

    return ret;
}

int hal_wifi_send_80211_raw_frame(netdev_t *dev, void *buffer, uint16_t len)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->send_80211_raw_frame) {
        ret = WIFI_DRIVER(dev)->send_80211_raw_frame(dev, buffer, len);
    }

    return ret;
}

int hal_wifi_set_channel(netdev_t *dev, uint8_t primary)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->set_channel) {
        ret = WIFI_DRIVER(dev)->set_channel(dev, primary);
    }

    return ret;
}

int hal_wifi_get_channel(netdev_t *dev, uint8_t *primary)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->get_channel) {
        ret = WIFI_DRIVER(dev)->get_channel(dev, (int *)primary);
    }

    return ret;
}

int hal_wifi_register_monitor_cb(netdev_t *dev, monitor_data_cb_t fn)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->register_monitor_cb) {
        ret = WIFI_DRIVER(dev)->register_monitor_cb(dev, fn);
    }

    return ret;
}

int hal_wifi_start_mgnt_monitor(netdev_t *dev)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->start_mgnt_monitor) {
        ret = WIFI_DRIVER(dev)->start_mgnt_monitor(dev);
    }

    return ret;
}

int hal_wifi_stop_mgnt_monitor(netdev_t *dev)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev)&& WIFI_DRIVER(dev)->stop_mgnt_monitor) {
        ret = WIFI_DRIVER(dev)->stop_mgnt_monitor(dev);
    }

    return ret;
}

int hal_wifi_register_mgnt_monitor_cb(netdev_t *dev, monitor_data_cb_t fn)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->register_mgnt_monitor_cb) {
        ret = WIFI_DRIVER(dev)->register_mgnt_monitor_cb(dev, fn);
    }

    return ret;
}


int hal_wifi_set_smartcfg(netdev_t *dev, int enable)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->set_smartcfg) {
        ret = WIFI_DRIVER(dev)->set_smartcfg(dev, enable);
    }

    return ret;
}

int hal_wifi_start_specified_scan(netdev_t *dev, ap_list_t *ap_list, int ap_num)
{
    int ret = -1;

    if(dev && WIFI_DRIVER(dev) && WIFI_DRIVER(dev)->start_specified_scan) {
        ret = WIFI_DRIVER(dev)->start_specified_scan(dev, ap_list, ap_num);
    }

    return ret;
}

//#endif

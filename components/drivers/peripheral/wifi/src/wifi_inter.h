/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef WIFI_INTER_H
#define WIFI_INTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int hal_wifi_init(netdev_t *dev);
int hal_wifi_deinit(netdev_t *dev);
int hal_wifi_get_base_info(netdev_t *dev, wifi_base_info_t* info);
int hal_wifi_set_mode(netdev_t *dev, wifi_mode_t mode);
int hal_wifi_get_mode(netdev_t *dev, wifi_mode_t *mode);
int hal_wifi_set_mac_addr(netdev_t *dev, const uint8_t *mac);
int hal_wifi_get_mac_addr(netdev_t *dev, uint8_t *mac);
int hal_wifi_sta_get_link_status(netdev_t *dev, wifi_ap_record_t *ap_info);
int hal_wifi_start_scan(netdev_t *dev, wifi_scan_config_t *config, bool block);
int hal_wifi_set_channel(netdev_t *dev, int channel);
int hal_wifi_get_channel(netdev_t *dev, int *channel);
int hal_wifi_start_monitor(netdev_t *dev, wifi_promiscuous_cb_t cb);
int hal_wifi_stop_monitor(netdev_t *dev);
int hal_wifi_send_80211_raw_frame(netdev_t *dev, void *buffer, uint16_t len);
int hal_wifi_install_event_cb(netdev_t *dev, wifi_event_func *evt_cb);
int hal_wifi_set_lpm(netdev_t *dev, wifi_lpm_mode_t mode);
int hal_wifi_get_lpm(netdev_t *dev, wifi_lpm_mode_t *mode);
int hal_wifi_notify_ip_state2drv(netdev_t *dev, wifi_ip_stat_t *out_net_para, wifi_mode_t wifi_type);
int hal_wifi_connect(netdev_t *dev, wifi_config_t *config);
int hal_wifi_disconnect(netdev_t *dev);
int hal_wifi_cancel_connect(netdev_t *dev);
int hal_wifi_ap_get_sta_list(netdev_t *dev, wifi_sta_list_t *sta);
int hal_wifi_start_specified_scan(netdev_t *dev, ap_list_t *ap_list, int ap_num);
int hal_wifi_register_monitor_cb(netdev_t *dev, monitor_data_cb_t fn);
int hal_wifi_start_mgnt_monitor(netdev_t *dev);
int hal_wifi_stop_mgnt_monitor(netdev_t *dev);
int hal_wifi_register_mgnt_monitor_cb(netdev_t *dev, monitor_data_cb_t fn);
int hal_wifi_set_smartcfg(netdev_t *dev, int enable);

#ifdef __cplusplus
}
#endif

#endif

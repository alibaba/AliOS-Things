/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "netmgr.h"
#include "hal/net.h"

typedef struct {
    hal_net_module_t *hal_mod;
    uint8_t interface_type;
    bool ip_available;
    int16_t disconnected_times;
    int32_t ipv4_owned;
#if AOS_NET_WITH_WIFI
    netmgr_ap_config_t saved_conf;
    netmgr_ap_config_t ap_config;
    autoconfig_plugin_t *autoconfig_chain;
    netmgr_wifi_scan_result_cb_t cb;
    bool wifi_scan_complete_cb_finished;
    bool doing_smartconfig;
#endif
} net_interface_t;

net_interface_t *netmgr_get_net_interface(uint8_t interface_type);

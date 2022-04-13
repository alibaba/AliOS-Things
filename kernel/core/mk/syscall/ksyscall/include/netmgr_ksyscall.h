/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef NETMGR_KSYSCALL_H
#define NETMGR_KSYSCALL_H

int sys_netmgr_service_init_stub(void);

int sys_netmgr_service_deinit_stub(void);

int sys_netmgr_get_dev_stub(void *arg);

int sys_netmgr_set_ifconfig_stub(void *arg);

int sys_netmgr_get_ifconfig_stub(void *arg);

int sys_netmgr_set_auto_reconnect_stub(void *arg);

int sys_netmgr_get_config_stub(void *arg);

int sys_netmgr_del_config_stub(void *arg);

int sys_netmgr_get_state_stub(void *arg);

int sys_netmgr_connect_stub(void *arg);

int sys_netmgr_disconnect_stub(void *arg);

int sys_netmgr_save_config_stub(void *arg);

int sys_netmgr_set_connect_params_stub(void *arg);

int sys_netmgr_set_msg_cb_stub(void *arg);

int sys_netmgr_del_msg_cb_stub(void *arg);
#if AOS_NETWORK_WITH_WIFI
int sys_netmgr_wifi_scan_result_stub(void *arg);

int sys_netmgr_wifi_start_monitor_stub(void *arg);

int sys_netmgr_wifi_stop_monitor_stub(void *arg);

int sys_netmgr_wifi_start_mgnt_monitor_stub(void *arg);

int sys_netmgr_wifi_stop_mgnt_monitor_stub(void *arg);

int sys_netmgr_wifi_register_mgnt_monitor_cb_stub(void *arg);

int sys_netmgr_wifi_send_80211_raw_frame_stub(void *arg);
#endif
#endif /* NETMGR_KSYSCALL_H */


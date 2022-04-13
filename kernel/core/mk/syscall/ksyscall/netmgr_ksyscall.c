/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include "netmgr.h"
#include "netmgr_syscall_arg.h"

int sys_netmgr_service_init_stub(void)
{
    return netmgr_service_init();
}

void sys_netmgr_service_deinit_stub(void)
{
    netmgr_service_deinit();
}

netmgr_hdl_t sys_netmgr_get_dev_stub(void *arg)
{
    netmgr_get_dev_syscall_arg_t *_arg = arg;
    return netmgr_get_dev(_arg->name);
}

int sys_netmgr_set_ifconfig_stub(void *arg)
{
    netmgr_set_ifconfig_syscall_arg_t *_arg = arg;
    return netmgr_set_ifconfig(_arg->hdl, _arg->info);
}

int sys_netmgr_get_ifconfig_stub(void *arg)
{
    netmgr_get_ifconfig_syscall_arg_t *_arg = arg;
    return netmgr_get_ifconfig(_arg->hdl, _arg->info);
}

void sys_netmgr_set_auto_reconnect_stub(void *arg)
{
    netmgr_set_auto_reconnect_syscall_arg_t *_arg = arg;
    netmgr_set_auto_reconnect(_arg->hdl, _arg->enable);
}

int sys_netmgr_get_config_stub(void *arg)
{
    netmgr_get_config_syscall_arg_t *_arg = arg;
    return netmgr_get_config(_arg->hdl, _arg->config);
}

int sys_netmgr_del_config_stub(void *arg)
{
    netmgr_del_config_syscall_arg_t *_arg = arg;
    return netmgr_del_config(_arg->hdl, _arg->config);
}

int sys_netmgr_get_state_stub(void *arg)
{
    netmgr_get_state_syscall_arg_t *_arg = arg;
    return netmgr_get_state(_arg->hdl);
}

int sys_netmgr_connect_stub(void *arg)
{
    netmgr_connect_syscall_arg_t *_arg = arg;
    return netmgr_connect(_arg->hdl, _arg->params);
}

int sys_netmgr_disconnect_stub(void *arg)
{
    netmgr_disconnect_syscall_arg_t *_arg = arg;
    return netmgr_disconnect(_arg->hdl);
}

int sys_netmgr_save_config_stub(void *arg)
{
    netmgr_save_config_syscall_arg_t *_arg = arg;
    return netmgr_save_config(_arg->hdl);
}

int sys_netmgr_set_connect_params_stub(void *arg)
{
    netmgr_set_connect_params_syscall_arg_t *_arg = arg;
    return netmgr_set_connect_params(_arg->hdl, _arg->params);
}

int sys_netmgr_set_msg_cb_stub(void *arg)
{
    netmgr_set_msg_cb_syscall_arg_t *_arg = arg;
    return netmgr_set_msg_cb(_arg->hdl, _arg->cb);
}

int sys_netmgr_del_msg_cb_stub(void *arg)
{
    netmgr_del_msg_cb_syscall_arg_t *_arg = arg;
    return netmgr_del_msg_cb(_arg->hdl, _arg->cb);
}
#if AOS_NETWORK_WITH_WIFI
int sys_netmgr_wifi_scan_result_stub(void *arg)
{
    netmgr_wifi_scan_result_syscall_arg_t *_arg = arg;
    return netmgr_wifi_scan_result(_arg->hdl, _arg->ap_info, _arg->num, _arg->type);
}

void sys_netmgr_wifi_start_monitor_stub(void *arg)
{
    netmgr_wifi_start_monitor_syscall_arg_t *_arg = arg;
    netmgr_wifi_start_monitor(_arg->hdl);
}

void sys_netmgr_wifi_stop_monitor_stub(void *arg)
{
    netmgr_wifi_stop_monitor_syscall_arg_t *_arg = arg;
    netmgr_wifi_stop_monitor(_arg->hdl);
}

void sys_netmgr_wifi_start_mgnt_monitor_stub(void *arg)
{
    netmgr_wifi_start_mgnt_monitor_syscall_arg_t *_arg = arg;
    netmgr_wifi_start_mgnt_monitor(_arg->hdl);
}

void sys_netmgr_wifi_stop_mgnt_monitor_stub(void *arg)
{
    netmgr_wifi_stop_mgnt_monitor_syscall_arg_t *_arg = arg;
    netmgr_wifi_stop_mgnt_monitor(_arg->hdl);
}

void sys_netmgr_wifi_register_mgnt_monitor_cb_stub(void *arg)
{
    netmgr_wifi_reg_mgnt_monitor_cb_syscall_arg_t *_arg = arg;
    netmgr_wifi_register_mgnt_monitor_cb(_arg->fn);
}

void sys_netmgr_wifi_send_80211_raw_frame_stub(void *arg)
{
    netmgr_wifi_send_80211_raw_frame_syscall_arg_t *_arg = arg;
    netmgr_wifi_send_80211_raw_frame(_arg->buf, _arg->len);
}
#endif
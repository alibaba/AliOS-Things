/*

 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_config.h"
#include "syscall.h"
#include "netmgr_syscall_arg.h"
#include "syscall_no.h"
#include "netmgr.h"

int netmgr_service_init(void)
{
    return (int)SYSCALL(SYS_NETMGR_SERVICE_INIT, NULL);
}

void netmgr_service_deinit(void)
{
    SYSCALL(SYS_NETMGR_SERVICE_DEINIT, NULL);
}

netmgr_hdl_t netmgr_get_dev(const char *name)
{
    netmgr_get_dev_syscall_arg_t _arg;
    _arg.name = name;
    return (int)SYSCALL(SYS_NETMGR_GET_DEV, (void *)&_arg);
}

int netmgr_set_ifconfig(netmgr_hdl_t hdl, netmgr_ifconfig_info_t *info)
{
    netmgr_set_ifconfig_syscall_arg_t _arg;
    _arg.hdl = hdl;
    _arg.info = info;
    SYSCALL(SYS_NETMGR_SET_IFCONFIG, (void *)&_arg);
}

int netmgr_get_ifconfig(netmgr_hdl_t hdl, netmgr_ifconfig_info_t *info)
{
    netmgr_get_ifconfig_syscall_arg_t _arg;
    _arg.hdl = hdl;
    _arg.info = info;
    SYSCALL(SYS_NETMGR_GET_IFCONFIG, (void *)&_arg);
}

void netmgr_set_auto_reconnect(netmgr_hdl_t hdl, bool enable)
{
    netmgr_set_auto_reconnect_syscall_arg_t _arg;
    _arg.hdl = hdl;
    _arg.enable = enable;
    return (int)SYSCALL(SYS_NETMGR_SET_AUTO_RECONNECT, (void *)&_arg);
}

int netmgr_get_config(netmgr_hdl_t hdl, netmgr_config_t *config)
{
    netmgr_get_config_syscall_arg_t _arg;
    _arg.hdl = hdl;
    _arg.config = config;
    return (int)SYSCALL(SYS_NETMGR_GET_CONFIG, &_arg);
}

int netmgr_del_config(netmgr_hdl_t hdl, netmgr_del_config_t *config)
{
    netmgr_del_config_syscall_arg_t _arg;
    _arg.hdl = hdl;
    _arg.config = config;
    return (int)SYSCALL(SYS_NETMGR_DEL_CONFIG, &_arg);
}

int netmgr_get_state(netmgr_hdl_t hdl)
{
    netmgr_get_state_syscall_arg_t _arg;
    _arg.hdl = hdl;
    return (int)SYSCALL(SYS_NETMGR_GET_STATE, (void *)&_arg);
}

int netmgr_connect(netmgr_hdl_t hdl, netmgr_connect_params_t *params)
{
    netmgr_connect_syscall_arg_t _arg;
    _arg.hdl = hdl;
    _arg.params = params;
    return (int)SYSCALL(SYS_NETMGR_CONNECT, &_arg);
}

int netmgr_disconnect(netmgr_hdl_t hdl)
{
    netmgr_disconnect_syscall_arg_t _arg;
    _arg.hdl = hdl;
    SYSCALL(SYS_NETMGR_DISCONNECT, (void *)&_arg);
}

int netmgr_save_config(netmgr_hdl_t hdl)
{
    netmgr_save_config_syscall_arg_t _arg;
    _arg.hdl = hdl;
    SYSCALL(SYS_NETMGR_SAVE_CONFIG, (void *)&_arg);
}

int netmgr_set_connect_params(netmgr_hdl_t hdl, netmgr_connect_params_t *params)
{
    netmgr_set_connect_params_syscall_arg_t _arg;
    _arg.hdl = hdl;
    _arg.params = params;
    SYSCALL(SYS_NETMGR_SET_CONNECT_PARAMS, (void *)&_arg);
}

int netmgr_set_msg_cb(netmgr_hdl_t hdl, netmgr_msg_cb_t cb)
{
    netmgr_set_msg_cb_syscall_arg_t _arg;
    _arg.hdl = hdl;
    _arg.cb = cb;
    SYSCALL(SYS_NETMGR_SET_MSG_CB, (void *)&_arg);
}

int netmgr_del_msg_cb(netmgr_hdl_t hdl, netmgr_msg_cb_t cb)
{
    netmgr_del_msg_cb_syscall_arg_t _arg;
    _arg.hdl = hdl;
    _arg.cb = cb;
    SYSCALL(SYS_NETMGR_DEL_MSG_CB, (void *)&_arg);
}

#if AOS_NETWORK_WITH_WIFI
int netmgr_wifi_scan_result(netmgr_hdl_t hdl, netmgr_wifi_ap_list_t *ap_info, int num, netmgr_wifi_scan_type_t type)
{
    netmgr_wifi_scan_result_syscall_arg_t _arg;
    _arg.hdl = hdl;
    _arg.ap_info = ap_info;
    _arg.num = num;
    _arg.type = type;
    return (int)SYSCALL(SYS_NETMGR_WIFI_SCAN_RESULT, (void *)&_arg);
}

void netmgr_wifi_start_monitor(netmgr_hdl_t hdl)
{
    netmgr_wifi_start_monitor_syscall_arg_t _arg;
    _arg.hdl = hdl;
    SYSCALL(SYS_NETMGR_WIFI_START_MONITOR, &_arg);
}

void netmgr_wifi_stop_monitor(netmgr_hdl_t hdl)
{
    netmgr_wifi_stop_monitor_syscall_arg_t _arg;
    _arg.hdl = hdl;
    SYSCALL(SYS_NETMGR_WIFI_STOP_MONITOR, &_arg);
}

void netmgr_wifi_start_mgnt_monitor(netmgr_hdl_t hdl)
{
    netmgr_wifi_start_mgnt_monitor_syscall_arg_t _arg;
    _arg.hdl = hdl;
    SYSCALL(SYS_NETMGR_WIFI_START_MGNT_MONITOR, &_arg);
}

void netmgr_wifi_stop_mgnt_monitor(netmgr_hdl_t hdl)
{
    netmgr_wifi_stop_mgnt_monitor_syscall_arg_t _arg;
    _arg.hdl = hdl;
    SYSCALL(SYS_NETMGR_WIFI_STOP_MGNT_MONITOR, &_arg);
}

void netmgr_wifi_register_mgnt_monitor_cb(netmgr_hdl_t hdl, monitor_data_cb_t fn)
{
    netmgr_wifi_reg_mgnt_monitor_cb_syscall_arg_t _arg;
    _arg.hdl = hdl;
    _arg.fn = fn;
    SYSCALL(SYS_NETMGR_WIFI_REGISTER_MGNT_MONITOR_CB, &_arg);
}

int netmgr_wifi_send_80211_raw_frame(netmgr_hdl_t hdl, uint8_t *buf, int len)
{
    netmgr_wifi_send_80211_raw_frame_syscall_arg_t _arg;
    _arg.hdl = hdl;
    _arg.buf = buf;
    _arg.len = len;
    return (int)SYSCALL(SYS_NETMGR_WIFI_SEND_80211_RAW_FRAME, &_arg);
}

#endif


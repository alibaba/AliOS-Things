/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef NETMGR_SYSCALL_ARG_H
#define NETMGR_SYSCALL_ARG_H
#include <stdint.h>
#include "netmgr.h"
#include "netmgr_wifi.h"
#include "hal2vfs/io_wifi.h"

typedef struct {
    const char *name;
} netmgr_get_dev_syscall_arg_t;

typedef struct {
    netmgr_hdl_t hdl;
    netmgr_ifconfig_info_t *info;
} netmgr_set_ifconfig_syscall_arg_t;

typedef struct {
    netmgr_hdl_t hdl;
    netmgr_ifconfig_info_t *info;
} netmgr_get_ifconfig_syscall_arg_t;

typedef struct {
    netmgr_hdl_t hdl;
    bool enable;
} netmgr_set_auto_reconnect_syscall_arg_t;

typedef struct {
    netmgr_hdl_t hdl;
    netmgr_config_t *config;
} netmgr_get_config_syscall_arg_t;

typedef struct {
    netmgr_hdl_t hdl;
    netmgr_del_config_t *config;
} netmgr_del_config_syscall_arg_t;

typedef struct {
    netmgr_hdl_t hdl;
} netmgr_get_state_syscall_arg_t;

typedef struct {
    netmgr_hdl_t hdl;
    netmgr_connect_params_t *params;
} netmgr_connect_syscall_arg_t;

typedef struct {
    netmgr_hdl_t hdl;
    netmgr_msg_cb_t cb;
} netmgr_set_msg_cb_syscall_arg_t;

typedef struct {
    netmgr_hdl_t hdl;
    netmgr_msg_cb_t cb;
} netmgr_del_msg_cb_syscall_arg_t;

typedef struct {
    netmgr_hdl_t hdl;
} netmgr_disconnect_syscall_arg_t;

typedef struct {
    netmgr_hdl_t hdl;
} netmgr_save_config_syscall_arg_t;

typedef struct {
    netmgr_hdl_t hdl;
    netmgr_connect_params_t *params;
} netmgr_set_connect_params_syscall_arg_t;

typedef struct {
    netmgr_hdl_t hdl;
    netmgr_wifi_ap_list_t *ap_info;
    int num;
    netmgr_wifi_scan_type_t type;
} netmgr_wifi_scan_result_syscall_arg_t;

typedef struct {
    netmgr_hdl_t hdl;
} netmgr_wifi_start_monitor_syscall_arg_t;

typedef struct {
    netmgr_hdl_t hdl;
} netmgr_wifi_stop_monitor_syscall_arg_t;

typedef struct {
    netmgr_hdl_t hdl;
} netmgr_wifi_start_mgnt_monitor_syscall_arg_t;

typedef struct {
    netmgr_hdl_t hdl;
} netmgr_wifi_stop_mgnt_monitor_syscall_arg_t;

typedef struct {
    netmgr_hdl_t hdl;
    monitor_data_cb_t fn;
} netmgr_wifi_reg_mgnt_monitor_cb_syscall_arg_t;

typedef struct {
    netmgr_hdl_t hdl;
    uint8_t *buf;
    int len;
} netmgr_wifi_send_80211_raw_frame_syscall_arg_t;
#endif /* NETMGR_SYSCALL_ARG_H */


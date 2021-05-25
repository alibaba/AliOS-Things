/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _BLE_NETCFG_H_
#define _BLE_NETCFG_H_

typedef enum {
    BLE_NETCFG_EVENT_NONE,
    BLE_NETCFG_EVENT_STARTED,
    BLE_NETCFG_EVENT_STOPPED,
    BLE_NETCFG_EVENT_CONNECTED,
    BLE_NETCFG_EVENT_SUCCESS,
    BLE_NETCFG_EVENT_GOT_IP,
    BLE_NETCFG_EVENT_FAILED,
}BLE_NETCFG_EVENT;

typedef enum {
    BLE_NETCFG_SUCCESS,
    BLE_NETCFG_COMMON_FAILED,
    BLE_NETCFG_ALREADY,
    BLE_NETCFG_UNKNOW,
    BLE_NETCFG_WRONG_INFO,
}BLE_NETCFG_STATE;

typedef enum {
    BLE_NETCFG_DHCP_SUCCESS = 0,
    BLE_NETCFG_DHCP_FAIL,
    BLE_NETCFG_PING_SUCCESS,
    BLE_NETCFG_PING_FAIL,
} BLE_NETCFG_RES;

typedef int (*BLE_netCfg_callck)(BLE_NETCFG_EVENT event, BLE_NETCFG_STATE state, void *event_data);

BLE_NETCFG_STATE BLE_NetCfg_init(BLE_netCfg_callck callback);
BLE_NETCFG_STATE BLE_NetCfg_start(void);
BLE_NETCFG_STATE BLE_NetCfg_stop(void);
BLE_NETCFG_STATE BLE_NetCfg_notificate(const uint8_t *data, uint16_t size);

BLE_NETCFG_STATE BLE_NetCfg_wifi_get(char **ssid, char **passwd);
BLE_NETCFG_STATE BLE_NetCfg_wifi_set(char *ssid, char *passwd);
BLE_NETCFG_STATE BLE_NetCfg_devinfo_get(char **pk, char **dn, char **ds);
BLE_NETCFG_STATE BLE_NetCfg_devinfo_set(char *pk, char *dn, char *ds);

#endif

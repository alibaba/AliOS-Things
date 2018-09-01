/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _BE_OSAL_NET_H
#define _BE_OSAL_NET_H

#if defined(__cplusplus)
extern "C" {
#endif

#define MAX_SSID_SIZE 32
#define MAX_PWD_SIZE 64
#ifndef ETH_ALEN
#define ETH_ALEN (6)
#endif
#ifndef MAX_SSID_LEN
#define MAX_SSID_LEN (MAX_SSID_SIZE + 1)
#endif

#ifdef BE_OS_AOS
#include "netmgr.h"
typedef netmgr_ap_config_t be_osal_wifi_config_t;
#else
typedef struct {
    char ssid[MAX_SSID_SIZE + 1];
    char bssid[ETH_ALEN];
    char pwd[MAX_PWD_SIZE + 1];
} be_osal_wifi_config_t;
#endif

typedef enum {
    BE_WIFI_RET_OK = 0,
    BE_WIFI_NOT_INIT,
    BE_WIFI_PARM_ERR,
} be_wifi_ret_t;

typedef struct {
    char ssid[MAX_SSID_LEN];
    char bssid[ETH_ALEN];
    int auth;
    int encry;
    char channel;
    char rssi;
} be_wifi_scan_result_t;

typedef int (*be_osal_wifi_scan_cb)(be_wifi_scan_result_t** argv);

int be_osal_wifi_connect(be_osal_wifi_config_t* config);
int be_osal_wifi_get_ip(char* ip);
int be_osal_wifi_init();
be_wifi_ret_t be_osal_wifi_scan(be_osal_wifi_scan_cb cb);
int be_osal_wifi_connect(be_osal_wifi_config_t* config);
void be_osal_wifi_suspend(void);
int be_osal_wifi_save_config(be_osal_wifi_config_t *config);


#if defined(__cplusplus)
}
#endif
#endif

/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _AMP_NETWORK_H_
#define _AMP_NETWORK_H_

#include "infra_types.h"

typedef struct sim_info {
    char imsi[32];
    char imei[32];
    char iccid[32];
} sim_info_t;

typedef struct locator_info {
    uint32_t cid;
    char mcc[4];
    char mnc[4];
    uint32_t lac;
} locator_info_t;

typedef struct signal_info {
    int rssi;
} signal_info_t;

typedef struct wifi_info {
    char *ssid;
    unsigned char mac[6];
    char ip[32];
    int rssi;
} wifi_info_t;

int32_t HAL_WiFi_Init();

int32_t HAL_WiFi_Connect(const char *ssid, const char *passwd);

int32_t HAL_WiFi_Get_Info(wifi_info_t *wifi_info);

int32_t HAL_Get_ip(char *ip);

int32_t HAL_Get_mac(unsigned char mac[8]);

int32_t HAL_Get_network_status(void);

int32_t HAL_WiFi_Disconnect();

int32_t HAL_Get_SimInfo(sim_info_t *sim_info);

int32_t HAL_Get_LocatorInfo(locator_info_t *locator_info);

int32_t HAL_Get_SignalInfo(signal_info_t *signal_info);

int32_t HAL_Get_neighborLocatorInfo(void (*cb)(locator_info_t*, int));

#endif /* _AMP_NETWORK_H_ */


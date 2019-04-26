/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef NETMGR_H
#define NETMGR_H

#if defined(__cplusplus)
extern "C"
{
#endif

#include <stdbool.h>

#ifdef NET_WITH_WIFI
#include "netmgr_wifi.h"
#endif

/**
 *
 * initialize netmgr module
 *
 * @return 0   initialize success
 * @return < 0 initialize fail
 */
int netmgr_init(void);

/**
 *
 * deinitialize netmgr module
 *
 * @return 0   deinitialize success
 * @return < 0 deinitialize fail
 */
void netmgr_deinit(void);

/**
 *
 * start netmgr
 *
 * @param[in] autoconfig start WiFi provision or not
 *
 * @return 0   start WiFi provision success
 * @return < 0 start WiFi provision fail
 */
int netmgr_start(bool autoconfig);

#ifdef NET_WITH_wIFI
/**
 *
 * connect to specified WiFi network with given SSID and password
 *
 * @param[in] ssid     WiFi SSID
 * @param[in] password WiFi password
 *
 * @return 0   begin to connect success
 * @return < 0 begin to connect fail
 */
int32_t netmgr_connect(const char *ssid, const uint8_t *password);
#endif

#if defined(__cplusplus)
}
#endif

#endif /* NETMGR_H */

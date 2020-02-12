/**
 * @file netmgr.h
 * netmgr API header file.
 *
 * @version   V1.0
 * @date      2019-11-08
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef NETMGR_H
#define NETMGR_H

#if defined(__cplusplus)
extern "C"
{
#endif

/** @addtogroup aos_netmgr netmgr
 *  Interfaces of net connection management.
 *
 *  @{
 */

#include <stdbool.h>
#include <stdint.h>

#ifdef AOS_NET_WITH_WIFI
#include "netmgr_wifi.h"
#endif

#ifdef AOS_NET_WITH_CELLULAR
#include "netmgr_cellular.h"
#endif

/**
 * Interface type
 */
enum {
    INTERFACE_WIFI,
    INTERFACE_CELLULAR,
    /* add other type here */
    INTERFACE_INDEX_MAX,
};

#define IP_STR_SIZE 32

/**
 * Netmgr status (e.g. ip availability and ip address) type
 */
typedef struct netmgr_stats_s {
    bool ip_available;
    char ip[IP_STR_SIZE];
} netmgr_stats_t;

/**
 *
 * Initialize netmgr module
 *
 * @return 0 on success, negative on failure
 */
int netmgr_init(void);

/**
 *
 * Deinitialize netmgr module
 *
 * @return none
 */
void netmgr_deinit(void);

/**
 *
 * Start netmgr module
 *
 * @param[in] autoconfig start WiFi provision or not
 *
 * @return 0 on success, negative on failure
 *
 */
int netmgr_start(bool autoconfig);

/**
 *
 * Get netmgr stats
 *
 * @param[in]     interface interface name
 * @param[in/out] stats network interface stats
 *
 * @return 0 on success, negative on failure
 *
 */
int netmgr_stats(int32_t interface, netmgr_stats_t *stats);

#ifdef AOS_NET_WITH_WIFI
/**
 *
 * Connect to specified WiFi network with given SSID and password
 *
 * @param[in] ssid     WiFi SSID
 * @param[in] password WiFi password
 * @param[in] timeout  timeout in milliseconds
 *
 * @return  0 on connect success, -1 on connect param error, -2 on connect timeout
 *
 */
int32_t netmgr_connect(const char *ssid, const uint8_t *password, uint32_t timeout);
#endif

/** @} */

#if defined(__cplusplus)
}
#endif

#endif /* NETMGR_H */

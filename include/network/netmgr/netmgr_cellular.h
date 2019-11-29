/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef NETMGR_WIFI_H
#define NETMGR_WIFI_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C"
{
#endif

/**
 *
 * @brief Initialize netmgr cellular module
 *
 * @return 0   initialize success
 * @return < 0 initialize fail
 */
int netmgr_cellular_init(void);

/**
 *
 * @brief Deinitialize netmgr cellular module
 */
void netmgr_cellular_deinit(void);

/**
 *
 * @brief Start netmgr cellular
 *
 * @param[in] autoconfig start cellular provision or not
 *
 * @return 0   start network interface success
 * @return < 0 start network interface fail
 */
int netmgr_cellular_start(bool autoconfig);

/**
 *
 * @brief Get netmgr ip stats
 *
 * @return true   ip ready
 * @return false  ip not ready yet
 *
 */
bool netmgr_get_ip_state(void);

#if defined(__cplusplus)
}
#endif

#endif /* NETMGR_WIFI_H */

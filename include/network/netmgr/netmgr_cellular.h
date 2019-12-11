/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef NETMGR_CELLULAR_H
#define NETMGR_CELLULAR_H

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
 * @return 0   start network interface success
 * @return < 0 start network interface fail
 */
int netmgr_cellular_start(void);

/**
 *
 * @brief Get netmgr ip stats
 *
 * @return true   ip ready
 * @return false  ip not ready yet
 *
 */
bool netmgr_cellular_get_ip_state(void);

/**
 * @brief Get ip address
 *
 */
void netmgr_cellular_get_ip(char *ip);

#if defined(__cplusplus)
}
#endif

#endif /* NETMGR_CELLULAR_H */

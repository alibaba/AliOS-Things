/**
 * @file netmgr_cellular.h
 * netmgr cellular API header file.
 *
 * @version   V1.0
 * @date      2019-11-08
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef NETMGR_CELLULAR_H
#define NETMGR_CELLULAR_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C"
{
#endif

/** @addtogroup aos_netmgr netmgr_cellular
 *  Interfaces of cellular net connection management.
 *
 *  @{
 */

/**
 *
 * Initialize netmgr cellular module
 *
 * @return 0 on success, negative on failure
 *
 */
int netmgr_cellular_init(void);

/**
 *
 * Deinitialize netmgr cellular module
 *
 * @return none
 */
void netmgr_cellular_deinit(void);

/**
 *
 * Start netmgr cellular
 *
 * @return 0 on success, negative on failure
 */
int netmgr_cellular_start(void);

/**
 *
 * Get netmgr ip stats
 *
 * @return true on ip ready, false on ip not ready yet
 */
bool netmgr_cellular_get_ip_state(void);

/**
 * Get ip address
 *
 * @param[out]  buffer for ip address
 *
 * @return none
 */
void netmgr_cellular_get_ip(char *ip);

/** @} */

#if defined(__cplusplus)
}
#endif

#endif /* NETMGR_CELLULAR_H */

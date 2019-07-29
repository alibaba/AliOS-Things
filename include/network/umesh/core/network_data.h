/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_NETWORK_DATA_H
#define UR_NETWORK_DATA_H

#include <stdbool.h>

#include "ip/ip.h"
#include "utilities/configs.h"
#include "hal/interface_context.h"

enum {
    STABLE_MAIN_VERSION_OFFSET = 5,
    STABLE_MAIN_VERSION_MASK   = 0xe0,
    STABLE_MINOR_VERSION_MASK  = 0x1f,
};

enum {
    MCAST_NUM = 1,
};

typedef struct stable_network_data_s {
    uint8_t main_version: 3;
    uint8_t minor_version: 5;
    uint16_t meshnetid;
    uint8_t channel;
    ur_ip6_addr_t mcast_addr[MCAST_NUM];
} stable_network_data_t;

ur_error_t nd_init(void);

// stable network data
ur_error_t nd_stable_set(stable_network_data_t *stable_network_data);
ur_error_t nd_set_stable_main_version(uint8_t version);
uint8_t nd_get_stable_main_version(void);
uint8_t nd_get_stable_minor_version(void);
uint16_t nd_get_stable_meshnetid(void);
ur_error_t nd_set_stable_meshnetid(uint16_t meshnetid);
const ur_ip6_addr_t *nd_get_subscribed_mcast(void);
bool nd_is_subscribed_mcast(const ur_ip6_addr_t *addr);

// network data
ur_error_t nd_set(network_context_t *network, network_data_t *network_data);
uint8_t nd_get_version(network_context_t *network);
uint16_t nd_get_meshnetsize(network_context_t *network);
ur_error_t nd_set_meshnetsize(network_context_t *network,
                              uint32_t meshnetsize);

#endif  /* UR_NETWORK_DATA_H */

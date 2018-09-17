/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_LWIP_ADAPTER_H
#define UR_LWIP_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lwip/ip.h"

ur_error_t ur_adapter_interface_init(void);

ur_error_t ur_adapter_interface_up(void);
ur_error_t ur_adapter_interface_down(void);
ur_error_t ur_adapter_interface_update(void);
struct netif *ur_adapter_get_netif(void);
const void *ur_adapter_get_default_ipaddr(void);
const void *ur_adapter_get_mcast_ipaddr(void);
ur_error_t ur_adapter_resolve_ip(const void *ip, ur_addr_t *addr);

#ifdef __cplusplus
}
#endif

#endif  /* UR_LWIP_ADAPTER_H */

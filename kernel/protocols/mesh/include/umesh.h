/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UMESH_MESH_H
#define UMESH_MESH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "umesh_types.h"

struct pbuf;
struct message_s;

/* for ip layer */
ur_error_t umesh_ipv4_output(struct pbuf *buf, uint16_t sid);
ur_error_t umesh_ipv6_output(struct pbuf *buf,
                             const ur_ip6_addr_t *ip6addr);

/* for mesh layer */
ur_error_t umesh_init(node_mode_t mode);
ur_error_t umesh_start(void);
bool       umesh_is_initialized(void);
ur_error_t umesh_stop(void);
ur_error_t umesh_register_callback(ur_adapter_callback_t *callback);

/* per device API */
uint8_t umesh_get_device_state(void);
uint8_t umesh_get_mode(void);
ur_error_t umesh_set_mode(uint8_t mode);

const mac_address_t *umesh_get_mac_address(media_type_t type);
uint16_t umesh_get_meshnetid(void);
uint16_t umesh_get_sid(void);
uint16_t umesh_get_meshnetsize(void);

slist_t *umesh_get_nbrs(media_type_t type);

bool umesh_is_mcast_subscribed(const ur_ip6_addr_t *addr);
const ur_netif_ip6_address_t *umesh_get_ucast_addr(void);
const ur_netif_ip6_address_t *umesh_get_mcast_addr(void);

ur_error_t umesh_resolve_dest(const ur_ip6_addr_t *dest,
                              ur_addr_t *dest_addr);

void umesh_get_extnetid(umesh_extnetid_t *extnetid);
ur_error_t umesh_set_extnetid(const umesh_extnetid_t *extnetid);

#ifdef __cplusplus
}
#endif

#endif  /* UMESH_MESH_H */

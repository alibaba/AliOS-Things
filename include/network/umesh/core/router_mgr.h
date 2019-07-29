/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_ROUTER_MGR_H
#define UR_ROUTER_MGR_H

#include "core/topology.h"
#include "core/mesh_forwarder.h"
#include "core/sid_allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    SID_ROUTER = 1,
    VECTOR_ROUTER = 2,
};

enum {
    EVENT_SID_UPDATED = 1,
    EVENT_PREFIX_UPDATED = 2,
};

typedef struct netids_s {
    uint16_t meshnetid;
    uint16_t sid;
} netids_t;

void ur_router_register_module(void);
uint16_t ur_router_get_next_hop(network_context_t *network,
                                  uint16_t dest_sid);
void ur_router_neighbor_updated(neighbor_t *neighbor);
uint8_t ur_router_get_default_router(void);
router_t *ur_get_router_by_id(uint8_t id);
ur_error_t ur_router_set_default_router(uint8_t id);
ur_error_t ur_router_send_message(router_t *router, uint16_t dst,
                                  uint8_t *payload, uint16_t length);
ur_error_t handle_router_message_received(message_t *message);
ur_error_t register_router(router_t *router);

void sid_allocator_init(network_context_t *network);
void sid_allocator_deinit(network_context_t *network);
ur_error_t sid_allocator_alloc(network_context_t *network, ur_node_id_t *node);
ur_error_t sid_allocator_free(network_context_t *network, ur_node_id_t *node);
uint16_t sid_allocator_get_num(network_context_t *network);

#ifdef __cplusplus
}
#endif

#endif /* UR_ROUTER_MGR_H */

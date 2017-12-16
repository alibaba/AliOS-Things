/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>

#include "yunit.h"

#include "aos/kernel.h"

#include "umesh_types.h"
#include "core/mesh_mgmt.h"
#include "core/link_mgmt.h"
#include "core/router_mgr.h"
#include "core/vector_router.h"

extern ur_error_t vector_router_init(void);
extern ur_error_t vector_router_deinit(void);
extern ur_error_t vector_router_neighbor_updated(neighbor_t *neighbor);
extern ur_error_t vector_router_message_received(const uint8_t *data, uint16_t length);
extern ur_error_t vector_router_event_triggered(uint8_t event, uint8_t *data, uint8_t len);
extern ur_error_t vector_router_add_vertex(uint16_t sid);
extern ur_error_t vector_router_update_edge(uint16_t src_sid, uint16_t dst_sid, uint8_t cost);
extern void vector_router_print_routing_table(void);
extern void vector_router_calculate_routing_info(void);
extern uint16_t vector_router_get_next_hop_shortid(uint16_t dest);

void test_uradar_vector_router_case(void)
{
    /* routing algorithm related test */
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_init());
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_add_vertex(2));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_add_vertex(3));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_add_vertex(4));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_add_vertex(5));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_add_vertex(6));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_add_vertex(7));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_add_vertex(8));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_add_vertex(9));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_add_vertex(10));
    YUNIT_ASSERT(UR_ERROR_FAIL == vector_router_add_vertex(10));

    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(1, 2, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(1, 3, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(1, 4, 1));
    YUNIT_ASSERT(UR_ERROR_FAIL == vector_router_update_edge(1, 4, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(2, 3, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(2, 6, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(3, 4, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(3, 5, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(3, 6, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(4, 5, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(4, 9, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(5, 6, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(5, 7, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(5, 8, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(5, 9, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(6, 7, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(7, 8, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(8, 9, 1));

    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(2, 1, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(3, 1, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(4, 1, 1));
    YUNIT_ASSERT(UR_ERROR_FAIL == vector_router_update_edge(4, 1, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(3, 2, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(6, 2, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(4, 3, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(5, 3, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(6, 3, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(5, 4, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(9, 4, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(6, 5, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(7, 5, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(8, 5, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(9, 5, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(7, 6, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(8, 7, 1));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(9, 8, 1));

    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(5, 7, 2));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(5, 8, 255));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_update_edge(7, 8, 2));
    vector_router_print_routing_table();
    vector_router_calculate_routing_info();
    vector_router_print_routing_table();
    YUNIT_ASSERT(2 == vector_router_get_next_hop_shortid(2));
    YUNIT_ASSERT(3 == vector_router_get_next_hop_shortid(3));
    YUNIT_ASSERT(4 == vector_router_get_next_hop_shortid(4));
    YUNIT_ASSERT(3 == vector_router_get_next_hop_shortid(5));
    YUNIT_ASSERT(2 == vector_router_get_next_hop_shortid(6));
    YUNIT_ASSERT(2 == vector_router_get_next_hop_shortid(7));
    YUNIT_ASSERT(4 == vector_router_get_next_hop_shortid(8));
    YUNIT_ASSERT(4 == vector_router_get_next_hop_shortid(9));
    YUNIT_ASSERT(INVALID_SID == vector_router_get_next_hop_shortid(10));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_deinit());

    /* function level uinit test */
    netids_t netids;
    uint8_t  data[512];
    uint16_t sid, len;
    router_command_t *cmd;
    vertex_tv_t      *vertex_tv;
    edge_tv_t        *edge_tv;
    none_tv_t        *none_tv;
    edge_tuple_t     *edge_tuple;
    neighbor_t       neighbor;

    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_init());
    netids.meshnetid = 0x0100;
    netids.sid = LEADER_SID;
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_event_triggered(EVENT_SID_UPDATED, (uint8_t *)&netids, sizeof(netids_t)));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_event_triggered(EVENT_SID_UPDATED, (uint8_t *)&netids, sizeof(netids_t)));
    sid = 0x2000;
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_event_triggered(EVENT_SID_UPDATED, (uint8_t *)&netids, sizeof(netids_t)));
    sid = LEADER_SID;
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_event_triggered(EVENT_SID_UPDATED, (uint8_t *)&netids, sizeof(netids_t)));

    /* heartbeat messages */
    cmd = (router_command_t *)data;
    len = sizeof(router_command_t);
    vertex_tv = (vertex_tv_t *)(data + len);
    len += sizeof(vertex_tv_t);
    cmd->cmd = COMMAND_HEARTBEAT;
    cmd->seq = 1;
    cmd->sid = 0x1000;
    vertex_tv->type = TYPE_VERTEX;
    memset(vertex_tv->ueid, 0x00, sizeof(vertex_tv->ueid));
    vertex_tv->ueid[0] = 0x02;
    vertex_tv->sid = 0x1000;
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_message_received(data, len));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_message_received(data, len));
    edge_tuple = (edge_tuple_t *)(data + len);
    len += sizeof(edge_tuple_t);
    edge_tuple->dst = 0x0000;
    edge_tuple->cost = 16;
    cmd->seq = 2;
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_message_received(data, len));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_message_received(data, len));
    cmd->seq = 1;
    vertex_tv->ueid[0] = 0x04;
    vertex_tv->sid = 0x4000;
    edge_tuple->dst = 0x1000;
    edge_tuple->cost = 16;
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_message_received(data, len));
    /* vertex update message */
    cmd->cmd = COMMAND_VERTEX_UPDATE;
    cmd->seq = 3;
    cmd->sid = 0x1000;
    vertex_tv->sid = 0x1000;
    vertex_tv->ueid[0] = 0x02;
    len = sizeof(router_command_t) + sizeof(vertex_tv_t);
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_message_received(data, len));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_message_received(data, len));

    cmd->cmd = COMMAND_VERTEX_DELETE;
    cmd->seq = 2;
    vertex_tv->ueid[0] = 0x04;
    vertex_tv->sid     = 0x4000;
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_message_received(data, len));
    /* edge update message */
    cmd->cmd = COMMAND_EDGE_UPDATE;
    cmd->seq = 4;
    edge_tv = (edge_tv_t *)(data + sizeof(router_command_t));
    edge_tv->src = 0x1000;
    edge_tv->dst = 0x0000;
    edge_tv->cost = 32;
    len = sizeof(router_command_t) + sizeof(edge_tv_t);
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_message_received(data, len));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_message_received(data, len));
    /* unkonw message */
    cmd->cmd = 0;
    YUNIT_ASSERT(UR_ERROR_FAIL == vector_router_message_received(data, len));
    /* neighbor updated */
    neighbor.sid = 0x1000;
    memset(neighbor.mac, 0x00, sizeof(neighbor.mac));
    neighbor.mac[0] = 0x02;
    neighbor.stats.link_cost = 256;
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_neighbor_updated(&neighbor));
    neighbor.stats.link_cost = LINK_COST_MAX;
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_neighbor_updated(&neighbor));
    neighbor.stats.link_cost = 512;
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_neighbor_updated(&neighbor));
    YUNIT_ASSERT(UR_ERROR_FAIL == vector_router_neighbor_updated(&neighbor));
    neighbor.mac[0] = 0x03;
    YUNIT_ASSERT(UR_ERROR_FAIL == vector_router_neighbor_updated(&neighbor));

    /* topology sync as server */
    cmd->cmd = COMMAND_TOPOLOGY_SYNC_SELECT;
    cmd->seq = 1;
    cmd->sid = 0x2000;
    len = sizeof(router_command_t);
    cmd->seq = 2;
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_message_received(data, len));
    aos_msleep(TOPOLOGY_SYNC_TIMEOUT + TOPOLOGY_SYNC_TIMEOUT / 2);
    cmd->cmd = COMMAND_TOPOLOGY_SYNC_ACK;
    cmd->seq = 1;
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_message_received(data, len));

    aos_msleep(HEARTBEAT_TIMEOUT);
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_deinit());

    /* topology sync as client */
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_init());
    sid = 0x1000;
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_event_triggered(EVENT_SID_UPDATED, (uint8_t *)&sid, 2));
    aos_msleep(STARTUP_TIMEOUT + TOPOLOGY_SYNC_TIMEOUT / 2);

    cmd->cmd = COMMAND_TOPOLOGY_SYNC_DATA;
    cmd->seq = 1;
    cmd->sid = LEADER_SID;
    len = sizeof(router_command_t);
    vertex_tv = (vertex_tv_t *)(data + len);
    vertex_tv->type = TYPE_VERTEX;
    memset(vertex_tv->ueid, 0x00, sizeof(vertex_tv->ueid));
    vertex_tv->ueid[0] = 0x11;
    vertex_tv->sid = 0x0000;
    len += sizeof(vertex_tv_t);
    vertex_tv = (vertex_tv_t *)(data + len);
    vertex_tv->type = TYPE_VERTEX;
    memset(vertex_tv->ueid, 0x00, sizeof(vertex_tv->ueid));
    vertex_tv->ueid[0] = 0x12;
    vertex_tv->sid = 0x2000;
    len += sizeof(vertex_tv_t);
    vertex_tv = (vertex_tv_t *)(data + len);
    vertex_tv->type = TYPE_VERTEX;
    memset(vertex_tv->ueid, 0x00, sizeof(vertex_tv->ueid));
    vertex_tv->ueid[0] = 0x13;
    vertex_tv->sid = 0x3000;
    len += sizeof(vertex_tv_t);
    edge_tv = (edge_tv_t *)(data + len);
    edge_tv->type = TYPE_EDGE;
    edge_tv->src = 0x0000;
    edge_tv->dst = 0x2000;
    edge_tv->cost = 16;
    len += sizeof(edge_tv_t);
    edge_tv = (edge_tv_t *)(data + len);
    edge_tv->type = TYPE_EDGE;
    edge_tv->src = 0x2000;
    edge_tv->dst = 0x0000;
    edge_tv->cost = 16;
    len += sizeof(edge_tv_t);
    edge_tv = (edge_tv_t *)(data + len);
    edge_tv->type = TYPE_EDGE;
    edge_tv->src = 0x2000;
    edge_tv->dst = 0x3000;
    edge_tv->cost = 16;
    len += sizeof(edge_tv_t);
    edge_tv = (edge_tv_t *)(data + len);
    edge_tv->type = TYPE_EDGE;
    edge_tv->src = 0x3000;
    edge_tv->dst = 0x2000;
    edge_tv->cost = 16;
    len += sizeof(edge_tv_t);
    edge_tv = (edge_tv_t *)(data + len);
    edge_tv->type = TYPE_EDGE;
    edge_tv->src = 0x0000;
    edge_tv->dst = 0x3000;
    edge_tv->cost = 16;
    len += sizeof(edge_tv_t);
    edge_tv = (edge_tv_t *)(data + len);
    edge_tv->type = TYPE_EDGE;
    edge_tv->src = 0x3000;
    edge_tv->dst = 0x0000;
    edge_tv->cost = 16;
    len += sizeof(edge_tv_t);
    none_tv = (none_tv_t *)(data + len);
    none_tv->type = TYPE_NONE;
    len += sizeof(none_tv_t);
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_message_received(data, len));
    YUNIT_ASSERT(UR_ERROR_NONE == vector_router_deinit());
}



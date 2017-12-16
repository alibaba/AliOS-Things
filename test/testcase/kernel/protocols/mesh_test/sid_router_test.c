/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>

#include "yunit.h"

#include "core/router_mgr.h"
#include "core/mesh_mgmt.h"
#include "hal/interfaces.h"

void test_uradar_sid_router_case(void)
{
    network_context_t network = {
        .meshnetid = 0x100,
        .attach_state = ATTACH_DONE
    };
    netids_t netids;

    YUNIT_ASSERT(UR_ERROR_NONE == ur_router_set_default_router(SID_ROUTER));
    network.router = ur_get_router_by_id(SID_ROUTER);
    network.router->cb.start();
    netids.meshnetid = network.meshnetid;
    netids.sid = 0x1000;
    network.router->cb.handle_subscribe_event(EVENT_SID_UPDATED, (uint8_t *)&netids, sizeof(netids_t));
    YUNIT_ASSERT(0x1000 == ur_router_get_next_hop(&network, 0x1000));
    YUNIT_ASSERT(0x0000 == ur_router_get_next_hop(&network, 0x0000));
    YUNIT_ASSERT(0x0000 == ur_router_get_next_hop(&network, 0x2000));
    YUNIT_ASSERT(0x0000 == ur_router_get_next_hop(&network, 0x2100));
    YUNIT_ASSERT(0x1100 == ur_router_get_next_hop(&network, 0x1100));
    YUNIT_ASSERT(0x1100 == ur_router_get_next_hop(&network, 0x1110));
    YUNIT_ASSERT(0x1100 == ur_router_get_next_hop(&network, 0x1120));
}



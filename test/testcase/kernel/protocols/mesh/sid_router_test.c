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

    ur_router_register_module();
    YUNIT_ASSERT(UR_ERROR_NONE == ur_router_set_default_router(SID_ROUTER));
    network.router = ur_get_router_by_id(SID_ROUTER);
    YUNIT_ASSERT(UR_ERROR_NONE == ur_router_start(&network));
    ur_router_sid_updated(&network, 0x1000);
    YUNIT_ASSERT(0x1000 == ur_router_get_next_hop(&network, 0x1000));
    YUNIT_ASSERT(0x0000 == ur_router_get_next_hop(&network, 0x0000));
    YUNIT_ASSERT(0x0000 == ur_router_get_next_hop(&network, 0x2000));
    YUNIT_ASSERT(0x0000 == ur_router_get_next_hop(&network, 0x2100));
    YUNIT_ASSERT(0x1100 == ur_router_get_next_hop(&network, 0x1100));
    YUNIT_ASSERT(0x1100 == ur_router_get_next_hop(&network, 0x1110));
    YUNIT_ASSERT(0x1100 == ur_router_get_next_hop(&network, 0x1120));
}



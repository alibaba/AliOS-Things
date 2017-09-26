/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "yunit.h"

#include "aos/aos.h"
#include "aos/kernel.h"

#include "umesh.h"
#include "core/link_mgmt.h"
#include "core/sid_allocator.h"
#include "core/router_mgr.h"
#include "tools/cli.h"

#include "dda_util.h"

static void me_as_leader(bool vector_router)
{
    /* start ourself(agent id 11) */
    cmd_to_agent("mode FIXED");
    if (vector_router)
        cmd_to_agent("router VECTOR_ROUTER");
    else
        cmd_to_agent("router SID_ROUTER");
    cmd_to_agent("start");
    check_cond_wait(umesh_get_device_state() == DEVICE_STATE_LEADER, 10);

    start_node_ext(12, -1, vector_router ? VECTOR_ROUTER : SID_ROUTER, -1);
    check_cond_wait(umesh_get_meshnetsize() == 2, 10);

    check_p2p_str_wait("1000", 12, "testcmd sid", 5);

    YUNIT_ASSERT(umesh_get_device_state() == DEVICE_STATE_LEADER);

    cmd_to_agent("router SID_ROUTER");
    cmd_to_agent("stop");
    stop_node(12);

    aos_msleep(2 * 1000);
}

static void me_as_router(bool vector_router)
{
    start_node_ext(12, MODE_RX_ON, -1, -1);
    check_p2p_str_wait("leader", 12, "testcmd state", 10);

    cmd_to_agent("mode FIXED");
    cmd_to_agent("start");
    check_cond_wait(umesh_get_device_state() == DEVICE_STATE_ROUTER ||
                    umesh_get_device_state() == DEVICE_STATE_SUPER_ROUTER, 10);

    cmd_to_agent("stop");
    stop_node(12);
}

void test_uradar_1hop_case(void)
{
    set_full_rssi(11, 12);
    run_times(me_as_leader(false), 2);
    run_times(me_as_router(false), 2);
    run_times(me_as_leader(true), 2);
    run_times(me_as_router(true), 2);
}

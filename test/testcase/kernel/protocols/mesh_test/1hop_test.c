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
#include "utilities/logging.h"
#include "hal/hals.h"
#include "hal/interface_context.h"
#include "hal/interfaces.h"
#include "tools/cli.h"
#include "ip/lwip_adapter.h"

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
    aos_msleep(2 * 1000);
}

static void me_as_router_lowpower(void)
{
    char ping_cmd[64];
    const ip6_addr_t *myaddr;

    start_node_ext(12, MODE_RX_ON, -1, -1);
    check_p2p_str_wait("leader", 12, "testcmd state", 10);

    umesh_set_mode(0);
    cmd_to_agent("start");
    check_cond_wait(umesh_get_device_state() == DEVICE_STATE_ROUTER, 10);

    aos_msleep(18 * 1000);
    myaddr = ur_adapter_get_default_ipaddr();
    snprintf(ping_cmd, sizeof ping_cmd, "send 12 ping " IP6_ADDR_FMT, IP6_ADDR_DATA(((ur_ip6_addr_t *)myaddr)));
    cmd_to_master(ping_cmd);
    check_p2p_str_wait("1", 12, "testcmd icmp_acked", 30);

    snprintf(ping_cmd, sizeof ping_cmd, "send 12 ping " IP6_ADDR_FMT " 1400", IP6_ADDR_DATA(((ur_ip6_addr_t *)myaddr)));
    cmd_to_master(ping_cmd);
    check_p2p_str_wait("2", 12, "testcmd icmp_acked", 30);

    umesh_stop();
    umesh_set_mode(MODE_RX_ON);
    stop_node(12);
    aos_msleep(2 * 1000);
}

static void me_as_leader_lowpower(void)
{
    char ping_cmd[64];
    const ip6_addr_t *myaddr;

    umesh_set_mode(MODE_RX_ON);
    cmd_to_agent("start");
    check_cond_wait(umesh_get_device_state() == DEVICE_STATE_LEADER, 10);

    start_node_ext(12, 0, -1, -1);
    check_p2p_str_wait("router", 12, "testcmd state", 10);

    aos_msleep(18 * 1000);
    myaddr = ur_adapter_get_default_ipaddr();
    snprintf(ping_cmd, sizeof ping_cmd, "send 12 ping " IP6_ADDR_FMT " 1400", IP6_ADDR_DATA(((ur_ip6_addr_t *)myaddr)));
    cmd_to_master(ping_cmd);

    aos_msleep(6 * 1000);
    cmd_to_master(ping_cmd);

    check_p2p_str_wait("1", 12, "testcmd icmp_acked", 30);

    umesh_stop();
    umesh_set_mode(MODE_RX_ON);
    stop_node(12);
    aos_msleep(2 * 1000);
}

void test_uradar_1hop_case(void)
{
    set_full_rssi(11, 12);
    run_times(me_as_leader(false), 2);
    run_times(me_as_router(false), 2);
    run_times(me_as_leader(true), 2);
    run_times(me_as_router(true), 2);
    run_times(me_as_router_lowpower(), 1);
    run_times(me_as_leader_lowpower(), 1);
}

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
#include "tools/cli.h"

#include "dda_util.h"

static void one_super_router_case(void)
{
    char ping_cmd[64];

    /* topology:
     *   leader        router
     *   151 <---wifi--> 152
     *   ^
     *   |             router
     *   |  <---wifi-->  153
     *   |
     *   |             router
     *   |  <---ble--->  154
     *   |
     *   |             router
     *   |  <---ble--->  155
     *
     */

    set_full_rssi(151, 155);
    cmd_to_agent("stop");

    start_node_ext(151, MODE_SUPER, -1, 3);
    check_p2p_str_wait("leader", 151, "testcmd state", 10);
    check_p2p_str_wait("VECTOR_ROUTER", 151, "testcmd router", 2);

    start_node_ext(152, MODE_RX_ON, -1, 1);
    check_p2p_str_wait("router", 152, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 152, "testcmd router", 2);

    start_node_ext(153, MODE_RX_ON, -1, 1);
    check_p2p_str_wait("router", 153, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 153, "testcmd router", 2);

    start_node_ext(154, MODE_RX_ON, -1, 2);
    check_p2p_str_wait("router", 154, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 154, "testcmd router", 2);

    start_node_ext(155, MODE_RX_ON, -1, 2);
    check_p2p_str_wait("router", 155, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 155, "testcmd router", 2);

    char *ipaddr = dda_p2p_req_and_wait(151, "testcmd ipaddr", 5);
    YUNIT_ASSERT(ipaddr != NULL);
    if (ipaddr) {
        snprintf(ping_cmd, sizeof ping_cmd, "send 155 ping %s", ipaddr);
        aos_free(ipaddr);

        cmd_to_master(ping_cmd);
        check_p2p_str_wait("1", 155, "testcmd icmp_acked", 5);
    }

    stop_node(151);
    stop_node(152);
    stop_node(153);
    stop_node(154);
    stop_node(155);
}

static void two_super_router_case(void)
{
    char ping_cmd[64];

    /* topology:
     *   leader        router
     *   151 <----------------> 152
     *   |                      |
     *   |             router   |
     *   |  <-----wifi----> 153 |
     *   |                      |
     *   |             router   |
     *   |  <-----wifi----> 154 |
     *   |                      |
     *   |             router   |
     *   |  <-----ble-----> 155 |
     *   |                      |
     *   |             router   |
     *   |  <-----ble-----> 156 |
     *
     */

    set_full_rssi(151, 156);
    cmd_to_agent("stop");

    start_node_ext(151, MODE_SUPER, -1, 3);
    check_p2p_str_wait("leader", 151, "testcmd state", 10);
    check_p2p_str_wait("VECTOR_ROUTER", 151, "testcmd router", 2);

    start_node_ext(152, MODE_SUPER, -1, 3);
    check_p2p_str_wait("super_router", 152, "testcmd state", 10);
    check_p2p_str_wait("VECTOR_ROUTER", 152, "testcmd router", 2);

    start_node_ext(153, MODE_RX_ON, -1, 1);
    check_p2p_str_wait("router", 153, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 153, "testcmd router", 2);

    start_node_ext(154, MODE_RX_ON, -1, 1);
    check_p2p_str_wait("router", 154, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 154, "testcmd router", 2);

    start_node_ext(155, MODE_RX_ON, -1, 2);
    check_p2p_str_wait("router", 155, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 155, "testcmd router", 2);

    start_node_ext(156, MODE_RX_ON, -1, 2);
    check_p2p_str_wait("router", 156, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 156, "testcmd router", 2);

    char *ipaddr = dda_p2p_req_and_wait(151, "testcmd ipaddr", 5);
    YUNIT_ASSERT(ipaddr != NULL);
    if (ipaddr) {
        snprintf(ping_cmd, sizeof ping_cmd, "send 156 ping %s", ipaddr);
        aos_free(ipaddr);

        cmd_to_master(ping_cmd);
        check_p2p_str_wait("1", 156, "testcmd icmp_acked", 5);
    }

    stop_node(151);
    stop_node(152);
    stop_node(153);
    stop_node(154);
    stop_node(155);
    stop_node(156);
}

void test_uradar_topo_case(void)
{
    run_times(one_super_router_case(), 1);
    run_times(two_super_router_case(), 1);
}

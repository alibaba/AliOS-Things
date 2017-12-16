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
#include "ip/lwip_adapter.h"

#include "dda_util.h"

static void topology_line_case(void)
{
    char ping_cmd[64];
    const ip6_addr_t *myaddr;

    /* topology:
     *   router     super       super    router
     *   11 <------> 12  <===>  13 <---> 14
     */
    set_line_rssi(11, 14);
    cmd_to_agent("stop");

    start_node_ext(13, MODE_SUPER | MODE_RX_ON, -1, -1);
    check_p2p_str_wait("leader", 13, "testcmd state", 10);
    check_p2p_str_wait("VECTOR_ROUTER", 13, "testcmd router", 2);

    start_node_ext(12, MODE_SUPER | MODE_RX_ON, -1, -1);
    check_p2p_str_wait("super_router", 12, "testcmd state", 10);
    check_p2p_str_wait("VECTOR_ROUTER", 12, "testcmd router", 2);

    start_node_ext(14, MODE_MOBILE | MODE_RX_ON, -1, -1);
    check_p2p_str_wait("leaf", 14, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 14, "testcmd router", 2);

    umesh_set_mode(MODE_MOBILE | MODE_RX_ON);
    cmd_to_agent("start");
    check_cond_wait((DEVICE_STATE_LEAF == umesh_mm_get_device_state()), 15);
    YUNIT_ASSERT(ur_router_get_default_router() == SID_ROUTER);

    myaddr = ur_adapter_get_default_ipaddr();
    snprintf(ping_cmd, sizeof ping_cmd, "send 14 ping " IP6_ADDR_FMT, IP6_ADDR_DATA(((ur_ip6_addr_t *)myaddr)));
    cmd_to_master(ping_cmd);
    check_p2p_str_wait("1", 14, "testcmd icmp_acked", 5);

    stop_node(12);
    stop_node(13);
    stop_node(14);
    cmd_to_agent("stop");
}

static void dual_if_topology_line_case(void)
{
    char ping_cmd[64];

    /* topology:
     *   ble       wifi/ble  wifi/ble  ble
     *   151 <------> 152  <===>  153 <---> 154
     */
    set_line_rssi(151, 154);
    cmd_to_agent("stop");

    start_node_ext(152, MODE_SUPER | MODE_RX_ON, -1, 3);
    check_p2p_str_wait("leader", 152, "testcmd state", 10);
    check_p2p_str_wait("VECTOR_ROUTER", 152, "testcmd router", 2);

    start_node_ext(153, MODE_SUPER | MODE_RX_ON, -1, 3);
    check_p2p_str_wait("super_router", 153, "testcmd state", 10);
    check_p2p_str_wait("VECTOR_ROUTER", 153, "testcmd router", 2);

    start_node_ext(151, MODE_MOBILE | MODE_RX_ON, -1, 2);
    check_p2p_str_wait("leaf", 151, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 151, "testcmd router", 2);

    start_node_ext(154, MODE_MOBILE | MODE_RX_ON, -1, 2);
    check_p2p_str_wait("leaf", 154, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 154, "testcmd router", 2);

    char *ipaddr = dda_p2p_req_and_wait(154, "testcmd ipaddr", 5);
    YUNIT_ASSERT(ipaddr != NULL);
    if (ipaddr) {
        snprintf(ping_cmd, sizeof ping_cmd, "send 151 ping %s", ipaddr);
        aos_free(ipaddr);

        cmd_to_master(ping_cmd);
        check_p2p_str_wait("1", 151, "testcmd icmp_acked", 5);
    }

    stop_node(151);
    stop_node(152);
    stop_node(153);
    stop_node(154);
    cmd_to_agent("stop");
}

void test_uradar_layer_routing_2mobile_case(void)
{
    run_times(topology_line_case(), 1);
    run_times(dual_if_topology_line_case(), 1);
}

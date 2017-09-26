/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "yunit.h"

#include "umesh.h"
#include "core/link_mgmt.h"
#include "utilities/logging.h"
#include "utilities/message.h"
#include "tools/cli.h"

#include "dda_util.h"

void test_uradar_mcast_case(void)
{
    neighbor_t *attach_node;
    set_line_rssi(11, 14);

    cmd_to_agent("stop");
    start_node_ext(12, MODE_RX_ON, -1, -1);
    check_p2p_str_wait("leader", 12, "testcmd state", 10);
    start_node_ext(13, MODE_RX_ON, -1, -1);
    check_p2p_str_wait("router", 13, "testcmd state", 10);
    start_node_ext(14, MODE_RX_ON | MODE_MOBILE, -1, -1);
    check_p2p_str_wait("leaf", 14, "testcmd state", 10);

    cmd_to_agent("mode FIXED");
    cmd_to_agent("start");
    check_cond_wait(!!(attach_node = umesh_mm_get_attach_node()), 15);

    cmd_to_master("sendall status");

    char ping_cmd[64];
    const ur_netif_ip6_address_t *myaddr;

    myaddr = umesh_get_mcast_addr();
    snprintf(ping_cmd, sizeof ping_cmd, "send 12 autotest " IP6_ADDR_FMT, IP6_ADDR_DATA(myaddr->addr.ip6_addr));
    cmd_to_master(ping_cmd);
    check_p2p_str_wait("3", 12, "testcmd autotest_acked", 10);

    myaddr = umesh_get_ucast_addr();
    snprintf(ping_cmd, sizeof ping_cmd, "send 12 autotest " IP6_ADDR_FMT " 1 1200", IP6_ADDR_DATA(myaddr->addr.ip6_addr));
    cmd_to_master(ping_cmd);
    check_p2p_str_wait("1", 12, "testcmd autotest_acked", 10);

    cmd_to_agent("stop");
    stop_node(14);
    stop_node(13);
    stop_node(12);
}

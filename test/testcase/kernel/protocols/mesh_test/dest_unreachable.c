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
#include "tools/cli.h"
#include "ip/lwip_adapter.h"

#include "dda_util.h"

static void one_layer_case(void)
{
    char ping_cmd[64];
    const ip6_addr_t *myaddr;

    /* topology:
     *   leader     router   router   mobile
     *   11 <------> 12 <---> 13 <---> 14
     */
    set_line_rssi(11, 14);

    umesh_set_mode(MODE_RX_ON);
    cmd_to_agent("start");
    check_cond_wait((DEVICE_STATE_LEADER == umesh_mm_get_device_state()), 15);
    YUNIT_ASSERT(ur_router_get_default_router() == SID_ROUTER);

    start_node_ext(12, MODE_RX_ON, -1, -1);
    check_p2p_str_wait("router", 12, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 12, "testcmd router", 2);

    start_node_ext(13, MODE_RX_ON, -1, -1);
    check_p2p_str_wait("router", 13, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 13, "testcmd router", 2);

    start_node_ext(14, MODE_RX_ON | MODE_MOBILE, -1, -1);
    check_p2p_str_wait("leaf", 14, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 14, "testcmd router", 2);

    myaddr = ur_adapter_get_default_ipaddr();
    snprintf(ping_cmd, sizeof ping_cmd, "send 14 ping " IP6_ADDR_FMT, IP6_ADDR_DATA(((ur_ip6_addr_t *)myaddr)));
    cmd_to_master(ping_cmd);
    check_p2p_str_wait("1", 14, "testcmd icmp_acked", 5);

    /* topology:
     *   leader     router   router
     *   11 <------> 12 <---> 13
     *               |       mobile
     *               |  <---> 14
     */

    set_rssi_ext(IF_WIFI, 13, 14, 0, 0);
    set_rssi_ext(IF_WIFI, 12, 14, 1, 1);

    aos_msleep(WIFI_NEIGHBOR_ALIVE_TIMEOUT + WIFI_ADVERTISEMENT_TIMEOUT);
    myaddr = ur_adapter_get_default_ipaddr();
    snprintf(ping_cmd, sizeof ping_cmd, "send 14 ping " IP6_ADDR_FMT, IP6_ADDR_DATA(((ur_ip6_addr_t *)myaddr)));
    cmd_to_master(ping_cmd);
    snprintf(ping_cmd, sizeof ping_cmd, "send 14 ping " IP6_ADDR_FMT, IP6_ADDR_DATA(((ur_ip6_addr_t *)myaddr)));
    aos_msleep(WIFI_ADVERTISEMENT_TIMEOUT);
    cmd_to_master(ping_cmd);
    check_p2p_str_wait("2", 14, "testcmd icmp_acked", 10);

    stop_node(12);
    stop_node(13);
    stop_node(14);
    cmd_to_agent("stop");
}


void test_uradar_dest_become_unreachable_case(void)
{
    run_times(one_layer_case(), 1);
}

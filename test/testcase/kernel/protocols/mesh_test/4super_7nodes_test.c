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

/* topology:
 *                     11(super)
 *                 ----------
 *                 |   |    |
 *     ____________|   |    |_________
 *    |                |              |
 *    2(super)---------1(leader)----- 4 (super)
 *    |                |              |
 *    ------           |              |
 *    |     |          |              |
 *    |     |          |              |
 *    5     8          9              10 (mobile)
 *    |
 *    |
 *    6
 *    |
 *    |
 *    7
 *    |
 *    |
 *    3
 */

static void build_topo_wifi(void)
{
    clear_full_rssi(1, 11);

    set_rssi_ext(IF_WIFI, 11, 2, 1, 1);
    set_rssi_ext(IF_WIFI, 11, 1, 1, 1);
    set_rssi_ext(IF_WIFI, 11, 4, 1, 1);
    set_rssi_ext(IF_WIFI, 2, 1, 1, 1);
    set_rssi_ext(IF_WIFI, 1, 4, 1, 1);
    set_rssi_ext(IF_WIFI, 2, 5, 1, 1);
    set_rssi_ext(IF_WIFI, 2, 8, 1, 1);
    set_rssi_ext(IF_WIFI, 5, 6, 1, 1);
    set_rssi_ext(IF_WIFI, 6, 7, 1, 1);
    set_rssi_ext(IF_WIFI, 7, 3, 1, 1);
    set_rssi_ext(IF_WIFI, 1, 9, 1, 1);
    set_rssi_ext(IF_WIFI, 4, 10, 1, 1);
}

/* topology:
 *                     151(super)
 *                 ----------
 *                 |   |    |
 *     ____________|   |    |_________
 *    |                |              |
 *   152(super)------153(leader)------154 (super)
 *    |                |              |
 *    ------           |              |
 *    |     |          |              |
 *    |     |          |              |
 *   155   156        157            158 (mobile)
 *    |
 *    |
 *   159
 *    |
 *    |
 *   160
 *    |
 *    |
 *   161
 */

static void build_topo_wifi_ble(void)
{
    clear_full_rssi(151, 161);

    set_rssi_ext(IF_WIFI, 151, 152, 1, 1);
    set_rssi_ext(IF_WIFI, 151, 153, 1, 1);
    set_rssi_ext(IF_WIFI, 151, 154, 1, 1);
    set_rssi_ext(IF_WIFI, 152, 153, 1, 1);
    set_rssi_ext(IF_WIFI, 153, 154, 1, 1);
    set_rssi_ext(IF_BLE, 152, 155, 1, 1);
    set_rssi_ext(IF_BLE, 152, 156, 1, 1);
    set_rssi_ext(IF_BLE, 155, 159, 1, 1);
    set_rssi_ext(IF_BLE, 159, 160, 1, 1);
    set_rssi_ext(IF_BLE, 160, 161, 1, 1);
    set_rssi_ext(IF_BLE, 153, 157, 1, 1);
    set_rssi_ext(IF_BLE, 154, 158, 1, 1);
}

static void subnet_is_wifi_case(void)
{
    char autotest_cmd[64];
    const ip6_addr_t *addr;
    uint8_t index;

    build_topo_wifi();
    umesh_stop();

    start_node_ext(1, MODE_SUPER | MODE_RX_ON, -1, 1);
    check_p2p_str_wait("leader", 1, "testcmd state", 10);
    check_p2p_str_wait("VECTOR_ROUTER", 1, "testcmd router", 2);

    umesh_set_mode(MODE_SUPER | MODE_RX_ON);
    cmd_to_agent("start");
    check_cond_wait((DEVICE_STATE_SUPER_ROUTER == umesh_mm_get_device_state()), 15);
    YUNIT_ASSERT(ur_router_get_default_router() == VECTOR_ROUTER);

    start_node_ext(2, MODE_SUPER | MODE_RX_ON, -1, 1);
    check_p2p_str_wait("super_router", 2, "testcmd state", 10);
    check_p2p_str_wait("VECTOR_ROUTER", 2, "testcmd router", 2);

    start_node_ext(4, MODE_SUPER | MODE_RX_ON, -1, 1);
    check_p2p_str_wait("super_router", 4, "testcmd state", 10);
    check_p2p_str_wait("VECTOR_ROUTER", 4, "testcmd router", 2);

    aos_msleep(15 * 1000);

    start_node_ext(5, MODE_RX_ON, -1, 1);
    check_p2p_str_wait("router", 5, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 5, "testcmd router", 2);

    start_node_ext(6, MODE_RX_ON, -1, 1);
    check_p2p_str_wait("router", 6, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 6, "testcmd router", 2);

    start_node_ext(7, MODE_RX_ON, -1, 1);
    check_p2p_str_wait("router", 7, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 7, "testcmd router", 2);

    start_node_ext(3, MODE_RX_ON, -1, 1);
    check_p2p_str_wait("leaf", 3, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 3, "testcmd router", 2);

    start_node_ext(8, MODE_RX_ON, -1, 1);
    check_p2p_str_wait("router", 8, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 8, "testcmd router", 2);

    start_node_ext(9, MODE_RX_ON, -1, 1);
    check_p2p_str_wait("router", 9, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 9, "testcmd router", 2);

    start_node_ext(10, MODE_MOBILE | MODE_RX_ON, -1, 1);
    check_p2p_str_wait("leaf", 10, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 10, "testcmd router", 2);

    addr = ur_adapter_get_mcast_ipaddr();
    for (index = 1; index < 12; index++) {
        if (index != 3 || index !=10 || index != 5) {
            continue;
        }
        snprintf(autotest_cmd, sizeof autotest_cmd, "send %d autotest " IP6_ADDR_FMT " 1 500",
                 index, IP6_ADDR_DATA(((ur_ip6_addr_t *)addr)));
        cmd_to_master(autotest_cmd);
        check_p2p_str_wait("10", index, "testcmd autotest_acked", 10);
    }

    for (index = 1; index < 11; index++) {
        stop_node(index);
    }
    cmd_to_agent("stop");
    aos_msleep(1 * 1000);
}

static void subnet_is_ble_case(void)
{
    char autotest_cmd[64];
    uint8_t index;
    uint8_t counter;

    build_topo_wifi_ble();

    umesh_stop();

    start_node_ext(153, MODE_SUPER | MODE_RX_ON, -1, 3);
    check_p2p_str_wait("leader", 153, "testcmd state", 10);
    check_p2p_str_wait("VECTOR_ROUTER", 153, "testcmd router", 2);

    start_node_ext(151, MODE_SUPER | MODE_RX_ON, -1, 3);
    check_p2p_str_wait("super_router", 151, "testcmd state", 10);
    check_p2p_str_wait("VECTOR_ROUTER", 151, "testcmd router", 2);

    start_node_ext(152, MODE_SUPER | MODE_RX_ON, -1, 3);
    check_p2p_str_wait("super_router", 152, "testcmd state", 10);
    check_p2p_str_wait("VECTOR_ROUTER", 152, "testcmd router", 2);

    start_node_ext(154, MODE_SUPER | MODE_RX_ON, -1, 3);
    check_p2p_str_wait("super_router", 154, "testcmd state", 10);
    check_p2p_str_wait("VECTOR_ROUTER", 154, "testcmd router", 2);

    aos_msleep(15 * 1000);

    start_node_ext(155, MODE_RX_ON, -1, 2);
    check_p2p_str_wait("router", 155, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 155, "testcmd router", 2);

    start_node_ext(159, MODE_RX_ON, -1, 2);
    check_p2p_str_wait("router", 159, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 159, "testcmd router", 2);

    start_node_ext(160, MODE_RX_ON, -1, 2);
    check_p2p_str_wait("router", 160, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 160, "testcmd router", 2);

    start_node_ext(161, MODE_RX_ON, -1, 2);
    check_p2p_str_wait("leaf", 161, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 161, "testcmd router", 2);

    start_node_ext(156, MODE_RX_ON, -1, 2);
    check_p2p_str_wait("router", 156, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 156, "testcmd router", 2);

    start_node_ext(157, MODE_RX_ON, -1, 2);
    check_p2p_str_wait("router", 157, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 157, "testcmd router", 2);

    start_node_ext(158, MODE_MOBILE | MODE_RX_ON, -1, 2);
    check_p2p_str_wait("leaf", 158, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 158, "testcmd router", 2);

    char *ipaddr = dda_p2p_req_and_wait(154, "testcmd ipaddr", 5);
    YUNIT_ASSERT(ipaddr != NULL);
    for (index = 151; index < 161; index++) {
        if (index != 153 || index != 158 || index != 159) {
            continue;
        }
        snprintf(autotest_cmd, sizeof autotest_cmd, "send %d autotest %s",
                 index, ipaddr);
        cmd_to_master(autotest_cmd);
        check_p2p_str_wait("1", index, "testcmd autotest_acked", 10);
    }
    aos_free(ipaddr);

    for (index = 151; index < 162; index++) {
        stop_node(index);
    }
    aos_msleep(1 * 1000);
}

void test_umesh_4super_7nodes_case(void)
{
    run_times(subnet_is_wifi_case(), 1);
    run_times(subnet_is_ble_case(), 1);
}

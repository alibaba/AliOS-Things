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

#include "dda_util.h"

static void topology_line_case(bool vector_router)
{
    int i;
    neighbor_t *attach_node;
    uint16_t netid;
    char ping_cmd[64];
    const ur_netif_ip6_address_t *myaddr;
    network_context_t *network;

    /* topology:
     *   11 <------> 12  <---->  13
     */
    set_line_rssi(11, 13);

    cmd_to_agent("stop");
    /* make 7 leader */
    start_node_ext(13, MODE_RX_ON, vector_router ? VECTOR_ROUTER : SID_ROUTER, -1);
    check_p2p_str_wait("leader", 13, "testcmd state", 10);

    start_node_ext(12, MODE_RX_ON, vector_router ? VECTOR_ROUTER : SID_ROUTER, -1);
    check_p2p_str_wait("router", 12, "testcmd state", 10);

    cmd_to_agent("mode FIXED");
    if (vector_router)
        cmd_to_agent("router VECTOR_ROUTER");
    else
        cmd_to_agent("router SID_ROUTER");
    cmd_to_agent("start");
    check_cond_wait(!!(attach_node = umesh_mm_get_attach_node()), 15);

    YUNIT_ASSERT(attach_node != NULL);
    if (!attach_node) {
        goto out;
    }

    netid = umesh_get_meshnetid();

    cmd_to_master("sendall status");
    /* check if ping work */
    aos_msleep(2 * 1000);
    myaddr = umesh_get_ucast_addr();
    snprintf(ping_cmd, sizeof ping_cmd, "send 13 ping " IP6_ADDR_FMT, IP6_ADDR_DATA(myaddr->addr.ip6_addr));
    cmd_to_master(ping_cmd);
    check_p2p_str_wait("1", 13, "testcmd icmp_acked", 5);

    /* kill leader */
    stop_node(13);

    /* wait network id changed, as we expect migration */
    int tmo_ms = 2 * (WIFI_NEIGHBOR_ALIVE_TIMEOUT + WIFI_ADVERTISEMENT_TIMEOUT * MIGRATE_TIMEOUT);
    check_cond_wait(netid != umesh_get_meshnetid() && umesh_get_device_state() == 6,
                    tmo_ms / 1000);

    /* check if ping work */
    aos_msleep(2 * 1000);
    myaddr = umesh_get_ucast_addr();
    snprintf(ping_cmd, sizeof ping_cmd, "send 12 ping " IP6_ADDR_FMT, IP6_ADDR_DATA(myaddr->addr.ip6_addr));
    cmd_to_master(ping_cmd);
    check_p2p_str_wait("1", 12, "testcmd icmp_acked", 5);

    attach_node = umesh_mm_get_attach_node();
    if (attach_node) {
        uint64_t ueid = ueid64(attach_node->mac);
        YUNIT_ASSERT(ueid == 12);
        YUNIT_ASSERT(umesh_get_meshnetid() == attach_node->netid);
        goto out;
    }

    /* we become leader, check sids */
    network = get_default_network_context();
    uint16_t num = get_allocated_number(network->sid_base);
    uint32_t sids_bitmap = get_allocated_bitmap(network->sid_base);
    neighbor_t *node = NULL;
    int found_12 = 0;
    YUNIT_ASSERT(num == 1);
    for (i=0;i<16 && num > 0;i++) {
        if (!(sids_bitmap & (1 << i)))
            continue;

        num --;
        node = get_neighbor_by_sid(NULL, i, umesh_get_meshnetid());
        if (!node)
            continue;

        uint64_t ueid = ueid64(node->mac);
        printf("node sid=%04x ueid=%llx\n", node->sid, ueid);
        if (ueid == 12)
            found_12 = 1;
    }
    YUNIT_ASSERT(found_12);

out:
    stop_node(12);
    stop_node(13);
    cmd_to_agent("stop");
}

void test_uradar_topology_line_case(void)
{
    run_times(topology_line_case(true), 1);
    run_times(topology_line_case(false), 1);
}

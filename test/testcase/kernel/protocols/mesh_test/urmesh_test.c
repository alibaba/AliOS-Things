/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "yunit.h"

#include "aos/kernel.h"
#include "umesh.h"
#include "core/topology.h"
#include "core/mesh_mgmt.h"
#include "hal/interfaces.h"
#include "hal/interface_context.h"
#include "ip/lwip_adapter.h"

#include "dda_util.h"

void test_uradar_urmesh_case(void)
{
    ur_ip6_addr_t ip6addr;

    YUNIT_ASSERT(INVALID_SID == umesh_get_sid());
    YUNIT_ASSERT(UR_ERROR_NONE == umesh_start());
    check_cond_wait(umesh_get_device_state() == DEVICE_STATE_LEADER, 5);

    YUNIT_ASSERT(UR_ERROR_NONE == umesh_stop());
    YUNIT_ASSERT(UR_ERROR_NONE == umesh_set_mode(MODE_MOBILE));
    YUNIT_ASSERT(MODE_MOBILE == umesh_get_mode());
    YUNIT_ASSERT(NULL != umesh_get_mac_address(MEDIA_TYPE_DFL));
    umesh_set_mode(MODE_RX_ON);
    YUNIT_ASSERT(MODE_RX_ON == umesh_get_mode());
    YUNIT_ASSERT(UR_ERROR_NONE == umesh_start());
    check_cond_wait(umesh_get_device_state() >= DEVICE_STATE_LEAF, 10);

    memset(ip6addr.m8, 0x00, sizeof(ip6addr.m8));
    ip6addr.m8[0] = 0xff;
    ip6addr.m8[1] = 0x08;
    ip6addr.m8[6] = (umesh_get_meshnetid() >> 8);
    ip6addr.m8[7] = umesh_get_meshnetid();
    ip6addr.m8[15] = 0xfc;
    YUNIT_ASSERT(true == umesh_is_mcast_subscribed(&ip6addr));

    YUNIT_ASSERT(NULL != ur_adapter_get_default_ipaddr());
    YUNIT_ASSERT(NULL != ur_adapter_get_mcast_ipaddr());
    YUNIT_ASSERT(UR_ERROR_NONE == umesh_stop());
}

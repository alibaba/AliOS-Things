/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "yunit.h"

#include "umesh.h"
#include "core/link_mgmt.h"
#include "core/mesh_mgmt.h"
#include "core/mesh_forwarder.h"
#include "hal/interfaces.h"

#define INVALID_UEID "\xff\xff\xff\xff\xff\xff\xff\xff"

extern void hal_arch_time_msleep(int ms);
extern void ur_ut_send_cmd_to_ddm(const char *cmd);

static ur_error_t dummy_interface_up(void)
{
    return UR_ERROR_NONE;
}

static ur_error_t dummy_interface_down(void)
{
    return UR_ERROR_NONE;
}

void test_uradar_mesh_mgmt_case(void)
{
    mm_cb_t mm_cb;
    mm_cb.interface_up = dummy_interface_up;
    mm_cb.interface_down = dummy_interface_down;

    interface_start();
    YUNIT_ASSERT(UR_ERROR_NONE == umesh_mm_start(&mm_cb));
    YUNIT_ASSERT(UR_ERROR_NONE == umesh_mm_stop());
    YUNIT_ASSERT(DEVICE_STATE_DISABLED == umesh_mm_get_device_state());
    YUNIT_ASSERT(UR_ERROR_NONE == umesh_mm_init(MODE_RX_ON));
    YUNIT_ASSERT(BCAST_SID == umesh_mm_get_local_sid());
    uint8_t *ueid;
    YUNIT_ASSERT_PTR_NOT_NULL((ueid = umesh_mm_get_local_ueid()));
    YUNIT_ASSERT(0 != memcmp(ueid, INVALID_UEID, 8));
    uint16_t num;
    YUNIT_ASSERT_PTR_NULL(get_neighbors(&num));
    YUNIT_ASSERT(0 == num);
    YUNIT_ASSERT_PTR_NULL(get_neighbors(NULL));

    mac_address_t mac_addr;
    mac_addr.len = sizeof(mac_addr.addr);
    memset(mac_addr.addr, 0x00, sizeof(mac_addr.addr));
    mac_addr.addr[0] = 0x03;
    YUNIT_ASSERT_PTR_NULL(get_neighbor_by_mac_addr((const uint8_t *)&mac_addr));
    YUNIT_ASSERT_PTR_NULL(get_neighbor_by_sid(NULL, 0x1200, umesh_mm_get_meshnetid(NULL)));
    uint8_t ueid1[8] = {0};
    YUNIT_ASSERT_PTR_NULL(get_neighbor_by_mac_addr(ueid1));
    YUNIT_ASSERT(0 != umesh_mm_get_meshnetid(NULL));
    YUNIT_ASSERT_PTR_NOT_NULL(umesh_mm_get_mac_address());
    YUNIT_ASSERT(UR_ERROR_NONE == umesh_mm_set_mode(MODE_MOBILE));
    YUNIT_ASSERT(MODE_MOBILE == umesh_mm_get_mode());
    YUNIT_ASSERT(UR_ERROR_NONE == umesh_mm_set_mode(MODE_RX_ON));
    YUNIT_ASSERT(MODE_RX_ON == umesh_mm_get_mode());
    YUNIT_ASSERT_PTR_NULL(umesh_mm_get_attach_node());

    uint8_t tlvs[10];
    uint8_t length = 10;
    umesh_mm_init_tv_base((mm_tv_t *)tlvs, TYPE_VERSION);
    YUNIT_ASSERT_PTR_NOT_NULL(umesh_mm_get_tv(tlvs, length, TYPE_VERSION));
    umesh_mm_init_tlv_base((mm_tlv_t *)tlvs, TYPE_FORWARD_RSSI, 1);

    if (umesh_mm_get_device_state() != DEVICE_STATE_LEADER) {
        YUNIT_ASSERT(0x0000 != umesh_mm_get_meshnetid(NULL));
    }

    YUNIT_ASSERT(UR_ERROR_NONE == umesh_mm_stop());
    YUNIT_ASSERT(UR_ERROR_NONE == umesh_mm_start(&mm_cb));
    YUNIT_ASSERT(UR_ERROR_NONE == umesh_mm_stop());
    interface_stop();
}



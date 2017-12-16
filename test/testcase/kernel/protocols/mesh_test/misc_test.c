/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "yunit.h"

#include "core/mesh_mgmt.h"
#include "utilities/mac_whitelist.h"
#include "utilities/message.h"

void test_uradar_misc_case(void)
{
    message_t *message;
    message = message_alloc(100, UT_MSG);
    YUNIT_ASSERT(UR_ERROR_FAIL == message_set_msglen(NULL, 100));
    YUNIT_ASSERT(UR_ERROR_NONE == message_set_msglen(message, 100));
    YUNIT_ASSERT(UR_ERROR_FAIL == message_set_buflen(NULL, 100));
    YUNIT_ASSERT(UR_ERROR_NONE == message_set_buflen(message, 100));
    YUNIT_ASSERT(0 == message_get_buflen(NULL));
    YUNIT_ASSERT(100 == message_get_buflen(message));

    message_t *message2, *message3;
    message2 = message_alloc(30, UT_MSG);
    message3 = message_alloc(30, UT_MSG);
    YUNIT_ASSERT(UR_ERROR_FAIL == message_concatenate(NULL, message2, false));
    YUNIT_ASSERT(UR_ERROR_NONE == message_concatenate(message, message2, false));
    YUNIT_ASSERT(UR_ERROR_NONE == message_concatenate(message, message3, false));
    message_free(message);

    extern ur_error_t nd_set_stable_meshnetid(uint16_t meshnetid);
    extern uint16_t nd_get_stable_meshnetid(void);
    extern uint8_t nd_get_version(network_context_t *network);
    if (umesh_mm_get_device_state() == DEVICE_STATE_LEADER) {
        YUNIT_ASSERT(UR_ERROR_NONE == nd_set_stable_meshnetid(0x1000));
        YUNIT_ASSERT(0x1000 == nd_get_stable_meshnetid());
        YUNIT_ASSERT(0 != nd_get_version(NULL));
    } else {
        YUNIT_ASSERT(UR_ERROR_FAIL == nd_set_stable_meshnetid(0x1000));
    }

    YUNIT_ASSERT_PTR_NOT_NULL(whitelist_get_entries());
    mac_address_t mac_addr;
    whitelist_entry_t *entry;
    int8_t rssi;
    mac_addr.len = sizeof(mac_addr.addr);
    memset(mac_addr.addr, 0x00, mac_addr.len);
    mac_addr.addr[0] = 0x12;
    YUNIT_ASSERT_PTR_NOT_NULL(whitelist_add(&mac_addr));
    entry = (whitelist_entry_t *)whitelist_get_entries();
    YUNIT_ASSERT_PTR_NOT_NULL(entry);
    whitelist_clear_constant_rssi(entry);
    YUNIT_ASSERT(UR_ERROR_FAIL == whitelist_get_constant_rssi(entry, &rssi));
}



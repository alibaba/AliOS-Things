/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <yunit.h>
#include <aos/aos.h>
#include <aos/kernel.h>

#include "core/mesh_mgmt.h"
#include "tools/diags.h"
#include "hal/interfaces.h"

void test_diags_case(void)
{
    network_context_t *network;
    ur_addr_t dest;
    message_t *message;
    mm_header_t *mm_header;
    mm_timestamp_tv_t *timestamp;
    message_info_t *info;
    uint8_t *data;
    uint8_t *data_orig;
    uint16_t length;

    interface_start();
    network = get_default_network_context();
    dest.netid = 0x100;
    dest.addr.len = SHORT_ADDR_SIZE;
    dest.addr.short_addr = 0x1000;
    YUNIT_ASSERT(UR_ERROR_NONE == send_trace_route_request(network, &dest));

    length = sizeof(mm_header_t) + sizeof(mm_timestamp_tv_t);
    data = (uint8_t *)ur_mem_alloc(length);
    data_orig = data;
    data += sizeof(mm_header_t);

    timestamp = (mm_timestamp_tv_t *)data;
    umesh_mm_init_tv_base((mm_tv_t *)timestamp, TYPE_TIMESTAMP);
    timestamp->timestamp = 10;
    data += sizeof(mm_timestamp_tv_t);

    message = mf_build_message(MESH_FRAME_TYPE_CMD, COMMAND_TRACE_ROUTE_RESPONSE,
                               data_orig, length, UT_MSG);
    info = message->info;
    info->network = network;
    memcpy(&info->dest, &dest, sizeof(info->dest));

    YUNIT_ASSERT(UR_ERROR_NONE == handle_diags_command(message, true));
    message_free(message);
    ur_mem_free(data_orig, length);

    length = sizeof(mm_header_t) + sizeof(mm_timestamp_tv_t);
    data = (uint8_t *)ur_mem_alloc(length);
    data_orig = data;
    data += sizeof(mm_header_t);
    timestamp = (mm_timestamp_tv_t *)data;
    umesh_mm_init_tv_base((mm_tv_t *)timestamp, TYPE_TIMESTAMP);
    timestamp->timestamp = ur_get_now();
    data += sizeof(mm_timestamp_tv_t);

    message = mf_build_message(MESH_FRAME_TYPE_CMD, COMMAND_TRACE_ROUTE_REQUEST,
                               data_orig, length, UT_MSG);

    info = message->info;
    info->network = network;
    memcpy(&info->dest, &dest, sizeof(info->dest));

    YUNIT_ASSERT(UR_ERROR_NONE == handle_diags_command(message, true));
    message_free(message);
    ur_mem_free(data_orig, length);
    interface_stop();
}

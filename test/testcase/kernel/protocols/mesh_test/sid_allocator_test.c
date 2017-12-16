/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "yunit.h"

#include "umesh_types.h"
#include "core/sid_allocator.h"
#include "hal/interface_context.h"
#include "hal/interfaces.h"

void test_uradar_sid_allocator_case(void)
{
    ur_node_id_t node_id = {.ueid = {0x00, 0x02, 0x03,0x04, 0x05, 0x06, 0x07, 0x08},
                            .sid = INVALID_SID,
                            .attach_sid = 0x0000};
    uint16_t index;
    allocator_t hdl = allocator_init(0x0000, STRUCTURED_SID);

    for(index = 1; index <= 11; index++) {
        node_id.ueid[0] += 1;
        YUNIT_ASSERT(UR_ERROR_NONE == allocate_sid(hdl, &node_id));
        YUNIT_ASSERT((index << 12) == node_id.sid);
    }
    YUNIT_ASSERT(11 == get_allocated_number(hdl));
    allocator_deinit(hdl);

    hdl = allocator_init(0x1000, STRUCTURED_SID);
    node_id.sid = 0x1000;
    free_sid(hdl, node_id.sid);
    node_id.mode = MODE_RX_ON;
    YUNIT_ASSERT(UR_ERROR_NONE == allocate_sid(hdl, &node_id));
    YUNIT_ASSERT(0x1100 == node_id.sid);
    YUNIT_ASSERT(1 == get_allocated_number(hdl));

    node_id.ueid[0] += 1;
    YUNIT_ASSERT(UR_ERROR_NONE == allocate_sid(hdl, &node_id));

    allocator_deinit(hdl);

    hdl = allocator_init(0x0000, STRUCTURED_SID);
    node_id.ueid[0] += 1;
    node_id.mode = MODE_MOBILE;
    YUNIT_ASSERT(UR_ERROR_NONE == allocate_sid(hdl, &node_id));
    YUNIT_ASSERT(0xc001 == node_id.sid);
    YUNIT_ASSERT(true == is_partial_function_sid(node_id.sid));
    YUNIT_ASSERT(1 == get_allocated_pf_number(hdl));
    YUNIT_ASSERT(UR_ERROR_NONE == allocate_sid(hdl, &node_id));
    YUNIT_ASSERT(0xc001 == node_id.sid);
    YUNIT_ASSERT(true == is_partial_function_sid(node_id.sid));
    YUNIT_ASSERT(1 == get_allocated_pf_number(hdl));
    node_id.ueid[0] += 1;
    node_id.mode = MODE_MOBILE;
    YUNIT_ASSERT(UR_ERROR_NONE == allocate_sid(hdl, &node_id));
    YUNIT_ASSERT(0xc002 == node_id.sid);
    YUNIT_ASSERT(true == is_partial_function_sid(node_id.sid));
    YUNIT_ASSERT(2 == get_allocated_pf_number(hdl));

    allocator_deinit(hdl);
}

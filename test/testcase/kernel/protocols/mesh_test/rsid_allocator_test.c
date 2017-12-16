/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "yunit.h"

#include "core/sid_allocator.h"
#include "hal/interfaces.h"
#include "hal/interface_context.h"

void test_uradar_rsid_allocator_case(void)
{
    ur_node_id_t node_id = {.ueid = {0x00, 0x02, 0x03,0x04, 0x05, 0x06, 0x07, 0x08},
                            .sid = INVALID_SID,
                            .attach_sid = 0x0000};
    uint16_t index;
    allocator_t hdl = rsid_allocator_init(SHORT_RANDOM_SID);

    for(index = 1; index <= 11; index++) {
        node_id.ueid[0] += 1;
        node_id.sid = INVALID_SID;
        YUNIT_ASSERT(UR_ERROR_NONE == rsid_allocate_sid(hdl, &node_id));
        YUNIT_ASSERT(index == node_id.sid);
    }
    YUNIT_ASSERT(11 == rsid_get_allocated_number(hdl));

    node_id.ueid[0] += 1;
    node_id.sid = 10;
    YUNIT_ASSERT(UR_ERROR_NONE == rsid_allocate_sid(hdl, &node_id));
    YUNIT_ASSERT(12 == node_id.sid);
    YUNIT_ASSERT(12 == rsid_get_allocated_number(hdl));

    rsid_allocator_deinit(hdl);

    hdl = rsid_allocator_init(SHORT_RANDOM_SID);

    node_id.ueid[0] = 0;
    for (index = 1; index <= 11; index++) {
        node_id.ueid[0] += 1;
        node_id.sid = index;
        YUNIT_ASSERT(UR_ERROR_NONE == rsid_allocate_sid(hdl, &node_id));
        YUNIT_ASSERT(index == node_id.sid);
    }

    node_id.ueid[0] = 10;
    node_id.sid = 10;
    YUNIT_ASSERT(UR_ERROR_NONE == rsid_allocate_sid(hdl, &node_id));
    YUNIT_ASSERT(10 == node_id.sid);
    YUNIT_ASSERT(11 == rsid_get_allocated_number(hdl));

    rsid_allocator_deinit(hdl);
}

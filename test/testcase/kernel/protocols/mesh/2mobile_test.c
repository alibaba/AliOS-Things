/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "yunit.h"

#include "aos/aos.h"
#include "aos/kernel.h"

#include "umesh.h"
#include "core/link_mgmt.h"
#include "core/sid_allocator.h"
#include "tools/cli.h"

#include "dda_util.h"

static void run_in_hop1(void)
{
    set_full_rssi(11, 14);

    start_node(12);
    start_node(13);
    aos_msleep(5000);

    cmd_to_agent("mode MOBILE");
    cmd_to_agent("start");
    aos_msleep(10000);

    YUNIT_ASSERT(umesh_get_sid() == 0xc001);

    cmd_to_agent("stop");
    stop_node(12);
    stop_node(13);

    aos_msleep(2 * 1000);
}

static void run_in_hop2(void)
{
    set_line_rssi(11, 13);

    start_node(13);
    aos_msleep(5000);
    start_node(12);
    aos_msleep(5000);

    cmd_to_agent("mode MOBILE");
    cmd_to_agent("start");

    aos_msleep(10 * 1000);
    YUNIT_ASSERT(umesh_get_sid() == 0xc001);

    cmd_to_agent("stop");
    stop_node(12);
    stop_node(13);

    aos_msleep(2 * 1000);
}


void test_uradar_1mobile_case(void)
{
    run_times(run_in_hop1(), 2);
    run_times(run_in_hop2(), 2);
}

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <yunit.h>
#include <aos/aos.h>
#include <aos/kernel.h>

#include "umesh_hal.h"

void test_hal_mesh_case(void)
{
    hal_umesh_get_bcast_mtu(NULL);
    hal_umesh_get_ucast_mtu(NULL);
    hal_umesh_get_channel(NULL);
    hal_umesh_get_chnlist(NULL, NULL);
    hal_umesh_set_txpower(NULL, 2);
    hal_umesh_get_txpower(NULL);
    hal_umesh_get_extnetid(NULL, NULL);
    hal_umesh_get_stats(NULL);
}

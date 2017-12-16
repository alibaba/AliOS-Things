/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "yunit.h"

#include "aos/aos.h"
#include "aos/kernel.h"

#include "core/mesh_mgmt.h"

#include "dda_util.h"

extern void topo_test_function(uint16_t first_node, uint16_t num, uint32_t timeout);
void test_uradar_scales_30nodes_case(void)
{
    topo_test_function(11, 30, 200);
}


/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>

#include <aos/kernel.h>
#include "yunit.h"

#include "dda_util.h"

#include "umesh.h"
#include "utilities/logging.h"
#include "core/mesh_mgmt.h"
#include "tools/cli.h"

void test_uradar_cli_case(void)
{
    cmd_to_agent("stop");
    cmd_to_agent("start");
    check_cond_wait(umesh_get_device_state() == DEVICE_STATE_LEADER, 10);

    cmd_to_agent("help");
    cmd_to_agent("channel");
    cmd_to_agent("loglevel");
    cmd_to_agent("ipaddr");
    cmd_to_agent("macaddr");
    cmd_to_agent("meshnetsize");
    cmd_to_agent("meshnetid");
    cmd_to_agent("autotest");
    cmd_to_agent("prefix");
    cmd_to_agent("seclevel");
    cmd_to_agent("state");
    cmd_to_agent("stats");
    cmd_to_agent("whitelist");
    cmd_to_agent("whitelist add 0100000000000000");
    cmd_to_agent("whitelist remove 0100000000000000");
    cmd_to_agent("whitelist enable");
    cmd_to_agent("whitelist disable");
    cmd_to_agent("whitelist clear");
    cmd_to_agent("networks");
    cmd_to_agent("stop");
    aos_msleep(1 * 1000);
}


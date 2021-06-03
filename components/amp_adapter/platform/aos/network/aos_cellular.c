/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos_network.h"
#include "amp_platform.h"

int aos_get_sim_info(aos_sim_info_t *sim_info)
{
    int ret = -1;

    return ret;
}

int aos_get_locator_info(aos_locator_info_t *locator_info)
{
    int ret = -1;

    return ret;
}

int aos_get_neighbor_locator_info(void (*cb)(aos_locator_info_t*, int))
{
    cb(NULL, 0);
    return 0;
}

AOS_NETWORK_SHAREMODE_E aos_get_netsharemode(void)
{
    return AOS_NETWORK_SHAREMODE_INVALID;
}

int aos_set_netsharemode(AOS_NETWORK_SHAREMODE_E share_mode)
{
    return -1;
}

int aos_get_netshareconfig(aos_sharemode_info_t *share_mode_info)
{
    return -1;
}

int aos_set_netshareconfig(int ucid, int auth_type, aos_sharemode_info_t *share_mode_info)
{
    return -1;
}


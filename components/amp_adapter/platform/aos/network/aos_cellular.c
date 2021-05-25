/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos_network.h"

int amp_get_sim_info(amp_sim_info_t *sim_info)
{
    int ret = -1;

    return ret;
}

int amp_get_locator_info(amp_locator_info_t *locator_info)
{
    int ret = -1;

    return ret;
}

int amp_get_neighbor_locator_info(void (*cb)(amp_locator_info_t*, int))
{
    // cb(NULL, 0);
    return 0;
}

AMP_NETWORK_SHAREMODE_E amp_get_netsharemode(void)
{
	return AMP_NETWORK_SHAREMODE_INVALID;
}

int amp_set_netsharemode(AMP_NETWORK_SHAREMODE_E share_mode)
{
	return -1;
}

int amp_get_netshareconfig(amp_sharemode_info_t *share_mode_info)
{
	return -1;
}

int amp_set_netshareconfig(int ucid, int auth_type, amp_sharemode_info_t *share_mode_info)
{
	return -1;
}


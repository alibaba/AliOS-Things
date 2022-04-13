/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "amp_network.h"

int32_t HAL_Get_SimInfo(sim_info_t *sim_info)
{
    int32_t ret = -1;

    return ret;
}

int32_t HAL_Get_LocatorInfo(locator_info_t *locator_info)
{
    int32_t ret = -1;

    char mcc[4] = {0};
    char mnc[4] = {0};

    memset(locator_info, 0x0, sizeof(locator_info_t));

    memcpy(locator_info->mcc, mcc, 4);
    memcpy(locator_info->mnc, mnc, 4);
    locator_info->cid = 0;
    locator_info->lac = 0;

    return 0;
}

int32_t HAL_Get_SignalInfo(signal_info_t *signal_info)
{
    int32_t ret = -1;

    memset(signal_info, 0x0, sizeof(signal_info_t));

    signal_info->rssi = -88;

    return 0;
}

int32_t HAL_Get_neighborLocatorInfo(void (*cb)(locator_info_t*, int))
{
    cb(NULL, 0);
    return 0;
}
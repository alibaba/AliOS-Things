/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "amp_network.h"
#include "amp_platform.h"
#include "ql_sim.h"
#include "ql_dev.h"
#include "ql_nw.h"

int32_t HAL_Get_SimInfo(sim_info_t *sim_info)
{
    int32_t ret = -1;

    char imsi[24] = {0};
    char imei[32] = {0};
    char iccid[24] = {0};

    ql_dev_get_imei(imei, 32);
    ql_sim_get_imsi(imsi, 24);
    ql_sim_get_iccid(iccid, 24);

    memset(sim_info, 0x0, sizeof(sim_info_t));

    memcpy(sim_info->imsi, imsi, 24);
    memcpy(sim_info->imei, imei, 32);
    memcpy(sim_info->iccid, iccid, 24);

    return 0;
}

int32_t HAL_Get_LocatorInfo(locator_info_t *locator_info)
{
    int32_t ret = -1;
    
    char mcc[4] = {0};
    char mnc[4] = {0};
    QL_NW_REG_STATUS_INFO_T  ql_nw_reg_status;
    QL_NW_SELECTION_INFO_T  ql_nw_selection_info;

    ql_nw_get_reg_status(&ql_nw_reg_status);
    ql_nw_get_selection(&ql_nw_selection_info);

    memset(locator_info, 0x0, sizeof(locator_info_t));

    memcpy(locator_info->mcc, ql_nw_selection_info.mcc, 4);
    memcpy(locator_info->mnc, ql_nw_selection_info.mnc, 4);

    locator_info->cid = ql_nw_reg_status.data_reg.cid;
    locator_info->lac = ql_nw_reg_status.data_reg.lac;

    return 0;
}

int32_t HAL_Get_SignalInfo(signal_info_t *signal_info)
{
    int32_t ret = -1;
    QL_NW_SIGNAL_STRENGTH_INFO_T  ql_nw_signal_strength;

    ql_nw_get_signal_strength(&ql_nw_signal_strength);

    memset(signal_info, 0x0, sizeof(signal_info_t));

    signal_info->rssi = ql_nw_signal_strength.LTE_SignalStrength.rssi;

    return 0;
}

int32_t HAL_Get_neighborLocatorInfo(void (*cb)(locator_info_t*, int))
{
    cb(NULL, 0);
    return 0;
}
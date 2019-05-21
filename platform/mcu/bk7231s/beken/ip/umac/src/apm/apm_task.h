/**
****************************************************************************************
*
* @file apm_task.h
*
* Copyright (C) RivieraWaves 2011-2016
*
* @brief APM task kernel interface declaration.
*
****************************************************************************************
*/

#ifndef _APM_TASK_H_
#define _APM_TASK_H_

/** @defgroup TASK_APM TASK_APM
* @ingroup APM
* @brief Declaration of all structures and functions used by the APM module.
* @{
*/

/*
* INCLUDE FILES
****************************************************************************************
*/

// KE include
#include "ke_task.h"
// APM include
#include "apm.h"
// SCAN include
#include "scan.h"

#if NX_BEACONING

/// Task max index number.
#define APM_IDX_MAX 1

/// Possible states of the task.
enum apm_state_tag
{
    /// IDLE State.
    APM_IDLE,
    /// Waiting for BSS parameter setting
    APM_BSS_PARAM_SETTING,
    /// Waiting for the beacon to be set to LMAC
    APM_BCN_SETTING,
    /// Number of states.
    APM_STATE_MAX
};

/// Messages that are logically related to the task.
enum apm_msg_tag
{
    /// Request to start the AP.
    APM_START_REQ = KE_FIRST_MSG(TASK_APM),
    /// Confirmation of the AP start.
    APM_START_CFM,
    /// Request to stop the AP.
    APM_STOP_REQ,
    /// Confirmation of the AP stop.
    APM_STOP_CFM,
    /// Request to start CAC
    APM_START_CAC_REQ,
    /// Confirmation of the CAC start
    APM_START_CAC_CFM,
    /// Request to stop CAC
    APM_STOP_CAC_REQ,
    /// Confirmation of the CAC stop
    APM_STOP_CAC_CFM,
    /// Indicate association of AP
    APM_ASSOC_IND,
    /// Indicate deassociation of AP
    APM_DEASSOC_IND,
    /// Indicate association failed of AP
    APM_ASSOC_FAILED_IND,
};

/// Structure containing the parameters of the @ref APM_START_REQ message.
struct apm_start_req
{
    /// Basic rate set
    struct mac_rateset basic_rates;
    /// Control channel on which we have to enable the AP
    struct scan_chan_tag chan;
    /// Center frequency of the first segment
    uint32_t center_freq1;
    /// Center frequency of the second segment (only in 80+80 configuration)
    uint32_t center_freq2;
    /// Width of channel
    uint8_t ch_width;
    /// Address, in host memory, to the beacon template
    uint32_t bcn_addr;
    /// Length of the beacon template
    uint16_t bcn_len;
    /// Offset of the TIM IE in the beacon
    uint16_t tim_oft;
    /// Beacon interval
    uint16_t bcn_int;
    /// Flags
    uint32_t flags;
    /// Control port Ethertype
    uint16_t ctrl_port_ethertype;
    /// Length of the TIM IE
    uint8_t tim_len;
    /// Index of the VIF for which the AP is started
    uint8_t vif_idx;
};

/// Structure containing the parameters of the @ref APM_START_CFM message.
struct apm_start_cfm
{
    /// Status of the AP starting procedure
    uint8_t status;
    /// Index of the VIF for which the AP is started
    uint8_t vif_idx;
    /// Index of the channel context attached to the VIF
    uint8_t ch_idx;
    /// Index of the STA used for BC/MC traffic
    uint8_t bcmc_idx;
};

/// Structure containing the parameters of the @ref APM_STOP_REQ message.
struct apm_stop_req
{
    /// Index of the VIF for which the AP has to be stopped
    uint8_t vif_idx;
};

/// Structure containing the parameters of the @ref APM_STOP_REQ message.
struct apm_stop_cfm
{
    uint8_t status;
};


/// Structure containing the parameters of the @ref APM_START_CAC_REQ message.
struct apm_start_cac_req
{
    /// Control channel on which we have to start the CAC
    struct scan_chan_tag chan;
    /// Center frequency of the first segment
    uint32_t center_freq1;
    /// Center frequency of the second segment (only in 80+80 configuration)
    uint32_t center_freq2;
    /// Width of channel
    uint8_t ch_width;
    /// Index of the VIF for which the CAC is started
    uint8_t vif_idx;
};

/// Structure containing the parameters of the @ref APM_START_CAC_CFM message.
struct apm_start_cac_cfm
{
    /// Status of the CAC starting procedure
    uint8_t status;
    /// Index of the channel context attached to the VIF for CAC
    uint8_t ch_idx;
};

/// Structure containing the parameters of the @ref APM_STOP_CAC_REQ message.
struct apm_stop_cac_req
{
    /// Index of the VIF for which the CAC has to be stopped
    uint8_t vif_idx;
};

/// Structure containing the parameters of the @ref APM_ASSOC_IND message.
struct apm_assoc_ind
{
    /// Mac of client
    uint8_t mac[6];
};

/// Structure containing the parameters of the @ref APM_DEASSOC_IND message.
struct apm_deassoc_ind
{
    /// Mac of client
    uint8_t mac[6];
};

/// Structure containing the parameters of the @ref APM_ASSOC_FAILED_IND message.
struct apm_assoc_failed_ind
{
    /// Mac of client
    uint8_t mac[6];
};

extern const struct ke_state_handler apm_state_handler[APM_STATE_MAX];

extern const struct ke_state_handler apm_default_handler;

extern ke_state_t apm_state[APM_IDX_MAX];

#endif

/// @} end of group

#endif // _APM_TASK_H_

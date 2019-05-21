/**
 ****************************************************************************************
 *
 * @file scan_task.h
 *
 * @brief Scanning task declarations.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _SCAN_TASK_H_
#define _SCAN_TASK_H_

/**
 *****************************************************************************************
 * @defgroup SCAN_TASK SCAN_TASK
 * @ingroup SCAN
 * @brief Task responsible for scanning process
 * @{
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"
#include "co_bool.h"

// inclusion to retrieve the task index
#include "ke_task.h"
// for MAC related elements (mac_addr, mac_ssid...)
#include "mac.h"
#include "phy.h"
#include "scan.h"

#if NX_HW_SCAN
/*
 * DEFINES
 ****************************************************************************************
 */
/// SCAN TASK Instance Max.
#define SCAN_IDX_MAX 1

/// Possible States of the MM STA Task.
enum scan_state_tag
{
    /// SCAN IDLE State.
    SCAN_IDLE,
    /// Wait ProbeReq IE download.
    SCAN_WAIT_IE_DWNLD,
    /// SCAN Wait for channel State.
    SCAN_WAIT_CHANNEL,
    /// Wait for beacon and probe responses State
    SCAN_WAIT_BEACON_PROBE_RSP,
    /// SCAN Max Number of states.
    SCAN_STATE_MAX
};

/// List of messages related to the task.
enum scan_msg_tag
{
    /// Scanning start Request.
    SCAN_START_REQ = KE_FIRST_MSG(TASK_SCAN),
    /// Scanning start Confirmation.
    SCAN_START_CFM,
    /// End of scanning indication.
    SCAN_DONE_IND,
    /// Cancel scan request
    SCAN_CANCEL_REQ,
    /// Cancel scan confirmation
    SCAN_CANCEL_CFM,

    /*
     * Section of internal SCAN messages. No SCAN API messages should be defined below this point
     */
    SCAN_TIMER,

    /// MAX number of messages
    SCAN_MAX,
};

/// Structure containing the parameters of the @ref SCAN_START_REQ message
struct scan_start_req
{
    /// List of channel to be scanned
    struct scan_chan_tag chan[SCAN_CHANNEL_MAX];
    /// List of SSIDs to be scanned
    struct mac_ssid ssid[SCAN_SSID_MAX];
    /// BSSID to be scanned
    struct mac_addr bssid;
    /// Address (in host memory) to the additional IEs that need to be added to the ProbeReq
    /// (following the SSID element)
    uint32_t add_ies;
    /// Length of the additional IEs
    uint16_t add_ie_len;
    /// Index of the VIF that is scanning
    uint8_t vif_idx;
    /// Number of channels to scan
    uint8_t chan_cnt;
    /// Number of SSIDs to scan for
    uint8_t ssid_cnt;
    /// no CCK - For P2P frames not being sent at CCK rate in 2GHz band.
    bool no_cck;
};

/// Structure containing the parameters of the @ref SCAN_START_CFM message
struct scan_start_cfm
{
    /// Status of the request
    uint8_t status;
};

/// Structure containing the parameters of the @ref SCAN_START_CFM message
struct scan_cancel_cfm
{
    /// Status of the request
    uint8_t status;
};

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */
extern const struct ke_state_handler scan_default_handler;

extern ke_state_t scan_state[SCAN_IDX_MAX];
#endif

/// @} end of group

#endif // _MM_TASK_H_

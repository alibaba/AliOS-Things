/**
****************************************************************************************
*
* @file me_task.h
*
* Copyright (C) RivieraWaves 2011-2016
*
* @brief Declaration of all structures and functions used by the SCAN module.
*
****************************************************************************************
*/

#ifndef _ME_TASK_H_
#define _ME_TASK_H_

#include "include.h"

#include "co_int.h"
// inclusion to retrieve the task index
#include "ke_task.h"
// for mac_addr and mac_ssid
#include "mac.h"
#include "scan.h"

#if RC_ENABLE
// for RC statistics structure rc_rate_stats
#include "rc.h"
#endif

/** @defgroup TASK_ME TASK_ME
 * @ingroup ME
 * @brief Declaration of all structures and functions used by the ME module.
 * @{
 */

/// Task max index number.
#define ME_IDX_MAX 1

/// Possible states of the task.
enum
{
    /// Idle State.
    ME_IDLE,
    /// Busy State.
    ME_BUSY,
    /// Max number of states
    ME_STATE_MAX
};

/// Messages that are logically related to the task.
enum
{
    /// Configuration request from host.
    ME_CONFIG_REQ = KE_FIRST_MSG(TASK_ME),
    /// Configuration confirmation.
    ME_CONFIG_CFM,
    /// Configuration request from host.
    ME_CHAN_CONFIG_REQ,
    /// Configuration confirmation.
    ME_CHAN_CONFIG_CFM,
    /// Set control port state for a station.
    ME_SET_CONTROL_PORT_REQ,
    /// Control port setting confirmation.
    ME_SET_CONTROL_PORT_CFM,
    /// TKIP MIC failure indication.
    ME_TKIP_MIC_FAILURE_IND,
    /// Management frame TX programming request
    ME_MGMT_TX_REQ,
    /// Management frame TX programming confirmation
    ME_MGMT_TX_CFM,
    /// Management frame transmission confirmation
    ME_MGMT_TX_DONE_IND,
    /// Add a station to the FW (AP mode)
    ME_STA_ADD_REQ,
    /// Confirmation of the STA addition
    ME_STA_ADD_CFM,
    /// Delete a station from the FW (AP mode)
    ME_STA_DEL_REQ,
    /// Confirmation of the STA deletion
    ME_STA_DEL_CFM,
    /// Indication of a TX RA/TID queue credit update
    ME_TX_CREDITS_UPDATE_IND,
    /// Request indicating to the FW that there is UAPSD traffic buffered on host
    ME_UAPSD_TRAFFIC_IND_REQ,
    /// Confirmation that the @ref ME_UAPSD_TRAFFIC_IND_REQ has been executed
    ME_UAPSD_TRAFFIC_IND_CFM,
#if RC_ENABLE
    /// Request RC statistics to a station
    ME_RC_STATS_REQ,
    /// RC statistics confirmation
    ME_RC_STATS_CFM,
    /// Request RC fixed rate
    ME_RC_SET_RATE_REQ,
#endif

    /*
     * Section of internal ME messages. No ME API messages should be defined below this point
     */
    /// Internal request to indicate that a VIF needs to get the HW going to ACTIVE or IDLE
    ME_SET_ACTIVE_REQ,
    /// Confirmation that the switch to ACTIVE or IDLE has been executed
    ME_SET_ACTIVE_CFM,
    /// Internal request to indicate that a VIF desires to de-activate/activate the Power-Save mode
    ME_SET_PS_DISABLE_REQ,
    /// Confirmation that the PS state de-activate/activate has been executed
    ME_SET_PS_DISABLE_CFM,
    //for change ps on-off
    ME_PS_REQ,
};

/// Structure containing the parameters of the @ref ME_CONFIG_REQ message
struct me_config_req
{
    /// HT Capabilities
    struct mac_htcapability ht_cap;
    /// VHT Capabilities
    struct mac_vhtcapability vht_cap;
    /// Lifetime of packets sent under a BlockAck agreement (expressed in TUs)
    uint16_t tx_lft;
    /// Boolean indicating if HT is supported or not
    bool ht_supp;
    /// Boolean indicating if VHT is supported or not
    bool vht_supp;
    /// Boolean indicating if PS mode shall be enabled or not
    bool ps_on;
};

/// Structure containing the parameters of the @ref ME_CHAN_CONFIG_REQ message
struct me_chan_config_req
{
    /// List of 2.4GHz supported channels
    struct scan_chan_tag chan2G4[SCAN_CHANNEL_2G4];
    /// List of 5GHz supported channels
    struct scan_chan_tag chan5G[SCAN_CHANNEL_5G];
    /// Number of 2.4GHz channels in the list
    uint8_t chan2G4_cnt;
    /// Number of 5GHz channels in the list
    uint8_t chan5G_cnt;
};

/// Structure containing the parameters of the @ref ME_SET_CONTROL_PORT_REQ message
struct me_set_control_port_req
{
    /// Index of the station for which the control port is opened
    uint8_t sta_idx;
    /// Control port state
    bool control_port_open;
};

/// Structure containing the parameters of the @ref ME_TKIP_MIC_FAILURE_IND message
struct me_tkip_mic_failure_ind
{
    /// Address of the sending STA
    struct mac_addr addr;
    /// TSC value
    uint64_t tsc;
    /// Boolean indicating if the packet was a group or unicast one (true if group)
    bool ga;
    /// Key Id
    uint8_t keyid;
    /// VIF index
    uint8_t vif_idx;
};

/// Structure containing the parameters of the @ref ME_MGMT_TX_REQ message
struct me_mgmt_tx_req
{
    /// Pointer to the frame buffer in host memory
    uint32_t addr;
    /// Identifier of the frame for the host (not used internally but confirmed
    /// back to host)
    uint32_t hostid;
    /// Length of the packet to be transmitted
    uint16_t len;
    /// Boolean indicating if the packet can be sent with a DSSS/CCK rate or not
    bool no_cck;
    /// Boolean indicating if an acknowledgment is expected for this frame
    bool no_ack;
    /// Boolean indicating if the frame is sent off-channel
    bool off_channel;
    /// VIF index
    uint8_t vif_idx;

    uint32_t req_malloc_flag;
};


/// Structure containing the parameters of the @ref ME_MGMT_TX_CFM message
struct me_mgmt_tx_cfm
{
    /// Identifier of the frame for the host
    uint32_t hostid;

    /// Status indicating if the frame will be transmitted or not
    uint8_t status;

    uint8_t  rsp_frm_new;
    uint16_t length;
};

/// Structure containing the parameters of the @ref ME_MGMT_TX_DONE_IND message
struct me_mgmt_tx_done_ind
{
    /// Identifier of the frame for the host
    uint32_t hostid;
    /// true if the frame was acknowledged, false otherwise
    bool ack;
};

/// Structure containing the parameters of the @ref ME_STA_ADD_REQ message
struct me_sta_add_req
{
    /// MAC address of the station to be added
    struct mac_addr mac_addr;
    /// Supported legacy rates
    struct mac_rateset rate_set;
    /// HT Capabilities
    struct mac_htcapability ht_cap;
    /// VHT Capabilities
    struct mac_vhtcapability vht_cap;
    /// Flags giving additional information about the station
    uint32_t flags;
    /// Association ID of the station
    uint16_t aid;
    /// Bit field indicating which queues have U-APSD enabled
    uint8_t uapsd_queues;
    /// Operation mode information (valid if bit @ref STA_OPMOD_NOTIF is
    /// set in the flags)
    uint8_t opmode;
    /// Index of the VIF the station is attached to
    uint8_t vif_idx;
    /// Whether the station is a TDLS station
    bool tdls_sta;
};

/// Structure containing the parameters of the @ref ME_STA_ADD_CFM message
struct me_sta_add_cfm
{
    /// Station index
    uint8_t sta_idx;
    /// Status of the station addition
    uint8_t status;
    /// PM state of the station
    uint8_t pm_state;
};

/// Structure containing the parameters of the @ref ME_STA_DEL_REQ message.
struct me_sta_del_req
{
    /// Index of the station to be deleted
    uint8_t sta_idx;
    /// Whether the station is a TDLS station
    bool tdls_sta;
};

/// Structure containing the parameters of the @ref ME_TX_CREDITS_UPDATE_IND message.
struct me_tx_credits_update_ind
{
    /// Index of the station for which the credits are updated
    uint8_t sta_idx;
    /// TID for which the credits are updated
    uint8_t tid;
    /// Offset to be applied on the credit count
    int8_t credits;
};

/// Structure containing the parameters of the @ref ME_SET_ACTIVE_REQ message
struct me_set_active_req
{
    /// Boolean indicating whether the VIF requests the HW to be passed in ACTIVE or IDLE
    bool active;
    /// VIF index
    uint8_t vif_idx;
};

/// Structure containing the parameters of the @ref ME_SET_PS_DISABLE_REQ message
struct me_set_ps_disable_req
{
    /// Boolean indicating whether the VIF requests the PS to be disabled or not
    uint8_t ps_disable;
    /// VIF index
    uint8_t vif_idx;
};

/// Structure containing the parameters of the @ref ME_UAPSD_TRAFFIC_IND_REQ message.
struct me_uapsd_traffic_ind_req
{
    /// Index of the station for which UAPSD traffic is available on host
    uint8_t sta_idx;
    /// Flag indicating the availability of UAPSD packets for the given STA
    uint8_t tx_avail;
};

#if RC_ENABLE
/// Structure containing the parameters of the @ref ME_RC_STATS_REQ message.
struct me_rc_stats_req
{
    /// Index of the station for which the RC statistics are requested
    uint8_t sta_idx;
};

/// Structure containing the parameters of the @ref ME_RC_STATS_CFM message.
struct me_rc_stats_cfm
{
    /// Index of the station for which the RC statistics are provided
    uint8_t sta_idx;
    /// Number of samples used in the RC algorithm
    uint16_t no_samples;
    /// Number of MPDUs transmitted (per sampling interval)
    uint32_t ampdu_len;
    /// Number of AMPDUs transmitted (per sampling interval)
    uint32_t ampdu_packets;
    /// Average number of MPDUs in each AMPDU frame (EWMA)
    uint32_t avg_ampdu_len;
    // Current step 0 of the retry chain
    uint8_t sw_retry_step;
    /// Trial transmission period
    uint8_t sample_wait;
    /// Retry chain steps
    struct step retry[RATE_CONTROL_STEPS];
    /// RC statistics
    struct rc_rate_stats rate_stats[RC_MAX_N_SAMPLE];
    // Throughput
    uint32_t tp[RC_MAX_N_SAMPLE];
};

/// Structure containing the parameters of the @ref ME_RC_SET_RATE_REQ message.
struct me_rc_set_rate_req
{
    /// Index of the station for which the fixed rate is requested
    uint8_t sta_idx;
    /// Fixed rate configuration
    uint16_t fixed_rate_cfg;
};
#endif

extern const struct ke_state_handler me_state_handler[ME_STATE_MAX];

extern const struct ke_state_handler me_default_handler;

extern ke_state_t me_state[ME_IDX_MAX];

/// @}

#endif // _ME_TASK_H_



/**
 ****************************************************************************************
 *
 * @file mm_task.h
 *
 * @brief MAC Management task declarations.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _MM_TASK_H_
#define _MM_TASK_H_

/**
 *****************************************************************************************
 * @defgroup MM_TASK MM_TASK
 * @ingroup MM
 * @brief LMAC Management module task
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

/*
 * DEFINES
 ****************************************************************************************
 */
/// MM STA TASK Instance Max.
#define MM_IDX_MAX 1

/// Possible States of the MM STA Task.
enum mm_state_tag
{
    /// MAC IDLE State.
    MM_IDLE,
    /// MAC ACTIVE State.
    MM_ACTIVE,
    /// MAC is going to IDLE
    MM_GOING_TO_IDLE,
    /// IDLE state internally controlled
    MM_HOST_BYPASSED,
    /// MAC Max Number of states.
    MM_STATE_MAX
};

/// Remain on channel operation codes
enum mm_remain_on_channel_op
{
    MM_ROC_OP_START = 0,
    MM_ROC_OP_CANCEL,

    MM_ROC_OP_MAX,
};

/// List of messages related to the task.
enum mm_msg_tag
{
    // 0
    /// RESET Request.
    MM_RESET_REQ = KE_FIRST_MSG(TASK_MM),
    /// RESET Confirmation.
    MM_RESET_CFM,
    /// START Request.
    MM_START_REQ,
    /// START Confirmation.
    MM_START_CFM,
    /// Read Version Request.
    MM_VERSION_REQ,
    /// Read Version Confirmation.
    MM_VERSION_CFM,
    /// ADD INTERFACE Request.
    MM_ADD_IF_REQ,
    /// ADD INTERFACE Confirmation.
    MM_ADD_IF_CFM,
    /// REMOVE INTERFACE Request.
    MM_REMOVE_IF_REQ,
    /// REMOVE INTERFACE Confirmation.
    MM_REMOVE_IF_CFM,
	
    // 10
    /// STA ADD Request.
    MM_STA_ADD_REQ,
    /// STA ADD Confirm.
    MM_STA_ADD_CFM,
    /// STA DEL Request.
    MM_STA_DEL_REQ,
    /// STA DEL Confirm.
    MM_STA_DEL_CFM,
    /// RX FILTER CONFIGURATION Request.
    MM_SET_FILTER_REQ,
    /// RX FILTER CONFIGURATION Confirmation.
    MM_SET_FILTER_CFM,
    /// CHANNEL CONFIGURATION Request.
    MM_SET_CHANNEL_REQ,
    /// CHANNEL CONFIGURATION Confirmation.
    MM_SET_CHANNEL_CFM,
    /// DTIM PERIOD CONFIGURATION Request.
    MM_SET_DTIM_REQ,
    /// DTIM PERIOD CONFIGURATION Confirmation.
    MM_SET_DTIM_CFM,
	
    // 20
    /// BEACON INTERVAL CONFIGURATION Request.
    MM_SET_BEACON_INT_REQ,
    /// BEACON INTERVAL CONFIGURATION Confirmation.
    MM_SET_BEACON_INT_CFM,
    /// BASIC RATES CONFIGURATION Request.
    MM_SET_BASIC_RATES_REQ,
    /// BASIC RATES CONFIGURATION Confirmation.
    MM_SET_BASIC_RATES_CFM,
    /// BSSID CONFIGURATION Request.
    MM_SET_BSSID_REQ,
    /// BSSID CONFIGURATION Confirmation.
    MM_SET_BSSID_CFM,
    /// EDCA PARAMETERS CONFIGURATION Request.
    MM_SET_EDCA_REQ,
    /// EDCA PARAMETERS CONFIGURATION Confirmation.
    MM_SET_EDCA_CFM,
    /// ABGN MODE CONFIGURATION Request.
    MM_SET_MODE_REQ,
    /// ABGN MODE CONFIGURATION Confirmation.
    MM_SET_MODE_CFM,
	
    // 30
    /// Request setting the VIF active state (i.e associated or AP started)
    MM_SET_VIF_STATE_REQ,
    /// Confirmation of the @ref MM_SET_VIF_STATE_REQ message.
    MM_SET_VIF_STATE_CFM,
    /// SLOT TIME PARAMETERS CONFIGURATION Request.
    MM_SET_SLOTTIME_REQ,
    /// SLOT TIME PARAMETERS CONFIGURATION Confirmation.
    MM_SET_SLOTTIME_CFM,
    /// IDLE mode change Request.
    MM_SET_IDLE_REQ,
    /// IDLE mode change Confirm.
    MM_SET_IDLE_CFM,
    /// KEY ADD Request.
    MM_KEY_ADD_REQ,
    /// KEY ADD Confirm.
    MM_KEY_ADD_CFM,
    /// KEY DEL Request.
    MM_KEY_DEL_REQ,
    /// KEY DEL Confirm.
    MM_KEY_DEL_CFM,
	
    // 40
    /// Block Ack agreement info addition
    MM_BA_ADD_REQ,
    /// Block Ack agreement info addition confirmation
    MM_BA_ADD_CFM,
    /// Block Ack agreement info deletion
    MM_BA_DEL_REQ,
    /// Block Ack agreement info deletion confirmation
    MM_BA_DEL_CFM,
    /// Indication of the primary TBTT to the upper MAC. Upon the reception of this
    /// message the upper MAC has to push the beacon(s) to the beacon transmission queue.
    MM_PRIMARY_TBTT_IND,
    /// Indication of the secondary TBTT to the upper MAC. Upon the reception of this
    /// message the upper MAC has to push the beacon(s) to the beacon transmission queue.
    MM_SECONDARY_TBTT_IND,
    /// Request for changing the TX power
    MM_SET_POWER_REQ,
    /// Confirmation of the TX power change
    MM_SET_POWER_CFM,
    /// Request to the LMAC to trigger the embedded logic analyzer and forward the debug
    /// dump.
    MM_DBG_TRIGGER_REQ,
    /// Set Power Save mode
    MM_SET_PS_MODE_REQ,
	
    // 50
    /// Set Power Save mode confirmation
    MM_SET_PS_MODE_CFM,
    /// Request to add a channel context
    MM_CHAN_CTXT_ADD_REQ,
    /// Confirmation of the channel context addition
    MM_CHAN_CTXT_ADD_CFM,
    /// Request to delete a channel context
    MM_CHAN_CTXT_DEL_REQ,
    /// Confirmation of the channel context deletion
    MM_CHAN_CTXT_DEL_CFM,
    /// Request to link a channel context to a VIF
    MM_CHAN_CTXT_LINK_REQ,
    /// Confirmation of the channel context link
    MM_CHAN_CTXT_LINK_CFM,
    /// Request to unlink a channel context from a VIF
    MM_CHAN_CTXT_UNLINK_REQ,
    /// Confirmation of the channel context unlink
    MM_CHAN_CTXT_UNLINK_CFM,
    /// Request to update a channel context
    MM_CHAN_CTXT_UPDATE_REQ,
	
    // 60
    /// Confirmation of the channel context update
    MM_CHAN_CTXT_UPDATE_CFM,
    /// Request to schedule a channel context
    MM_CHAN_CTXT_SCHED_REQ,
    /// Confirmation of the channel context scheduling
    MM_CHAN_CTXT_SCHED_CFM,
    /// Request to change the beacon template in LMAC
    MM_BCN_CHANGE_REQ,
    /// Confirmation of the beacon change
    MM_BCN_CHANGE_CFM,
    /// Request to update the TIM in the beacon (i.e to indicate traffic bufferized at AP)
    MM_TIM_UPDATE_REQ,
    /// Confirmation of the TIM update
    MM_TIM_UPDATE_CFM,
    /// Connection loss indication
    MM_CONNECTION_LOSS_IND,
    /// Channel context switch indication to the upper layers
    MM_CHANNEL_SWITCH_IND,
	
    // 70
    /// Channel context pre-switch indication to the upper layers
    MM_CHANNEL_PRE_SWITCH_IND,
    /// Request to remain on channel or cancel remain on channel
    MM_REMAIN_ON_CHANNEL_REQ,
    /// Confirmation of the (cancel) remain on channel request
    MM_REMAIN_ON_CHANNEL_CFM,
    /// Remain on channel expired indication
    MM_REMAIN_ON_CHANNEL_EXP_IND,
    /// Indication of a PS state change of a peer device
    MM_PS_CHANGE_IND,
    /// Indication that some buffered traffic should be sent to the peer device
    MM_TRAFFIC_REQ_IND,
    /// Request to modify the STA Power-save mode options
    MM_SET_PS_OPTIONS_REQ,
    /// Confirmation of the PS options setting
    MM_SET_PS_OPTIONS_CFM,
    /// Indication of PS state change for a P2P VIF
    MM_P2P_VIF_PS_CHANGE_IND,
    /// Indication that CSA counter has been updated
    MM_CSA_COUNTER_IND,
	
	//80
    /// Message containing channel information
    MM_CHANNEL_SURVEY_IND,
    /// Message containing Beamformer information
    MM_BFMER_ENABLE_REQ,
    /// Request to Start/Stop NOA - GO Only
    MM_SET_P2P_NOA_REQ,
    /// Request to Start/Stop Opportunistic PS - GO Only
    MM_SET_P2P_OPPPS_REQ,
    /// Start/Stop NOA Confirmation
    MM_SET_P2P_NOA_CFM,
    /// Start/Stop Opportunistic PS Confirmation
    MM_SET_P2P_OPPPS_CFM,
    /// P2P NoA Update Indication - GO Only
    MM_P2P_NOA_UPD_IND,
    /// Request to set RSSI threshold and RSSI hysteresis
    MM_CFG_RSSI_REQ,
    /// Indication that RSSI is below or above the threshold
    MM_RSSI_STATUS_IND,
    /// Indication that CSA is done
    MM_CSA_FINISH_IND,
	
	//90
    /// Indication that CSA is in prorgess (resp. done) and traffic must be stopped (resp. restarted)
    MM_CSA_TRAFFIC_IND,
    /// Request to update the group information of a station
    MM_MU_GROUP_UPDATE_REQ,
    /// Confirmation of the @ref MM_MU_GROUP_UPDATE_REQ message
    MM_MU_GROUP_UPDATE_CFM,

    /*
     * Section of internal MM messages. No MM API messages should be defined below this point
     */
    /// Internal request to force the HW going to IDLE
    MM_FORCE_IDLE_REQ,
    /// Message indicating that the switch to the scan channel is done
    MM_SCAN_CHANNEL_START_IND,
    /// Message indicating that the scan on the channel is finished
    MM_SCAN_CHANNEL_END_IND,

	///Get the monitor channel
	MM_GET_CHANNEL_REQ,
	MM_GET_CHANNEL_CFM,

    /// MAX number of messages
    MM_MAX,
};

/// Structure containing the parameters of the @ref MM_START_REQ message
struct mm_start_req
{
    /// PHY configuration
    struct phy_cfg_tag phy_cfg;
    /// UAPSD timeout
    uint32_t uapsd_timeout;
    /// Local LP clock accuracy (in ppm)
    uint16_t lp_clk_accuracy;
};

/// Structure containing the parameters of the @ref MM_SET_CHANNEL_REQ message
struct mm_set_channel_req
{
    /// Band (2.4GHz or 5GHz)
    uint8_t band;
    /// Channel type: 20,40,80,160 or 80+80 MHz
    uint8_t type;
    /// Frequency for Primary 20MHz channel (in MHz)
    uint16_t prim20_freq;
    /// Frequency for Center of the contiguous channel or center of Primary 80+80
    uint16_t center1_freq;
    /// Frequency for Center of the non-contiguous secondary 80+80
    uint16_t center2_freq;
    /// Index of the RF for which the channel has to be set (0: operating (primary), 1: secondary
    /// RF (used for additional radar detection). This parameter is reserved if no secondary RF
    /// is available in the system
    uint8_t index;
    /// Max tx power for this channel
    int8_t tx_power;
};

/// Structure containing the parameters of the @ref MM_SET_CHANNEL_CFM message
struct mm_set_channel_cfm
{
    /// Radio index to be used in policy table
    uint8_t radio_idx;
    /// Actual TX power configured (in dBm)
    int8_t power;
};

struct mm_get_channel_cfm
{
	uint16_t center1_freq;
};

/// Structure containing the parameters of the @ref MM_SET_DTIM_REQ message
struct mm_set_dtim_req
{
    /// DTIM period
    uint8_t dtim_period;
};

/// Structure containing the parameters of the @ref MM_SET_POWER_REQ message
struct mm_set_power_req
{
    /// Index of the interface for which the parameter is configured
    uint8_t inst_nbr;
    /// TX power (in dBm)
    int8_t power;
};

/// Structure containing the parameters of the @ref MM_SET_POWER_CFM message
struct mm_set_power_cfm
{
    /// Radio index to be used in policy table
    uint8_t radio_idx;
    /// Actual TX power configured (in dBm)
    int8_t power;
};

/// Structure containing the parameters of the @ref MM_SET_BEACON_INT_REQ message
struct mm_set_beacon_int_req
{
    /// Beacon interval
    uint16_t beacon_int;
    /// Index of the interface for which the parameter is configured
    uint8_t inst_nbr;
};

/// Structure containing the parameters of the @ref MM_SET_BASIC_RATES_REQ message
struct mm_set_basic_rates_req
{
    /// Basic rate set (as expected by bssBasicRateSet field of Rates MAC HW register)
    uint32_t rates;
    /// Index of the interface for which the parameter is configured
    uint8_t inst_nbr;
    /// Band on which the interface will operate
    uint8_t band;
};

/// Structure containing the parameters of the @ref MM_SET_BSSID_REQ message
struct mm_set_bssid_req
{
    /// BSSID to be configured in HW
    struct mac_addr bssid;
    /// Index of the interface for which the parameter is configured
    uint8_t inst_nbr;
};

/// Structure containing the parameters of the @ref MM_SET_FILTER_REQ message
struct mm_set_filter_req
{
    /// RX filter to be put into rxCntrlReg HW register
    uint32_t filter;
};

/// Structure containing the parameters of the @ref MM_ADD_IF_REQ message.
struct mm_add_if_req
{
    /// Type of the interface (AP, STA, ADHOC, ...)
    uint8_t type;
    /// MAC ADDR of the interface to start
    struct mac_addr addr;
    /// P2P Interface
    bool p2p;
};

/// Structure containing the parameters of the @ref MM_SET_EDCA_REQ message
struct mm_set_edca_req
{
    /// EDCA parameters of the queue (as expected by edcaACxReg HW register)
    uint32_t ac_param;
    /// Flag indicating if UAPSD can be used on this queue
    bool uapsd;
    /// HW queue for which the parameters are configured
    uint8_t hw_queue;
    /// Index of the interface for which the parameters are configured
    uint8_t inst_nbr;
};

/// Structure containing the parameters of the @ref MM_SET_SLOTTIME_REQ message
struct mm_set_slottime_req
{
    /// Slot time expressed in us
    uint8_t slottime;
};

/// Structure containing the parameters of the @ref MM_SET_MODE_REQ message
struct mm_set_mode_req
{
    /// abgnMode field of macCntrl1Reg register
    uint8_t abgnmode;
};

/// Structure containing the parameters of the @ref MM_SET_VIF_STATE_REQ message
struct mm_set_vif_state_req
{
    /// Association Id received from the AP (valid only if the VIF is of STA type)
    uint16_t aid;
    /// Flag indicating if the VIF is active or not
    bool active;
    /// Interface index
    uint8_t inst_nbr;
};

/// Structure containing the parameters of the @ref MM_ADD_IF_CFM message.
struct mm_add_if_cfm
{
    /// Status of operation (different from 0 if unsuccessful)
    uint8_t status;
    /// Interface index assigned by the LMAC
    uint8_t inst_nbr;
};

/// Structure containing the parameters of the @ref MM_REMOVE_IF_REQ message.
struct mm_remove_if_req
{
    /// Interface index assigned by the LMAC
    uint8_t inst_nbr;
};

/// Structure containing the parameters of the @ref MM_VERSION_CFM message.
struct mm_version_cfm
{
    /// Version of the LMAC FW
    uint32_t version_lmac;
    /// Version1 of the MAC HW (as encoded in version1Reg MAC HW register)
    uint32_t version_machw_1;
    /// Version2 of the MAC HW (as encoded in version2Reg MAC HW register)
    uint32_t version_machw_2;
    /// Version1 of the PHY (depends on actual PHY)
    uint32_t version_phy_1;
    /// Version2 of the PHY (depends on actual PHY)
    uint32_t version_phy_2;
    /// Supported Features
    uint32_t features;
};

/// Structure containing the parameters of the @ref MM_STA_ADD_REQ message.
struct mm_sta_add_req
{
    /// Maximum A-MPDU size, in bytes, for VHT frames
    uint32_t ampdu_size_max_vht;
    /// Maximum A-MPDU size, in bytes, for HT frames
    uint16_t ampdu_size_max_ht;
    /// MAC address of the station to be added
    struct mac_addr mac_addr;
    /// A-MPDU spacing, in us
    uint8_t ampdu_spacing_min;
    /// Interface index
    uint8_t inst_nbr;
    /// TDLS station
    bool tdls_sta;
};

/// Structure containing the parameters of the @ref MM_STA_ADD_CFM message.
struct mm_sta_add_cfm
{
    /// Status of the operation (different from 0 if unsuccessful)
    uint8_t status;
    /// Index assigned by the LMAC to the newly added station
    uint8_t sta_idx;
    /// MAC HW index of the newly added station
    uint8_t hw_sta_idx;
};

/// Structure containing the parameters of the @ref MM_STA_DEL_REQ message.
struct mm_sta_del_req
{
    /// Index of the station to be deleted
    uint8_t sta_idx;
};

/// Structure containing the parameters of the @ref MM_STA_DEL_CFM message.
struct mm_sta_del_cfm
{
    /// Status of the operation (different from 0 if unsuccessful)
    uint8_t     status;
};

/// Structure containing the parameters of the @ref MM_SET_IDLE_REQ message.
struct mm_set_idle_req
{
    /// 0x01: HW IDLE, 0x00: HW_ACTIVE
    uint8_t hw_idle;
};

/// Structure containing the parameters of the @ref MM_KEY_ADD_REQ message.
struct mm_key_add_req
{
    /// Key index (valid only for default keys)
    uint8_t key_idx;
    /// STA index (valid only for pairwise keys or mesh group keys)
    uint8_t sta_idx;
    /// Key material
    struct mac_sec_key  key;
    /// Cipher suite (WEP64, WEP128, TKIP, CCMP)
    uint8_t cipher_suite;
    /// Index of the interface for which the key is set (valid only for default keys or mesh group keys)
    uint8_t inst_nbr;
    /// A-MSDU SPP parameter
    uint8_t spp;
    /// Indicate if provided key is a pairwise key or not
    bool pairwise;
};

/// Structure containing the parameters of the @ref MM_KEY_ADD_CFM message.
struct mm_key_add_cfm
{
    /// Status of the operation (different from 0 if unsuccessful)
    uint8_t status;
    /// HW index of the key just added
    uint8_t hw_key_idx;
};

/// Structure containing the parameters of the @ref MM_KEY_DEL_REQ message.
struct mm_key_del_req
{
    /// HW index of the key to be deleted
    uint8_t hw_key_idx;
};

/// Structure containing the parameters of the @ref MM_BA_ADD_REQ message.
struct mm_ba_add_req
{
    ///Type of agreement (0: TX, 1: RX)
    uint8_t  type;
    ///Index of peer station with which the agreement is made
    uint8_t  sta_idx;
    ///TID for which the agreement is made with peer station
    uint8_t  tid;
    ///Buffer size - number of MPDUs that can be held in its buffer per TID
    uint8_t  bufsz;
    /// Start sequence number negotiated during BA setup - the one in first aggregated MPDU counts more
    uint16_t ssn;
};

/// Structure containing the parameters of the @ref MM_BA_ADD_CFM message.
struct mm_ba_add_cfm
{
    ///Index of peer station for which the agreement is being confirmed
    uint8_t sta_idx;
    ///TID for which the agreement is being confirmed
    uint8_t tid;
    /// Status of ba establishment
    uint8_t status;
};

#if (NX_AMPDU_TX || NX_REORD)
/// Structure containing the parameters of the @ref MM_BA_DEL_REQ message.
struct mm_ba_del_req
{
    ///Type of agreement (0: TX, 1: RX)
    uint8_t type;
    ///Index of peer station for which the agreement is being deleted
    uint8_t sta_idx;
    ///TID for which the agreement is being deleted
    uint8_t tid;
};

/// Structure containing the parameters of the @ref MM_BA_DEL_CFM message.
struct mm_ba_del_cfm
{
    ///Index of peer station for which the agreement deletion is being confirmed
    uint8_t sta_idx;
    ///TID for which the agreement deletion is being confirmed
    uint8_t tid;
    /// Status of ba deletion
    uint8_t status;
};
#endif //NX_AMPDU_TX || NX_REORD

/// Structure containing the parameters of the @ref MM_CHANNEL_SWITCH_IND message
struct mm_channel_switch_ind
{
    /// Index of the channel context we will switch to
    uint8_t chan_index;
    /// Indicate if the switch has been triggered by a Remain on channel request
    bool roc;
    /// VIF on which remain on channel operation has been started (if roc == 1)
    uint8_t vif_index;
    /// Indicate if the switch has been triggered by a TDLS Remain on channel request
    bool roc_tdls;
};

/// Structure containing the parameters of the @ref MM_CHANNEL_PRE_SWITCH_IND message
struct mm_channel_pre_switch_ind
{
    /// Index of the channel context we will switch to
    uint8_t chan_index;
};

/// Structure containing the parameters of the @ref MM_CONNECTION_LOSS_IND message.
struct mm_connection_loss_ind
{
    /// VIF instance number
    uint8_t inst_nbr;
};

/// Structure containing the parameters of the @ref MM_DBG_TRIGGER_REQ message.
struct mm_dbg_trigger_req
{
    /// Error trace to be reported by the LMAC
    char error[64];
};

#if NX_POWERSAVE
/// Structure containing the parameters of the @ref MM_SET_PS_MODE_REQ message.
struct mm_set_ps_mode_req
{
    /// Power Save is activated or deactivated
    uint8_t  new_state;
};
#endif

/// Structure containing the parameters of the @ref MM_CHAN_CTXT_ADD_REQ message
struct mm_chan_ctxt_add_req
{
    /// Band (2.4GHz or 5GHz)
    uint8_t band;
    /// Channel type: 20,40,80,160 or 80+80 MHz
    uint8_t type;
    /// Frequency for Primary 20MHz channel (in MHz)
    uint16_t prim20_freq;
    /// Frequency for Center of the contiguous channel or center of Primary 80+80
    uint16_t center1_freq;
    /// Frequency for Center of the non-contiguous secondary 80+80
    uint16_t center2_freq;
    /// Max tx power for this channel
    int8_t tx_power;
};

/// Structure containing the parameters of the @ref MM_CHAN_CTXT_ADD_REQ message
struct mm_chan_ctxt_add_cfm
{
    /// Status of the addition
    uint8_t status;
    /// Index of the new channel context
    uint8_t index;
};


/// Structure containing the parameters of the @ref MM_CHAN_CTXT_DEL_REQ message
struct mm_chan_ctxt_del_req
{
    /// Index of the new channel context to be deleted
    uint8_t index;
};


/// Structure containing the parameters of the @ref MM_CHAN_CTXT_LINK_REQ message
struct mm_chan_ctxt_link_req
{
    /// VIF index
    uint8_t vif_index;
    /// Channel context index
    uint8_t chan_index;
    /// Indicate if this is a channel switch (unlink current ctx first if true)
    uint8_t chan_switch;
};

/// Structure containing the parameters of the @ref MM_CHAN_CTXT_UNLINK_REQ message
struct mm_chan_ctxt_unlink_req
{
    /// VIF index
    uint8_t vif_index;
};

/// Structure containing the parameters of the @ref MM_CHAN_CTXT_UPDATE_REQ message
struct mm_chan_ctxt_update_req
{
    /// Channel context index
    uint8_t chan_index;
    /// Band (2.4GHz or 5GHz)
    uint8_t band;
    /// Channel type: 20,40,80,160 or 80+80 MHz
    uint8_t type;
    /// Frequency for Primary 20MHz channel (in MHz)
    uint16_t prim20_freq;
    /// Frequency for Center of the contiguous channel or center of Primary 80+80
    uint16_t center1_freq;
    /// Frequency for Center of the non-contiguous secondary 80+80
    uint16_t center2_freq;
    /// Max TX power for this channel
    int8_t tx_power;
};

/// Structure containing the parameters of the @ref MM_CHAN_CTXT_SCHED_REQ message
struct mm_chan_ctxt_sched_req
{
    /// VIF index
    uint8_t vif_index;
    /// Channel context index
    uint8_t chan_index;
    /// Type of the scheduling request (0: normal scheduling, 1: derogatory
    /// scheduling)
    uint8_t type;
};

/// Structure containing the parameters of the @ref MM_BCN_CHANGE_REQ message.
#define BCN_MAX_CSA_CPT                   2

struct mm_bcn_change_req
{
    /// Pointer, in host memory, to the new beacon template
    uint32_t bcn_ptr;
    /// Length of the beacon template
    uint16_t bcn_len;
    /// Offset of the TIM IE in the beacon
    uint16_t tim_oft;
    /// Length of the TIM IE
    uint8_t tim_len;
    /// Index of the VIF for which the beacon is updated
    uint8_t inst_nbr;
    /// Offset of CSA (channel switch announcement) counters (0 means no counter)
    uint8_t csa_oft[BCN_MAX_CSA_CPT];
	
	uint32_t bcn_ptr_malloc_flag;
};

/// Structure containing the parameters of the @ref MM_TIM_UPDATE_REQ message.
struct mm_tim_update_req
{
    /// Association ID of the STA the bit of which has to be updated (0 for BC/MC traffic)
    uint16_t aid;
    /// Flag indicating the availability of data packets for the given STA
    uint8_t tx_avail;
    /// Index of the VIF for which the TIM is updated
    uint8_t inst_nbr;
};

/// Structure containing the parameters of the @ref MM_PS_CHANGE_IND message
struct mm_ps_change_ind
{
    /// Index of the peer device that is switching its PS state
    uint8_t sta_idx;
    /// New PS state of the peer device (0: active, 1: sleeping)
    uint8_t ps_state;
};

/// Structure containing the parameters of the @ref MM_P2P_VIF_PS_CHANGE_IND message
struct mm_p2p_vif_ps_change_ind
{
    /// Index of the P2P VIF that is switching its PS state
    uint8_t vif_index;
    /// New PS state of the P2P VIF interface (0: active, 1: sleeping)
    uint8_t ps_state;
};

/// Structure containing the parameters of the @ref MM_TRAFFIC_REQ_IND message
struct mm_traffic_req_ind
{
    /// Index of the peer device that needs traffic
    uint8_t sta_idx;
    /// Number of packets that need to be sent (if 0, all buffered traffic shall be sent)
    uint8_t pkt_cnt;
    /// Flag indicating if the traffic request concerns U-APSD queues or not
    bool uapsd;
};


/// Structure containing the parameters of the @ref MM_REMAIN_ON_CHANNEL_REQ message.
struct mm_remain_on_channel_req
{
    /// Operation Code
    uint8_t op_code;
    /// VIF Index
    uint8_t vif_index;
    /// Band (2.4GHz or 5GHz)
    uint8_t band;
    /// Channel type: 20,40,80,160 or 80+80 MHz
    uint8_t type;
    /// Frequency for Primary 20MHz channel (in MHz)
    uint16_t prim20_freq;
    /// Frequency for Center of the contiguous channel or center of Primary 80+80
    uint16_t center1_freq;
    /// Frequency for Center of the non-contiguous secondary 80+80
    uint16_t center2_freq;
    /// Duration
    uint32_t duration_ms;
    /// Tx Power
    int8_t tx_power;
};

/// Structure containing the parameters of the @ref MM_REMAIN_ON_CHANNEL_CFM message
struct mm_remain_on_channel_cfm
{
    /// Operation Code
    uint8_t op_code;
    /// Status of the operation
    uint8_t status;
    /// Channel Context index
    uint8_t chan_ctxt_index;
};

/// Structure containing the parameters of the @ref MM_REMAIN_ON_CHANNEL_EXP_IND message
struct mm_remain_on_channel_exp_ind
{
    /// VIF Index
    uint8_t vif_index;
    /// Channel Context Index
    uint8_t chan_ctxt_index;
};

/// Structure containing the parameters of the @ref MM_SET_PS_OPTIONS_REQ message.
struct mm_set_ps_options_req
{
    /// VIF Index
    uint8_t vif_index;
    /// Listen interval (0 if wake up shall be based on DTIM period)
    uint16_t listen_interval;
    /// Flag indicating if we shall listen the BC/MC traffic or not
    bool dont_listen_bc_mc;
};

/// Structure containing the parameters of the @ref MM_CSA_COUNTER_IND message
struct mm_csa_counter_ind
{
    /// Index of the VIF
    uint8_t vif_index;
    /// Updated CSA counter value
    uint8_t csa_count;
};

/// Structure containing the parameters of the @ref MM_CHANNEL_SURVEY_IND message
struct mm_channel_survey_ind
{
    /// Frequency of the channel
    uint16_t freq;
    /// Noise in dbm
    int8_t noise_dbm;
    /// Amount of time spent on the channel (in ms)
    uint32_t chan_time_ms;
    /// Amount of time the primary channel was sensed busy
    uint32_t chan_time_busy_ms;
};

/// Structure containing the parameters of the @ref MM_BFMER_ENABLE_REQ message
struct mm_bfmer_enable_req
{
    /// Address of Beamforming Report in host memory
    uint32_t host_bfr_addr;
    /// AID
    uint16_t aid;
    /// STA Index
    uint8_t sta_idx;
    /// Maximum number of spatial streams the station can receive
    uint8_t rx_nss;
    /// Indicate if peer STA is MU Beamformee (VHT) capable
    bool vht_mu_bfmee;
};

/// Structure containing the parameters of the @ref MM_CFG_RSSI_REQ message
struct mm_cfg_rssi_req
{
    /// Index of the VIF
    uint8_t vif_index;
    /// RSSI threshold
    int8_t rssi_thold;
    /// RSSI hysteresis
    uint8_t rssi_hyst;
};

/// Structure containing the parameters of the @ref MM_RSSI_STATUS_IND message
struct mm_rssi_status_ind
{
    /// Index of the VIF
    uint8_t vif_index;
    /// Status of RSSI (0=RSSI is high, 1=RSSI is low)
    bool rssi_status;
};

/// Structure containing the parameters of the @ref MM_CSA_FINISH_IND message
struct mm_csa_finish_ind
{
    /// Index of the VIF
    uint8_t vif_index;
    /// Status of the operation
    uint8_t status;
    /// New channel ctx index
    uint8_t chan_idx;
};

/// Structure containing the parameters of the @ref MM_CSA_TRAFFIC_IND message
struct mm_csa_traffic_ind
{
    /// Index of the VIF
    uint8_t vif_index;
    /// Is tx traffic enable or disable
    bool enable;
};

/// Pointer to callback function
typedef void (*cb_idle_func_ptr)(void);

/// Structure containing the parameters of the @ref MM_FORCE_IDLE_REQ message.
struct mm_force_idle_req
{
    /// Callback function to call when mode has been changed
    cb_idle_func_ptr cb;
};

/// Structure containing the parameters of the @ref MM_SET_P2P_NOA_REQ message.
struct mm_set_p2p_noa_req
{
    /// VIF Index
    uint8_t vif_index;
    /// Allocated NOA Instance Number - Valid only if count = 0
    uint8_t noa_inst_nb;
    /// Count
    uint8_t count;
    /// Indicate if NoA can be paused for traffic reason
    bool dyn_noa;
    /// Duration (in us)
    uint32_t duration_us;
    /// Interval (in us)
    uint32_t interval_us;
    /// Start Time offset from next TBTT (in us)
    uint32_t start_offset;
};

/// Structure containing the parameters of the @ref MM_SET_P2P_OPPPS_REQ message.
struct mm_set_p2p_oppps_req
{
    /// VIF Index
    uint8_t vif_index;
    /// CTWindow
    uint8_t ctwindow;
};

/// Structure containing the parameters of the @ref MM_SET_P2P_NOA_CFM message.
struct mm_set_p2p_noa_cfm
{
    /// Request status
    uint8_t status;
};

/// Structure containing the parameters of the @ref MM_SET_P2P_OPPPS_CFM message.
struct mm_set_p2p_oppps_cfm
{
    /// Request status
    uint8_t status;
};

/// Structure containing the parameters of the @ref MM_P2P_NOA_UPD_IND message.
struct mm_p2p_noa_upd_ind
{
    /// VIF Index
    uint8_t vif_index;
    /// NOA Instance Number
    uint8_t noa_inst_nb;
    /// NoA Type
    uint8_t noa_type;
    /// Count
    uint8_t count;
    /// Duration (in us)
    uint32_t duration_us;
    /// Interval (in us)
    uint32_t interval_us;
    /// Start Time
    uint32_t start_time;
};

/// Structure containing the parameters of the @ref MM_MU_GROUP_UPDATE_REQ message.
struct mm_mu_group_update_req
{
    /// Station index
    uint8_t sta_idx;
    /// Number of groups the STA belongs to
    uint8_t group_cnt;
    /// Group information
    struct
    {
        /// Group Id
        uint8_t group_id;
        /// User position
        uint8_t user_pos;
    } groups[];
};

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */
extern const struct ke_state_handler mm_state_handler[MM_STATE_MAX];

extern const struct ke_state_handler mm_default_handler;

extern ke_state_t mm_state[MM_IDX_MAX];

/// @} end of group

#endif // _MM_TASK_H_

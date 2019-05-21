/**
 ****************************************************************************************
 *
 * @file tdls_task.h
 *
 * @brief TDLSt task declarations.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef TDLS_TASK_H_
#define TDLS_TASK_H_

#if (TDLS_ENABLE)
/**
 *****************************************************************************************
 * @defgroup TDLS_TASK TDLS_TASK
 * @ingroup TDLS
 * @brief Task responsible for TDLS operations
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

struct chan_ctxt_tag;
struct vif_info_tag;

/*
 * DEFINES
 ****************************************************************************************
 */
/// TDLS TASK Instance Max.
#define TDLS_IDX_MAX 1

/// Possible States of the TDLS Task.
enum tdls_state_tag
{
    /// TDLS link on the base channel
    TDLS_BASE_CHANNEL,
    /// TDLS peer STA is sleeping
    TDLS_PEER_SLEEPING,
    /// TDLS Peer Traffic Indication frame sent
    TDLS_TRAFFIC_IND_TX,
    /// TDLS Peer Traffic Indication frame received
    TDLS_TRAFFIC_IND_RX,
    /// TDLS Channel Switch Request frame sent
    TDLS_CHSW_REQ_TX,
    /// TDLS Channel Switch Request frame received
    TDLS_CHSW_REQ_RX,
    /// TDLS link on the off-channel
    TDLS_OFF_CHANNEL,
    /// TDLS Max Number of states.
    TDLS_STATE_MAX
};

/// List of messages related to the task.
enum tdls_msg_tag
{
    /// TDLS channel Switch Request.
    TDLS_CHAN_SWITCH_REQ = KE_FIRST_MSG(TASK_TDLS),
    /// TDLS channel switch confirmation.
    TDLS_CHAN_SWITCH_CFM,
    /// TDLS channel switch indication.
    TDLS_CHAN_SWITCH_IND,
    /// TDLS channel switch to base channel indication.
    TDLS_CHAN_SWITCH_BASE_IND,
    /// TDLS cancel channel switch request.
    TDLS_CANCEL_CHAN_SWITCH_REQ,
    /// TDLS cancel channel switch confirmation.
    TDLS_CANCEL_CHAN_SWITCH_CFM,
    /// TDLS peer power save indication.
    TDLS_PEER_PS_IND,
    /// TDLS peer traffic indication request.
    TDLS_PEER_TRAFFIC_IND_REQ,
    /// TDLS peer traffic indication confirmation.
    TDLS_PEER_TRAFFIC_IND_CFM,
    /// MAX number of messages
    TDLS_MAX
};

/// Structure containing the parameters of the @ref TDLS_OPER_REQUEST_IND message
struct tdls_oper_request_ind
{
    /// Index of the VIF
    uint8_t vif_index;
    /// MAC address of the TDLS peer
    struct mac_addr peer_mac_addr;
    /// TDLS operation
    uint8_t operation;
    /// Reason code
    uint16_t reason_code;
};

/// Structure containing the parameters of the @ref TDLS_CHAN_SWITCH_REQ message
struct tdls_chan_switch_req
{
    /// Index of the VIF
    uint8_t vif_index;
    /// MAC address of the TDLS peer
    struct mac_addr peer_mac_addr;
    /// Flag to indicate if the TDLS peer is the TDLS initiator
    bool initiator;
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
    /// TX power (in dBm)
    int8_t tx_power;
    /// Operating class
    uint8_t op_class;
};

/// Structure containing the parameters of the @ref TDLS_CANCEL_CHAN_SWITCH_REQ message
struct tdls_cancel_chan_switch_req
{
    /// Index of the VIF
    uint8_t vif_index;
    /// MAC address of the TDLS peer
    struct mac_addr peer_mac_addr;
};

/// Structure containing the parameters of the @ref TDLS_CHAN_SWITCH_CFM message
struct tdls_chan_switch_cfm
{
    /// Status of the operation
    uint8_t status;
};

/// Structure containing the parameters of the @ref TDLS_CANCEL_CHAN_SWITCH_CFM message
struct tdls_cancel_chan_switch_cfm
{
    /// Status of the operation
    uint8_t status;
};

/// Structure containing the parameters of the @ref TDLS_CHAN_SWITCH_IND message
struct tdls_chan_switch_ind
{
    /// VIF Index
    uint8_t vif_index;
    /// Channel Context Index
    uint8_t chan_ctxt_index;
    /// Status of the operation
    uint8_t status;
};

/// Structure containing the parameters of the @ref TDLS_CHAN_SWITCH_BASE_IND message
struct tdls_chan_switch_base_ind
{
    /// VIF Index
    uint8_t vif_index;
    /// Channel Context Index
    uint8_t chan_ctxt_index;
};

/// Structure containing the parameters of the @ref TDLS_PEER_PS_IND message
struct tdls_peer_ps_ind
{
    /// VIF Index
    uint8_t vif_index;
    /// MAC address of the TDLS peer
    struct mac_addr peer_mac_addr;
    /// Flag to indicate if the TDLS peer is going to sleep
    bool ps_on;
};

/// Structure containing the parameters of the @ref TDLS_PEER_TRAFFIC_IND_REQ message
struct tdls_peer_traffic_ind_req
{
    /// VIF Index
    uint8_t vif_index;
    /// MAC address of the TDLS peer
    struct mac_addr peer_mac_addr;
    /// Dialog token
    uint8_t dialog_token;
    /// TID of the latest MPDU transmitted over the TDLS direct link to the TDLS STA
    uint8_t last_tid;
    /// Sequence number of the latest MPDU transmitted over the TDLS direct link
    /// to the TDLS STA
    uint16_t last_sn;
};

/// Structure containing the parameters of the @ref TDLS_PEER_TRAFFIC_IND_CFM message
struct tdls_peer_traffic_ind_cfm
{
    /// Status of the operation
    uint8_t status;
};

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */
extern const struct ke_state_handler tdls_default_handler;
extern ke_state_t tdls_state[TDLS_IDX_MAX];

#endif //TDLS_ENABLE

#endif /* TDLS_TASK_H_ */

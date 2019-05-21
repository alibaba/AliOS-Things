/**
 ****************************************************************************************
 *
 * @file mesh_task.h
 *
 * Copyright (C) RivieraWaves 2016-2016
 *
 * @brief Mesh Task Kernel Interface Declaration.
 *
 ****************************************************************************************
 */

#ifndef _MESH_TASK_H_
#define _MESH_TASK_H_

/**
 * @defgroup TASK_MESH TASK_MESH
 * @ingroup MESH
 * @brief Declaration of all structures and functions used by the Mesh module.
 * @{
 */

/**
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mesh.h"

#if (RW_MESH_EN)

#include "ke_task.h"
#include "scan.h"

/**
 * DEFINES
 ****************************************************************************************
 */

/// Number of Mesh Task instances
#define MESH_IDX_MAX        (1)

/**
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Possible states of the MESH task.
enum mesh_state_tag
{
    /// IDLE State.
    MESH_IDLE,
    /// BUSY State
    MESH_BUSY,

    /// Number of states.
    MESH_STATE_MAX
};

/// Messages that are logically related to the task.
enum mesh_msg_tag
{
    /// Request to start the Mesh Point
    MESH_START_REQ = KE_FIRST_MSG(TASK_MESH),
    /// Confirmation of the MP start.
    MESH_START_CFM,

    /// Request to stop the MP.
    MESH_STOP_REQ,
    /// Confirmation of the MP stop.
    MESH_STOP_CFM,

    // Request to update the MP
    MESH_UPDATE_REQ,
    /// Confirmation of the MP update
    MESH_UPDATE_CFM,

    /// Request information about a given link
    MESH_PEER_INFO_REQ,
    /// Response to the MESH_PEER_INFO_REQ message
    MESH_PEER_INFO_RSP,

    /// Request establishment of a path with a given mesh STA
    MESH_PATH_CREATE_REQ,
    /// Confirmation to the MESH_PATH_CREATE_REQ message
    MESH_PATH_CREATE_CFM,

    /// Request a path update (delete path, modify next hop mesh STA)
    MESH_PATH_UPDATE_REQ,
    /// Confirmation to the MESH_PATH_UPDATE_REQ message
    MESH_PATH_UPDATE_CFM,

    /// Indication from Host that the indicated Mesh Interface is a proxy for an external STA
    MESH_PROXY_ADD_REQ,

    /// Indicate that a connection has been established or lost
    MESH_PEER_UPDATE_IND,
    /// Notification that a connection has been established or lost (when MPM handled by userspace)
    MESH_PEER_UPDATE_NTF = MESH_PEER_UPDATE_IND,

    /// Indicate that a path is now active or inactive
    MESH_PATH_UPDATE_IND,
    /// Indicate that proxy information have been updated
    MESH_PROXY_UPDATE_IND,
};

/**
* API MESSAGES STRUCTURES
****************************************************************************************
*/

/// Structure containing the parameters of the @ref MESH_START_REQ message.
struct mesh_start_req
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
    /// DTIM period
    uint8_t dtim_period;
    /// Beacon interval
    uint16_t bcn_int;
    /// Index of the VIF for which the MP is started
    uint8_t vif_idx;
    /// Length of the Mesh ID
    uint8_t mesh_id_len;
    /// Mesh ID
    uint8_t mesh_id[MESH_MESHID_MAX_LEN];
    /// Address of the IEs to download
    uint32_t ie_addr;
    /// Length of the provided IEs
    uint8_t ie_len;
    /// Indicate if Mesh Peering Management (MPM) protocol is handled in userspace
    bool user_mpm;
    /// Indicate if Mesh Point is using authentication
    bool is_auth;
    /// Indicate which authentication method is used
    uint8_t auth_id;
};

/// Structure containing the parameters of the @ref MESH_START_CFM message.
struct mesh_start_cfm
{
    /// Status of the MP starting procedure
    uint8_t status;
    /// Index of the VIF for which the MP is started
    uint8_t vif_idx;
    /// Index of the channel context attached to the VIF
    uint8_t ch_idx;
    /// Index of the STA used for BC/MC traffic
    uint8_t bcmc_idx;
};

/// Structure containing the parameters of the @ref MESH_STOP_REQ message.
struct mesh_stop_req
{
    /// Index of the VIF for which the MP has to be stopped
    uint8_t vif_idx;
};

/// Structure containing the parameters of the @ref MESH_STOP_CFM message.
struct mesh_stop_cfm
{
    /// Index of the VIF for which the MP has been stopped
    uint8_t vif_idx;
    /// Status
    uint8_t status;
};

/// Structure containing the parameters of the @ref MESH_UPDATE_REQ message.
struct mesh_update_req
{
    /// Flags, indicate fields which have been updated
    uint8_t flags;
    /// VIF Index
    uint8_t vif_idx;
    /// Root Mode
    uint8_t root_mode;
    /// Gate Announcement
    bool gate_announ;
    /// Mesh Forwarding
    bool mesh_forward;
    /// Local PS Mode
    uint8_t local_ps_mode;
};

/// Structure containing the parameters of the @ref MESH_UPDATE_CFM message.
struct mesh_update_cfm
{
    /// Status
    uint8_t status;
};

/// Structure containing the parameters of the @ref MESH_PEER_INFO_REQ message.
struct mesh_peer_info_req
{
    /// Index of the station allocated for the peer
    uint8_t sta_idx;
};

/// Structure containing the parameters of the @ref MESH_PEER_INFO_RSP message.
struct mesh_peer_info_rsp
{
    /// Response status
    uint8_t status;
    /// Index of the station allocated for the peer
    uint8_t sta_idx;
    /// Local Link ID
    uint16_t local_link_id;
    /// Peer Link ID
    uint16_t peer_link_id;
    /// Local PS Mode
    uint8_t local_ps_mode;
    /// Peer PS Mode
    uint8_t peer_ps_mode;
    /// Non-peer PS Mode
    uint8_t non_peer_ps_mode;
    /// Link State
    uint8_t link_state;
};

/// Structure containing the parameters of the @ref MESH_PATH_CREATE_REQ message.
struct mesh_path_create_req
{
    /// Index of the interface on which path has to be created
    uint8_t vif_idx;
    /// Indicate if originator MAC Address is provided
    bool has_orig_addr;
    /// Path Target MAC Address
    struct mac_addr tgt_mac_addr;
    /// Originator MAC Address
    struct mac_addr orig_mac_addr;
};

/// Structure containing the parameters of the @ref MESH_PATH_CREATE_CFM message.
struct mesh_path_create_cfm
{
    /// Confirmation status
    uint8_t status;
    /// VIF Index
    uint8_t vif_idx;
};

/// Structure containing the parameters of the @ref MESH_PATH_UPDATE_REQ message.
struct mesh_path_update_req
{
    /// Indicate if path must be deleted
    bool delete;
    /// Index of the interface on which path has to be created
    uint8_t vif_idx;
    /// Path Target MAC Address
    struct mac_addr tgt_mac_addr;
    /// Next Hop MAC Address
    struct mac_addr nhop_mac_addr;
};

/// Structure containing the parameters of the @ref MESH_PATH_UPDATE_CFM message.
struct mesh_path_update_cfm
{
    /// Confirmation status
    uint8_t status;
    /// VIF Index
    uint8_t vif_idx;
};

/// Structure containing the parameters of the @ref MESH_PROXY_ADD_REQ message.
struct mesh_proxy_add_req
{
    /// VIF Index
    uint8_t vif_idx;
    /// MAC Address of the External STA
    struct mac_addr ext_sta_addr;
};

/// Structure containing the parameters of the @ref MESH_PROXY_UPDATE_IND
struct mesh_proxy_update_ind
{
    /// Indicate if proxy information has been added or deleted
    bool delete;
    /// Indicate if we are a proxy for the external STA
    bool local;
    /// VIF Index
    uint8_t vif_idx;
    /// MAC Address of the External STA
    struct mac_addr ext_sta_addr;
    /// MAC Address of the proxy (only valid if local is false)
    struct mac_addr proxy_mac_addr;
};

/// Structure containing the parameters of the @ref MESH_PEER_UPDATE_IND message.
struct mesh_peer_update_ind
{
    /// Indicate if connection has been established or lost
    bool estab;
    /// VIF Index
    uint8_t vif_idx;
    /// STA Index
    uint8_t sta_idx;
    /// Peer MAC Address
    struct mac_addr peer_addr;
};

/// Structure containing the parameters of the @ref MESH_PEER_UPDATE_NTF message.
struct mesh_peer_update_ntf
{
    /// VIF Index
    uint8_t vif_idx;
    /// STA Index
    uint8_t sta_idx;
    /// Mesh Link State
    uint8_t state;
};

/// Structure containing the parameters of the @ref MESH_PATH_UPDATE_IND message.
struct mesh_path_update_ind
{
    /// Indicate if mesh path is deleted or not
    bool delete;
    /// Indicate if path is towards an external STA (not part of MBSS)
    bool ext_sta;
    /// VIF Index
    uint8_t vif_idx;
    /// Path Index
    uint8_t path_idx;
    /// Target MAC Address
    struct mac_addr tgt_mac_addr;
    /// External STA MAC Address (only if ext_sta is true)
    struct mac_addr ext_sta_mac_addr;
    /// Next Hop STA Index
    uint8_t nhop_sta_idx;
};

/**
* GLOBAL VARIABLES DEFINITION
****************************************************************************************
*/

/// Task Information
extern const struct ke_state_handler mesh_state_handler[MESH_STATE_MAX];
extern const struct ke_state_handler mesh_default_handler;
extern ke_state_t mesh_state[MESH_IDX_MAX];

#endif //(RW_MESH_EN)

/// @} end of group

#endif // _MESH_TASK_H_

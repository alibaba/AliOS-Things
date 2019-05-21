/**
****************************************************************************************
*
* @file sm_task.h
*
* Copyright (C) RivieraWaves 2011-2016
*
* @brief Declaration of the SM state machine.
*
****************************************************************************************
*/
#ifndef _SM_TASK_H_
#define _SM_TASK_H_

/** @defgroup TASK_SM TASK_SM
 * @ingroup SM
 * @brief Declaration of the SM state machine.
 */

/** @addtogroup TASK_SM
* @{
*/

#include "sm.h"
#include "scan.h"

/// Task max index number.
#define SM_IDX_MAX 1

/// SM task possible states.
enum sm_state_tag
{
    /// IDLE state
    SM_IDLE,
    /// SCANNING state
    SM_SCANNING,
    /// JOIN state
    SM_JOINING,
    /// Addition of station
    SM_STA_ADDING,
    ///PS disabling state
    SM_DISABLING_PS,
    /// Configuration of BSS parameters
    SM_BSS_PARAM_SETTING,
    ///AUTHENTICATE state
    SM_AUTHENTICATING,
    ///ASSOCIATE state
    SM_ASSOCIATING,
    ///ACTIVATE state
    SM_ACTIVATING,
    ///DISCONNECTING state
    SM_DISCONNECTING,
    /// Number of states
    SM_STATE_MAX
};

/// Message API of the SM task
enum sm_msg_tag
{
    /// Request to reset SM
    SM_RESET_REQ = KE_FIRST_MSG(TASK_SM),
    /// Confirmation of reset req
    SM_RESET_CFM,
    /// Request to connect to an AP
    SM_CONNECT_REQ,
    /// Confirmation of connection
    SM_CONNECT_CFM,
    /// Indicates that the SM associated to the AP
    SM_CONNECT_IND,
    /// Request to disconnect
    SM_DISCONNECT_REQ,
    /// Confirmation of disconnection
    SM_DISCONNECT_CFM,
    /// Indicates that the SM disassociated the AP
    SM_DISCONNECT_IND,
    /// Request to Set power mode in SM
    SM_POWER_MGMT_REQ,
    /// Confirmation of power mgmt req
    SM_POWER_MGMT_CFM,
    /// Syncloss Indication
    SM_SYNCLOST_IND,
    /// Timeout message for procedures requiring a response from peer
    SM_RSP_TIMEOUT_IND,
    /// Roaming timer  (start with 10 sec)
    SM_ROAMING_TIMER_IND,
	/// Request to get bss infomation
	SM_GET_BSS_INFO_REQ,
	/// Confirmation of get bss information
	SM_GET_BSS_INFO_CFM,
	// Indicates start connection
	SM_CONNCTION_START_IND,
	// Indicats that the beacon is lost
	SM_BEACON_LOSE_IND,
	// Indicate authentication failure
	SM_AUTHEN_FAIL_IND,
	// Indicates association failure
	SM_ASSOC_FAIL_INID,
    /// Indicates that the SM associated the AP
    SM_ASSOC_IND,
    /// Indicates that the SM associated the AP
    SM_DEASSOC_IND,
    /// Indicates that the SM associated the AP
    SM_ASSOC_FAILED_IND,
};

struct sm_fail_stat
{
	uint16_t status;
};

struct sm_get_bss_info_req
{
    uint8_t vif_idx;
};

struct sm_get_bss_info_cfm
{
    int8_t rssi;
    uint8_t ssid[32];
    uint8_t bssid[6];
    uint16_t freq;
};

/// Structure containing the parameters of SM_RESET_REQ the message.
struct sm_reset_req
{
    // Set to TRUE if all configuration MIBs will be reset to default
    bool set_defmib;
};

/// Structure containing the parameters of @ref SM_CONNECT_REQ message.
struct sm_connect_req
{
    /// SSID to connect to
    struct mac_ssid ssid;
    /// BSSID to connect to (if not specified, set this field to WILDCARD BSSID)
    struct mac_addr bssid;
    /// Channel on which we have to connect (if not specified, set -1 in the chan.freq field)
    struct scan_chan_tag chan;
    /// Connection flags
    uint32_t flags;
    /// Control port Ethertype
    uint16_t ctrl_port_ethertype;
    /// Length of the association request IEs
    uint16_t ie_len;
    /// Listen interval to be used for this connection
    uint16_t listen_interval;
    /// Flag indicating if the we have to wait for the BC/MC traffic after beacon or not
    bool dont_wait_bcmc;
    /// Authentication type
    uint8_t auth_type;
    /// UAPSD queues (bit0: VO, bit1: VI, bit2: BK, bit3: BE)
    uint8_t uapsd_queues;
    /// VIF index
    uint8_t vif_idx;
    /// Buffer containing the additional information elements to be put in the
    /// association request
    uint32_t ie_buf[64];
};

/// Structure containing the parameters of the @ref SM_CONNECT_CFM message.
struct sm_connect_cfm
{
    /// Status. If 0, it means that the connection procedure will be performed and that
    /// a subsequent @ref SM_CONNECT_IND message will be forwarded once the procedure is
    /// completed.
    uint8_t status;
};

#define SM_ASSOC_IE_LEN   800
/// Structure containing the parameters of the @ref SM_CONNECT_IND message.
struct sm_connect_indication
{
    /// Status code of the connection procedure
    uint16_t status_code;
    /// BSSID
    struct mac_addr bssid;
    /// Flag indicating if the indication refers to an internal roaming or from a host request
    bool roamed;
    /// Index of the VIF for which the association process is complete
    uint8_t vif_idx;
    /// Index of the STA entry allocated for the AP
    uint8_t ap_idx;
    /// Index of the LMAC channel context the connection is attached to
    uint8_t ch_idx;
    /// Flag indicating if the AP is supporting QoS
    bool qos;
    /// ACM bits set in the AP WMM parameter element
    uint8_t acm;
    /// Length of the AssocReq IEs
    uint16_t assoc_req_ie_len;
    /// Length of the AssocRsp IEs
    uint16_t assoc_rsp_ie_len;
    /// IE buffer
    uint32_t assoc_ie_buf[SM_ASSOC_IE_LEN/4];

    uint16_t aid;
    uint8_t band;
    uint16_t center_freq;
    uint8_t width;
    uint32_t center_freq1;
    uint32_t center_freq2;
};

/// Structure containing the parameters of the @ref SM_DISCONNECT_REQ message.
struct sm_disconnect_req
{
    /// Reason of the deauthentication.
    uint16_t reason_code;
    /// Index of the VIF.
    uint8_t vif_idx;
};

/// Structure containing the parameters of the @ref SM_DISCONNECT_IND message.
struct sm_disconnect_ind
{
    /// Reason of the disconnection.
    uint16_t reason_code;
    /// Index of the VIF.
    uint8_t vif_idx;
    /// FT over DS is ongoing
    bool ft_over_ds;
};

/// Structure containing the parameters of APMSTA_DEAUTHENTICATION_CFM message.
struct sm_deauthentication_cfm
{
    // MAC ADDR of the STA to be deauthenticated
    struct mac_addr     mac_addr;
};

/// Structure containing the parameters of SM_POWER_MGMT_REQ the message.
struct sm_power_mgmt_req
{
    // Power Mode
    uint8_t         power_mode;
};

/// Structure containing the parameters of SM_POWER_MGMT_CFM the message.
struct sm_power_mgmt_cfm
{
    // Power Mode
    uint8_t         power_mode;
};

/// Structure containing the parameters of the @ref SM_ASSOC_IND message.
struct sm_assoc_ind
{
    /// Mac of client
    uint8_t mac[6];
};

/// Structure containing the parameters of the @ref SM_DEASSOC_IND message.
struct sm_deassoc_ind
{
    /// Mac of client
    uint8_t mac[6];
};

/// Structure containing the parameters of the @ref SM_ASSOC_FAILED_IND message.
struct sm_assoc_failed_ind
{
    /// Mac of client
    uint8_t mac[6];
};


extern const struct ke_state_handler sm_default_handler;

/// Table including the state of each instance of the SM task.
extern ke_state_t sm_state[SM_IDX_MAX];

/// @}

#endif // _SM_TASK_H_

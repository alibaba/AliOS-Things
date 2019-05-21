/**
****************************************************************************************
*
* @file me.h
*
* Copyright (C) RivieraWaves 2011-2016
*
* @brief Declaration of the initialization functions used in UMAC ME.
*
****************************************************************************************
*/

#ifndef _ME_H_
#define _ME_H_

/**
 *****************************************************************************************
 * @defgroup UMAC UMAC
 * @ingroup MACSW
 * @brief Upper MAC SW module.
 *****************************************************************************************
 */

/**
 *****************************************************************************************
 * @defgroup ME ME
 * @ingroup UMAC
 * @brief Declaration of all functions used for the initialization of the UMAC ME modules.
 * @{
 *****************************************************************************************
 */

/*
* INCLUDE FILES
****************************************************************************************
*/
#include "rwnx_config.h"
#include "scanu_task.h"
#include "me_task.h"
#include "mac.h"
#include "txl_frame.h"

/**
 * DEFINES
 ****************************************************************************************
 */

/// Default system values
#define DEFAULT_ASSOCRSP_TIMEOUT        (200 * TU_DURATION)
#define DEFAULT_AUTHRSP_TIMEOUT         (200 * TU_DURATION)

/**
 * STRUCTURES/TYPES DEFINITION
 ****************************************************************************************
 */

/// ME environment structure
struct me_env_tag
{
    /// Bitfield containing the ACTIVE state request of the different VIFs
    uint32_t active_vifs;
    /// Bitfield containing the PS state of the different VIFs
    uint32_t ps_disable_vifs;
    /// TaskID of the task requesting the last command
    ke_task_id_t requester_id;
    /// HT capabilities of the device
    struct mac_htcapability ht_cap;
    /// Lifetime of packets sent under a BlockAck agreement (expressed in TUs)
    uint16_t tx_lft;
    /// Flag indicating whether HT is supported
    bool ht_supported;
#if NX_VHT
    /// VHT capabilities of the device
    struct mac_vhtcapability vht_cap;
    /// Flag indicating whether VHT is supported
    bool vht_supported;
#endif
    /// List of supported channels
    struct me_chan_config_req chan;

    /// Maximum number of Spatial Streams supported for a STBC transmission
    uint8_t stbc_nss;
#if NX_POWERSAVE
    /// Boolean indicating if PS mode shall be enabled or not
    bool ps_on;
#endif
};

/// Mobility Domain IE
struct mobility_domain
{
    /// Mobility Domain ID
    uint16_t mdid;
    /// Fast Transition capability and policy
    uint8_t ft_capability_policy;
};

/// Scan result element, parsed from beacon or probe response frames.
struct mac_bss_info
{
    /// HT capabilities
    struct mac_htcapability ht_cap;
#if NX_VHT
    /// VHT capabilities
    struct mac_vhtcapability vht_cap;
#endif
    /// Network BSSID.
    struct mac_addr bssid;
    /// Network name.
    struct mac_ssid ssid;
    /// Network type (IBSS or ESS).
    uint16_t bsstype;
    /// Network channel number.
    struct scan_chan_tag *chan;
    /// Center frequency 1
    uint16_t center_freq1;
    /// Center frequency 2
    uint16_t center_freq2;
    /// Network beacon period.
    uint16_t beacon_period;
    /// Capabilities information of the BSS
    uint16_t cap_info;
    /// Legacy rate set
    struct mac_rateset rate_set;
    /// EDCA parameter set
    struct mac_edca_param_set edca_param;
    /// QoS capabilities of this BSS
    struct mac_qoscapability qos_cap;
    /// RSSI of the beacons received from this BSS
    int8_t rssi;
    /// Highest 11g rate that can be used
    uint8_t high_11b_rate;
    /// Highest legacy rate that can be used
    uint8_t high_legacy_rate;
    /// Protection Status (Bit field)
    uint16_t prot_status;
    /// Operational bandwidth
    uint8_t bw;
    /// PHY bandwidth
    uint8_t phy_bw;
    /// Power Constraint
    uint8_t power_constraint;
    /// Flags indicating the validity of some fields above (HT/VHT/QoS)
    uint32_t valid_flags;
    /// Mobility Domain element
    struct mobility_domain mde;
};

/**
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
extern struct me_env_tag me_env;

/**
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */


/**
 ****************************************************************************************
 * @brief This function returns the TX lifetime of packets sent under BlockAck agreement
 *
 * @return The lifetime that was configured by the upper layers
 ****************************************************************************************
 */
__INLINE uint16_t me_tx_lft_get(void)
{
    return me_env.tx_lft;
}

/**
 ****************************************************************************************
 * @brief This function requests some buffered packets to be transmitted
 *
 * @param[in] sta_idx  Index of the peer device we have to transmit to
 * @param[in] pck_cnt  Number of packets that need to be transmitted
 ****************************************************************************************
 */
__INLINE void me_credits_update_ind(uint8_t sta_idx, uint8_t tid, int8_t credits)
{
    struct me_tx_credits_update_ind *ind = KE_MSG_ALLOC(ME_TX_CREDITS_UPDATE_IND, TASK_API,
                                           TASK_ME, me_tx_credits_update_ind);

    // Fill-in the parameters
    ind->sta_idx = sta_idx;
    ind->tid = tid;
    ind->credits = credits;

    // Send the message
    ke_msg_send(ind);
}

/**
 ****************************************************************************************
 * @brief Initialize the ME modules
 ****************************************************************************************
 */
void me_init(void);

/**
 ****************************************************************************************
 * @brief Search the channel structure corresponding to the parameters, and returns a
 * a pointer to this structure.
 *
 * @param[in] band PHY band (@ref PHY_BAND_2G4 or @ref PHY_BAND_5G)
 * @param[in] freq Frequency of the channel (in MHz)
 *
 * @return The pointer to the channel structure found if available, NULL otherwise
 ****************************************************************************************
 */
struct scan_chan_tag *me_freq_to_chan_ptr(uint8_t band, uint16_t freq);

uint8_t me_mgmt_tx(struct me_mgmt_tx_req *param);

/// @}

#endif // _ME_INIT_H_

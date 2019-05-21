/**
****************************************************************************************
*
* @file sm.h
*
* Copyright (C) RivieraWaves 2011-2016
*
* @brief Declaration of all structures and functions used by the SM module.
*
****************************************************************************************
*/
#ifndef _SM_H_
#define _SM_H_

/** @defgroup SM SM
 * @ingroup UMAC
 * @brief Declaration of all structures and functions used by the SM module.
 * @{
 */
#pragma once

// for mac_addr and other definitions
#include "mac.h"
// for MAX_BSS_LIST
#include "mac_common.h"
#include "ke_task.h"



/// Maximum length of the AssocReq IEs
#define SM_MAX_IE_LEN   256

#if NX_ROAMING

#define RSSI_THRESHOLD_LIST_LEN    12

// Roaming definition

// initial roaming timer 10 sec
#define ROAMING_TIMER_DUR       10 * TU_DURATION * TU_DURATION
#define MIN_ROAMING_TIMER_DUR   5  * TU_DURATION * TU_DURATION
#define MAX_ROAMING_TIMER_DUR   10 * ROAMING_TIMER_DUR

// wait 16 usec before sending probe request
#define PROBE_DELAY             16
// wait 5 time unit (~ 5 msec) to check if network activity.
#define MIN_CH_TIME             5
// wait 10 time unit (~ 10 msec) for the all probes.
#define MAX_CH_TIME             10
// wait 15 time unit (~ 15 msec) for probe (join is more important than scan)
#define DEFAULT_ACTIVE_JOIN_TO  15


// Defining the values of the RSSI threshold, hysteresis and gap for b-mode
#define B_MODE_RSSI_THRESHOLD            -60
#define B_MODE_RSSI_GAP                   10
#define B_MODE_RSSI_HYSTERESIS            11

// Defining the values of the RSSI hysteresis and gap for a-mode
#define A_MODE_5_2GHZ_RSSI_GAP            10
#define A_MODE_5_3GHZ_RSSI_GAP            7
#define A_MODE_5_75GHZ_RSSI_GAP           7

#define A_MODE_5_2GHZ_RSSI_HYSTERESIS     10
#define A_MODE_5_3GHZ_RSSI_HYSTERESIS     9
#define A_MODE_5_75GHZ_RSSI_HYSTERESIS    8


/*
 * Defining the center frequency for the first and last operating channels in
 * the 5.2 MHz band of a-mode
 */
#define A_MODE_5_2GHZ_FIRST_FREQ       4915
#define A_MODE_5_2GHZ_LAST_FREQ        5250

/*
 * Defining the center frequency for the first and last operating channels in
 * the 5.3 MHz band of a-mode
 */
#define A_MODE_5_3GHZ_FIRST_FREQ       5260
#define A_MODE_5_3GHZ_LAST_FREQ        5430

/*
 * Defining the center frequency for the first and last operating channels in
 * the 5.75 MHz band of a-mode
 */
#define A_MODE_5_75GHZ_FIRST_FREQ      5500
#define A_MODE_5_75GHZ_LAST_FREQ       5805


/*
 * The following macro is used to the operating channel center frequency for a- mode
 * based on the channel number where:
 *      ChannelFreq  = (ChannelNbr * 5) + 5000;    (For Channel number < 170)
 *      ChannelFreq  = (ChannelNbr * 5) + 4000;    (For Channel number > 170)
 */
#define A_MODE_GET_CHANNEL_FREQ(ch_nbr)                 \
 (4 * (ch_nbr) + (ch_nbr) ) +  (((ch_nbr) < 170) ? 5000 : 4000)

#define UPDATE_RSSI(rssi)          \
    sm_env.cur_rssi = (rssi)

#define UPDATE_RATE(rx_rate)       \
{                                  \
    sm_env.rx_rate += (rx_rate);   \
    sm_env.pckt_cnt++;             \
}                                  \

#endif  // NX_ROAMING


// the structure contain info a bout the best candidate AP
struct best_cand_info
{
    int32_t                 rssi;
    struct mac_addr         bssid;
    struct mac_ssid         ssid;
    uint16_t                ch_nbr;
    uint16_t                bsstype;
};

struct rxu_mgt_ind;
struct vif_info_tag;

/// station environment structure
struct sm_env_tag
{
    /// GP DMA descriptors
    //struct hal_dma_desc_tag dma_desc;
    /// Pointer to the scanning parameters
    struct sm_connect_req *connect_param;
    /// Pointer to the structure used for the connect indication upload
    struct sm_connect_indication *connect_ind;
    /// List of BSS configuration messages to send
    struct co_list bss_config;
    struct best_cand_info join_param;
    /// The reset reason value
    uint8_t reset_reason;
    /// Indicate if passive scan has been used during join procedure
    bool join_passive;
    /// FT over DS is ongoing
    bool ft_over_ds;
    /// MAC address of old AP - during FT over DS transition
    struct mac_addr ft_old_bssid;

#if NX_ROAMING
    // Roaming variables

    // the SSID of the current AP
    struct mac_ssid cur_ssid;
    // the RSSI of the current AP
    int32_t cur_rssi;
    // the old RSSI of the current AP
    int32_t cur_mrssi;
    // the current RSSI gap of the current AP
    int32_t cur_rssi_gap;
    // the RSSI gap of the current AP
    int32_t rssi_gap;
    // the RSSI hysteresis of the current AP
    int32_t cur_rssi_hyst;
    // roaming timer duration
    uint32_t roaming_timer_dur;
    // number of available candidate
    uint32_t avail_cand;
    // the accumulated rx rate of the current AP
    uint32_t rx_rate;
    // the number of packets received from the current AP since the last roaming period
    uint32_t pckt_cnt;
    // the band of the current AP
    uint8_t cur_band;
    // hand off operation is ongoing
    bool hand_off_ongoing;

    /* list contains the the best candidates APs info,
     * with the highest RSSI is the first entry, the list is updated after scan,
     * or a failure attempt to perform hand off
     */
    struct best_cand_info   best_cand_info_list[MAX_BSS_LIST];
    /* store the original AP info to be used if all the attempt to connect to
     * all the best candidate APs failed
     */
    struct best_cand_info   orig_ap_info;

#endif  // NX_ROAMING
};

/**
 ****************************************************************************************
 * @brief Initialize the SM context.
 ****************************************************************************************
 */
void sm_rw_init(void);

/**
 ****************************************************************************************
 * @brief Search for the BSSID and Channel information in the scan results and/or the
 * connection parameters.
 *
 * @param[out] bssid  Pointer to the BSSID to join
 * @param[out] chan   Pointer to the channel on which the BSSID is
 ****************************************************************************************
 */
void sm_get_bss_params(struct mac_addr const **bssid,
                       struct scan_chan_tag const **chan);

/**
 ****************************************************************************************
 * @brief Try to join the BSS indicated by the parameters.
 *
 * @param[in] bssid   Pointer to the BSSID to join
 * @param[in] chan    Pointer to the channel on which the BSSID is
 * @param[in] passive Indicate if passive scan has to be started
 ****************************************************************************************
 */
void sm_join_bss(struct mac_addr const *bssid,
                 struct scan_chan_tag const *chan,
                 bool passive);

void sm_scan_bss(struct mac_addr const *bssid,
                 struct scan_chan_tag const *chan);
void sm_connect_ind(uint16_t status);

void sm_auth_handler(struct rxu_mgt_ind const *param);
void sm_assoc_rsp_handler(struct rxu_mgt_ind const *param);
int sm_deauth_handler(struct rxu_mgt_ind const *param);

/**
 ****************************************************************************************
 * @brief Add the channel context for the new connection
 * 
 * @param[in|out] p_chan_idx    Pointer to the value that will contain the allocated channel
 *                              index.
 * 
 * @return the status of the MM_CHAN_CTXT_ADD_REQ handling
 ****************************************************************************************
 */
uint8_t sm_add_chan_ctx(uint8_t *p_chan_idx);

/**
 ****************************************************************************************
 * @brief Set the BSS parameters
 * This function prepares the list of BSS configuration messages that will be transmitted
 * to the Lower MAC
 *
 ****************************************************************************************
 */
void sm_set_bss_param(void);

/**
 ****************************************************************************************
 * @brief Send the next BSS parameter message present in the list
 *
 ****************************************************************************************
 */
void sm_send_next_bss_param(void);

void sm_disconnect(uint8_t vif_index, uint16_t reason_code);

void sm_disconnect_process(struct vif_info_tag *vif, uint16_t reason);

/**
 ****************************************************************************************
 * @brief send SM_ASSOCIATE_IND message to SME.
 ****************************************************************************************
 */
void sm_connect_ind_send(void);

/**
 ****************************************************************************************
 * @brief this function handles the MM_RESET_CFM message from MM when the SM in state reseting.
 * This message reports end of reset procedure for LMAC SW, HW, PHY, Modem, ..etc.
 * @param[in] msgid Id of the message received (probably unused)
 * @param[in] dest_id TaskId of the receiving task.
 * @param[in] senderid TaskId of the sending task.
 * @return Whether the message was consumed or not.
 ****************************************************************************************
 */
void sm_resetting_handler(ke_msg_id_t const msgid,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const senderid);

/**
 ****************************************************************************************
 * @brief send ACTIVE MM_JOIN_REQ to the LMAC.
 *
 * @param[in] join_param   pointer to join req parameters. (BSID, SSID,BSSTYPE,CH_NO).
 * @param[in] jointimeout  max time to wait to detect the network.
 * @param[in] capa_info    Capability Info. field.
 ****************************************************************************************
 */
void sm_join_req_send(struct best_cand_info *join_param,
                      uint16_t jointimeout,
                      uint16_t capa_info);

/**
 ****************************************************************************************
 * @brief send connect confirmation message to SME.
 *
 * @param[in] status status of the connection.
 ****************************************************************************************
 */
void sm_connect_cfm_send(uint16_t status);

/**
 ****************************************************************************************
 * @brief set the operation MIBs as chosen in the desired AP scan results
 *
 * @param[in] desired_ap_ptr   pointer to the current AP information
 * @param[in] sta_idx          station index
 ****************************************************************************************
 */
void sm_operation_mib_update(struct mac_scan_result const *desired_ap_ptr,
                             uint16_t sta_idx);

/**
 ****************************************************************************************
 * @brief send SM_DEAUTHENTICATION_IND message to SME.
 *
 * @param[in] reason_code Reason field of the deauthenticate frame.
 ****************************************************************************************
 */
void sm_deauth_ind_send(uint16_t reason_code);

/**
 ****************************************************************************************
 * @brief send MM_RESET_REQ to LMAC.
 *
 * @param[in]  setdefaultmib       reset to default mibs flag
 * @param[in]  reason              reason for the reset
 ****************************************************************************************
 */
void sm_lmac_reset(uint8_t setdefaultmib, uint8_t reason);

/**
****************************************************************************************
* @brief send AIR_ASSOC_REQ to the AIR.
****************************************************************************************
*/
void sm_air_assoc_req_send(void);
/**
 ****************************************************************************************
 * @brief Association completed operations.
 *
 * @param[in] aid Association Identifier provided by AP on connection (0 if IBSS).
 ****************************************************************************************
 */
void sm_assoc_done(uint16_t aid);

/**
 ****************************************************************************************
 * @brief Send MAC_FCTRL_ASSOCREQ or MAC_FCTRL_REASSOCREQ to the air
 ****************************************************************************************
 */
void sm_assoc_req_send(void);

/**
 ****************************************************************************************
 * @brief Send Authentication frame
 * 
 * param[in] auth_seq  Authentication sequence
 * param[in] challenge Pointer on authentication challenge. Only needed for thrid 
                       sequence of SHARED_KEY authentication. Should be NULL otherwise
 ****************************************************************************************
 */
void sm_auth_send(uint16_t auth_seq, uint32_t *challenge);
void sm_build_broadcast_deauthenticate(void);

#if NX_ROAMING
/**
 ****************************************************************************************
 * @brief update the roaming candidate list.
 ****************************************************************************************
 */
void sm_update_cand_list(void);

/**
 ****************************************************************************************
 * @brief update the roaming candidate list by replacing the last best candidate
 *  with a new best candidate.
 ****************************************************************************************
 */
void update_best_cand(void);

/**
 ****************************************************************************************
 * @brief send scan request to the SCAN module.
 ****************************************************************************************
 */
void sm_full_scan(void);

/**
 ****************************************************************************************
 * @brief send fast scan request to SCAN
 *
 * @param[in] *best_cand_info_ptr  pointer to the best bss info
 ****************************************************************************************
 */
void sm_fast_scan(struct best_cand_info  *best_cand_info_ptr);

/**
 ****************************************************************************************
 * @brief save the roaming parameters.
 *
 * @param[in]  desired_ap_ptr       pointer to the current AP information
 ****************************************************************************************
 */
void save_roaming_param(struct mac_scan_result *desired_ap_ptr);

/**
 ****************************************************************************************
 * @brief return the RSSI threshold according to the mode.
 *
 * @return RSSI threshold
 ****************************************************************************************
 */
int32_t get_roaming_thrsh(void);

/**
 ****************************************************************************************
 * @brief this function handles the MM_RESET_CFM message from MM when the sm in state hand off.
 * This message reports end of reset procedure for LMAC SW, HW, PHY, Modem, ..etc.
 *
 * @param[in]  unregister_old_ap       indicate if the previous AP need to be unregistered
 ****************************************************************************************
 */
void handoff_handler(bool unregister_old_ap);

#endif  // NX_ROAMING

/// SM module environment
extern struct sm_env_tag sm_env;

/// @} //end of group

#endif // _SM_H_

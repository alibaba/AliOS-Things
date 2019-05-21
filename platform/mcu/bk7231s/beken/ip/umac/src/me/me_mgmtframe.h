/**
 ****************************************************************************************
 *
 * @file me_mgmtframe.h
 *
 * @brief Declaration of all functions required either to build or to extract data from
 * air frames
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _ME_MGMTFRAME_H_
#define _ME_MGMTFRAME_H_

/** @defgroup UMACMEMGMTFRAME MGMTFRAME
* @ingroup ME
* @brief Declaration of all structures and functions used by the MGMT Frames
*/

/** @addtogroup UMACMEMGMTFRAME
* @{
*/

// standard includes
#include "co_int.h"
#include "co_bool.h"
#include "co_utils.h"
#include "bam.h"
#include "vif_mgmt.h"

//forward declarations
struct mac_frame;
struct mac_addr;
struct mac_raw_rsn_ie;
struct mac_rateset;
struct mac_htcapability;
struct mac_htoprnelmt;
struct mac_raw_rsn_ie;
struct mac_tspec_param;
struct mac_ts_info;
struct mac_ba_param;
struct mac_request_set;
struct mac_report_set;
struct mac_bss_info;
struct sm_connect_req;
struct mm_chan_ctxt_add_req;

/**
 ****************************************************************************************
 * @brief Add SSID Information Element in a frame.
 *          Byte 0: Element ID
 *          Byte 1: Length
 *          Byte 2-variable: SSID
 *
 * @param[in|out] frame_addr  Pointer to the address at which IE has to be added. When leaving
 *                            the function, the pointer value matches with the new end of the frame.
 * @param[in] ssid_len        Length of the SSID
 * @param[in] p_ssid          Pointer to the SSID value in the memory
 *
 * @return Number of bytes that have been added to the provided frame.
 ****************************************************************************************
 */
uint32_t me_add_ie_ssid(uint32_t *frame_addr, uint8_t ssid_len, uint8_t *p_ssid);

/**
 ****************************************************************************************
 * @brief Add Supported Rate Information Element in a frame.
 *          Byte 0: Element ID
 *          Byte 1: Length
 *          Byte 2-variable: Rates
 *
 * @param[in|out] frame_addr  Pointer to the address at which IE has to be added. When leaving
 *                            the function, the pointer value matches with the new end of the frame.
 * @param[in] p_rateset       Supported Rates. If number of rates is higher than 8, rates from
 *                            position 8 will have to be part of a Extended Supported Rate IE.
 *
 * @return Number of bytes that have been added to the provided frame.
 ****************************************************************************************
 */
uint32_t me_add_ie_supp_rates(uint32_t *frame_addr, struct mac_rateset *p_rateset);

/**
 ****************************************************************************************
 * @brief Add Extended Supported Rate Information Element in a frame.
 *          Byte 0: Element ID
 *          Byte 1: Length
 *          Byte 2-variable: Rates
 *
 * @param[in|out] frame_addr  Pointer to the address at which IE has to be added. When leaving
 *                            the function, the pointer value matches with the new end of the frame.
 * @param[in] p_rateset       Supported Rates. Number of rates shall be higher than 8 when entering
 *                            in this function.
 *
 * @return Number of bytes that have been added to the provided frame.
 ****************************************************************************************
 */
uint32_t me_add_ie_ext_supp_rates(uint32_t *frame_addr, struct mac_rateset *p_rateset);

#if (RW_MESH_EN)
/**
 ****************************************************************************************
 * @brief Add TIM (Traffic Indication Map) Information Element in a frame.
 *          Byte 0: Element ID
 *          Byte 1: Length
 *          Byte 2: DTIM Count (updated by mm_bcn module before beacon transmission)
 *          Byte 3: DTIM Period (read and used by mm_bcn module)
 *          Byte 4: Bitmap Control (updated by mm_bcn module)
 *          Byte 5: Partial Virtual Bitmap (updated by mm_bcn module)
 *
 * @param[in|out] frame_addr  Pointer to the address at which IE has to be added. When leaving
 *                            the function, the pointer value matches with the new end of the frame.
 * @param[in] dtim_period     DTIM Period
 *
 * @return Number of bytes that have been added to the provided frame.
 ****************************************************************************************
 */
uint32_t me_add_ie_tim(uint32_t *frame_addr, uint8_t dtim_period);

/**
 ****************************************************************************************
 * @brief Add DS Parameter Information Element in a frame.
 *          Byte 0: Element ID
 *          Byte 1: Length
 *          Byte 2: Channel
 *
 * @param[in|out] frame_addr  Pointer to the address at which IE has to be added. When leaving
 *                            the function, the pointer value matches with the new end of the frame.
 * @param[in] chan            Current Channel
 *
 * @return Number of bytes that have been added to the provided frame.
 ****************************************************************************************
 */
uint32_t me_add_ie_dsss_param(uint32_t *frame_addr, uint8_t chan);
#endif //(RW_MESH_EN)

/**
 ****************************************************************************************
 * @brief Add HT Capabilities Information Element in a frame.
 *
 * @param[in|out] frame_addr  Pointer to the address at which IE has to be added. When leaving
 *                            the function, the pointer value matches with the new end of the frame.
 *
 * @return Number of bytes that have been added to the provided frame.
 ****************************************************************************************
 */
uint32_t me_add_ie_ht_capa(uint32_t *frame_addr);

#if (RW_MESH_EN)
/**
 ****************************************************************************************
 * @brief Add HT Operation Information Element in a frame.
 *
 * @param[in|out] frame_addr  Pointer to the address at which IE has to be added. When leaving
 *                            the function, the pointer value matches with the new end of the frame.
 * @param[in] p_vif_entry     Pointer to the structure containing information about the VIF for which
 *                            the Information Element has to be filled.
 *
 * @return Number of bytes that have been added to the provided frame.
 ****************************************************************************************
 */
uint32_t me_add_ie_ht_oper(uint32_t *frame_addr, struct vif_info_tag *p_vif_entry);
#endif //(RW_MESH_EN)

#if (NX_VHT)
/**
 ****************************************************************************************
 * @brief Add VHT Capabilities Information Element in a frame.
 *
 * @param[in|out] frame_addr  Pointer to the address at which IE has to be added. When leaving
 *                            the function, the pointer value matches with the new end of the frame.
 *
 * @return Number of bytes that have been added to the provided frame.
 ****************************************************************************************
 */
uint32_t me_add_ie_vht_capa(uint32_t *frame_addr);

#if (RW_MESH_EN)
/**
 ****************************************************************************************
 * @brief Add VHT Operation Information Element in a frame.
 *
 * @param[in|out] frame_addr  Pointer to the address at which IE has to be added. When leaving
 *                            the function, the pointer value matches with the new end of the frame.
 * @param[in] p_vif_entry     Pointer to the structure containing information about the VIF for which
 *                            the Information Element has to be filled.
 *
 * @return Number of bytes that have been added to the provided frame.
 ****************************************************************************************
 */
uint32_t me_add_ie_vht_oper(uint32_t *frame_addr, struct vif_info_tag *p_vif_entry);
#endif //(RW_MESH_EN)
#endif //(NX_VHT)

/**
 ****************************************************************************************
 * @brief Build an Authentication frame at the AP and the STA
 *
 * @param[in]  frame                    Address of the payload
 * @param[in]  algo_type                indicates a single authentication algorithm.
 * @param[in]  seq_nbr                  indicates the current state of progress through a
 *                                      multi-step transaction
 * @param[in]  status_code              indicate the success or failure of a req operation
 * @param[in]  challenge_array_ptr      pointer to the challenge_array
 *
 * @return     The length of the built frame
 *
 ****************************************************************************************
 */
uint16_t me_build_authenticate(uint32_t frame,
                               uint16_t algo_type,
                               uint16_t seq_nbr,
                               uint16_t status_code,
                               uint32_t *challenge_array_ptr);

/**
 ****************************************************************************************
 * @brief Build an Association Response frame.
 *
 * @param[in]  mac_frame_ptr    pointer to the mac_frame
 * @param[in]  vap_idx          index to vAP instance
 * @param[in]  sta_idx          STA index to get stored information.
 * @param[in]  status_code      indicate the success or failure of a requested operation
 *
 * @return     The length of the built frame
 *
 ****************************************************************************************
 */
uint16_t me_build_associate_rsp(struct mac_frame *mac_frame_ptr,
                                uint8_t vap_idx,
                                uint16_t sta_idx,
                                uint16_t status_code);


/**
 ****************************************************************************************
 * @brief Build a De authentication frame at the AP and the STA
 *
 * @param[in]  mac_frame_ptr    pointer to the mac_frame
 * @param[in]  reason_code      used to indicate reason for generating Deauthentication
 *
 * @return     The length of the built frame
 *
 ****************************************************************************************
 */
uint16_t me_build_deauthenticate(uint32_t frame, uint16_t reason_code);

// Functions used in the SM
/**
****************************************************************************************
* @brief used to build the Associated REQ frame at STA
*
* @param[in]  mac_frame_ptr            pointer to the mac_frame
* @param[in]  old_ap_addr_ptr          pointer to the old AP address
* @param[in]  frame_type               indicates the type of the frame
* @param[in]  vap_idx                  index to vAP instance
*
* @return     The length of the built frame
*
****************************************************************************************
*/
uint16_t me_build_associate_req(uint32_t frame,
                                struct mac_bss_info *bss,
                                struct mac_addr *old_ap_addr_ptr,
                                uint8_t vif_idx,
                                uint32_t *ie_addr,
                                uint16_t *ie_len,
                                struct sm_connect_req const *con_par);

/**
 ****************************************************************************************
 * @brief This function is called to extract the rateset from a management frame
 *
 * @param[in] buffer             Pointer on beacon/probe response management frame buffer
 * @param[in] buflen             beacon/probe response management frame length
 * @param[out] mac_rate_set_ptr  found rates
 ****************************************************************************************
 */
void me_extract_rate_set(uint32_t buffer,
                         uint16_t buflen,
                         struct mac_rateset *mac_rate_set_ptr);

/**
 ****************************************************************************************
 * @brief builds the ADDBA REQ frame
 *
 * @param[in]  mac_frame_ptr      pointer to the mac frame
 * @param[in]  ba_param_ptr       pointer to BA Param
 * @param[in]  token              value of the Dialog Token.
 * @param[in]  tsid               value of tsid.
 ****************************************************************************************
 */
uint16_t me_build_add_ba_req(uint32_t frame, struct bam_env_tag *bam_env);

/**
 ****************************************************************************************
 * @brief builds the ADDBA RSP frame.
 *
 * @param[in]  mac_frame_ptr     pointer to the mac frame.
 * @param[in]  ba_param_ptr      pointer to BA Param.
 * @param[in]  status_code       used to indicate status of the response.
 * @param[in]  token             value of the Dialog Token.
 * @param[in]  tsid              value of tsid.
 *
 * @return     The length of the built frame.
 ****************************************************************************************
 */
uint16_t me_build_add_ba_rsp(uint32_t frame, struct bam_env_tag *bam_env,
                             uint16_t param,
                             uint8_t dialog_token,
                             uint16_t status_code);

/**
 ****************************************************************************************
 * @brief builds the DELBA IND frame.
 *
 * @param[in]  mac_frame_ptr      pointer to the mac frame.
 * @param[in]  ba_param_ptr       pointer to BA Param.
 * @param[in]  reason_code        used to indicate reason for deleting.
 * @param[in]  tsid               value of tsid.
 *
 * @return     The length of the built frame.
 *
 ****************************************************************************************
 */
uint16_t me_build_del_ba(uint32_t frame, struct bam_env_tag *bam_env, uint16_t reason_code);


/**
 ****************************************************************************************
 * @brief Extract local power constriant from Power Constraint IE
 *
 * @param[in] buffer    Pointer on beacon/probe response management frame buffer
 * @param[in] buflen    beacon/probe response management frame length
 * @param[out] bss      pointer to the bss info structure in which power_constraint will
 *                      be updated
 *
 ****************************************************************************************
 */
void me_extract_power_constraint(uint32_t buffer, uint16_t buflen,
                                 struct mac_bss_info *bss);

/**
 ****************************************************************************************
 * @brief Update tx_power of bss channel from Country IE
 *
 * If country IE is found, update tx power of the bss channel with the tx power set
 * in country IE.
 * Note: Other channel tx power are not updated (assume driver will provide updated
 * channel decription after association)
 *
 * @param[in] buffer    Pointer on beacon/probe response management frame buffer
 * @param[in] buflen    beacon/probe response management frame length
 * @param[out] bss      pointer to the bss info structure
 *
 ****************************************************************************************
 */
void me_extract_country_reg(uint32_t buffer, uint16_t buflen,
                            struct mac_bss_info *bss);


/**
 ****************************************************************************************
 * @brief Extract Mobility Domain IE
 *
 * @param[in] buffer    Pointer on beacon/probe response management frame buffer
 * @param[in] buflen    beacon/probe response management frame length
 * @param[out] bss      pointer to the bss info structure
 *
 ****************************************************************************************
 */
void me_extract_mobility_domain(uint32_t buffer, uint16_t buflen,
                                struct mac_bss_info *bss);

/**
 * @brief Extract Channel Switch Annoucement (csa) information from beacon
 *
 * If CSA (or ECSA) IE is detected in the beacon, this function will extract target
 * channel configuration, csa counter and csa mode.
 *
 * @param[in] buffer  Pointer on beacon management frame buffer
 * @param[in] buflen  beacon management frame length
 * @param[out] mode   Indicates whether transmission must be paused or not until channel switch
 * @param[out] chan   Configuration of target channel
 *
 * @return The CSA counter (i.e. number of bacon before actual channel switch).
 *         0 means no CSA detected.
 *
 ****************************************************************************************
 */
int me_extract_csa(uint32_t buffer, uint16_t buflen, uint8_t *mode,
                   struct mm_chan_ctxt_add_req *chan_desc);

/// @}

#endif  // _ME_MGMTFRAME_H_

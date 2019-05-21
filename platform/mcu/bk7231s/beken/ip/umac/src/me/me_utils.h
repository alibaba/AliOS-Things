/**
 ****************************************************************************************
 *
 * @file me_utils.h
 *
 * @brief All utility functions manipulating rates, etc.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _ME_UTILS_H_
#define _ME_UTILS_H_

#include "co_int.h"
#include "co_bool.h"

#include "tx_swdesc.h"

// forward declarations
struct mac_mcsset;
struct mac_rateset;
struct mac_rates;
struct sta_info_tag;
struct txdesc;
struct mac_bss_info;

/**
 ****************************************************************************************
 * @brief Build the basic rate bitfield according to the rate sets passed as parameter
 *
 * @param[in]  rateset     The rate set containing the legacy rates
 *
 * @return The basic rate bitfield
 *
 ****************************************************************************************
 */
uint32_t me_basic_rate_bitfield_build(struct mac_rateset *rateset);

/**
 ****************************************************************************************
 * @brief Build a rate bitfield according to the rate sets passed as parameter
 *
 * @param[out] ratefield   The rate field to be built
 * @param[in]  rateset     The rate set containing the legacy rates
 * @param[in]  basic_only  If this parameter is true, then only basic rates are extracted
 *                         from the rate set
 *
 ****************************************************************************************
 */
void me_rate_bitfield_legacy_build(struct mac_rates   *ratefield,
                                   struct mac_rateset *rateset,
                                   bool                basic_only);


/**
 ****************************************************************************************
 * @brief Build a rate bitfield according to the rate sets passed as parameter
 *
 * @param[out] ratefield   The rate field to be built
 * @param[in]  mcsset      The rate set containing the HT rates
 * @param[in]  basic_only  If this parameter is true, then only basic rates are extracted
 *                         from the MCS set
 *
 ****************************************************************************************
 */
void me_rate_bitfield_ht_build(struct mac_rates   *ratefield,
                               struct mac_mcsset  *mcsset,
                               bool                basic_only);

/**
 ****************************************************************************************
 * @brief Build a VHT-MCS map according to the MCS maps passed as parameters
 *
 * @param[in]  mcs_map_1  The first VHT-MCS Map
 * @param[in]  mcs_map_2  The second VHT-MCS Map
 * @return The VHT-MCS map of the station
 ****************************************************************************************
 */
uint16_t me_rate_bitfield_vht_build(uint16_t mcs_map_1, uint16_t mcs_map_2);

/**
 ****************************************************************************************
 * @brief Build the capability information
 *
 * @param[in]  vap_idx     instance of the device
 * @return     capability information
 *
 ****************************************************************************************
 */
uint16_t me_build_capability(uint8_t vap_idx);

void me_init_rate(struct sta_info_tag *sta_entry);

void me_init_bcmc_rate(struct sta_info_tag *sta_entry);

uint8_t me_check_rc(uint8_t sta_idx, bool *tx_ampdu);

void me_tx_cfm_singleton(struct txdesc *txdesc);

void me_tx_cfm_ampdu(uint8_t sta_idx, uint32_t txed, uint32_t txok, bool retry_required);

/**
 ****************************************************************************************
 * @brief Update the field contained in the Policy Table
 *
 * @param[in]  sta_info      STA Interface Information
 * @param[in]  umac_desc     UMAC Descriptor to be update
 *
 ****************************************************************************************
 */
void me_update_buffer_control(struct sta_info_tag *sta_info, struct umacdesc *umac_desc);

/**
 ****************************************************************************************
 * @brief Check content of the HT and VHT operation element passed as parameters and
 * update the bandwidth and channel information of the BSS accordingly.
 *
 * @param[in] ht_op_addr Address of the HT operation element to check (0 if not present)
 * @param[in] vht_op_addr Address of the VHT operation element to check (0 if not present)
 * @param[in] bss Pointer to the BSS information structure
 *
 ****************************************************************************************
 */
void me_bw_check(uint32_t ht_op_addr, uint32_t vht_op_addr, struct mac_bss_info *bss);

/**
 ****************************************************************************************
 * @brief Check in the received (in case of STA mode) or just modified (in case of AP mode)
 * beacon if the protection and/or bandwidth of operation have been modified.
 * If this is the case, then all the STA associated to the VIF are updated accordingly.
 *
 * @param[in] vif_idx Index of the VIF for which the beacon is checked
 * @param[in] length Length of the beacon
 * @param[in] bcn_addr Payload address of the beacon
 *
 ****************************************************************************************
 */
void me_beacon_check(uint8_t vif_idx, uint16_t length, uint32_t bcn_addr);

/**
 ****************************************************************************************
 * @brief Update the maximum bandwidth supported by a peer STA. This function is called
 * upon the reception of a HT Notify Channel Width or VHT Operation Mode Notification
 * action frame.
 *
 * @param[in] sta_idx Index of the STA for which the BW is updated
 * @param[in] bw Maximum bandwidth to use with this STA
 *
 ****************************************************************************************
 */
void me_sta_bw_nss_max_upd(uint8_t sta_idx, uint8_t bw, uint8_t nss);

/**
 ****************************************************************************************
 * @brief Return the A-MSDU size allowed for a TID of one STA. This is sent to host for each
 * tx confirmation.
 *
 * @param[in] txdesc Tx descriptor (Containing STA and TID information)
 * @return The amsdu size allowed for the TID of this STA
 *
 ****************************************************************************************
 */
uint16_t me_tx_cfm_amsdu(struct txdesc *txdesc);

/**
 ****************************************************************************************
 * @brief Gets the maximum MCS supported (11ac station)
 * @param[in]  mcs_map Bitmap of supported MCSs
 * @return The maximum MCS
 ****************************************************************************************
 */
uint8_t me_11ac_mcs_max(uint16_t mcs_map);

/**
 ****************************************************************************************
 * @brief Gets the maximum number of spatial streams supported (11ac station)
 * @param[in]  mcs_map Bitmap of supported MCSs
 * @return The maximum number of spatial streams
 ****************************************************************************************
 */
uint8_t me_11ac_nss_max(uint16_t mcs_map);

/**
 ****************************************************************************************
 * @brief Gets the maximum number of spatial streams supported (11n station)
 * @param[in]  mcs_set Pointer to the Supported MCS Set array
 * @return The RX maximum number of spatial streams
 ****************************************************************************************
 */
uint8_t me_11n_nss_max(uint8_t *mcs_set);


/**
 ****************************************************************************************
 * @brief Gets the minimum legacy rate index supported
 * @param[in]  rate_map Bitmap of legacy rate indexes
 * @return The minimum rate index
 ****************************************************************************************
 */
uint8_t me_legacy_ridx_min(uint16_t rate_map);

/**
 ****************************************************************************************
 * @brief Gets the maximum legacy rate index supported
 * @param[in]  rate_map Bitmap of legacy rate indexes
 * @return The maximum rate index
 ****************************************************************************************
 */
uint8_t me_legacy_ridx_max(uint16_t rate_map);

/**
 ****************************************************************************************
 * @brief Add a channel context
 *
 * @param[in|out] p_chan_idx   Pointer to the channel index value
 * @param[in] p_chan           Channel Information
 * @param[in] center_freq1     Center Freq 1
 * @param[in] center_freq2     Center Freq 2
 * @param[in] ch_width         Channel Width
 *
 * @return If request has been properly handled or not
 ****************************************************************************************
 */
uint8_t me_add_chan_ctx(uint8_t *p_chan_idx, struct scan_chan_tag *p_chan,
                        uint32_t center_freq1, uint32_t center_freq2, uint8_t ch_width);

/// @}

#endif  // _ME_UTILS_H_

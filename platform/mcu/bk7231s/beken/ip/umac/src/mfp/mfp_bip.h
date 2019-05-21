/**
 ******************************************************************************
 * @file mfp_bip.h
 *
 * @brief Manangement Frames protection module definitions.
 *
 * Copyright (C) RivieraWaves 2015
 *
 ******************************************************************************
 */

#ifndef _MFP_BIP_H_
#define _MFP_BIP_H_

#include "mac.h"

/**
 * @brief Compute MIC for BIP (Broadcast Integrity Protocol)
 * The function takes a complete frame as parameter, with a MGMT MIC IE
 * whose MIC field has been set to zero.
 * It does not update MIC field in MGMT MIC IE.
 *
 * @param[in] key        Key for BIP (aka IGTK)
 * @param[in] frame      Address of frame (must be an HW address)
 * @param[in] length     Size in bytes of the frame (including MGMT MIC IE)
 * @param[in] machdr_len Size ibn bytes on MAC header in the frame
 *
 * @return MIC value for this frame
 */
uint64_t mfp_compute_bip(struct key_info_tag *key, uint32_t frame,
                         uint16_t length, uint16_t machdr_len);

#endif

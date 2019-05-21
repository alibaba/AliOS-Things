/**
 ******************************************************************************
 * @file mfp.h
 *
 * @brief Manangement Frames protection module definitions.
 *
 * Copyright (C) RivieraWaves 2015
 *
 ******************************************************************************
 */

#ifndef _MFP_H_
#define _MFP_H_

#include "co_bool.h"
#include "co_int.h"
#include "rxu_cntrl.h"

/**
 * @defgroup UMACMFP MFP
 * @ingroup UMAC
 * @brief Structures and Helper functions of the UMAC's MFP Module.
 */

/**
 * @addtogroup UMACMFP
 * @{
 */

enum mfp_protection
{
    /// no protection needed
    MFP_NO_PROT,
    /// need unicast protection
    MFP_UNICAST_PROT,
    /// need multicast protection (i.e. MGMT MIC)
    MFP_MULTICAST_PROT,
};

/**
 * @brief Check if the management frame must be ignored
 * To be called when a management frame is received.
 * The function will return true if :
 * - MFP is enalbed on the vif that received the frame
 * - The frame is a robust management frame (deauth/disassoc/action)
 * - This is a unicast frame and it is not protected
 * - This is a multicast frame and it doesn't have a valid MGMT MIC IE
 *
 * Even if a frame is ignored, it may be needed to report it to upper layer
 * (TO start SA query procedure for example). In this case, if a frame is
 * ignored @param upload is used to indicate if it must be reported to host.
 *
 * Note: In case of multicast frame with MGMT MIC IE, the function will clear
 * the mic value.
 *
 * @param[in] rx_status  Information on received frame
 * @param[in] frame      Address of frame (must be an HW address)
 * @param[in] length     Size in bytes of the frame (including MGMT MIC IE)
 * @param[out] upload    Updated only if it returns true
 *
 * @return True is the frame must be discarded, false otherwise.
 */
bool mfp_ignore_mgmt_frame(struct rx_cntrl_rx_status *rx_status,
                           uint32_t *frame, uint16_t frmlen, bool *upload);


/**
 * @brief Check if the management frame must be protected
 * Return ig the frame must be protected or not. A frame may need unicast
 * protection (i.e. encrypted with pairwise key) or multicast protection
 * (i.e. adding a MGMT MIC IE at the end)
 * See @ref txu_cntrl_protect_mgmt_frame and @ref mfp_add_mgmt_mic for adding
 * the protection to the frame.
 *
 * @param[in] txdesc TX descriptor for the frame (need updated vif and sta id)
 * @param[in] fctl   Frame control field of the frame
 * @param[in] action First byte on the MPDU body (i.e. action category for Action frame)
 *
 * @return The type of protection to apply on the frame.
 */
enum mfp_protection mfp_protect_mgmt_frame(struct txdesc *txdesc,
                                           uint16_t fctl, uint8_t action);


/**
 * @brief Add the MGMT MIC IE at the end of the frame
 *
 * @param[in] txdesc TX descriptor for the frame (need updated vif id)
 * @param[in] frame  Address of frame (must be an HW address)
 * @param[in] length Size in bytes of the frame (Does NOT include MGMT MIC IE)
 *
 * @return Size of the MGMT MIC IE added
 */
int mfp_add_mgmt_mic(struct txdesc *txdesc, uint32_t frame, int lentgh);


/**
 * @}
 */
#endif /* _MFP_H_ */

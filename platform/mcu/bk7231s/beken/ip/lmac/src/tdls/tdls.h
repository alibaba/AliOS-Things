/**
 ****************************************************************************************
 * @file tdls.h
 *
 * @brief Tunneled direct-link setup (TDLS) module definition.
 *
 * Copyright (C) RivieraWaves 2016
 *
 ****************************************************************************************
 */

#ifndef TDLS_H_
#define TDLS_H_

#if (TDLS_ENABLE)
/**
 *****************************************************************************************
 * @defgroup LMAC LMAC
 * @ingroup MACSW
 * @brief Lower MAC SW module.
 *****************************************************************************************
 */

/**
 *****************************************************************************************
 * @defgroup TDLS TLDS
 * @ingroup LMAC
 * @brief LMAC TDLS module.
 * @{
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// standard includes
#include "co_int.h"
#include "co_bool.h"

// for maximum station index
#include "mac_common.h"

#include "co_utils.h"
#include "ke_timer.h"
#include "tdls_task.h"
#include "hal_desc.h"
#include "hal_machw.h"
#include "hal_dma.h"
#include "sta_mgmt.h"

/*
 * DEFINES
 ****************************************************************************************
 */
// Time spent to switch channel, in us
#define TDLS_CHSW_SWITCH_TIME_US 5000
// Time spent to transmit a frame included SIFS, in us
#define TDLS_CHSW_TX_FRAME_TIME_US 1000
// Max channel switch timeout, in us
#define TDLS_MAX_CHSW_SWITCH_TIME_US 15000
// Delay of Channel Switch Request on TBTT time, in us
#define TDLS_CHSW_REQ_DELAY_US 10000
// Delay of Channel Switch Request when there is traffic with AP, in us
#define TDLS_CHSW_REQ_DELAY_AP_TRAFFIC_US 20000

// Forward declaration
struct vif_info_tag;

/*
 * ENUMERATIONS
 ****************************************************************************************
 */


/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */


/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

__INLINE uint32_t tdls_get_dt_us(uint32_t next_tbtt, uint32_t now)
{
    return (uint32_t)((0xFFFFFFFF - now) + next_tbtt + 1) - TDLS_CHSW_REQ_DELAY_US;
}

/**
 ****************************************************************************************
 * @brief This function processes the received TDLS frames.
 *
 * @param[in] vif_entry Pointer to the VIF entry
 *
 * @return A boolean indicating if the frame is handled internally, or needs to be
 * forwarded to the host.
 ****************************************************************************************
 */
bool tdls_check_frame(uint8_t *frame, struct vif_info_tag *vif_entry);

/**
 ****************************************************************************************
 * @brief Callback called upon TDLS channel switch request timer expiration.
 * This timer is started when channel switch is enabled and periodically transmits the
 * TDLS Channel Switch Request frame.
 * In case there is traffic with the AP, the transmission of TDLS Channel Switch Request
 * frame is delayed.
 * Next TDLS channel switch request timer time is set to 10 ms after next TBTT.
 *
 * @param[in] env   Pointer the VIF entry
 ****************************************************************************************
 */
void tdls_chsw_req_evt(void *env);

/**
 ****************************************************************************************
 * @brief TDLS channel switch to base channel indication.
 * This function sends the @TDLS_CHAN_SWITCH_BASE_IND message and sets the new state of
 * TDLS task as TDLS_BASE_CHANNEL.
 *
 * @param[in] roc_chan_ctxt pointer to the context allocated for the remain of channel
 *                          operation
 ****************************************************************************************
 */
void tdls_send_chan_switch_base_ind(struct chan_ctxt_tag *roc_chan_ctxt);

/**
 ****************************************************************************************
 * @brief TDLS peer power save indication.
 * This function sends the @TDLS_PEER_PS_IND message and sets the new state of
 * TDLS task as TDLS_PEER_SLEEPING.
 *
 * @param[in] vif_entry pointer to VIF entry
 * @param[in] ps_on     power save status
 ****************************************************************************************
 */
void tdls_send_peer_ps_ind(struct vif_info_tag *vif_entry, bool ps_on);

#endif // TDLS_ENABLE

/// @} end of group

#endif // TDLS_H_

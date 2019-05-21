/**
****************************************************************************************
*
* @file rxu_task.h
*
* Copyright (C) RivieraWaves 2011-2016
*
* @brief Declaration of the RXU task
*
****************************************************************************************
*/

#ifndef _RXU_TASK_H_
#define _RXU_TASK_H_

/** @defgroup TASK_RXU TASK_RXU
 * @ingroup UMACRX
 * @brief Declaration of the RXU task.
 */

// for frame and frame length
#include "mac_common.h"
// for mac_addr and other MAC definitions
#include "mac.h"
#include "ke_task.h"

/// Message API of the RXU task
enum rxu_msg_tag
{
    /// Management frame reception indication
    RXU_MGT_IND = KE_FIRST_MSG(TASK_RXU),
    /// NULL frame reception indication
    RXU_NULL_DATA,
};

/// Parameters of the @ref RXU_MGT_IND message
struct rxu_mgt_ind
{
    /// Length of the frame
    uint16_t length;
    /// Frame control field of the frame.
    uint16_t framectrl;
    /// Center frequency on which we received the packet
    uint16_t center_freq;
    /// PHY band
    uint8_t band;
    /// Index of the station that sent the frame. 0xFF if unknown.
    uint8_t sta_idx;
    /// Index of the VIF that received the frame. 0xFF if unknown.
    uint8_t inst_nbr;
    /// RSSI of the received frame.
    int8_t rssi;
    /// Frame payload.
    uint32_t payload[];
};

/// Parameters of the @ref RXU_NULL_IND message
struct rxu_null_ind
{
    /// Frame control field of the frame.
    uint16_t framectrl;
    /// Index of the station that sent the frame. 0xFF if unknown.
    uint8_t sta_idx;
    /// Index of the VIF that received the frame. 0xFF if unknown.
    uint8_t inst_nbr;
    /// RSSI of the received frame.
    int8_t rssi;
};

/// @}

#endif // _RXU_TASK_H_

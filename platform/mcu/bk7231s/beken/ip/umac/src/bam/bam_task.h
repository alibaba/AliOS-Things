/**
 ****************************************************************************************
 *
 * @file bam_task.h
 *
 * @brief UMAC ME BAM (Block ACK manager) module state machine.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _BAM_TASK_H_
#define _BAM_TASK_H_

/**
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "bam.h"


/** @defgroup TASK_BAM TASK_BAM
 * @ingroup BAM
 * @brief UMAC ME BAM module state machine.
 * @{
 */

/**
 * ENUMERATIONS
 ****************************************************************************************
 */

/// BAM Possible states.
enum bam_state_tag
{
    /// BAM_IDLE initial state of the BAM state machine.
    BAM_IDLE = 0,

    /// Put the BAM as active.
    BAM_ACTIVE,
    /// Waits for the reception of the block ack rsp frame from the responder
    BAM_WAIT_RSP,
    /// Check admission of BA in the responder.
    BAM_CHECK_ADMISSION,
    /// Reset BA TACK.
    BAM_RESET,

    /// Number of states.
    BAM_STATE_MAX
};

/// BAM task message
enum bam_msg_tag
{
    /// BAM addition response timeout
    BAM_ADD_BA_RSP_TIMEOUT_IND  = KE_FIRST_MSG(TASK_BAM),
    /// BAM Inactivity timeout
    BAM_INACTIVITY_TIMEOUT_IND,
};

/**
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

extern const struct ke_state_handler bam_state_handler[BAM_STATE_MAX];
extern const struct ke_state_handler bam_default_handler;
extern ke_state_t bam_state[BAM_IDX_MAX];

/// @} end of group

#endif // _BAM_TASK_H_

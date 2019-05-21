/**
 ****************************************************************************************
 *
 * @file ke_timer.h
 *
 * @brief This file contains the definitions used for timer management
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _KE_TIMER_H_
#define _KE_TIMER_H_

/**
 ****************************************************************************************
 * @defgroup KETIMER KETIMER
 * @ingroup KERNEL
 * @brief Timer management module.
 *
 * The KETIMER module implements the functions used for managing kernel timers.
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwnx_config.h"

#include "ke_queue.h"
#include "ke_msg.h"
// for co_status_t
#include "co_status.h"

#if NX_EMB
#include "hal_machw.h"
#endif



/*
 * DEFINES
 ****************************************************************************************
 */
/** @addtogroup KETIMER
 * @{
 */
/// Timers can be set up to 300s in the future
#define KE_TIMER_DELAY_MAX           300000000
#define KE_TIMER_COUNT_HALF_MAX      0x8FFFFFFF
#define KE_TIMER_COUNT_MAX           0xFFFFFFFF

/// Standard Multiplier
#define MILLI2MICRO             1000

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Timer Object
struct ke_timer
{
    /// Pointer to the next timer element in the list
    struct ke_timer *next;
    /// Message identifier of the timer
    ke_msg_id_t     id;
    /// Identifier of the task that programmed the timer
    ke_task_id_t    task;
    /// Expiration time of the timer
    uint32_t        time;
    uint32_t        duration;
};

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */


/**
 ****************************************************************************************
 * @brief Get the current kernel time.
 *
 * @return The current kernel time
 ****************************************************************************************
 */
__INLINE uint32_t ke_time()
{
    return hal_machw_time();
}

/**
 ****************************************************************************************
 * @brief Compare absolute expiration time.
 *
 * @param[in] time1 First time to compare.
 * @param[in] time2 Second time to compare.
 *
 * @return true if time1 is earlier than time2.
 ****************************************************************************************
 */
__INLINE bool ke_time_abs_cmp(uint32_t time1, uint32_t time2)
{
    uint32_t diff = time1 - time2;

    return (((int32_t)diff) < 0);
}


/**
 ****************************************************************************************
 * @brief Check if the kernel time passed as parameter is in the past
 *
 * @param[in] time Time to check
 *
 * @return true if time is in the past, false otherwise
 ****************************************************************************************
 */
__INLINE bool ke_time_past(uint32_t time)
{
    return ke_time_abs_cmp(time, ke_time());
}

void ke_timer_set(ke_msg_id_t const timer_id,
                  ke_task_id_t const task,
                  uint32_t const delay);

void ke_timer_clear(ke_msg_id_t const timerid,
                           ke_task_id_t const task);


void ke_timer_schedule(int dummy);

void ke_timer_save_before_soft_reset(void);
void ke_timer_restore_after_soft_reset(void);

bool ke_timer_active(ke_msg_id_t const timer_id,
                    ke_task_id_t const task_id);


/// @}

#endif // _KE_TIMER_H_

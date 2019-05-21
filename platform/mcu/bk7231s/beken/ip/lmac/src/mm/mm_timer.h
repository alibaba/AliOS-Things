/**
 ****************************************************************************************
 * @file mm_timer.h
 *
 * @brief MAC Management timer module declarations.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _MM_TIMER_H_
#define _MM_TIMER_H_

/**
 *****************************************************************************************
 * @defgroup MM_TIMER MM_TIMER
 * @ingroup MM
 * @brief LMAC MAC Management timer module.
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

// for target dependent directives
#include "rwnx_config.h"
#include "reg_mac_core.h"

#include "co_list.h"

#define MM_TIMER_DELAY_MAX           300000000
#define MM_TIMER_COUNT_HALF_MAX      0x8FFFFFFF
#define MM_TIMER_COUNT_MAX           0xFFFFFFFF

#if NX_MM_TIMER
/*
 * DEFINES
 ****************************************************************************************
 */
/// Pointer to callback function
typedef void (*cb_timer_func_ptr)(void *);

/// Structure defining the properties of a timer
struct mm_timer_tag
{
    /// List element for chaining
    struct co_list_hdr list_hdr;
    /// Function to be called upon timer expiry
    cb_timer_func_ptr cb;
    /// Pointer to be passed to the callback
    void *env;
    /// Absolute expiry time
    uint32_t        time;	
	
    uint32_t        duration;
};

/// LMAC MAC Management Timer Context
struct mm_timer_env_tag
{
    /// List of programmed timers
    struct co_list prog;
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
extern struct mm_timer_env_tag mm_timer_env;

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief MM Timer Module initialization function.
 * This function is called after reset and initializes all MM timer related env and data.
 ****************************************************************************************
 */
void mm_timer_init(void);


/**
 ****************************************************************************************
 * @brief Program a timer to be scheduled in the future.
 *
 * @param[in] timer Pointer to the pre-filled timer structure (except value)
 * @param[in] value Absolute timeout value
 *
 ****************************************************************************************
 */
void mm_timer_set(struct mm_timer_tag *timer, uint32_t value);


/**
 ****************************************************************************************
 * @brief Clear a programmed timer.
 * This function searches for the timer passed as parameter in the list of programmed
 * timers and extract it from the list.
 *
 * @param[in] timer Pointer to the timer to be extracted
 *
 ****************************************************************************************
 */
void mm_timer_clear(struct mm_timer_tag *timer);
void mm_timer_save_before_soft_reset(void);
void mm_timer_restore_after_soft_reset(void);

/**
 ****************************************************************************************
 * @brief Schedule the next timer(s).
 *
 * This function pops the first timer from the timer queue and notifies calls the attached
 * callback function. The function checks also the next timers and process them if they
 * have expired or are about to expire.
 ****************************************************************************************
 */
void mm_timer_schedule(int dummy);

#endif


/// @} end of group

#endif // _MM_TIMER_H_

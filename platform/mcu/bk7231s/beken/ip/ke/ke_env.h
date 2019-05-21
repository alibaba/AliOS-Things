/**
 ****************************************************************************************
 *
 * @file ke_env.h
 *
 * @brief This file contains the definition of the kernel environment.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _KE_ENV_H_
#define _KE_ENV_H_

/**
 ****************************************************************************************
 * @defgroup KE_ENV KE_ENV
 * @ingroup KERNEL
 * @brief Kernel environment
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// for target dependent directives
#include "rwnx_config.h"
#include "ke_config.h"
// for queues
#include "co_list.h"

/*
 * DEFINES
 ****************************************************************************************
 */
// forward declaration
struct mblock_free;

/// Event bit field definition
typedef uint32_t evt_field_t;

/// Kernel environment definition
struct ke_env_tag
{
    /// Bit field indicating the events that are currently set
    volatile evt_field_t evt_field;

    /// Queue of sent messages but not yet delivered to receiver
    struct co_list queue_sent;
    /// Queue of messages delivered but not consumed by receiver
    struct co_list queue_saved;
    /// Queue of timers
    struct co_list queue_timer;
};

/// Kernel environment
extern struct ke_env_tag ke_env;

/// @} //end of group KE_ENV

#endif // _KE_ENV_H_

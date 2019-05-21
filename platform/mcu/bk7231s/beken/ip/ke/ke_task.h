/**
 ****************************************************************************************
 *
 * @file ke_task.h
 *
 * @brief This file contains the definition related to kernel task management.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _KE_TASK_H_
#define _KE_TASK_H_

/**
 ****************************************************************************************
 * @defgroup KE_TASK KE_TASK
 * @ingroup KERNEL
 * @brief Task management module.
 *
 * This module implements the functions used for managing tasks.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"
#include "co_bool.h"
#include "rwnx_config.h"
#include "compiler.h"
#include "ke_msg.h"


/*
 * DEFINES
 ****************************************************************************************
 */
/// Tasks types.
enum
{
    TASK_NONE = (uint8_t) -1,

    /// MAC Management task.
    TASK_MM = 0,

    #if NX_HW_SCAN
    /// SCAN task
    TASK_SCAN,
    #endif

    #if (TDLS_ENABLE)
    /// TDLS task
    TASK_TDLS,
    #endif

    /// SCAN task
    TASK_SCANU,
    /// SCAN task
    TASK_ME,
    /// SM task
    TASK_SM,

    #if NX_BEACONING
    /// APM task
    TASK_APM,
    #endif
    
    /// BAM task
    TASK_BAM,

    #if (RW_MESH_EN)
    /// MESH task
    TASK_MESH,
    #endif
    
    /// RXU task
    TASK_RXU,
    // This is used to define the last task that is running on the EMB processor
    TASK_LAST_EMB = TASK_RXU,

    // nX API task
    TASK_API,
    TASK_MAX,
};

#define DRV_TASK_ID                     100


/// Build the first message ID of a task.
/**
 ****************************************************************************************
 * @brief Builds the first message ID of a task
 * @param[in] task Task identifier
 * @return The identifier of the first message of the task
 ****************************************************************************************
 */
#define KE_FIRST_MSG(task) ((ke_msg_id_t)((task) << 10))

/**
 ****************************************************************************************
 * @brief Returns the task identifier based on a message identifier
 * @param[in] msg Message identifier
 * @return The identifier of the task this message belongs to
 ****************************************************************************************
 */
#define MSG_T(msg) ((ke_task_id_t)((msg) >> 10))

/**
 ****************************************************************************************
 * @brief Returns the message index in the task based on a message identifier
 * @param[in] msg Message identifier
 * @return The message index in the task
 ****************************************************************************************
 */
#define MSG_I(msg) ((msg) & ((1<<10)-1))

/// @brief Format of a task message handler function
typedef int (*ke_msg_func_t)(ke_msg_id_t const msgid,
                             void const *param,
                             ke_task_id_t const dest_id,
                             ke_task_id_t const src_id);

/// Element of a message handler table.
struct ke_msg_handler
{
    /// Id of the handled message.
    ke_msg_id_t id;
    /// Pointer to the handler function for the msgid above.
    ke_msg_func_t func;
};

/// Element of a state handler table.
struct ke_state_handler
{
    /// Pointer to the message handler table of this state.
    const struct ke_msg_handler *msg_table;
    /// Number of messages handled in this state.
    uint16_t msg_cnt;
};

/// Helps writing the initialization of the state handlers without errors.
#define KE_STATE_HANDLER(hdl) {hdl, sizeof(hdl)/sizeof(struct ke_msg_handler)}

/// Helps writing empty states.
#define KE_STATE_HANDLER_NONE {NULL, 0}


/// Task descriptor grouping all information required by the kernel for the scheduling.
struct ke_task_desc
{
    /// Pointer to the state handler table (one element for each state).
    const struct ke_state_handler* state_handler;
    /// Pointer to the default state handler (element parsed after the current state).
    const struct ke_state_handler* default_handler;
    /// Pointer to the state table (one element for each instance).
    ke_state_t* state;
    /// Maximum number of states in the task.
    uint16_t state_max;
    /// Maximum index of supported instances of the task.
    uint16_t idx_max;
};

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Check if a task is executed on the local platform
 *
 * @param[in] id Task instance identifier
 *
 * @return          True if the task is executed on the local platform
 ****************************************************************************************
 */
__INLINE bool ke_task_local(ke_task_id_t const id)
{
    return (id <= TASK_LAST_EMB);
}


ke_state_t ke_state_get(ke_task_id_t const id);

void ke_state_set(ke_task_id_t const id,
                  ke_state_t const state_id);

void ke_task_schedule(int dummy);


int ke_msg_discard(ke_msg_id_t const msgid,
                   void const *param,
                   ke_task_id_t const dest_id,
                   ke_task_id_t const src_id);

int ke_msg_save(ke_msg_id_t const msgid,
                void const *param,
                ke_task_id_t const dest_id,
                ke_task_id_t const src_id);


/// @} TASK

#endif // _KE_TASK_H_


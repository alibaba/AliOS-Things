/**
 ****************************************************************************************
 *
 * @file ke_msg.h
 *
 * @brief This file contains the definition related to message scheduling.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _KE_MSG_H_
#define _KE_MSG_H_

/**
 ****************************************************************************************
 * @defgroup KE_MSG KE_MSG
 * @ingroup KERNEL
 * @brief Message scheduling module.
 *
 * The MSG module implements message scheduling functions.

 * A kernel message has an ID, a receiver task ID and a source task ID.
 * In most cases, it also has parameters which are defined in
 * a structure dynamically embedded in the message structure,
 * so the whole message will be managed internally as one block.
 *
 * A message can also have one extra parameter which is referenced
 * in the normal parameter structure. This extra block is assumed
 * to be large by the kernel and will be moved by DMA if needed.
 * This feature allows moving MMPDU from LMAC to UMAC.
 *
 * In order to send a message, a function first have to allocate
 * the memory for this message. It can be done with the wrapper
 * macro KE_MSG_ALLOC() (which will call ke_msg_alloc()).

 * The message can then be sent with ke_msg_send(). The kernel
 * will take care of freeing the allocated memory.

 * If the message has no parameters, the ke_msg_send_basic() function
 * can be used.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stddef.h>
#include "co_int.h"
#include "co_bool.h"
#include "arch.h"
#include "compiler.h"

#include "ke_config.h"
#include "co_list.h"

/// Task Identifier. Composed by the task type and the task index.
typedef uint16_t ke_task_id_t;

/// Builds the task identifier from the type and the index of that task.
#define KE_BUILD_ID(type, index) ( (ke_task_id_t)(((index) << 8)|(type)) )

/// Retrieves task type from task id.
#define KE_TYPE_GET(ke_task_id) ((ke_task_id) & 0xFF)

/// Retrieves task index number from task id.
#define KE_IDX_GET(ke_task_id) (((ke_task_id) >> 8) & 0xFF)

/// Task State
typedef uint16_t ke_state_t;

/// Message Identifier. The number of messages is limited to 0xFFFF.
/// The message ID is divided in two parts:
/// - bits[15..10] : task index (no more than 64 tasks supported).
/// - bits[9..0] : message index (no more that 1024 messages per task).
typedef uint16_t ke_msg_id_t;

/// Message structure.
struct ke_msg
{
    struct co_list_hdr hdr;     ///< List header for chaining
    ke_msg_id_t     id;         ///< Message id.
    ke_task_id_t    dest_id;    ///< Destination kernel identifier.
    ke_task_id_t    src_id;     ///< Source kernel identifier.
    uint16_t        param_len;  ///< Parameter embedded struct length.
    uint32_t        param[1];   ///< Parameter embedded struct. Must be word-aligned.
};

/// Status returned by a task when handling a message
enum ke_msg_status_tag
{
    KE_MSG_CONSUMED = 0, ///< consumed, msg and ext are freed by the kernel
    KE_MSG_NO_FREE,      ///< consumed, nothing is freed by the kernel
    KE_MSG_SAVED,        ///< not consumed, will be pushed in the saved queue
};

/**
 ****************************************************************************************
 * @brief Convert a parameter pointer to a message pointer
 *
 * @param[in]  param_ptr Pointer to the parameter member of a ke_msg
 *                       Usually retrieved by a ke_msg_alloc()
 *
 * @return The pointer to the ke_msg
 ****************************************************************************************
 */
__INLINE struct ke_msg * ke_param2msg(void const *param_ptr)
{
	if(0 == param_ptr)
	{
		return 0;
	}
	
    return (struct ke_msg*) (((uint8_t*)param_ptr) - offsetof(struct ke_msg, param));
}

/**
 ****************************************************************************************
 * @brief Convert a message pointer to a parameter pointer
 *
 * @param[in]  msg Pointer to the ke_msg.
 *
 * @return The pointer to the param member
 ****************************************************************************************
 */
__INLINE void * ke_msg2param(struct ke_msg const *msg)
{
    return (void*) (((uint8_t*) msg) + offsetof(struct ke_msg, param));
}

/**
 ****************************************************************************************
 * @brief Convenient wrapper to ke_msg_alloc()
 *
 * This macro calls ke_msg_alloc() and cast the returned pointer to the
 * appropriate structure. Can only be used if a parameter structure exists
 * for this message (otherwise, use ke_msg_send_basic()).
 *
 * @param[in] id        Message identifier
 * @param[in] dest      Destination Identifier
 * @param[in] src       Source Identifier
 * @param[in] param_str parameter structure tag
 *
 * @return Pointer to the parameter member of the ke_msg.
 ****************************************************************************************
 */
#define KE_MSG_ALLOC(id, dest, src, param_str) \
    (struct param_str*) ke_msg_alloc(id, dest, src, sizeof(struct param_str))

/**
 ****************************************************************************************
 * @brief Convenient wrapper to ke_msg_alloc(), for variable size message parameter
 * structures.
 *
 * This macro calls ke_msg_alloc() and cast the returned pointer to the
 * appropriate structure. Can only be used if a parameter structure exists
 * for this message (otherwise, use ke_msg_send_basic()).
 *
 * @param[in] id        Message identifier
 * @param[in] dest      Destination Identifier
 * @param[in] src       Source Identifier
 * @param[in] param_str parameter structure tag
 * @param[in] var_len   Length of the variable part to be allocated
 *
 * @return Pointer to the parameter member of the ke_msg.
 ****************************************************************************************
 */
#define KE_MSG_ALLOC_VAR(id, dest, src, param_str, var_len) \
    (struct param_str*) ke_msg_alloc(id, dest, src, sizeof(struct param_str) + var_len)

/**
 ****************************************************************************************
 * @brief Convenient wrapper to ke_msg_free()
 *
 * This macro calls ke_msg_free() with the appropriate msg pointer as parameter, according
 * to the message parameter pointer passed.
 *
 * @param[in] param_ptr parameter structure pointer
 ****************************************************************************************
 */
#define KE_MSG_FREE(param_ptr) ke_msg_free(ke_param2msg((param_ptr)))

void *ke_msg_alloc(ke_msg_id_t const id,
                   ke_task_id_t const dest_id,
                   ke_task_id_t const src_id,
                   uint16_t const param_len);

void ke_msg_send(void const *param_ptr);

void ke_msg_send_basic(ke_msg_id_t const id,
                       ke_task_id_t const dest_id,
                       ke_task_id_t const src_id);

void ke_msg_forward(void const *param_ptr,
                    ke_task_id_t const dest_id,
                    ke_task_id_t const src_id);

void ke_msg_forward_and_change_id(void const *param_ptr,
                                  ke_msg_id_t const msg_id,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id);

void ke_msg_free(struct ke_msg const *param);

/// @} KE_MSG

#endif // _KE_MSG_H_

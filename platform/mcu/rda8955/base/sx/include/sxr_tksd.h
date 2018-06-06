/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/







#ifndef __SXR_TKSD_H__
#define __SXR_TKSD_H__

#include "sxs_type.h"
#include "sxr_cnf.h"
#ifdef __cplusplus
extern "C" {
#endif

/// Task context static descriptor
typedef struct
{
    /// A pointer to a task body.
    /// The task body corresponds to a function implementing the task.
    /// When the task enters in ACTIVE state for the first time, the task body is
    /// invoked with the address of the dynamic context as a parameter.
    void  (*TaskBody)(void *);
    /// A pointer to an exit function. The exit function, when it exists,
    /// is called when the task is stopped. The exit function, when called,
    /// is given the address of the dynamic context.
    void  (*TaskExit)(void *);
    /// A pointer to a string that contains the name of the task.
    const ascii *Name;
    /// A stack size expressed in number of double words, to be allocated for the task.
    u16   StackSize;
    /// The priority [0..255] to be given to the task.
    u8    Priority;

    void *user_info[];
} sxr_TaskDesc_t;



// =============================================================================
// sxr_TkSdInit
// -----------------------------------------------------------------------------
/// Nucleus initialization.
// =============================================================================
void sxr_TkSdInit (void);


// =============================================================================
// sxr_NewTask
// -----------------------------------------------------------------------------
/// Allocate a new task.
/// @param Desc Pointer onto the task context static descriptor.
/// @return     Task Id.
// =============================================================================
u8 sxr_NewTask  (sxr_TaskDesc_t const *Desc);
u8 sxr_NewTaskExt(sxr_TaskDesc_t const *Desc, u32 *pStack);

// =============================================================================
// sxr_FreeTask
// -----------------------------------------------------------------------------
/// Free a task.
/// @param Id Task Id.
// =============================================================================
void sxr_FreeTask (u8 Id);


// =============================================================================
// sxr_StartScheduling
// -----------------------------------------------------------------------------
/// Activation of the first eligible task.
// =============================================================================
void sxr_StartScheduling (void);


// =============================================================================
// sxr_SchedulingStarted
// -----------------------------------------------------------------------------
/// Tell if the scheduling has been started.
// =============================================================================
bool sxr_SchedulingStarted (void);


// =============================================================================
// sxr_StartTask
// -----------------------------------------------------------------------------
/// Initialize dynamic context and wake up the task.
/// @param Id       Task Id.
/// @param DataCtx  Pointer onto data context.
// =============================================================================
void sxr_StartTask       (u8 Id, void *DataCtx);

// =============================================================================
// sxr_StopTask
// -----------------------------------------------------------------------------
/// Stop a task.
/// @param Id Task Id.
// =============================================================================
void sxr_StopTask        (u8 Id);

// =============================================================================
// sxr_SuspendTask
// -----------------------------------------------------------------------------
/// Suspend a task.
/// @param Id Task Id.
/// @return
// =============================================================================
void sxr_SuspendTask     (u8 Id);

// =============================================================================
// sxr_ResumeTask
// -----------------------------------------------------------------------------
/// Resume a task.
/// @param Id Task Id.
// =============================================================================
void sxr_ResumeTask      (u8 Id);

// =============================================================================
// sxr_IsSuspendedTask
// -----------------------------------------------------------------------------
/// Check if a task is suspended.
/// @param Id Task Id.
/// @return   TRUE if the task is suspended, FALSE otherwise
// =============================================================================
u8   sxr_IsSuspendedTask (u8 Id);


// =============================================================================
// sxr_EnterScSchedule
// -----------------------------------------------------------------------------
/// Disable tasks scheduling.
// =============================================================================
void sxr_EnterScSchedule (void);

// =============================================================================
// sxr_ExitScSchedule
// -----------------------------------------------------------------------------
/// Enable tasks scheduling.
// =============================================================================
void sxr_ExitScSchedule  (void);

// =============================================================================
// sxr_ScheduleIdle
// -----------------------------------------------------------------------------
/// Force Idle task scheduling and disable the scheduling.
// =============================================================================
void sxr_ScheduleIdle    (void);


// =============================================================================
// sxr_GetTaskPriority
// -----------------------------------------------------------------------------
/// Get the active task priority.
/// @return Active task priority.
// =============================================================================
u8   sxr_GetTaskPriority (void);

// =============================================================================
// sxr_GetTaskName
// -----------------------------------------------------------------------------
/// Get the active task Name.
/// @return Pointer onto active task Name.
// =============================================================================
const ascii *sxr_GetTaskName (void);

// =============================================================================
// sxr_GetTaskNameById
// -----------------------------------------------------------------------------
/// Get the task name by task ID.
/// @param Id Task Id.
/// @return Pointer onto the task Name.
// =============================================================================
const ascii *sxr_GetTaskNameById (u8 Id);

// =============================================================================
// sxr_GetTaskParam
// -----------------------------------------------------------------------------
/// Get the dynamic context of a task as defined by sxr_StartTask().
/// @param Id Task Id.
/// @return   user defined data context.
// =============================================================================
void *sxr_GetTaskParam   (u8 Id);

// =============================================================================
// sxr_GetTaskState
// -----------------------------------------------------------------------------
/// Get task current state.
/// @param Id Task Id.
/// @return   the task's state.
// =============================================================================
u8   sxr_GetTaskState   (u8 Id);

// =============================================================================
// sxr_GetStackMaxUsage
// -----------------------------------------------------------------------------
/// Calculate the max used bytes in stack for a task.
/// @param TkNb Task Id.
// =============================================================================
u32 sxr_GetStackMaxUsage(u8 TkNb);

// =============================================================================
// sxr_ChkTask
// -----------------------------------------------------------------------------
/// Trace out a diagnostic information for a task.
/// @param TkNb Task Id.
// =============================================================================
void sxr_ChkTask (u8 TkNb);

// =============================================================================
// sxr_CheckTask
// -----------------------------------------------------------------------------
/// Trace out a diagnostic information for all the existing task.
// =============================================================================
void sxr_CheckTask (void);

// =============================================================================
// sxs_CheckStkTk
// -----------------------------------------------------------------------------
/// Lookup a task from a stack pointer.
/// Trace out a diagnostic information about the task found..
/// @param Sp Stack pointer.
/// @return TRUE if task was found.
// =============================================================================
u8   sxs_CheckStkTk (u32 *Sp);

// =============================================================================
// sxs_CheckPendingTk
// -----------------------------------------------------------------------------
/// Trace out a diagnostic information for all the pending or active task.
// =============================================================================
void sxs_CheckPendingTk (void);

// =============================================================================
// sxs_GetTaskSp
// -----------------------------------------------------------------------------
/// Get the stack pointer from the task context list.
/// @param Id Task Id.
/// @return The stack pointer.
// =============================================================================
u32 * sxr_GetTaskSp(u8 Id);


char * sxr_GetStatckTop(u8 id);
char * sxr_GetStatckBottom(u8 id);
u8 sxr_GetCurrentTaskId (void);
void *sxr_GetCurrentTaskDesc (void);

#ifdef __cplusplus
}
#endif

#endif

/// @} // <-- End of sx_task group

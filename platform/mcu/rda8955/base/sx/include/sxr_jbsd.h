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







#ifndef __SXR_JBSD_H__
#define __SXR_JBSD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sxs_type.h"


#define SXR_NIL_JOB         0xFF

#define SXR_FREE_JOB        ((u16)1 << 4)
#define SXR_ALLOCATED_JOB   ((u16)1 << 5)
#define SXR_STOPPED_JOB     ((u16)1 << 6)
#define SXR_INIT_JOB        ((u16)1 << 7)
#define SXR_ACTIVE_JOB      ((u16)1 << 8)
#define SXR_WAIT_DUR_JOB    ((u16)1 << 9)
#define SXR_WAIT_EVT_JOB(n) (((u16)1 << 10) | (n & 0xF))

#define SXR_GET_EVT(State)   (State & 0xF)

#define SXR_NEXT_FRAME_EVT   13
#define SXR_FRAME_END_EVT    14
#define SXR_CLEAR_EVT        15
#define SXR_USR_EVT(n)       n

#define SXR_CUR_FRAME        0
#define SXR_NEXT_FRAME       1
#define SXR_ANY_FRAME        2

#define SXR_JOB_NB_IDX       0
#define SXR_JOB_ID_IDX       1

#define SXR_JOB_IS_STOPPED        (1 << 0)
#define SXR_JOB_IS_BEING_DELETED  (1 << 1)

typedef struct
{
    void (*JbBody)(void *);
    void (*JbExit)(void *, u8, u8);
    const ascii *Name;
    u16  Id;
    u8   StackSize;
    u8   Priority;
} sxr_JbDesc_t;


// =============================================================================
// sxr_JbSdInit
// -----------------------------------------------------------------------------
/// Jobs scheduler data context initialization.
// =============================================================================
void  sxr_JbSdInit (void);

// =============================================================================
// sxr_NewJob
// -----------------------------------------------------------------------------
/// Allocate a free job context.
/// @param  Desc    Pointer onto sxr_JbDesc_t
/// @return         Job number.
// =============================================================================
u8    sxr_NewJob (sxr_JbDesc_t const *Desc);

// =============================================================================
// sxr_FreeJob
// -----------------------------------------------------------------------------
/// Free a job context.
/// @param  JobNb   Job number.
// =============================================================================
void  sxr_FreeJob (u8 JobNb);

// =============================================================================
// sxr_StartJob
// -----------------------------------------------------------------------------
/// Start identified job with the data context given in parameter.
/// @param  JobNb   job number.
/// @param  DataCtx pointer onto data context.
// =============================================================================
void  sxr_StartJob (u8 JobNb, void *DataCtx);

// =============================================================================
// sxr_InitJobEventStatus
// -----------------------------------------------------------------------------
/// Init job event status at the beginning of a new frame period.
// =============================================================================
VOID sxr_InitJobEventStatus (VOID);

// =============================================================================
// sxr_FrameJobScheduling
// -----------------------------------------------------------------------------
/// Called on frame interrupt. Indicate the beginning of a new frame period.
/// @param  NbFrames    Number of frames occured since last frame interrupt.
// =============================================================================
void  sxr_FrameJobScheduling (u16 NbFrames);

// =============================================================================
// sxr_WaitJDuration
// -----------------------------------------------------------------------------
/// Wait for the required number of frames periods.
/// @param  NbFrames    Number of frames to wait.
// =============================================================================
void  sxr_WaitJDuration (u16 NbFrames);

// =============================================================================
// sxr_GetJobWaitingPeriod
// -----------------------------------------------------------------------------
/// Return the number of frame period to be waited of a job in wait state.
/// @param  Id      job number or job user Id.
/// @param  NbVsId  boolean indicating if parameter Id corresponds to the
///                 job number or to the user Id.
/// @return         Duration in frame period or SXR_NO_DUE_DATE when
///                 job is not found in wait state.
// =============================================================================
u32   sxr_GetJobWaitingPeriod (u16 Id, u8 NbVsId);

// =============================================================================
// sxr_FrameBoundary
// -----------------------------------------------------------------------------
/// Look for the virtual frame end boundary. Must be invoked in
/// each interruption that is likely to raise a short lived event.
/// Events clear and last, when expected, are raised when the
/// virtual end of the frame is detected.
/// @return TRUE when the end frame boundary has been reached FALSE otherwise.
// =============================================================================
u8    sxr_FrameBoundary (void);

// =============================================================================
// sxr_RaiseJEvent
// -----------------------------------------------------------------------------
/// Raise a job event. A job waiting for the raised event
/// is scheduled immediately.
/// @param  Evt     Event number.
/// @param  Data    Event data.
// =============================================================================
void  sxr_RaiseJEvent (u8 Evt, void *Data);

// =============================================================================
// sxr_RaiseLongLivedJEvent
// -----------------------------------------------------------------------------
/// Raise a long lived job event. A job waiting for the
/// raised event is scheduled immediately. Event is held until
/// cleared or consumed.
/// @param  Evt     Event number.
/// @param  Data    Event data.
// =============================================================================
void  sxr_RaiseLongLivedJEvent (u8 Evt, void *Data);

// =============================================================================
// sxr_RaiseFrameJEvent
// -----------------------------------------------------------------------------
/// Raise a frame lived job event. A job waiting for the
/// raised event is scheduled immediately. Event is held for the
/// duration of the frame only.
/// @param  Evt     Event number.
/// @param  Data    Event data.
// =============================================================================
void  sxr_RaiseFrameJEvent (u8 Evt, void *Data);

// =============================================================================
// sxr_ClearJEvent
// -----------------------------------------------------------------------------
/// Clear an event.
/// @param  Evt Event number.
/// @return
// =============================================================================
void  sxr_ClearJEvent (u8 Evt);

// =============================================================================
// sxr_WaitJEvent
// -----------------------------------------------------------------------------
/// Wait for a job event.
/// @param  Evt     Event number.
/// @param  When    specifiy if the expected event is a frame lived
///                 (SXR_THIS_FRAME or SXR_NEXT_FRAME) or a long lived event
///                 (SXR_ANY_FRAME).
// =============================================================================
void *sxr_WaitJEvent (u8 Evt, u8 When);

// =============================================================================
// sxr_AutoStop
// -----------------------------------------------------------------------------
/// Job suspend itself and switch into STOPPED state.
// =============================================================================
void  sxr_AutoStop (void);

// =============================================================================
// sxr_StopJob
// -----------------------------------------------------------------------------
/// Suspend or delete one or several jobs.
/// When Id given in parameter is 0, job to be considered is
/// identified by its number. Otherwise, all job such that
/// JobId & Msk = parameter Id are considered for suspension/deletion.
/// @param  JobNb       Job number.
/// @param  Id
/// @param  Msk
/// @param  Deletion    when TRUE, job is deleted, othewise, job is suspended.
/// @return
// =============================================================================
void  sxr_StopJob (u8 JobNb, u16 Id, u16 Msk, u8 Deletion);

// =============================================================================
// sxr_ResumeJob
// -----------------------------------------------------------------------------
/// Resume one or several jobs.
/// When Id given in parameter is 0, job to be considered is
/// identified but its number. Otherwise, all job such that
/// JobId & Msk = parameter Id are resumed.
/// @param  JobNb   Job number.
/// @param  Id
/// @param  Msk
// =============================================================================
void  sxr_ResumeJob (u8 JobNb, u16 Id, u16 Msk);

// =============================================================================
// sxr_ResetJob
// -----------------------------------------------------------------------------
/// Reset one or several jobs.
/// When Id given in parameter is 0, job to be considered is
/// identified but its number. Otherwise, all job such that
/// JobId & Msk = parameter Id are reset.
/// @param  JobNb   Job number.
/// @param  Id
/// @param  Msk
// =============================================================================
void  sxr_ResetJob (u8 JobNb, u16 Id, u16 Msk);

// =============================================================================
// sxr_SearchJob
// -----------------------------------------------------------------------------
/// Search a job such that JobId & Msk = Id Parameter.
/// only the first found job is retrieved.
/// @param  Id
/// @param  Msk
/// @return     Job number or SXR_NIL_JOB.
// =============================================================================
u8    sxr_SearchJob (u16 Id, u16 Msk);

// =============================================================================
// sxr_SkipFrameEndEvt
// -----------------------------------------------------------------------------
/// Tels sxr_FrameBoundary() to skip the #SXR_FRAME_END_EVT event
// =============================================================================
void  sxr_SkipFrameEndEvt (void);

// =============================================================================
// sxr_GetJobId
// -----------------------------------------------------------------------------
/// Retreived the user Id of the job.
/// @param  JobNb   Job number.
/// @return         User Id.
// =============================================================================
u16   sxr_GetJobId (u16 JobNb);

// =============================================================================
// sxr_GetActiveJob
// -----------------------------------------------------------------------------
/// Retrieve the current active job number.
/// @return Active job number.
// =============================================================================
u8    sxr_GetActiveJob (void);

// =============================================================================
// sxr_NextFrameClear
// -----------------------------------------------------------------------------
/// Check whether the next frame is clear.
/// @return TRUE if next frame is clear
// =============================================================================
u8    sxr_NextFrameClear (void);

// =============================================================================
// sxr_IsJobInitializing
// -----------------------------------------------------------------------------
/// Return the number of Jobs in the INIT state.
/// @return the number of Jobs in the INIT state.
// =============================================================================
u8    sxr_IsJobInitializing (void);

// =============================================================================
// sxr_GetJobState
// -----------------------------------------------------------------------------
/// Get the state of the identified job.
/// @param  JobNb   Job number.
/// @return         State.
// =============================================================================
u16   sxr_GetJobState (u8 JobNb);

// =============================================================================
// sxr_GetJobParam
// -----------------------------------------------------------------------------
/// Get the job data context.
/// parameter \p DataCtx of sxr_StartJob().
/// @param  JobNb   job number.
/// @return         pointer onto data context.
// =============================================================================
void *sxr_GetJobParam (u8 JobNb);

// =============================================================================
// sxr_GetJobName
// -----------------------------------------------------------------------------
/// Get the active job name.
/// @return the active job name or "No Active Job"
// =============================================================================
const ascii *sxr_GetJobName (void);

// =============================================================================
// sxr_CheckJob
// -----------------------------------------------------------------------------
/// Print status for all allocated jobs.
/// @return number of jobs.
// =============================================================================
u8    sxr_CheckJob (void);

// =============================================================================
// sxr_ChkJob
// -----------------------------------------------------------------------------
/// Print status for a job.
/// @param  JbNb    job number.
// =============================================================================
void  sxr_ChkJob (u8 JbNb);

// =============================================================================
// sxs_CheckStkJb
// -----------------------------------------------------------------------------
/// Find a job from a stack pointer and Print status for this job.
/// @param  Sp  Stack Pointer
/// @return True if job was found.
// =============================================================================
u8    sxs_CheckStkJb (u32 *Sp);

// =============================================================================
// sxs_GetJobSpById
// -----------------------------------------------------------------------------
/// Get the stack pointer from the job context list.
/// @param Id Job Id.
/// @return The stack pointer.
// =============================================================================
u32 * sxr_GetJobSpById(u16 Id);

#ifdef __cplusplus
}
#endif

#endif

/// @} // <-- End of sx_jbsd group


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






#ifndef __SXR_SBX_H__
#define __SXR_SBX_H__

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// sxr_SbxInit
// -----------------------------------------------------------------------------
/// Synchronization boxes data context initialization.
// =============================================================================
void  sxr_SbxInit (void);

// =============================================================================
// sxr_NewMailBox
// -----------------------------------------------------------------------------
/// provide a free mail box Id.
/// @return Mail box Id.
// =============================================================================
u8    sxr_NewMailBox (void);

// =============================================================================
// sxr_FreeMailBox
// -----------------------------------------------------------------------------
/// Free the previously allocated mail box.
/// @param Sbx  Mail box Id.
// =============================================================================
void  sxr_FreeMailBox (u8 Sbx);

// =============================================================================
// sxr_NewSemaphore
// -----------------------------------------------------------------------------
/// Provide a free semaphore.
/// @param InitValue    Semaphore initialization value:
///                     - 0 Means semaphore is taken,
///                     - n Means semaphore is free for n users.
///                     .
/// @return             Semaphore Id.
// =============================================================================
u8    sxr_NewSemaphore (u8 InitValue);

// =============================================================================
// sxr_FreeSemaphore
// -----------------------------------------------------------------------------
/// Free a previously allocated semaphore.
/// @param Id   Semaphore Id.
// =============================================================================
void  sxr_FreeSemaphore (u8 Id);

// =============================================================================
// sxr_SetRmtMbx
// -----------------------------------------------------------------------------
/// Set or unset a mail box in remote state.
/// @param Sbx  Sbx index.
/// @param Set  boolean Set.
// =============================================================================
void  sxr_SetRmtMbx (u8 Sbx, u8 Set);

// =============================================================================
// sxr_IsRmtSbx
// -----------------------------------------------------------------------------
/// Get a mail box in remote state.
/// @param Sbx  Sbx index.
/// @return     remote state.
// =============================================================================
u8    sxr_IsRmtSbx (u8 Sbx);

// =============================================================================
// sxr_SetRxTxRmtMsg
// -----------------------------------------------------------------------------
/// Set the remote conversion function pointers.
/// @param TxMsg    Pointer on Tx conversion function.
/// @param RxMsg    Pointer on Rx conversion function.
// =============================================================================
void  sxr_SetRxTxRmtMsg (void *(*TxMsg)(void *), void *(*RxMsg)(void *));

// =============================================================================
// sxr_SbxRxMsg
// -----------------------------------------------------------------------------
/// Call remote conversion function for received messages.
/// @param Msg  Pointer on received message.
/// @return     Pointer on converted message.
// =============================================================================
void *sxr_SbxRxMsg (void *Msg);

#define SXR_SEND_EVT    (1 << 0)
#define SXR_SEND_MSG    0
#define SXR_QUEUE_FIRST (1 << 1)
#define SXR_QUEUE_LAST  0

// =============================================================================
// sxr_Send
// -----------------------------------------------------------------------------
/// Send a message or an event to a synchronization box.
/// @param Msg          Pointer onto message or event.
/// @param Sbx          Box number.
/// @param MsgStatus    Message status.
// =============================================================================
void  sxr_Send (void *Msg, u8 Sbx, u8 MsgStatus);

// =============================================================================
// sxr_Wait
// -----------------------------------------------------------------------------
/// Wait for a message or an event on a synchronization box.
/// @param Evt  Pointer onto Event to be received.
/// @param Sbx  Synchronization box number.
/// @return     Pointer onto message or NIL for event.
// =============================================================================
void *sxr_Wait (u32 *Evt, u8 Sbx);

// =============================================================================
// sxr_WaitEx
// -----------------------------------------------------------------------------
/// Wait for a message or an event on a synchronization box.
/// @param Evt  Pointer onto Event to be received.
/// @param Sbx  Synchronization box number.
/// @param SenderTaskId Pointer onto the sender's task ID.
/// @param SenderAddr Pointer onto the sender's function address.
/// @return     Pointer onto message or NIL for event.
// =============================================================================
void *sxr_WaitEx (u32 *Evt, u8 Sbx, u8 *SenderTaskId, u32 *SenderAddr);

// =============================================================================
// sxr_SbxHot
// -----------------------------------------------------------------------------
/// Check if at least a message or an event is in the synchronisation box.
/// @param Sbx  Synchronization box number.
/// @return     TRUE when box is hot, FALSE otherwise.
// =============================================================================
u8    sxr_SbxHot (u8 Sbx);

// =============================================================================
// sxr_RemoveCustMsgFromMbox
// -----------------------------------------------------------------------------
/// Remove one message  from a mail box.add by licheng
/// @param Sbx          Mail box number.
/// @param msgid      message wan't to remove
/// @param isAny      remove all message or only the customed msgid message
/// @return             TRUE when message or event has been found, FALSE otherwise.
///
//   cmni, add, CreselPlmnScan, 2016.03.21
// =============================================================================
u8 sxr_RemoveOneCustMsgFromMbox (u8 Sbx, u32 msgid, u8 isAny);

// =============================================================================
// sxr_RmvTaskFromSbx
// -----------------------------------------------------------------------------
/// Remove a task waiting on a SBX.
/// @param TaskId   Task Id.
/// @return         TRUE when task has been removed, FALSE otherwise.
// =============================================================================
u8    sxr_RmvTaskFromSbx (u8 TaskId);

// =============================================================================
// sxr_RmvMsgFromMbx
// -----------------------------------------------------------------------------
/// Remove a message or an event from a mail box.
/// @param Msg          Pointer onto a message.
/// @param Evt          Event description.
/// @param Sbx          Mail box number.
/// @param EvtCtxSize   Event size(0 for message).
/// @return             TRUE when message or event has been found, FALSE otherwise.
// =============================================================================
u8    sxr_RmvMsgFromMbx (void *Msg, u8 Evt, u8 Sbx, u8 EvtCtxSize);

// =============================================================================
// sxr_RmvAllMsgFromMbx
// -----------------------------------------------------------------------------
/// Remove all event/msg already sent to the mail box.
/// @param Sbx  Mail box Id.
// =============================================================================
void  sxr_RmvAllMsgFromMbx (u8 Sbx);

// =============================================================================
// sxr_CheckMsgInMbx
// -----------------------------------------------------------------------------
/// Check whether a message or an event existed in a mail box.
/// @param Msg          Pointer onto a message.
/// @param Evt          Event description.
/// @param Sbx          Mail box number.
/// @param EvtCtxSize   Event size(0 for message).
/// @return             TRUE when message or event has been found, FALSE otherwise.
// =============================================================================
u8    sxr_CheckMsgInMbx (void *Msg, u8 Evt, u8 Sbx, u8 EvtCtxSize);

// =============================================================================
// sxr_TakeSemaphore
// -----------------------------------------------------------------------------
/// Take the semaphore.
/// @param Id   Semaphore Id.
// =============================================================================
void  sxr_TakeSemaphore (u8 Id);

// =============================================================================
// sxr_ReleaseSemamphore
// -----------------------------------------------------------------------------
/// Release a semaphore.
/// @param Id   Semaphore Id.
void  sxr_ReleaseSemaphore (u8 Id);

// =============================================================================
// sxr_SemaphoreHot
// -----------------------------------------------------------------------------
/// Check if a semaphore is free.
/// @param Id   Semaphore Id.
/// @return     TRUE when the semaphore is free, FALSE when it is hot.
// =============================================================================
u8    sxr_SemaphoreHot (u8 Id);

// =============================================================================
// sxr_GetTaskWaitingForSbx
// -----------------------------------------------------------------------------
/// Return the Id of the task waiting on the Sbx.
/// @param Sbx  Sbx index.
/// @return     Id of the task.
// =============================================================================
u8    sxr_GetTaskWaitingForSbx (u8 Sbx);


// =============================================================================
// sxr_CheckSbx
// -----------------------------------------------------------------------------
/// Trace out a diagnostic information for all sbx
// =============================================================================
void  sxr_CheckSbx (void);

// =============================================================================
// sxr_ChkSbx
// -----------------------------------------------------------------------------
/// Trace out a diagnostic information for sbx
/// @param Sbx  Sbx index.
// =============================================================================
void  sxr_ChkSbx   (u8 Sbx);

// =============================================================================
// sxr_TraceSbx
// -----------------------------------------------------------------------------
/// Set trace diagnostic for sbx action (very verbose)
/// @param Sbx      Sbx index.
/// @param Level    Verbosity level, currently 0: off, else on.
// =============================================================================
void  sxr_TraceSbx (u8 Sbx, u8 Level);

typedef struct
{
    u8  NbAvailMsg;    // Number of message available in the Sbx.
    u8  NbWaitingTask; // Number of task waiting on the Sbx
    u8  FirstTaskId;   // Identifier of the first task waiting on the Sbx.
    u8  Pad;
    u32 FirstMsg;      // First message available in the Sbx.
} sxr_SbxInfo_t;


// =============================================================================
// sxr_GetSbxInfo
// -----------------------------------------------------------------------------
/// Get information about the identified synchronization box.
/// @param Sbx      Sbx index.
/// @param SbxInfo  Pointer to sxr_SbxInfo_t to fill.
// =============================================================================
void sxr_GetSbxInfo (u8 Sbx, sxr_SbxInfo_t *SbxInfo);

// =============================================================================
// sxr_TaskWaitToQueue
// -----------------------------------------------------------------------------
/// Put current/active task to SXR_WAIT_MSG_TSK state.
/// @param Sbx  Synchronization box number.
// =============================================================================
void sxr_TaskWaitToQueue (u8 Sbx);

// =============================================================================
// sxr_WakeupFirstTaskFromQueue
// -----------------------------------------------------------------------------
/// Wakeup the first waiting task (if existed) from queue/sbx.
/// @param Sbx  Synchronization box number.
// =============================================================================
void sxr_WakeupFirstTaskFromQueue (u8 Sbx);

// =============================================================================
// sxr_WakeupTaskFromQueue
// -----------------------------------------------------------------------------
/// Wakeup the given task (if existed) from queue/sbx.
/// @param Sbx  Synchronization box number.
// =============================================================================
void sxr_WakeupTaskFromQueue (u8 Task, u8 Sbx);

// =============================================================================
// sxr_FreeEnvCountAvail
// -----------------------------------------------------------------------------
/// Check whether the requested free env are aviable.
// =============================================================================
u8 sxr_FreeEnvCountAvail (u16 Cnt);

#ifdef __cplusplus
}
#endif

#endif

/// @} // <-- End of sx_sbx group

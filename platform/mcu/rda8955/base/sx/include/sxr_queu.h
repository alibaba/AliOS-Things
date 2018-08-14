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





#ifndef __SXR_QUEU_H__
#define __SXR_QUEU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sxs_type.h"

#define __SXR_QUEUE_DBG__

void  sxr_InitQueue (void);

#ifdef __SXR_QUEUE_DBG__
u8 _sxr_NewQueue (const ascii *Name, void (*Print)(void *));
#define sxr_NewQueue(a) _sxr_NewQueue(a, NIL)
#else
u8 __sxr_NewQueue (void);
#define sxr_NewQueue(a)   __sxr_NewQueue()
#define _sxr_NewQueue(a,b) __sxr_NewQueue()
#endif

void  sxr_FreeQueue (u8 Queue);

// =============================================================================
// _sxr_QueueFirst
// -----------------------------------------------------------------------------
/// Insert an item first in queue. Current position (user 0) is set to first.
/// If the usage is specified to be a low priority one, the element is inserted
/// if, and only if, enough envelop are available for the high priority usage.
///
/// @param Data Pointer to the item to be inserted.
/// @param Queue Index.
/// @param highPrio \c TRUE, if and only if, the usage is a high priority
/// one.
/// @return \c TRUE if the operation could be done, and the data is inserted.
///         \c FALSE if no envelop where available for a low priority use, and
/// the data couldn't be inserted.
// =============================================================================
PUBLIC BOOL _sxr_QueueFirst (void *Data, u8 Queue, BOOL highPrio);


// =============================================================================
// _sxr_QueueLast
// -----------------------------------------------------------------------------
/// Insert an item last in queue. Current position is set to last.
/// If the usage is specified to be a low priority one, the element is inserted
/// if, and only if, enough envelop are available for the high priority usage.
///
/// @param Data Pointer to the item to be inserted.
/// @param Queue Index.
/// @param highPrio \c TRUE, if and only if, the usage is a high priority
/// one.
/// @return \c TRUE if the operation could be done, and the data is inserted.
///         \c FALSE if no envelop where available for a low priority use, and
/// the data couldn't be inserted.
// =============================================================================
PUBLIC BOOL _sxr_QueueLast (void *Data, u8 Queue, BOOL highPrio);


// =============================================================================
// _sxr_QueueNext
// -----------------------------------------------------------------------------
/// Insert item after current position.
/// Current position is set to inserted item.
/// If the usage is specified to be a low priority one, the element is inserted
/// if, and only if, enough envelop are available for the high priority usage.
///
/// @param Data Pointer to the item to be inserted.
/// @param Queue Index.
/// @param highPrio \c TRUE, if and only if, the usage is a high priority
/// one.
/// @return \c TRUE if the operation could be done, and the data is inserted.
///         \c FALSE if no envelop where available for a low priority use, and
/// the data couldn't be inserted.
// =============================================================================
PUBLIC BOOL _sxr_QueueNext (void *Data, u8 Queue, BOOL highPrio);


// =============================================================================
// _sxr_QueueSorted
// -----------------------------------------------------------------------------
/// Insert item sorted according to Sort function criteria.
/// Current position is set to inserted item.
/// If the usage is specified to be a low priority one, the element is inserted
/// if, and only if, enough envelop are available for the high priority usage.
///
/// @param Data Pointer to the item to be inserted.
/// @param Sort Sort Function: this function takes as parameter 2 pointers
/// on 2 differents items. Returns \c TRUE when items order must
/// be inverted, \c FALSE otherwise.
/// @param Queue Index.
/// @param highPrio \c TRUE, if and only if, the usage is a high priority
/// one.
/// @return \c TRUE if the operation could be done, and the data is inserted.
///         \c FALSE if no envelop where available for a low priority use, and
/// the data couldn't be inserted.
// ==============================================================================
PUBLIC BOOL _sxr_QueueSorted (void *Data, u8 (*Sort)(void *Data1, void *Data2), u8 Queue, BOOL highPrio);


// Legacy interface, not priority aware, for stack use. Those are always high
// priority accesses.

// =============================================================================
// sxr_QueueFirst
// -----------------------------------------------------------------------------
/// Insert an item first in queue. Current position (user 0) is set to first.
/// This is the high priority wrapper for _sxr_QueueFirst.
///
/// @param Data Pointer to the item to be inserted.
/// @param Queue Index.
/// @return \c TRUE.
// =============================================================================
#define sxr_QueueFirst(Data, Queue) _sxr_QueueFirst(Data, Queue, TRUE)


// =============================================================================
// sxr_QueueLast
// -----------------------------------------------------------------------------
/// Insert an item last in queue. Current position is set to last.
/// This is the high priority wrapper for _sxr_QueueLast.
///
/// @param Data Pointer to the item to be inserted.
/// @param Queue Index.
/// @return \c TRUE.
// =============================================================================
#define sxr_QueueLast(Data, Queue) _sxr_QueueLast(Data, Queue, TRUE)


// =============================================================================
// sxr_QueueNext
// -----------------------------------------------------------------------------
/// Insert item after current position.
/// Current position is set to inserted item.
/// This is the high priority wrapper for _sxr_QueueNext.
///
/// @param Data Pointer to the item to be inserted.
/// @param Queue Index.
/// @return \c TRUE.
// =============================================================================
#define sxr_QueueNext(Data, Queue) _sxr_QueueNext(Data, Queue, TRUE)


// =============================================================================
// _sxr_QueueSorted
// -----------------------------------------------------------------------------
/// Insert item sorted according to Sort function criteria.
/// Current position is set to inserted item.
/// This is the high priority wrapper _sxr_QueueSorted.
///
/// @param Data Pointer to the item to be inserted.
/// @param Sort Sort Function: this function takes as parameter 2 pointers
/// on 2 differents items. Returns \c TRUE when items order must
/// be inverted, \c FALSE otherwise.
/// @param Queue Index.
/// @return \c TRUE.
// ==============================================================================
#define sxr_QueueSorted(Data, Sort, Queue) _sxr_QueueSorted(Data, Sort, Queue, TRUE)

// =============================================================================
// sxr_QueueFirstLP
// -----------------------------------------------------------------------------
/// Insert an item first in queue, if there is enough envelop available, or do
/// nothing. Current position (user 0) is set to first.
/// This is the low priority wrapper for _sxr_QueueFirst.
///
/// @param Data Pointer to the item to be inserted.
/// @param Queue Index.
/// @return \c TRUE if the item could be queued, of \c FALSE if there is no
/// envelop for low priority access anymore, in which case the data is not
/// queued.
// =============================================================================
#define sxr_QueueFirstLP(Data, Queue) _sxr_QueueFirst(Data, Queue, FALSE)


// =============================================================================
// sxr_QueueLastLP
// -----------------------------------------------------------------------------
/// Insert an item last in queue, if there is enough envelop available, or do
/// nothing. Current position is set to last.
/// This is the high priority wrapper for _sxr_QueueLast.
///
/// @param Data Pointer to the item to be inserted.
/// @param Queue Index.
/// @return \c TRUE if the item could be queued, of \c FALSE if there is no
/// envelop for low priority access anymore, in which case the data is not
/// queued.
// =============================================================================
#define sxr_QueueLastLP(Data, Queue) _sxr_QueueLast(Data, Queue, FALSE)


// =============================================================================
// sxr_QueueNextLP
// -----------------------------------------------------------------------------
/// Insert item after current position, if there is enough envelop available, or do
/// nothing.
/// Current position is set to inserted item.
/// This is the high priority wrapper for _sxr_QueueNext.
///
/// @param Data Pointer to the item to be inserted.
/// @param Queue Index.
/// @return \c TRUE if the item could be queued, of \c FALSE if there is no
/// envelop for low priority access anymore, in which case the data is not
/// queued.
// =============================================================================
#define sxr_QueueNextLP(Data, Queue) _sxr_QueueNext(Data, Queue, FALSE)


// =============================================================================
// _sxr_QueueSortedLP
// -----------------------------------------------------------------------------
/// Insert item sorted according to Sort function criteria, if there is enough
/// envelop available, or do nothing.
/// Current position is set to inserted item.
/// This is the high priority wrapper _sxr_QueueSorted.
///
/// @param Data Pointer to the item to be inserted.
/// @param Sort Sort Function: this function takes as parameter 2 pointers
/// on 2 differents items. Returns \c TRUE when items order must
/// be inverted, \c FALSE otherwise.
/// @param Queue Index.
/// @return \c TRUE if the item could be queued, of \c FALSE if there is no
/// envelop for low priority access anymore, in which case the data is not
/// queued.
// ==============================================================================
#define sxr_QueueSortedLP(Data, Sort, Queue) _sxr_QueueSorted(Data, Sort, Queue, FALSE)


void *sxr_QueueGetFirst (u8 User, u8 Queue);
void *sxr_QueueGetLast (u8 User, u8 Queue);
void *sxr_QueueGetNext (u8 Loop, u8 User, u8 Queue);
void *sxr_QueueGetCur (u8 User, u8 Queue);
void *sxr_QueueGet (void *UserData, u8 (*Get)(void *UserData, void *Data), u8 User, u8 Queue);
void *sxr_QueueRmvFirst (u8 Queue);
void *sxr_QueueRmvLast (u8 Queue);
void *sxr_QueueRmvCur (u8 Queue);
void *sxr_QueueRmv (void *Data, u8 Queue);
u16   sxr_QueuePurge (void *UserData, u8 (*Purge)(void *UserData, void *QueueData), bool All, u8 Queue);
void  sxr_QueueSort (u8 (*Sort)(void *Data1, void *Data2), u8 Queue);
u16   sxr_QueueGetCurPos (u8 User, u8 Queue);
void  sxr_QueueSetCurPos (u16 Pos, u8 User, u8 Queue);
u16   sxr_QueueGetLoad (u8 Queue);
const ascii *sxr_QueueGetName (u8 Queue);
void  sxr_CheckQueue (u8 Queue);
void  sxr_CheckAllQueues (void);

#define sxr_PopIn(Data, Queue) sxr_QueueLast(Data, Queue)
#define sxr_PopOut(Queue)      sxr_QueueRmvFirst(Queue)

#define SXR_QUEUE_USR(a)  a
#define SXR_QUEUE_LOOP    TRUE
#define SXR_QUEUE_NO_LOOP FALSE



#ifdef __cplusplus
}
#endif

#endif


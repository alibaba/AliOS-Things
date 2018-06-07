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





#ifndef __SXR_CONF_H__
#define __SXR_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sxr_csp.h"

#ifndef __TARGET__
#define __SXR_DEBUG__
#endif

#define __SXR_STACK_CHECK__
#define NO__SXR_MEM_CHECK__
#define NO__SXR_CLUSTER__               /* Cluster memory is handle with heap.                 */
#define __SXR_RELEASED_PTR_TO_NIL__     /* A called to Free(Ptr) provoke Ptr to be set to NIL. */
#define __SXR_MEM_TRACKER__

/* ---------------- OS configuration defines. ------------------------ */
#define SXR_NB_MAX_TASK           60     /* Must be < SXR_NB_MAX_MBX + SXR_NB_MAX_SEM */

#ifndef __TARGET__
#define SXR_STACK_SIZE_MULTIPLIER     15    // when not on target increase all the stacks by 15
#else
#define SXR_STACK_SIZE_MULTIPLIER     1
#endif

#define SXR_SET_STACK(Size)       (Size*SXR_STACK_SIZE_MULTIPLIER)

#define SXR_TK_STCK_HEAP           SXR_CSP_TK_STCK_HEAP   /* Heap to be considered for task stacks allocation. */
#define SXR_JB_STCK_HEAP           SXR_CSP_JB_STCK_HEAP //0   /* Heap to be considered for job stacks allocation. */
#define SXR_JB_CTX_HEAP            SXR_CSP_JB_CTX_HEAP   /* Heap used for the job context. */

/* ---------- Mail boxes & semaphores configuration defines. -------------- */
#ifdef __EGPRS__
#define SXR_NB_MAX_MBX_ENV         500  /* Number max of enveloppes.        */
#else
#define SXR_NB_MAX_MBX_ENV         500  /* Number max of enveloppes.        */
#endif
#define SXR_EVT_MBX_SIZE           4    /* Event size. Min = 2 u32          */
#define SXR_NB_MAX_MBX             160  /* Number max of mail boxes.        */
#define SXR_NB_MAX_SEM             40   /* Number max of semaphores.        */
#define SXR_NB_MAX_MUTEX           6    /* Number max of mutex.             */

/* ----------------- Timer configuration defines. ------------------------  */
#define SXR_TIMER_NB_INSTANCE      2      /* Number of timer instances.       */
#define SXR_NB_MAX_TIMER_ENVELOPPE 200    /* Number Max of actif timerS.      */
#define SXR_TIMER_CTX_SIZE         3      /* Min = 2 !!!                      */
#define SXR_TIMER_LIST_SIZE        8      /* Maximum timer list size.         */

#define SXR_REGULAR_TIMER          PAL_REGULAR_TIMER
#define SXR_FRAMED_TIMER           PAL_FRAMED_TIMER

/* ------------- Memory management configuration defines. ----------------- */
#define SXR_NB_MAX_POOL            7
#define SXR_NB_HEAP_USER           5

#define SXR_MEM_PATTERN            0xFD
#define SXR_WD_MEM_PATTERN         0xFDFD
#define SXR_DW_MEM_PATTERN         0xFDFDFDFD


/* ------------------- Queues configuration defines. ---------------------- */
#define SXR_NB_MAX_QUEUE                            240
#define SXR_NB_MAX_USR_QUEUE                        2
#ifdef __EGPRS__
#define SXR_NB_QUEUE_ENVELOPPE                      800
#else
#define SXR_NB_QUEUE_ENVELOPPE                      500
#endif
/// The queue are considered overloaded (Reserved for high priority usage) if
/// the number of free envelops fall beneath that value.
#define SXR_NB_FREE_QUEUE_ENVELOPPE_OVERLOAD_START  200
/// The queue are considered not overloaded anymore (Low priority usage is
/// allowed again) once the number of free envelops reaches that value.
#define SXR_NB_FREE_QUEUE_ENVELOPPE_OVERLOAD_END    350

/* ------------------- Trace configuration defines. ---------------------- */
#define SXR_NB_MAX_TRACEID         30

/* --------------------------- Files defines. ----------------------------- */
#define SXS_FLH_NB_FILES_MAX       16

/* --------------------------- Spy defines. ------------------------------- */
#define SXS_SPY_NB_MAX             8


#ifdef __cplusplus
}
#endif

#endif


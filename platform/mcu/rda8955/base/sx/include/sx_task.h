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


#ifndef _SX_TASK_H_
#define _SX_TASK_H_



// =============================================================================
//  MACROS
// =============================================================================
#define SXR_NO_TASK                              (0XFF)
#define SXR_FREE_TSK                             ((1 << 0))
#define SXR_ALLOCATED_TSK                        ((1 << 1))
#define SXR_PENDING_TSK                          ((1 << 2))
#define SXR_SUSPENDED_TSK                        ((1 << 3))
#define SXR_ACTIVE_TSK                           ((1 << 4))
#define SXR_WAIT_MSG_TSK                         ((1 << 5))
#define SXR_STOPPED_TSK                          (SXR_ALLOCATED_TSK)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SXR_TASKDESC_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    UINT32*                        TaskBody;                     //0x00000000
    UINT32*                        TaskExit;                     //0x00000004
    CONST INT8*                    Name;                         //0x00000008
    UINT16                         StackSize;                    //0x0000000C
    UINT8                          Priority;                     //0x0000000E
} SXR_TASKDESC_T; //Size : 0x10



// ============================================================================
// SXR_TASKCTX_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    UINT32*                        Sp;                           //0x00000000
    VOID*                          Pc;                           //0x00000004
    UINT32*                        StackTop;                     //0x00000008
    CONST SXR_TASKDESC_T*          Desc;                         //0x0000000C
    VOID*                          DataCtx;                      //0x00000010
    UINT8                          State;                        //0x00000014
    UINT8                          Id;                           //0x00000015
    UINT8                          Next;                         //0x00000016
    UINT8                          Free;                         //0x00000017
} SXR_TASKCTX_T; //Size : 0x18





#endif


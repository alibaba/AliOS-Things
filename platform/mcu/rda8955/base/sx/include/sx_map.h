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


#ifndef _SX_MAP_H_
#define _SX_MAP_H_


#include "sx_task.h"

// =============================================================================
//  MACROS
// =============================================================================
#define SXR_NB_MAX_JOB                           (12)
#define SXR_NB_MAX_JOB_EVT                       (16)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SXR_JOB_PREAL_STACK_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    UINT32*                        stackTop;                     //0x00000000
    UINT32*                        stackBottom;                  //0x00000004
} SXR_JOB_PREAL_STACK_T; //Size : 0x8



// ============================================================================
// SXR_JOB_STACK_DESCRIPTOR_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef SXR_JOB_PREAL_STACK_T SXR_JOB_STACK_DESCRIPTOR_T[10];


// ============================================================================
// SXR_JBDESC_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    UINT32*                        JbBody;                       //0x00000000
    UINT32*                        JbExit;                       //0x00000004
    INT8*                          Name;                         //0x00000008
    UINT16                         Id;                           //0x0000000C
    UINT8                          StackSize32;                  //0x0000000E
    UINT8                          Priority;                     //0x0000000F
} SXR_JBDESC_T; //Size : 0x10



// ============================================================================
// SXR_JBCTX_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    UINT32*                        Sp;                           //0x00000000
    CONST SXR_JBDESC_T*            Desc;                         //0x00000004
    VOID*                          Data;                         //0x00000008
    UINT32*                        StackSwp;                     //0x0000000C
    UINT32*                        StackTop;                     //0x00000010
    UINT32*                        StackBottom;                  //0x00000014
    UINT16                         State;                        //0x00000018
    UINT8                          Free;                         //0x0000001A
    UINT8                          IdxSort;                      //0x0000001B
} SXR_JBCTX_T; //Size : 0x1C



// ============================================================================
// SXR_JOB_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    UINT8                          Active;                       //0x00000000
    UINT8                          LastActive;                   //0x00000001
    UINT8                          InitCnt;                      //0x00000002
    UINT8                          IdxFree;                      //0x00000003
    UINT8                          IdxQueue;                     //0x00000004
    UINT8                          Load;                         //0x00000005
    UINT8                          EvtQueue[SXR_NB_MAX_JOB_EVT]; //0x00000006
    VOID*                          EvtData[SXR_NB_MAX_JOB_EVT];  //0x00000018
    SXR_JBCTX_T                    Ctx[SXR_NB_MAX_JOB];          //0x00000058
    UINT8                          Sorted[SXR_NB_MAX_JOB];       //0x000001A8
    UINT8                          FirstReady;                   //0x000001B4
    UINT8                          SkipFrameEndEvt;              //0x000001B5
    UINT32                         BitMapReady;                  //0x000001B8
    UINT16                         EvtStatus[4];                 //0x000001BC
} SXR_JOB_T; //Size : 0x1C4



// ============================================================================
// SX_MAP_ACCESS_T
// -----------------------------------------------------------------------------
/// This global variable is the shared structure of SX.
// =============================================================================
typedef struct
{
    UINT32*                        sxExternalTraceEnablePtr;     //0x00000000
    UINT32*                        sxsDebugIdleHookEnablePtr;    //0x00000004
    UINT32*                        sxrTaskPtr;                   //0x00000008
    UINT32*                        sxrSbxPtr;                    //0x0000000C
    UINT32*                        sxrMutexPtr;                  //0x00000010
    UINT32*                        sxrHeapPtr;                   //0x00000014
    UINT32*                        sxrClusterPtr;                //0x00000018
    UINT32*                        sxrUsrClusterPtr;             //0x0000001C
} SX_MAP_ACCESS_T; //Size : 0x8






// =============================================================================
// sx_RegisterYourself
// -----------------------------------------------------------------------------
/// This function registers the module itself to HAL so that the version and
/// the map accessor are filled. Then, the CSW get version function and the
/// CoolWatcher get version command will work.
// =============================================================================
PUBLIC VOID sx_RegisterYourself(VOID);



#endif


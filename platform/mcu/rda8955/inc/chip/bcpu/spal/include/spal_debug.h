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


#ifndef SPAL_DEBUG_H
#define SPAL_DEBUG_H

#include "cs_types.h"

#include "chip_id.h"

#include "global_macros.h"
#ifdef CHIP_DIE_8809E
#include "debug_port.h"
#endif


// =============================================================================
//  MACROS
// =============================================================================

#define CPMASK 0x3fff
#define CPEXITFLAG 0x8000
#define CPFINT 0x7fff

#define CPBCPU 0x4000 // hardcoded in spal_PXTS_SendProfilingCode function

#define CPPULSESSPACESTART 0x2f00
#define CPPULSESSPACEMASK 0x7ff

#define CPWINDOWSPACESTART 0x3700
#define CPWINDOWSPACEMASK 0x7ff

#define CPTASKSPACESTART 0x3f00
#define CPTASKSPACEMASK 0x1f

#define CPIRQSPACESTART 0x3f20
#define CPIRQSPACEMASK 0x1f

#define CPJOBPACESTART 0x3f40
#define CPJOBSPACEMASK 0x1f

#define CPBBIFC2SYMCNTSTART 0x7000
#define CPBBIFC2SYMCNTMAX 0x2ff


// SPAL DEBUG PROFILING FUNCTIONS

///@defgroup spal_debug SPAL Debug Functions
///@{



// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// SPAL_DBG_PXTS_LEVEL_T
// -----------------------------------------------------------------------------
/// The PXTS level PXTS level on which to send the code. This level is used to
/// filter the PXTS whithout having to recompile. Generally, each software
/// module has a level attributed.
// =============================================================================
typedef enum
{
    SPAL_DBG_PXTS_DUMP        = 0,
    SPAL_DBG_PXTS_RESERVED_1  = 1,
    SPAL_DBG_PXTS_RESERVED_2  = 2,
    SPAL_DBG_PXTS_RESERVED_3  = 3,
    SPAL_DBG_PXTS_RESERVED_4  = 4,
    SPAL_DBG_PXTS_RESERVED_5  = 5,
    SPAL_DBG_PXTS_RESERVED_6  = 6,
    SPAL_DBG_PXTS_RESERVED_7  = 7,
    SPAL_DBG_PXTS_RESERVED_8  = 8,
    SPAL_DBG_PXTS_RESERVED_9  = 9,
    SPAL_DBG_PXTS_BB_IFC2_SYM_CNT = 10,
    SPAL_DBG_PXTS_BB_RX_WIN   = 11,
    SPAL_DBG_PXTS_BB_IRQ      = 12,
    SPAL_DBG_PXTS_SPAL        = 13,
    SPAL_DBG_PXTS_SPP         = 14,
    SPAL_DBG_PXTS_SPC         = 15,
} SPAL_DBG_PXTS_LEVEL_T;



// =============================================================================
// FUNCTIONS
// =============================================================================


// =============================================================================
// spal_DbgPxtsProfileFunctionEnter
// -----------------------------------------------------------------------------
/// This function has to be called when entering the function you want to profile.
///
/// @param functionId Representing the function.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID spal_DbgPxtsProfileFunctionEnter(SPAL_DBG_PXTS_LEVEL_T level, UINT16 functionId);


// =============================================================================
// spal_DbgPxtsProfileFunctionExit
// -----------------------------------------------------------------------------
/// This function has to be called when exiting the function you want to profile.
///
/// @param functionId Code representing the function.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID spal_DbgPxtsProfileFunctionExit(SPAL_DBG_PXTS_LEVEL_T level, UINT16 functionId);


// =============================================================================
// spal_DbgPxtsProfileWindowEnter
// -----------------------------------------------------------------------------
/// This function has to be called to mark the entrance in a window.
///
/// @param windowId Code representing the window which has been entered in.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID spal_DbgPxtsProfileWindowEnter(SPAL_DBG_PXTS_LEVEL_T level, UINT16 windowId);


// =============================================================================
// spal_DbgPxtsProfileWindowExit
// -----------------------------------------------------------------------------
/// This function has to be called to mark the exit of a window.
///
/// @param windowId Code representing the window which has been exited.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID spal_DbgPxtsProfileWindowExit(SPAL_DBG_PXTS_LEVEL_T level, UINT16 windowId);


// =============================================================================
// spal_DbgPxtsProfileTaskEnter
// -----------------------------------------------------------------------------
/// Send to the profiling tool the code corresponding to scheduled task Id.
///
/// @param taskId Task id of the scheduled task.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID spal_DbgPxtsProfileTask(SPAL_DBG_PXTS_LEVEL_T level, UINT16 taskId);


// =============================================================================
// spal_DbgPxtsProfilePulse
// -----------------------------------------------------------------------------
/// Send to the profiling tool the code correponding to a pulse.
///
/// @param code Code representing the pulse
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID spal_DbgPxtsProfilePulse(SPAL_DBG_PXTS_LEVEL_T level, UINT16 code);


//======================================================================
// spal_DbgHstSendEvent
//----------------------------------------------------------------------
/// Send event to host
/// @param ch channel
//======================================================================
PUBLIC BOOL spal_DbgHstSendEvent(UINT32 ch);


#ifndef CHIP_HAS_AP
//======================================================================
// spal_DbgPxtsDump
//----------------------------------------------------------------------
// Send data buffer to host
//======================================================================
PUBLIC VOID spal_DbgPxtsDump(UINT16* StartAdrr, UINT16 length, UINT16 Marker);
#endif


// =============================================================================
// hal_DbgGdbBreakPoint
// -----------------------------------------------------------------------------
/// Set a break point in the source code
// =============================================================================
#define spal_DbgGdbBreakPoint() asm volatile("break 1\n\tnop\n\tnop\n\tnop")


// =============================================================================
// spal_GdbOpen
// -----------------------------------------------------------------------------
/// Initializes the variables used by GDB.
// =============================================================================
VOID spal_GdbOpen(VOID);

///@}


#endif // SPAL_DEBUG_H

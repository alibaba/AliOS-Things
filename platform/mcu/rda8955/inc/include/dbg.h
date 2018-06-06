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




#ifndef _DBG_H_
#define _DBG_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "cs_types.h"
#include "stdarg.h"

// =============================================================================
// DBG_ERR_T
// -----------------------------------------------------------------------------
/// This types is used to describes all possible error returned by DBG's
/// API functions.
// =============================================================================
typedef enum
{
    /// No error occured
    DBG_ERR_NO,

    /// Unsupported operation or parameter
    DBG_ERR_UNSUPPORTED,

    DBG_ERR_QTY
} DBG_ERR_T;


// =============================================================================
//  dbg_GdbBreakpoint
// -----------------------------------------------------------------------------
/// Software breakpoint.
/// Use this macro to insert a breakpoint in your code. You can then connect
/// GDB to the target to start stepping from there.
// =============================================================================
#define dbg_GdbBreakpoint() asm volatile("break 1\n\tnop")



// =============================================================================
// dbg_GdbPrint
// -----------------------------------------------------------------------------
/// GDB print function. Send a trace through GDB.
/// @param string Pointer to the string to send.
// =============================================================================
PUBLIC VOID dbg_GdbPrint(CHAR* string);



// =============================================================================
// dbg_AssertUtil
// -----------------------------------------------------------------------------
/// Utilitary function to define the assert.
/// Do not use this function directly. It is used in the public macro that
/// implements the minimal side-effect assert: use DBG_ASSERT to create
/// asserts.
/// @param format String format a-la printf, displayed in the trace.
// =============================================================================
PUBLIC VOID dbg_AssertUtil(CONST CHAR* format, ...);



#ifdef DBG_NO_ASSERT
#define DBG_ASSERT(condition, format, ...)
#else
// =============================================================================
//  ASSERT
// -----------------------------------------------------------------------------
/// Assert: If the boolean condition (first parameter) is false,
/// raise an assert and print the decorated string passed in the other
/// parameter.
// =============================================================================
#define DBG_ASSERT(condition, format, ...)                   \
    if (!(condition)) {            \
        dbg_AssertUtil(format, ##__VA_ARGS__);                             \
    }
#endif

// =============================================================================
// dbg_PxtsSendTrigger
// -----------------------------------------------------------------------------
/// Send to the profiling tool the code correponding to a pre-defined "trigger
/// tag". If the "PXTS Trigger" mode is enabled in the profiling tool, this
/// "trigger tag" will stop the PXTS recording. This can be used to stop the
/// PXTS recording from the embedded code.
// =============================================================================
PUBLIC VOID dbg_PxtsSendTrigger(VOID);




// =============================================================================
// dbg_PxtsProfileFunctionEnter
// -----------------------------------------------------------------------------
/// This function has to be called when entering the function you want to profile.
///
/// @param functionId Code representing the function.
// =============================================================================
PUBLIC VOID dbg_PxtsProfileFunctionEnter(UINT16 functionId);




// =============================================================================
// dbg_PxtsProfileFunctionExit
// -----------------------------------------------------------------------------
/// This function has to be called when exiting the function you want to profile.
///
/// @param functionId Code representing the function.
// =============================================================================
PUBLIC VOID dbg_PxtsProfileFunctionExit(UINT16 functionId);




// =============================================================================
// dbg_PxtsProfileWindowEnter
// -----------------------------------------------------------------------------
/// This function has to be called to mark the entrance in a window.
///
/// @param windowId Code representing the window which has been entered in.
// =============================================================================
PUBLIC VOID dbg_PxtsProfileWindowEnter(UINT16 windowId);




// =============================================================================
// dbg_PxtsProfileWindowExit
// -----------------------------------------------------------------------------
/// This function has to be called to mark the exit of a window.
///
/// @param windowId Code representing the window which has been exited.
// =============================================================================
PUBLIC VOID dbg_PxtsProfileWindowExit(UINT16 windowId);



// =============================================================================
// dbg_PxtsProfilePulse
// -----------------------------------------------------------------------------
/// Send to the profiling tool the code correponding to a pulse.
///
/// @param code Code representing the pulse
// =============================================================================
PUBLIC VOID dbg_PxtsProfilePulse(UINT16 code);



// =============================================================================
// dbg_TraceOpen
// -----------------------------------------------------------------------------
/// Open Trace Module
// =============================================================================
DBG_ERR_T dbg_TraceOpen(BOOL clearLevels);



// =============================================================================
// dbg_TraceClose
// -----------------------------------------------------------------------------
// Close Trace Module
// =============================================================================
DBG_ERR_T dbg_TraceClose();



// =============================================================================
// dbg_TraceSetOutputMask
// -----------------------------------------------------------------------------
// Set enabled trace level
// =============================================================================
DBG_ERR_T dbg_TraceSetOutputMask(UINT16 nIndex, BOOL bMode);



// =============================================================================
// dbg_TraceOutputText
// -----------------------------------------------------------------------------
// Output a trace
// =============================================================================
#ifndef BT_NO_PRINTF
PUBLIC DBG_ERR_T BT_TraceOutputText(UINT16 nIndex, PCSTR fmt, ...);
#else
#define BT_TraceOutputText(nIndex, fmt, ...)
#endif

PUBLIC DBG_ERR_T dbg_TraceOutputText(UINT16 nIndex, PCSTR fmt, ...);
// =============================================================================
// dbg_TraceVariadicOutputText
// -----------------------------------------------------------------------------
// Output a trace, variadic format.
// =============================================================================
PUBLIC DBG_ERR_T dbg_TraceVariadicOutputText(UINT16 nIndex, PCSTR fmt, va_list ap);

#ifdef __cplusplus
}
#endif

#endif // _DBG_H_



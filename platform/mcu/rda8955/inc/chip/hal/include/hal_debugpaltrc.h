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





#ifndef _HAL_DEBUGPALTRC_H_
#define _HAL_DEBUGPALTRC_H_

#include "cs_types.h"


//#define HAL_PAL_TRC_TCU_VERBOSE



/// @defgroup palTrace HAL Debug Pal Trace
/// The PAL Trace mechanism is a debug feature used by the
/// PAL layer. It holds in a buffer several traces from
/// a predefined set of items (HAL_DBGPALTRC_ITEM_T).
/// When the system crashes, this buffer is emptied through
/// the traces to the host, showing the latest PAL activities
/// before the crash.
///
/// @{



// =============================================================================
// HAL_DBGPALTRC_ITEM_T
// -----------------------------------------------------------------------------
/// PAL Debug Trace traceable items
// =============================================================================
typedef enum
{
    HAL_DBGPALTRC_FRAMESTART,
    HAL_DBGPALTRC_FRAMEEND,
    HAL_DBGPALTRC_PALINIT,
    HAL_DBGPALTRC_TIMETUNING,
    HAL_DBGPALTRC_SYNCCHANGE,
    HAL_DBGPALTRC_FREQTUNING,
    HAL_DBGPALTRC_SETCOUNTERS,
    HAL_DBGPALTRC_RXBUFFCFG,
    HAL_DBGPALTRC_SETMONWIN,
    HAL_DBGPALTRC_GETMONRES,
    HAL_DBGPALTRC_SETFCHWIN,
    HAL_DBGPALTRC_CLOSEFCHWIN,
    HAL_DBGPALTRC_GETFCHOK,
    HAL_DBGPALTRC_GETFCHKO,
    HAL_DBGPALTRC_SETSCHWIN,
    HAL_DBGPALTRC_GETSCHOK,
    HAL_DBGPALTRC_GETSCHKO,
    HAL_DBGPALTRC_SETINTWIN,
    HAL_DBGPALTRC_GETINTRES,
    HAL_DBGPALTRC_SETNBWIN,
    HAL_DBGPALTRC_GETNBSTRES,
    HAL_DBGPALTRC_GETNBLKRES,
    HAL_DBGPALTRC_GETUSF,
    HAL_DBGPALTRC_ABSTENC,
    HAL_DBGPALTRC_NBLKENC,
    HAL_DBGPALTRC_SETTXWIN,
    HAL_DBGPALTRC_SETRACH,
    HAL_DBGPALTRC_WINRESIZE,
    HAL_DBGPALTRC_STARTCIPH,
    HAL_DBGPALTRC_STOPCIPH,
    HAL_DBGPALTRC_GPRSCIPH,
    HAL_DBGPALTRC_STARTTCH,
    HAL_DBGPALTRC_CHANGETCH,
    HAL_DBGPALTRC_RETAMRCFG,
    HAL_DBGPALTRC_SETDTX,
    HAL_DBGPALTRC_GETDTX,
    HAL_DBGPALTRC_SETLOOP,
    HAL_DBGPALTRC_STOPTCH,
    HAL_DBGPALTRC_FSUSPEND,
    HAL_DBGPALTRC_FRESUME,
    HAL_DBGPALTRC_GETELAPSED,
    HAL_DBGPALTRC_RFWAKEUP,
    HAL_DBGPALTRC_RFSLEEP,
    HAL_DBGPALTRC_RESACTIVE,
    HAL_DBGPALTRC_RESINACTIVE,
    HAL_DBGPALTRC_PALSLEEP,
    HAL_DBGPALTRC_SETUSRVECT,
    HAL_DBGPALTRC_SETUSRVECTMSK,
    HAL_DBGPALTRC_TEMP1,
    HAL_DBGPALTRC_TEMP2,
} HAL_DBGPALTRC_ITEM_T;


// =============================================================================
// hal_DbgPalFillTrace
// -----------------------------------------------------------------------------
/// Add one item to the trace buffer
/// The parameters are the Trace id, from the previous enum,
/// and eventual parameters.
// =============================================================================
PUBLIC VOID hal_DbgPalFillTrace(UINT32 TrcId, UINT32 arg0, UINT32 arg1, UINT32 arg2, UINT32 arg3, UINT32 arg4, UINT32 arg5);




// =============================================================================
// hal_DbgPalFlushTrace
// -----------------------------------------------------------------------------
/// Flush the trace
/// A call to this function prints the last items
/// recorded into the trace circular buffer.
// =============================================================================
PUBLIC VOID hal_DbgPalFlushTrace(VOID);

/// @}


#endif



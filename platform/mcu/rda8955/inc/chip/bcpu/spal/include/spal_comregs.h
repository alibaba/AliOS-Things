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


#ifndef __SPAL_COMREGS_H__
#define __SPAL_COMREGS_H__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "cs_types.h"
#include "comregs.h"
#include "global_macros.h"


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

//======================================================================
// spal_ComregsSetIrq
//----------------------------------------------------------------------
//  Set Inter-Cpu Interrupt Cause
//
// @param Irq UINT32. Bitmap of cause to be set. Depending on the current
//              Comregs Irq Mask setup this will trigger an interrupt
//              to the Xcpu.
//======================================================================
INLINE VOID spal_ComregsSetIrq(UINT32 Irq)
{
    hwp_bbComregs->ItReg_Set = Irq;
}

//======================================================================
// spal_ComregsClrIrq
//----------------------------------------------------------------------
// Clear Inter-Cpu Interrupt Cause
//
// @param Irq UINT32. Bitmap of cause to be cleared.
//
//======================================================================
INLINE VOID spal_ComregsClrIrq(UINT32 Irq)
{
    hwp_bbComregs->ItReg_Clr = Irq;
}

//======================================================================
// spal_ComregsGetSnap
//----------------------------------------------------------------------
// Get current Frame snapshot index
//
// @return Current Frame snapshot index
//======================================================================
INLINE UINT8 spal_ComregsGetSnap(VOID)
{
    return ((UINT8)hwp_bbComregs->Snapshot);
}

#endif // __SPAL_COMREGS_H__


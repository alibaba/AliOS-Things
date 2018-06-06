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


#ifndef __SPAL_TCU_H__
#define __SPAL_TCU_H__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "cs_types.h"
#include "tcu.h"
#include "global_macros.h"


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------
#if defined(CHIP_DIE_8909)
#define hwp_tcu                 hwp_gsmTcu
#endif
//======================================================================
// spal_TcuReadEnable()
//----------------------------------------------------------------------
/// this function ors the value of the Enable_Event register with the
/// value of event parameter.
//
/// @param event
//======================================================================

INLINE VOID spal_TcuEnableEvent(UINT32 event)
{
    hwp_tcu->Enable_Event |= event;
}

//======================================================================
// spal_TcuWriteDisableEvent()
//----------------------------------------------------------------------
/// this function writes the Disable_Event register.
//
/// @param event
//======================================================================

INLINE VOID spal_TcuDisableEvent(UINT32 event)
{
    hwp_tcu->Disable_Event = event;
}


// =============================================================================
// spal_TcuSetTco
// -----------------------------------------------------------------------------
/// This function sets the corresponding TCO line to its
/// active level (which depends on TCO's polarity)
/// @param tcoNumber : TCO bitmap to be set
// =============================================================================
INLINE VOID  spal_TcuSetTco(UINT32 BitMap)
{
    hwp_tcu->Set_TCO = BitMap;
}



// =============================================================================
// spal_TcuClrTco
// -----------------------------------------------------------------------------
/// This function sets the corresponding TCO line to its
/// inactive level (which depends on TCO's polarity)
/// @param tcoNumber : TCO bitmap to be reset
// =============================================================================
INLINE VOID spal_TcuClrTco(UINT32 BitMap)
{
    hwp_tcu->Clr_TCO = BitMap;
}



#endif // __SPAL_TCU_H__

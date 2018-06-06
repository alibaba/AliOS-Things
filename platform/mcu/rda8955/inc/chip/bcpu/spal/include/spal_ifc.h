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


#ifndef __SPAL_IFC_H__
#define __SPAL_IFC_H__


//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "cs_types.h"
#include "global_macros.h"

PUBLIC VOID spal_IfcTxFifoReset(VOID);

//======================================================================
// spal_IfcTxFifoSetup
//----------------------------------------------------------------------
/// Configure the Tx dedicate channel to output the data contained in the
/// Tx fifo (if any) through the Rf Interface.
/// @param pFifo. Pointer to the fifo start address
/// @param sizeFifo. Size of the fifo
//======================================================================
PUBLIC VOID spal_IfcTxFifoSetup(UINT32* pFifo, UINT32 sizeFifo);

//======================================================================
// spal_IfcDisableRf
//----------------------------------------------------------------------
/// Disable the Ifc channel CH2 used for Rf samples
//======================================================================
PUBLIC VOID spal_IfcDisableRf(VOID);

//======================================================================
// spal_IfcParamRf
//----------------------------------------------------------------------
/// Set the Channel2 parameters for Rf Ifc
//======================================================================
PUBLIC VOID spal_IfcParamRf(UINT32 mask,
                            UINT32 startAddr,
                            UINT32 endAddr,
                            UINT32 tc,
                            UINT32 control);

#endif // __SPAL_IFC_H__



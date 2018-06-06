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


#ifndef __SPAL_RFIF_H__
#define __SPAL_RFIF_H__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "cs_types.h"
#include "rf_if.h"
#include "global_macros.h"


//------------------------------------------------------------------------------
// Structure
//------------------------------------------------------------------------------

typedef enum
{
    SPAL_UNKNOWN_MOD                             = 0x00000000,
    SPAL_GMSK_MOD                                = 0x00000001,
    SPAL_8PSK_MOD                                = 0x00000002
} SPAL_MODULATION_TYPE_T;

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------



//======================================================================
// spal_RfIfTxFifoReset
//----------------------------------------------------------------------
/// reset the Tx fifo.
//======================================================================

INLINE VOID spal_RfIfTxFifoReset(VOID)
{
#if (CHIP_IFC_RF_TX_CHANNEL==1)
//   UINT32 bstIdx;
//   for (bstIdx=0;bstIdx<4;bstIdx++)
//       hwp_rfIf->Tx_Burst_descriptor[bstIdx] = 0;
#endif
    // TODO check the behavior in digRf when the
    // following disable is commented
    //hwp_rfIf->Ctrl &= ~(RF_IF_ENABLE_ENABLE);
    hwp_rfIf->Ctrl |= (RF_IF_ENABLE_ENABLE);
    hwp_rfIf->Ctrl |= RF_IF_TX_FIFO_RESET;
}

//======================================================================
// spal_RfIfWriteCtrl
//----------------------------------------------------------------------
/// write the RfIf Ctrl register.
/// @param ctrl
//======================================================================

INLINE VOID spal_RfIfWriteCtrl(UINT32 ctrl)
{

    hwp_rfIf->Ctrl = ctrl;
}

//======================================================================
// spal_RfIfTxFifoWrite
//----------------------------------------------------------------------
/// Write to the Rfif Tx fifo. Depending on implementation, this function will
/// directly write to the fifo of the Rfif interface on APB or write in a
/// software fifo emptied by a dedicated IFC channel.
//
/// @param pFifo UINT32*. Pointer to the software Tx fifo (if used).
/// @param data UINT3*. Data to be written.
//======================================================================
INLINE VOID spal_RfIfTxFifoWrite(UINT32* pFifo, UINT32 data)
{
#if (CHIP_IFC_RF_TX_CHANNEL==0)
    hwp_rfIf->Buffer  = data;
#else // (CHIP_IFC_RF_TX_CHANNEL)
    *pFifo = data;
#endif // (!CHIP_IFC_RF_TX_CHANNEL)
}

//======================================================================
// spal_RfIfTxFifoWrite
//----------------------------------------------------------------------
//
/// @param bstIdx
/// @param nbSymbols
/// @param BstMod
/// @param endOfWin
//======================================================================
INLINE VOID spal_RfIfTxFifoSetBurst(UINT8 bstIdx,
                                    UINT16 nbSymbols,
                                    SPAL_MODULATION_TYPE_T BstMod,
                                    BOOL endOfWin)
{
#if (CHIP_IFC_RF_TX_CHANNEL==1)
    UINT32 bstDesc = (BstMod==SPAL_8PSK_MOD)?RF_IF_MODULATION:0;
    if (endOfWin) bstDesc |= RF_IF_END_BURST;
    bstDesc = bstDesc | RF_IF_NB_SYMBOLS(nbSymbols-1);
    hwp_rfIf->Tx_Burst_descriptor[bstIdx] = bstDesc;
#endif
}

#endif // __SPAL_RFIF_H__


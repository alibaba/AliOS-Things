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


#ifndef SPAL_ITLV_H
#define SPAL_ITLV_H

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "cs_types.h"
#include "global_macros.h" ///@deprecated

// SPAL ITLV
///@defgroup spal_itlv SPAL Interleaver API
///@{


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

//======================================================================
// spal_ItlvBusy
//----------------------------------------------------------------------
/// Check the status of the Interleaver
/// To be deleted after
///
/// @return BOOL. TRUE if interleaver is busy, FALSE otherwise.
///
//======================================================================
PUBLIC BOOL spal_ItlvBusy();

//======================================================================
// spal_ItlvItlv
//----------------------------------------------------------------------
/// Interleaving driver
///
/// @param ItlvType UINT8. INPUT. ItlvType may take the following values:
/// - 0 : TYPE_1A. Valid for TCH/FS, TCH/EFS, FACCH/F, TCH/F2.4,
/// TCH/AFS (speech, ratscch, sid_first).
/// - 1 : TYPE_1B. Valid for SACCH, SDCCH, TCH/AFS(sid_update), PDTCH(CS-1 to CS-4),
/// BCCH, PCH, AGCH, PACCH, PBCCH, PAGCH, PPCH, PNCH, PTCCH/D.
/// - 2 : TYPE_1C. Valid for FACCH/H.
/// - 3 : TYPE_2A. Valid for TCH/HS and TCH/AHS (speech, ratscch, sid_first).
/// - 4 : TYPE_2B. Valid for TCH/AHS (sid_update).
/// - 5 : TYPE_3. Valid for TCH/F14.4, TCH/F9.6, TCH/F4.8, TCH/H4.8
/// and TCH/H2.4.
/// .
/// @param burst_offset UINT16. INPUT. Offset position for interleaving (see ITLV
///                      documentation).
/// @param nb_bits UINT16. INPUT. Number of bits to interleave.
/// @param bb_addr UINT32*. INPUT. Pointer to channel coded burst buffer.
/// @param fb_addr UINT32*. OUTPUT. Output buffer pointer.
///
//======================================================================
PUBLIC VOID spal_ItlvItlv(
    UINT8 ItlvType,
    UINT16 burst_offset,
    UINT16 nb_bits,
    UINT32* bb_addr,
    UINT32* fb_addr);

//======================================================================
// spal_ItlvDitlv
//----------------------------------------------------------------------
/// Deinterleaving driver
///
/// @param ItlvType UINT8. ItlvType may take the following values:
/// - 0 : TYPE_1A. Valid for TCH/FS, TCH/EFS, FACCH/F, TCH/F2.4,
/// TCH/AFS (speech, ratscch, sid_first).
/// - 1 : TYPE_1B. Valid for SACCH, SDCCH, TCH/AFS(sid_update), PDTCH(CS-1 to CS-4),
/// BCCH, PCH, AGCH, PACCH, PBCCH, PAGCH, PPCH, PNCH, PTCCH/D.
/// - 2 : TYPE_1C. Valid for FACCH/H.
/// - 3 : TYPE_2A. Valid for TCH/HS and TCH/AHS (speech, ratscch, sid_first).
/// - 4 : TYPE_2B. Valid for TCH/AHS (sid_update).
/// - 5 : TYPE_3. Valid for TCH/F14.4, TCH/F9.6, TCH/F4.8, TCH/H4.8
/// and TCH/H2.4.
/// .
/// @param burst_offset UINT16. INPUT. Offset position for deinterleaving (see ITLV
///                      documentation).
/// @param nb_bits UINT16. INPUT. Number of bits to deinterleave.
/// @param bb_addr UINT32*. INPUT. Pointer to equalized bursts buffer.
/// @param fb_addr UINT32*. OUTPUT. Output buffer pointer.
//======================================================================
PUBLIC VOID spal_ItlvDitlv(
    UINT8 ItlvType,
    UINT16 burst_offset,
    UINT16 nb_bits,
    UINT32* bb_addr,
    UINT32* fb_addr);

//======================================================================
// spal_ItlvIntClear
//----------------------------------------------------------------------
/// Clear the transfer done interrupt
//
//======================================================================
PUBLIC VOID spal_ItlvIntClear();



///@}


#endif                                          // #ifndef SPAL_ITLV_H


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


#ifndef _BASEBAND_DEFS_H_
#define _BASEBAND_DEFS_H_



// =============================================================================
//  MACROS
// =============================================================================
/// -------------------------------------------------------------------- Timings
/// and Sizes These timings are shared by different SW modules and are derived from
/// baseband signal processing capabilities and needs --------------------------------------------------------------------
/// Values in symbols
#define BB_RX_SCH_MARGIN                         (12)
#define BB_RX_NBURST_MARGIN                      (4)
/// One Qb is dropped
#define BB_BURST_TOTAL_SIZE                      (156)
/// Used for EQU_HBURST_MODE'
#define BB_BURST_HALF_SIZE                       (96)
#define BB_BURST_ACTIVE_SIZE                     (148)
#define BB_SCH_TOTAL_SIZE                        ((BB_BURST_ACTIVE_SIZE + 2*BB_RX_SCH_MARGIN))

// =============================================================================
//  TYPES
// =============================================================================



/**
@file
Defines related to the BaseBand signal processing
*/


#endif


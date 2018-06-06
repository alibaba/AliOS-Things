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


#ifndef _BB_SRAM_H_
#define _BB_SRAM_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'bb_sram'."
#endif


#include "globals.h"

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// BB_SRAM_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_BB_SRAM_BASE           0x01980000

typedef volatile struct
{
    UINT8 ram_array[24576];                     //0x00000000
} HWP_BB_SRAM_T;

#define hwp_bbSram                 ((HWP_BB_SRAM_T*) KSEG1(REG_BB_SRAM_BASE))






#endif


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


#ifndef _XCPU_PMEM_H_
#define _XCPU_PMEM_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'xcpu_pmem'."
#endif



// =============================================================================
//  MACROS
// =============================================================================
#define PMEM_SRAM_SIZE                          (32768)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// XCPU_PMEM_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef volatile struct
{
    UINT8 sram_array[32768];                    //0x00000000
} HWP_XCPU_PMEM_T;







#endif


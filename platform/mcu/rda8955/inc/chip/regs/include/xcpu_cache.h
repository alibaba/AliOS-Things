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


#ifndef _XCPU_CACHE_H_
#define _XCPU_CACHE_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'xcpu_cache'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// XCPU_CACHE_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_XCPU_IDATA_BASE        0x01A1B000
#define REG_XCPU_DDATA_BASE        0x01A1C000

typedef volatile struct
{
    struct
    {
        /// The XCpu cache is accessible only when it is disabled or the cpu is
        /// stalled.
        /// The bits "3:0" of the address select the Data in the line.
        /// The bits "13:4" of the address select the line.
        /// If the Tag at the corresponding line is valid, the bits "25:14" of
        /// the tag are the corresponding address bits.
        REG32                      Data[4];                      //0x00000000
    } Line[EXP2(8)];
} HWP_XCPU_CACHE_T;

#define hwp_xcpuIdata              ((HWP_XCPU_CACHE_T*) KSEG1(REG_XCPU_IDATA_BASE))
#define hwp_xcpuDdata              ((HWP_XCPU_CACHE_T*) KSEG1(REG_XCPU_DDATA_BASE))


//Data
#define XCPU_CACHE_DATA(n)         (((n)&0xFFFFFFFF)<<0)





#endif


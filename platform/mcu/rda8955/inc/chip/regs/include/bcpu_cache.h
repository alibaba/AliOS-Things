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


#ifndef _BCPU_CACHE_H_
#define _BCPU_CACHE_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'bcpu_cache'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// BCPU_CACHE_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_BCPU_IDATA_BASE        0x0190C000
#define REG_BCPU_DDATA_BASE        0x0190D000

typedef volatile struct
{
    struct
    {
        /// The BCpu cache is accessible only when it is disabled or the cpu is
        /// stalled.
        /// The bits "9:4" of the address select the line.
        /// If the Tag at the corresponding line is valid, the bits "24:10" of
        /// the tag are the corresponding address bits.
        /// The bits "3:0" of the address select the Data in the line.
        REG32                      Data[4];                      //0x00000000
    } Line[EXP2(6)];
} HWP_BCPU_CACHE_T;

#define hwp_bcpuIdata              ((HWP_BCPU_CACHE_T*) KSEG1(REG_BCPU_IDATA_BASE))
#define hwp_bcpuDdata              ((HWP_BCPU_CACHE_T*) KSEG1(REG_BCPU_DDATA_BASE))


//Data
#define BCPU_CACHE_DATA(n)         (((n)&0xFFFFFFFF)<<0)





#endif


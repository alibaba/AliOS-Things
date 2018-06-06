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


#ifndef _BCPU_TAG_H_
#define _BCPU_TAG_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'bcpu_tag'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// BCPU_TAG_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_BCPU_TAG_BASE          0x0190B000

typedef volatile struct
{
    /// The BCpu tags are accessible only when the corresponding cache is
    /// disabled or the cpu is stalled.
    /// The bits "9:4" of the address select the line
    REG32                          Line[EXP2(6)];                //0x00000000
} HWP_BCPU_TAG_T;

#define hwp_bcpuTag                ((HWP_BCPU_TAG_T*) KSEG1(REG_BCPU_TAG_BASE))


//Line
#define BCPU_TAG_TAG(n)            (((n)&0x7FFF)<<10)
#define BCPU_TAG_TAG_MASK          (0x7FFF<<10)
#define BCPU_TAG_TAG_SHIFT         (10)
#define BCPU_TAG_VALID             (1<<29)
#define BCPU_TAG_VALID_MASK        (1<<29)
#define BCPU_TAG_VALID_SHIFT       (29)





#endif


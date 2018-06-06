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


#ifndef _XCPU_TAG_H_
#define _XCPU_TAG_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'xcpu_tag'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// XCPU_TAG_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_XCPU_TAG_BASE          0x01A1A000

typedef volatile struct
{
    /// The XCpu tags are accessible only when the corresponding cache is
    /// disabled or the cpu is stalled.
    /// The bits "13:4" of the address select the line
    REG32                          Line[EXP2(8)];                //0x00000000
} HWP_XCPU_TAG_T;

#define hwp_xcpuTag                ((HWP_XCPU_TAG_T*) KSEG1(REG_XCPU_TAG_BASE))


//Line
#define XCPU_TAG_TAG(n)            (((n)&0xFFF)<<14)
#define XCPU_TAG_TAG_MASK          (0xFFF<<14)
#define XCPU_TAG_TAG_SHIFT         (14)
#define XCPU_TAG_VALID             (1<<29)
#define XCPU_TAG_VALID_MASK        (1<<29)
#define XCPU_TAG_VALID_SHIFT       (29)





#endif


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


#ifndef _CORDIC_H_
#define _CORDIC_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'cordic'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// CORDIC_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_CORDIC_BASE            0x01911000

typedef volatile struct
{
    REG32                          yin;                          //0x00000000
    REG32                          xin;                          //0x00000004
    REG32                          cmd;                          //0x00000008
    REG32                          status;                       //0x0000000C
    REG32                          dout;                         //0x00000010
    REG32                          amp;                          //0x00000014
} HWP_CORDIC_T;

#define hwp_cordic                 ((HWP_CORDIC_T*) KSEG1(REG_CORDIC_BASE))


//yin
#define CORDIC_Y_ADDR(n)           (((n)&0xFFFFFFFF)<<0)

//xin
#define CORDIC_X_ADDR(n)           (((n)&0xFFFFFFFF)<<0)

//cmd
#define CORDIC_CMD_RST             (0<<0)
#define CORDIC_CMD_START           (1<<0)

//status
#define CORDIC_OP_BUSY             (0x0<<0)
#define CORDIC_OP_FREE             (0xFFFFFFFF<<0)

//dout
#define CORDIC_RESULT_ANGLE(n)     (((n)&0xFFFF)<<0)
#define CORDIC_RESULT_AMPLITUDE(n) (((n)&0xFFFF)<<16)

//amp
#define CORDIC_RESULT_AMP(n)       (((n)&0x1FFFF)<<0)
#define CORDIC_RESULT_AMP_MASK     (0x1FFFF<<0)





#endif

